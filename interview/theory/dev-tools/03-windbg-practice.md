# WinDbg 实战与特殊场景

## 概述

WinDbg 面试真正考察的不是单个命令，而是面对复杂问题时的分析路径。本文件聚焦实战 workflow：拿到 dump 后如何下手、堆损坏如何定位真正破坏者、没有符号时如何分析、A 写坏内存但 B 才崩溃如何追凶，以及死锁、栈损坏、句柄泄漏等常见场景。

---

## 1. 拿到 Crash Dump 的标准流程

### 1.1 前 5 个命令

```
.symfix; .reload
!analyze -v
.ecxr
kP
lm vm <可疑模块名>
```

### 1.2 标准分析步骤

```text
1. 配置符号
   .symfix; .reload

2. 自动分析
   !analyze -v
   关注 ExceptionCode、Faulting IP、FAULTING_SOURCE_LINE、STACK_TEXT

3. 切换异常上下文
   .ecxr
   这是很多人会漏掉的一步。Dump 当前上下文不一定是崩溃上下文。

4. 查看真实调用栈
   k / kv / kP

5. 查看崩溃指令与寄存器
   r
   u @rip-20 L40   # x64
   u @eip-20 L40   # x86

6. 查看参数与局部变量
   .frame <n>
   dv /t /V
   dt <type> <addr>

7. 判断问题类型
   - 空指针 / 野指针
   - 越界访问
   - 堆损坏
   - 栈损坏
   - 多线程竞态
   - 模块版本/符号不匹配
```

### 1.3 面试回答模板

> 我会先保证符号正确，然后 `!analyze -v` 看异常类型和故障模块；接着 `.ecxr` 切到异常上下文，再用 `kP` 看真实调用栈。之后结合 `r` 和 `u` 看崩溃指令访问了哪个地址，再用 `!address` / `!heap -p -a` 判断该地址属于栈、堆、模块还是无效内存。最后根据类型进入空指针、UAF、堆损坏或栈损坏的专项排查流程。

---

## 2. Access Violation 定位

### 2.1 读/写空指针

```
!analyze -v
.ecxr
r
u @rip-20 L30
```

典型输出：

```text
ExceptionCode: c0000005 (Access violation)
Attempt to read from address 0000000000000008

mov rax, qword ptr [rcx+8]
rcx=0000000000000000
```

分析：
- 指令访问 `[rcx+8]`
- `rcx == 0`
- 所以是空指针对象访问成员偏移 8

下一步：

```
kP
.frame 1
dv /t
```

找到是谁传入了空对象。

### 2.2 野指针 / Use-After-Free

```text
ExceptionCode: c0000005
Attempt to read from address 0000023f`12345678
```

判断地址属性：

```
!address 0000023f12345678
!heap -p -a 0000023f12345678
```

如果启用了 Page Heap，`!heap -p -a` 可能直接给出：
- 分配调用栈
- 释放调用栈
- 当前访问调用栈

这就能证明：对象已释放，后续又被使用。

---

## 3. 堆内存损坏：A 写坏，B 崩溃

这是面试高频难点。核心问题是：**崩溃点通常不是破坏点**。

### 3.1 典型现象

```cpp
void A() {
    char* p = new char[16];
    strcpy(p, "this string is longer than 16 bytes"); // 写越界，破坏堆元数据
}

void B() {
    char* q = new char[32];  // 或 delete 某个对象时崩溃
}
```

崩溃可能发生在：
- `new`
- `delete`
- `malloc`
- `free`
- `RtlAllocateHeap`
- `RtlFreeHeap`
- `ntdll!RtlReportCriticalFailure`

但真正破坏者是更早的 A。

### 3.2 普通 dump 中怎么分析

```
!analyze -v
.ecxr
kP
!heap -s
!heap -v
```

如果看到：

```text
HEAP_CORRUPTION_ACTIONABLE
heap failure detected
corrupted suffix pattern
```

说明堆管理器检测到堆块边界或元数据被破坏。

继续：

```
!heap -p -a <可疑地址>
!address <可疑地址>
db <可疑地址>-40 L100
```

观察：
- 堆块前后是否有固定填充值被覆盖
- 是否有明显字符串、对象 vtable、业务数据痕迹
- 被破坏地址附近属于哪个堆块

但仅凭事后 dump，经常只能定位到"哪个堆坏了"，不一定能定位谁写坏。

### 3.3 正确追凶：开启 Page Heap

命令行启用：

```
gflags /p /enable MyApp.exe /full
```

然后重新运行程序复现。

Page Heap 的作用：

```text
普通堆：
[用户块][堆元数据][下一个块]
越界写可能暂时没事，直到后续分配/释放才崩。

Page Heap：
[用户块][不可访问保护页]
一旦越界写，立即 Access Violation。
崩溃点就是写坏内存的位置。
```

复现崩溃后：

```
!analyze -v
.ecxr
kP
r
u @rip-20 L30
```

这时调用栈通常就指向真正的 A。

### 3.4 如果不能复现怎么办？

1. 尽量收集 full dump，而不是 mini dump
2. 在关键分配点加日志或埋点
3. 开启 Application Verifier + Page Heap 灰度运行
4. 缩小范围：根据被破坏数据内容反推写入者
5. 若可使用 WinDbg Preview，使用 TTD 录制复现路径

---

## 4. 数据断点：定位谁改坏了变量

### 4.1 适用场景

- 某个对象字段突然变成非法值
- vtable 被改坏
- 指针从有效地址变成 `0xdddddddd` / `0xfeeefeee`
- 引用计数异常

### 4.2 操作流程

先找到被破坏字段地址：

```
dt MyApp!CSession 000001ab`12340000
```

假设字段：

```text
+0x030 m_state : 0n5
```

设置写断点：

```
ba w 4 000001ab`12340030
g
```

下一次该字段被写入时中断：

```
kP
r
u @rip-10 L20
```

调用栈就是写入者。

### 4.3 限制

- 硬件断点数量有限（通常 4 个）
- 只适合 live debugging，不适合单个事后 dump
- 地址必须稳定；对象重新分配后地址变化，需要重新设置

---

## 5. 无符号文件如何定位崩溃

### 5.1 基本思路

没有 PDB 时，仍然可以通过：
- 模块基址
- 崩溃地址偏移
- 导出符号
- MAP 文件
- 反汇编
- 版本号/时间戳

定位到函数或代码区域。

### 5.2 计算偏移

```
lm vm MyApp
```

输出：

```text
start             end                 module name
00007ff6`10000000 00007ff6`10500000   MyApp
```

崩溃地址：

```text
00007ff6`10234567
```

偏移：

```text
0x10234567 - 0x10000000 = 0x234567
```

在构建产物中查：
- MAP 文件：查 RVA 附近的函数
- 反汇编文件：查地址范围
- IDA/Ghidra：加载同版本 exe，跳转 RVA `0x234567`

### 5.3 没有 MAP 文件怎么办

```
u 00007ff6`10234500 L80
ub 00007ff6`10234567
```

看：
- 函数 prologue：`push rbp`, `sub rsp, xx`
- call 目标：是否调用了已知 API
- 字符串引用：`s -u MyAppBase L?Size "关键字符串"`
- vtable / RTTI 字符串

如果崩溃附近调用了导入函数：

```
dps @rsp L40
```

栈上可能仍有返回地址，可反推出调用链。

### 5.4 面试回答模板

> 没有符号时，我会先用 `lm vm` 确认模块基址和版本，用崩溃地址减去模块基址得到 RVA，然后拿同版本二进制、MAP 文件或反汇编工具定位函数。如果没有 MAP，则在 WinDbg 中用 `u/ub` 看崩溃指令和附近 call，结合导入 API、字符串引用、栈上的返回地址手动还原调用路径。

---

## 6. 栈损坏与栈溢出

### 6.1 栈溢出

典型异常：

```text
ExceptionCode: c00000fd (Stack overflow)
```

命令：

```
!analyze -v
.ecxr
k
!teb
```

`!teb` 查看栈范围：

```text
StackBase: 00000000`00190000
StackLimit: 00000000`0018c000
```

如果 `rsp` 接近 `StackLimit`，通常是栈溢出。

常见原因：
- 无限递归
- 递归深度过大
- 栈上分配大数组
- 互相递归调用

### 6.2 栈损坏

表现：
- `k` 调用栈断裂
- 返回地址是非法地址
- 栈中出现 `cccccccc` / `dddddddd`
- `!analyze -v` 指向奇怪模块

排查：

```
r
!teb
dps @rsp L200
```

手动扫描栈上的返回地址，判断哪些地址落在有效模块范围内：

```
lm
!address <疑似返回地址>
```

常见原因：
- 栈数组越界
- 调用约定不匹配（x86 常见）
- 函数指针类型不匹配
- 手写汇编破坏栈平衡

---

## 7. 死锁与卡死分析

### 7.1 Dump 分析流程

```
~* k
!locks
!cs -l
!runaway
```

看每个线程在等什么：

```text
线程 A:
ntdll!NtWaitForSingleObject
kernel32!WaitForSingleObject
MyApp!ThreadA

线程 B:
ntdll!NtWaitForSingleObject
kernel32!WaitForSingleObject
MyApp!ThreadB
```

如果 A 持有锁 1 等锁 2，B 持有锁 2 等锁 1，就是经典死锁。

### 7.2 Critical Section

```
!cs -l
!cs <addr>
```

关注：
- `LockCount`
- `OwningThread`
- `RecursionCount`
- `EntryCount`

根据 owning thread 切过去：

```
~~[线程ID]s
kP
```

### 7.3 UI 线程卡死

```
~* k
```

如果 UI 线程在：
- `WaitForSingleObject`
- `SendMessage`
- 网络请求
- 磁盘 IO
- 长时间计算

说明主线程被阻塞。Duilib/Win32 UI 都要求主线程持续 pump message，否则窗口假死。

---

## 8. 句柄泄漏

### 8.1 现象

- 程序运行久了无法创建窗口/文件/线程
- GDI 绘制异常
- `CreateFile/CreateEvent/CreateThread` 失败
- 任务管理器 Handle Count 持续增长

### 8.2 分析

```
!handle
!handle 0 f
```

如果要追踪句柄创建/关闭，需要 Application Verifier 或 GFlags 开启 handle tracing：

```
!htrace -enable
```

复现后：

```
!htrace <handle>
```

查看创建调用栈和关闭调用栈，找出只创建不关闭的位置。

---

## 9. DLL 加载与初始化崩溃

### 9.1 加载时中断

```
sxe ld:MyPlugin.dll
g
```

模块加载时停下：

```
lm vm MyPlugin
!dlls
```

### 9.2 DllMain 崩溃

```
!analyze -v
.ecxr
kP
```

如果调用栈在 loader lock 期间，注意 DllMain 中不能做复杂操作：
- 启动线程并等待
- LoadLibrary 其他 DLL
- 初始化 COM
- 加锁等待其他线程

### 9.3 Loader Snaps

命令行开启：

```
gflags /i MyApp.exe +sls
```

可以输出 DLL 加载详细日志，定位依赖缺失、初始化顺序问题。

---

## 10. C++ 异常与异常被吞

### 10.1 捕获 first-chance C++ exception

```
sxe 0xE06D7363
g
```

每次 `throw` 都会中断。用于定位异常最早抛出位置，而不是最终被 catch 后消失。

### 10.2 SEH 链

```
!exchain
```

可查看当前线程异常处理链，定位是否有全局异常过滤器、异常被某层吞掉。

### 10.3 常见回答

> 如果异常被 catch 吞掉，事后 dump 可能看不到真正抛出点。我会在 live debug 中设置 first-chance exception 断点，例如 C++ 异常 `sxe 0xE06D7363`，让程序在 throw 的瞬间停下，再看调用栈。

---

## 11. TTD 定位延迟崩溃

### 11.1 适用场景

- 某块内存被提前破坏，但很久之后才崩
- 竞态条件偶现
- 状态变量在某次操作后变坏
- 无法通过日志精确复现

### 11.2 典型流程

1. 用 WinDbg Preview 录制 TTD
2. 运行到崩溃
3. 找到被破坏的地址
4. 对该地址设置写入断点
5. 反向执行

```
!analyze -v
.ecxr
r
# 假设崩溃访问了地址 000001ab`12340030
ba w 4 000001ab`12340030
g-
```

反向执行停下后：

```
kP
u @rip-20 L30
```

这时看到的是最后一次写坏该地址的位置，通常就是真正元凶。

---

## 12. 实战命令组合

### 12.1 Crash Dump 快速定位

```
.symfix; .reload
!analyze -v
.ecxr
kP
r
u @rip-20 L40
!address <fault_addr>
```

### 12.2 堆损坏

```
!analyze -v
.ecxr
kP
!heap -s
!heap -v
!heap -p -a <addr>
db <addr>-40 L100
```

复现阶段：

```
gflags /p /enable MyApp.exe /full
windbg MyApp.exe
sxe av
g
```

### 12.3 死锁 / 卡死

```
~* k
!locks
!cs -l
!runaway
!handle 0 f
```

### 12.4 无符号崩溃

```
lm vm MyApp
.ecxr
r
u @rip-40 L80
ub @rip
dps @rsp L100
!address @rip
```

### 12.5 栈损坏

```
.ecxr
k
!teb
dps @rsp L200
lm
!address <疑似返回地址>
```

---

## 13. 面试常见问题

### Q1: `!analyze -v` 不准怎么办？

`!analyze -v` 是入口，不是结论。它可能误判，尤其是栈损坏、堆损坏、无符号、优化编译场景。应继续：

```
.ecxr
kP
r
u @rip-20 L40
!address <访问地址>
```

通过指令、寄存器、地址属性来人工验证。

### Q2: 如何定位 A 写坏内存但 B 崩溃？

如果能复现，启用 Page Heap：

```
gflags /p /enable MyApp.exe /full
```

让越界写当场崩溃。如果不能复现，分析 dump 中被破坏区域，结合 `!heap -p -a`、内存内容、业务数据特征缩小范围。若可录制，用 TTD + 数据断点反向执行找到最后写入者。

### Q3: 没有 PDB 如何定位崩溃函数？

用 `lm vm` 得到模块基址，崩溃地址减基址得到 RVA，再用同版本二进制、MAP、反汇编工具定位。没有 MAP 时，用 `u/ub` 看指令、`dps @rsp` 扫返回地址、搜索字符串和导入 API 辅助判断。

### Q4: 如何区分空指针和野指针？

- 访问地址接近 0（如 `0x0`, `0x8`, `0x20`）通常是空指针 + 成员偏移
- 访问地址是看似随机的高地址，需用 `!address` 判断
- 如果地址属于 freed heap，可能是 UAF
- 如果地址是 `0xdddddddd`、`0xfeeefeee`、`0xcccccccc`，通常是调试填充值

### Q5: 如何定位内存泄漏？

用户态 native 程序常用：
- UMDH 做堆快照差异
- Application Verifier + stack trace database
- WinDbg `!heap -s`, `!heap -stat`, `!heap -flt s <size>`
- 业务对象加分配日志或引用计数日志

### Q6: Full dump 和 Mini dump 的区别？

Mini dump 通常只有线程栈、模块列表、异常信息；Full dump 包含完整进程内存。堆损坏、UAF、对象状态分析、内存泄漏等问题通常需要 full dump，否则很多地址内容不可读。

---

## 14. 总结

| 场景 | 核心命令/工具 | 关键思想 |
|------|--------------|----------|
| 普通崩溃 | `!analyze -v`, `.ecxr`, `kP` | 先回到异常上下文 |
| 空指针 | `r`, `u`, `dv` | 看寄存器和访问偏移 |
| UAF | `!address`, `!heap -p -a` | 判断地址是否已释放 |
| 堆损坏 | Page Heap, `!heap -v` | 让延迟崩溃变成立即崩溃 |
| A 写坏 B 崩 | Page Heap / TTD / `ba w` | 找最后一次写入者 |
| 无符号 | `lm vm`, `u`, `dps` | 用 RVA 和反汇编还原路径 |
| 栈损坏 | `!teb`, `dps @rsp` | 手动扫描返回地址 |
| 死锁 | `~*k`, `!locks`, `!cs -l` | 找等待链和锁拥有者 |
| 句柄泄漏 | `!handle`, `!htrace` | 找创建未关闭的调用栈 |
