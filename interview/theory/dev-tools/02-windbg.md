# WinDbg 命令体系

## 概述

WinDbg 是 Windows 平台最强大的调试器，支持用户态和内核态调试、Dump 分析、远程调试。面试中考察 WinDbg 通常从命令熟练度入手，进而考察对 Windows 内存模型、异常机制、进程结构的理解深度。

---

## 1. 启动与连接

### 附加进程 / 打开 Dump

```
# 附加到运行中的进程
windbg -p <pid>
windbg -pn <进程名.exe>

# 打开 Dump 文件
windbg -z crash.dmp

# 启动并调试程序
windbg app.exe arg1 arg2

# 远程调试（服务端）
windbg -server tcp:port=5000 -p <pid>
# 远程调试（客户端）
windbg -remote tcp:server=192.168.1.100,port=5000
```

### 符号配置

```
# 设置符号路径（本地 + 微软符号服务器）
.sympath srv*C:\Symbols*https://msdl.microsoft.com/download/symbols
.sympath+ D:\MyProject\bin\Release  # 追加路径

# 重新加载符号
.reload
.reload /f MyApp.exe    # 强制加载指定模块符号

# 验证符号是否正确加载
lm vm MyApp            # 查看模块详细信息（含 PDB 路径）
!sym noisy             # 开启符号加载诊断输出
.symopt+0x40           # SYMOPT_LOAD_LINES 加载行号信息
```

### 源码配置

```
.srcpath D:\MyProject\src
.srcpath+ D:\ThirdParty\src
```

---

## 2. 基础命令

### 模块信息

```
lm                     # 列出所有已加载模块
lm vm MyApp           # 模块详情（基址、大小、PDB 状态）
lm f                  # 显示完整路径
!lmi MyApp.dll        # 模块更详细信息（时间戳、校验和）

# 查找模块中的符号
x MyApp!*Main*        # 通配符搜索函数名
x MyApp!CMyClass::*   # 列出类的所有方法
```

### 线程

```
~                     # 列出所有线程
~*                    # 所有线程简要信息
~0s                   # 切换到 0 号线程
~* k                  # 显示所有线程调用栈
~* e !clrstack        # 对每个线程执行命令

!runaway              # 线程运行时间排行（定位 CPU 高占用）
!runaway 7            # 显示用户态+内核态+已过时间
```

### 调用栈

```
k                     # 当前线程调用栈
kn                    # 带帧编号
kv                    # 带参数和 FPO 信息
kP                    # 带完整参数类型
kb                    # 带前三个参数（x86）
kf                    # 带每帧的栈空间大小

.frame 3              # 切换到第 3 帧
dv                    # 显示当前帧的局部变量

# 指定栈范围手动解析
dps esp esp+100       # x86：扫描栈上的可能返回地址
dps rsp rsp+100       # x64
```

### 寄存器

```
r                     # 所有寄存器
r eax                 # 单个寄存器
r eax=0              # 修改寄存器值
r $ip                # 指令指针（跨平台写法）
```

### 内存查看

```
# 按格式查看内存
dd <addr>             # DWORD（4字节）
dq <addr>             # QWORD（8字节）
db <addr>             # 字节 + ASCII
da <addr>             # ASCII 字符串
du <addr>             # Unicode 字符串
dps <addr>            # 指针 + 符号解析（最常用之一）

# 指定长度
dd <addr> L10         # 显示 16 个 DWORD
db <addr> L100        # 显示 256 字节

# 内存搜索
s -a 0 L?80000000 "error"       # 全地址空间搜索 ASCII
s -u 0 L?80000000 "错误"        # 搜索 Unicode
s -d 0 L?80000000 0xdeadbeef    # 搜索 DWORD 值
```

### 内存属性

```
!address <addr>       # 查看地址属于哪个区域（堆/栈/模块/保留）
!address -summary     # 内存使用汇总
!vprot <addr>         # 页面保护属性
```

---

## 3. 断点

### 普通断点

```
bp MyApp!main                    # 函数入口断点
bp MyApp!CMyClass::OnClick       # 成员函数
bp `source.cpp:42`               # 源码行号（需要符号+源码）
bp kernel32!CreateFileW          # 系统 API

bu MyApp!CParser::Parse          # 延迟断点（模块尚未加载时设置）
bm MyApp!CMyClass::*             # 通配符批量断点
```

### 条件断点

```
# 条件满足时才中断
bp MyApp!ProcessItem ".if (poi(esp+4) == 0x42) {} .else {gc}"
# poi = pointer dereference, gc = go continue

# x64 等价写法（参数在寄存器）
bp MyApp!ProcessItem ".if (@rcx == 0x42) {} .else {gc}"

# 命中 N 次后中断
bp MyApp!WorkerFunc 5            # 第 5 次命中时中断

# 断点时执行命令（不中断）
bp MyApp!Alloc "kb; gc"          # 每次命中打印调用栈然后继续
```

### 数据断点（硬件断点）

```
# ba = Break on Access
ba r 4 <addr>         # 读取 4 字节时中断
ba w 4 <addr>         # 写入 4 字节时中断
ba e 1 <addr>         # 执行时中断

# 典型用法：监控变量被谁修改
0:000> dt MyApp!g_pInstance
       +0x000 m_value : 42
0:000> ba w 4 MyApp!g_pInstance+0x000
# 下次 m_value 被写入时中断 → 调用栈显示修改者
```

### 断点管理

```
bl                    # 列出所有断点
bd 0                  # 禁用 0 号断点
be 0                  # 启用 0 号断点
bc 0                  # 删除 0 号断点
bc *                  # 删除所有断点
```

---

## 4. 执行控制

```
g                     # 继续执行 (Go)
p                     # 单步步过 (Step Over)
t                     # 单步步入 (Step Into)
gu                    # 执行到当前函数返回 (Go Up)
pc                    # 执行到下一个 call 指令
tt                    # 执行到下一个 ret 指令

# 执行到指定地址
g <addr>

# 带条件继续
gc                    # 条件断点中使用，继续执行
```

---

## 5. 类型与结构体

### 显示类型信息

```
dt ntdll!_PEB                         # 显示结构体定义
dt ntdll!_PEB @$peb                   # 显示实例内容
dt ntdll!_TEB @$teb                   # 当前线程 TEB
dt MyApp!CMyClass <addr>              # 查看对象实例

dt -r ntdll!_PEB @$peb               # 递归展开子结构
dt -r2 ntdll!_HEAP <addr>            # 递归 2 层
```

### 局部变量

```
dv                    # 当前帧所有局部变量
dv /t                 # 带类型信息
dv /V                 # 带地址
dv this               # 查看 this 指针内容
```

---

## 6. 反汇编

```
u <addr>              # 反汇编（从指定地址开始）
u <addr> L20          # 反汇编 20 条指令
uf MyApp!CParser::Parse   # 反汇编整个函数
ub <addr>             # 向前反汇编（找崩溃前的指令）

# 混合源码与汇编
.lines                # 开启行号显示
lsf source.cpp        # 加载源文件
ls 40                 # 显示第 40 行附近源码
```

---

## 7. 异常与事件

### 异常处理控制

```
# 设置异常处理方式
sxe av               # Access Violation 时第一次中断（first-chance）
sxd av               # Access Violation 只在二次时中断（second-chance）
sxn av               # 通知但不中断
sxi av               # 忽略

# 常用异常类型
sxe 0xC0000005       # Access Violation
sxe 0xC00000FD       # Stack Overflow
sxe 0xC0000094       # Integer Divide by Zero
sxe 0xE06D7363       # C++ Exception (throw)
sxe 0xC0000374       # Heap Corruption

# DLL 加载/卸载事件
sxe ld:MyPlugin.dll  # MyPlugin.dll 加载时中断
sxe ud:MyPlugin.dll  # 卸载时中断
```

### 异常记录

```
.exr -1              # 最近一次异常记录
.ecxr               # 切换到异常上下文（崩溃时的寄存器/栈）
!exchain            # 异常处理链（SEH chain）

# 标准崩溃分析流程
.ecxr              # 先切换到异常上下文
k                  # 再看调用栈 → 这才是崩溃时的真实栈
```

---

## 8. 堆分析

### 堆基本命令

```
!heap -s              # 堆摘要（所有堆的大小/提交/保留）
!heap -stat           # 堆统计
!heap -h <heap_addr>  # 指定堆详情
!heap -flt s <size>   # 过滤：查找指定大小的分配

# 验证堆完整性
!heap -v              # 验证所有堆（慢）
!heap -v <heap_addr>  # 验证指定堆
```

### Page Heap（全页堆）

```
# 启用 Page Heap（在命令行，非 WinDbg 内）
gflags /p /enable MyApp.exe /full

# 启用后的效果：
# - 每次分配在尾部放置不可访问页
# - 释放后整个页标记为不可访问
# - 越界写入/释放后访问 → 立即 AV（而非延迟崩溃）

# 查看 Page Heap 分配信息
!heap -p -a <addr>    # 查看地址的分配/释放调用栈

# 关闭 Page Heap
gflags /p /disable MyApp.exe
```

---

## 9. 句柄与同步对象

```
!handle               # 所有句柄统计
!handle 0 f          # 所有句柄详细信息
!handle <handle> f   # 指定句柄详情

# Handle 跟踪（需要 Application Verifier 启用）
!htrace -enable      # 开启句柄跟踪
!htrace <handle>     # 查看句柄的创建/关闭调用栈

# 锁与同步
!locks               # 所有 Critical Section 信息
!cs <addr>           # 指定 Critical Section 详情
!cs -l               # 列出所有被锁定的 CS（定位死锁）
```

---

## 10. 进程与环境信息

```
!peb                  # 进程环境块（命令行、环境变量、加载路径）
!teb                  # 当前线程环境块（栈范围、TLS）
!dlls                 # 加载的 DLL 列表（带初始化顺序）

# 进程内存布局
!address -summary     # 内存类型分布
!vadump              # 虚拟地址空间完整 dump
```

---

## 11. 自动分析

```
!analyze -v           # 自动分析崩溃原因（最常用命令之一）
                      # 输出包含：
                      # - 异常类型和地址
                      # - 可能的原因
                      # - 故障模块
                      # - 崩溃调用栈
                      # - 关键寄存器

!analyze -hang       # 分析挂起/死锁
```

---

## 12. 脚本与批处理

### 命令组合

```
# 分号分隔多条命令
.ecxr; k; dv

# 条件执行
j (条件) '真命令' ; '假命令'
j (@eax == 0) 'r eax; k' ; 'gc'

# 循环：对每个匹配项执行命令
.foreach (addr { s -[1]d 0 L?80000000 0xBAADF00D }) { !heap -p -a ${addr} }

# 日志输出
.logopen C:\debug.log     # 开始记录
.logclose                  # 停止记录
```

### 常用伪寄存器

```
@$ip          # 指令指针（EIP/RIP）
@$sp          # 栈指针（ESP/RSP）
@$teb         # TEB 地址
@$peb         # PEB 地址
@$proc        # 当前进程 EPROCESS
@$thread      # 当前线程 ETHREAD
```

---

## 13. Time Travel Debugging (TTD)

### 概念

TTD 记录程序执行的每一条指令，允许事后任意回放和回溯。对间歇性 bug、竞态条件、延迟崩溃的定位有革命性帮助。

### 基本操作

```
# 录制（在 WinDbg Preview 中）
# File → Attach to process → 勾选 "Record with Time Travel Debugging"
# 或命令行：
tttracer.exe -attach <pid> -out C:\traces

# 回放
!tt 0                 # 跳到起始位置
!tt 100               # 跳到结束位置（百分比）
!tt 50                # 跳到中间
g-                    # 反向执行（回退）
p-                    # 反向单步
t-                    # 反向步入

# 数据断点 + 反向执行（杀手级组合）
ba w 4 <被破坏的地址>
g-                    # 反向执行到上次写入该地址的位置
k                     # 看调用栈 → 找到真正的破坏者
```

---

## 14. 命令速查表

| 类别 | 命令 | 说明 |
|------|------|------|
| 模块 | `lm`, `x` | 列出模块、搜索符号 |
| 线程 | `~`, `~*k`, `!runaway` | 线程列表、全栈、CPU 时间 |
| 调用栈 | `k`, `kv`, `kP`, `.frame` | 栈回溯、切换帧 |
| 内存 | `dd/dq/db/da/du`, `dps` | 按格式查看内存 |
| 搜索 | `s -a/u/d` | 搜索内存内容 |
| 断点 | `bp/bu/bm`, `ba` | 软件断点、数据断点 |
| 类型 | `dt`, `dv` | 结构体、局部变量 |
| 异常 | `.ecxr`, `!exchain`, `sxe` | 异常上下文、处理链 |
| 堆 | `!heap`, `!heap -p -a` | 堆信息、Page Heap 查询 |
| 分析 | `!analyze -v` | 自动崩溃分析 |
| 控制 | `g/p/t/gu`, `g-/p-` | 执行、单步、反向 |
| 环境 | `!peb`, `!teb`, `!address` | 进程/线程环境、内存布局 |
| TTD | `!tt`, `g-`, `ba w + g-` | 时间旅行调试 |

---

## 15. 总结

| 层级 | 掌握内容 |
|------|---------|
| **入门** | 打开 dump、配符号、`!analyze -v`、`k` 看栈 |
| **基础** | 内存查看、断点设置、线程切换、`.ecxr` |
| **进阶** | 条件断点、`!heap`、数据断点 `ba`、脚本 `.foreach` |
| **高级** | Page Heap 堆损坏定位、TTD 反向调试、无符号反汇编分析 |
