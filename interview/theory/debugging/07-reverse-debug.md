# 逆向调试 (Reverse Debugging)

逆向调试是在缺少源码或符号时分析程序的高级技能。

---

## 1. 逆向调试基础

### 1.1 什么是逆向调试

```
┌─────────────────────────────────────────────────────────────┐
│                    逆向调试场景                              │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  1. 无符号调试                                               │
│     - Release 版本无 PDB/dSYM                               │
│     - 第三方库崩溃                                          │
│     - 线上 dump 缺少符号                                    │
│                                                              │
│  2. 无源码调试                                               │
│     - 分析第三方库行为                                       │
│     - 逆向工程                                               │
│     - 安全分析                                               │
│                                                             │
│  3. 二进制分析                                               │
│     - 优化后代码分析                                         │
│     - 编译器行为理解                                         │
│     - 底层问题定位                                           │
│                                                             │
│  核心技能：                                                  │
│  • 汇编语言阅读                                              │
│  • 调用约定理解                                              │
│  • 内存布局分析                                              │
│  • 数据结构推断                                              │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

### 1.2 工具链

```
┌─────────────────────────────────────────────────────────────┐
│                    逆向调试工具链                            │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  反汇编器/反编译器                                          │
│  ├── IDA Pro: 业界标准，强大的反汇编和反编译                │
│  ├── Ghidra: NSA 开源，免费                                │
│  ├── Binary Ninja: 现代化界面                              │
│  └── Hopper: macOS 友好                                    │
│                                                             │
│  调试器                                                     │
│  ├── WinDbg: Windows 内核级调试                            │
│  ├── x64dbg: Windows 用户态，开源                          │
│  ├── GDB: Linux/Unix                                       │
│  └── LLDB: macOS/iOS                                       │
│                                                             │
│  辅助工具                                                   │
│  ├── objdump: GNU 反汇编                                   │
│  ├── dumpbin: MSVC 工具                                    │
│  ├── otool: macOS 工具                                     │
│  └── readelf: ELF 分析                                     │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

---

## 2. x86-64 汇编基础

### 2.1 寄存器

```
┌─────────────────────────────────────────────────────────────┐
│                    x86-64 寄存器                             │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  通用寄存器（64位）：                                        │
│  RAX, RBX, RCX, RDX, RSI, RDI, RSP, RBP, R8-R15            │
│                                                             │
│  RAX: 返回值                                                │
│  RBX: 被调用者保存（callee-saved）                          │
│  RCX: 第 1 参数 (Windows) / 第 4 参数 (Linux)              │
│  RDX: 第 2 参数 (Windows) / 第 3 参数 (Linux)              │
│  RSI: 第 2 参数 (Linux)                                    │
│  RDI: 第 1 参数 (Linux)                                    │
│  RSP: 栈指针                                                │
│  RBP: 基址指针（可选）                                      │
│  R8:  第 3 参数 (Windows) / 第 5 参数 (Linux)              │
│  R9:  第 4 参数 (Windows) / 第 6 参数 (Linux)              │
│                                                             │
│  特殊寄存器：                                                │
│  RIP: 指令指针（当前执行地址）                               │
│  RFLAGS: 标志寄存器（ZF, CF, SF 等）                        │
│                                                             │
│  寄存器部分访问：                                            │
│  RAX (64) → EAX (32) → AX (16) → AH/AL (8)                 │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

### 2.2 常用指令

```
┌─────────────────────────────────────────────────────────────┐
│                    常用 x86-64 指令                          │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  数据传输                                                   │
│  MOV dst, src      dst = src                               │
│  LEA dst, [addr]   dst = &addr（计算地址）                  │
│  PUSH src          压栈                                     │
│  POP dst           出栈                                     │
│                                                             │
│  算术运算                                                   │
│  ADD dst, src      dst += src                              │
│  SUB dst, src      dst -= src                              │
│  IMUL dst, src     dst *= src（有符号）                    │
│  INC dst           dst++                                   │
│  DEC dst           dst--                                   │
│                                                             │
│  逻辑运算                                                   │
│  AND dst, src      dst &= src                              │
│  OR dst, src       dst |= src                              │
│  XOR dst, src      dst ^= src                              │
│  NOT dst           dst = ~dst                              │
│  SHL dst, n        dst <<= n                               │
│  SHR dst, n        dst >>= n（逻辑右移）                    │
│  SAR dst, n        dst >>= n（算术右移）                    │
│                                                             │
│  比较和跳转                                                  │
│  CMP a, b          设置标志 (a - b)                        │
│  TEST a, b         设置标志 (a & b)                        │
│  JMP addr          无条件跳转                               │
│  JE/JZ addr        相等/零时跳转                            │
│  JNE/JNZ addr      不等/非零时跳转                          │
│  JG/JL addr        大于/小于时跳转（有符号）                 │
│  JA/JB addr        大于/小于时跳转（无符号）                 │
│                                                             │
│  函数调用                                                   │
│  CALL addr         调用函数（push RIP; jmp addr）          │
│  RET               返回（pop RIP）                         │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

### 2.3 调用约定

```
┌─────────────────────────────────────────────────────────────┐
│                    x86-64 调用约定                           │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  System V AMD64 (Linux/macOS)                               │
│  ─────────────────────────────────────────────────────────  │
│  参数传递: RDI, RSI, RDX, RCX, R8, R9（整数/指针）          │
│           XMM0-7（浮点）                                    │
│           更多参数通过栈                                     │
│  返回值:   RAX (整数), XMM0 (浮点)                         │
│  保存:     RBX, RBP, R12-R15 由被调用者保存                │
│  栈对齐:   16 字节                                          │
│                                                             │
│  Microsoft x64 (Windows)                                    │
│  ─────────────────────────────────────────────────────────  │
│  参数传递: RCX, RDX, R8, R9（前 4 个参数）                 │
│           更多参数通过栈                                     │
│  返回值:   RAX                                              │
│  保存:     RBX, RBP, RDI, RSI, R12-R15 由被调用者保存      │
│  栈对齐:   16 字节                                          │
│  Shadow:   调用前预留 32 字节影子空间                       │
│                                                             │
│  示例：func(a, b, c, d)                                     │
│                                                             │
│  Linux:   RDI=a, RSI=b, RDX=c, RCX=d                       │
│  Windows: RCX=a, RDX=b, R8=c, R9=d                         │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

---

## 3. 无符号调试技巧

### 3.1 定位崩溃函数

```
// WinDbg 无符号分析

// 1. 查看崩溃地址
.ecxr
r

// 2. 查看崩溃地址属于哪个模块
lm a <address>
// 或
!address <address>

// 3. 计算模块内偏移
? <crash_address> - <module_base>

// 4. 找最近的导出函数
ln <address>

// 5. 反汇编崩溃位置
u <address>
u <address> L20    // 反汇编 20 条指令

// 6. 使用相同版本的带符号程序分析
// 崩溃偏移 + 带符号程序 = 定位函数

// 示例：
// 崩溃地址: 0x00007ff6abc01234
// 模块基址: 0x00007ff6abc00000
// 偏移:     0x1234
// 在带符号版本中: ln MyApp+0x1234
```

### 3.2 分析调用栈

```
// 手动栈回溯

// 1. 查看栈内容
dps @rsp L30    // 显示 30 个指针和符号

// 2. 识别返回地址
// 返回地址通常指向 CALL 指令之后
// 查看地址是否在代码段

// 3. 分析栈帧
// 标准栈帧：
// [RBP]     保存的 RBP
// [RBP+8]   返回地址
// [RBP+16]  第一个栈参数（如果有）

// 4. 无帧指针时
// 使用 .frame /r 或手动分析
// 查找 CALL 后的地址模式

// GDB/LLDB 无符号回溯
// (gdb) x/20a $rsp    // 查看栈上的地址
// (gdb) info symbol <address>  // 查看地址对应符号
```

### 3.3 内存模式识别

```
┌─────────────────────────────────────────────────────────────┐
│                    内存模式识别                              │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  常见内存模式：                                              │
│                                                             │
│  0x00000000           空指针                                │
│  0xCDCDCDCD           未初始化堆内存 (MSVC Debug)          │
│  0xDDDDDDDD           已释放堆内存 (MSVC Debug)            │
│  0xFDFDFDFD           堆边界标记                           │
│  0xCCCCCCCC           未初始化栈内存 (MSVC Debug)          │
│  0xDEADBEEF           常见魔数                              │
│  0xBAADF00D           坏食物（bad food）                    │
│  0xFEEEFEEE           已释放堆（HeapFree）                 │
│                                                             │
│  vtable 指针识别：                                           │
│  - 对象第一个成员                                           │
│  - 指向代码段的指针                                         │
│  - 通常对齐到 8 字节                                        │
│                                                             │
│  字符串识别：                                                │
│  - ASCII: 可见字符 0x20-0x7E                               │
│  - UTF-16: 每两字节一个字符                                 │
│  - 以 0 结尾                                                │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

---

## 4. Release 版本调试

### 4.1 优化对调试的影响

```
┌─────────────────────────────────────────────────────────────┐
│                   编译优化影响                               │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  1. 内联展开 (Inlining)                                     │
│     小函数被内联到调用点                                     │
│     调用栈不完整                                             │
│                                                             │
│  2. 指令重排序                                               │
│     源码行号与执行顺序不对应                                 │
│                                                             │
│  3. 变量优化                                                 │
│     变量可能只存在于寄存器                                   │
│     变量可能被消除                                           │
│                                                             │
│  4. 尾调用优化 (Tail Call)                                  │
│     函数调用变为跳转                                         │
│     调用栈缺少帧                                             │
│                                                             │
│  5. 循环优化                                                 │
│     循环展开、向量化                                         │
│     难以对应源码                                             │
│                                                             │
│  6. 帧指针省略 (FPO)                                         │
│     不使用 RBP 作为帧指针                                   │
│     栈回溯困难                                               │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

### 4.2 保留调试信息

```bash
# GCC/Clang: 保留帧指针
g++ -O2 -g -fno-omit-frame-pointer main.cpp

# 保留调试信息
g++ -O2 -g main.cpp              # 基本调试信息
g++ -O2 -g3 main.cpp             # 包含宏定义
g++ -O2 -g -gsplit-dwarf main.cpp  # 分离调试信息

# MSVC: 生成 PDB
cl /O2 /Zi /DEBUG main.cpp

# 链接时保留调试信息
link /DEBUG /OPT:REF main.obj
# /DEBUG: 生成 PDB
# /OPT:REF: 删除未引用代码（Release 需要）

# Xcode: 保留 dSYM
# Archive 时自动生成
# 或手动：dsymutil MyApp
```

### 4.3 Release 调试技巧

```cpp
// 1. 关键位置日志
#define LOG_FUNCTION() \
    fprintf(stderr, "%s:%d %s\n", __FILE__, __LINE__, __FUNCTION__)

void critical_function() {
    LOG_FUNCTION();
    // ...
}

// 2. 崩溃前保存状态
void save_debug_state() {
    FILE* f = fopen("/tmp/debug_state.txt", "w");
    fprintf(f, "Last operation: %s\n", last_operation);
    fprintf(f, "State: %d\n", current_state);
    fclose(f);
}

// 3. 条件触发断点
volatile bool g_break_here = false;

void check_break() {
    if (g_break_here) {
        __debugbreak();  // Windows
        // raise(SIGTRAP);  // Unix
    }
}

// 4. 保留符号
#ifdef _MSC_VER
#pragma optimize("", off)  // 禁用优化
#endif

void debug_this_function() {
    // 不会被优化
}

#ifdef _MSC_VER
#pragma optimize("", on)
#endif
```

---

## 5. 反汇编分析

### 5.1 函数识别

```
// 函数入口特征

// x86-64 Linux/macOS
push rbp
mov rbp, rsp
sub rsp, <size>

// x86-64 Windows
sub rsp, <size>      // 分配栈空间（含 shadow space）
mov [rsp+...], ...   // 保存参数到影子空间

// 无帧指针优化
sub rsp, <size>      // 直接分配

// 函数出口特征
leave                 // mov rsp, rbp; pop rbp
ret

// 或无帧指针
add rsp, <size>
ret
```

### 5.2 控制流分析

```
// if-else 结构
cmp eax, 10
jle else_branch      // if (eax > 10) 跳转到 else
; then branch
jmp end_if
else_branch:
; else branch
end_if:

// for 循环
xor ecx, ecx         // i = 0
loop_start:
cmp ecx, 10          // i < 10
jge loop_end
; loop body
inc ecx              // i++
jmp loop_start
loop_end:

// while 循环
loop_start:
test eax, eax        // while (eax != 0)
je loop_end
; loop body
jmp loop_start
loop_end:

// switch-case（跳转表）
cmp eax, 4           // 检查范围
ja default_case
jmp [jump_table + rax*8]  // 跳转表
```

### 5.3 数据结构推断

```cpp
// 通过访问模式推断结构体

// 汇编代码
mov rax, [rdi]           // 偏移 0
mov rcx, [rdi+8]         // 偏移 8
mov edx, [rdi+16]        // 偏移 16
movzx r8d, byte [rdi+20] // 偏移 20

// 推断结构体
struct Unknown {
    void* ptr;     // 偏移 0,  大小 8
    void* ptr2;    // 偏移 8,  大小 8
    int value;     // 偏移 16, 大小 4
    char flag;     // 偏移 20, 大小 1
    // 可能有 3 字节填充
};

// vtable 识别
mov rax, [rdi]           // 获取 vtable 指针
call [rax+16]            // 调用第 3 个虚函数（偏移 16 / 8 = 2）

// 数组访问
mov eax, [rbx+rcx*4]     // int arr[]; arr[rcx]
mov rax, [rbx+rcx*8]     // void* arr[]; arr[rcx]
```

---

## 6. IDA Pro / Ghidra 使用

### 6.1 IDA Pro 基础

```
常用快捷键：

导航：
G       跳转到地址
X       查看交叉引用
ESC     返回
Ctrl+P  跳转到函数

分析：
N       重命名
;       添加注释
Y       修改类型
T       指定结构体偏移
C       转为代码
D       转为数据
U       取消定义

视图：
Space   图形/文本视图切换
F5      反编译（需要 Hex-Rays）

函数：
P       创建函数
Alt+P   编辑函数
```

### 6.2 Ghidra 基础

```
Ghidra 使用步骤：

1. 创建项目
   File → New Project → Non-Shared Project

2. 导入二进制
   File → Import File
   选择可执行文件

3. 分析
   自动分析（Auto Analyze）
   或 Analysis → Auto Analyze

4. 常用视图
   - Listing: 反汇编视图
   - Decompile: 反编译视图（C 伪代码）
   - Function Graph: 控制流图
   - Symbol Tree: 符号列表

5. 常用快捷键
   G: 跳转
   L: 重命名
   ;: 注释
   Ctrl+Shift+E: 查看引用

6. 脚本
   支持 Python/Java 脚本
   Window → Script Manager
```

### 6.3 分析实例

```
// 分析未知函数

// 1. 看函数入口
sub_401000:
    push rbp
    mov rbp, rsp
    sub rsp, 0x40

// 2. 分析参数使用
    mov [rbp-8], rdi     // 第一个参数保存
    mov [rbp-16], rsi    // 第二个参数保存

// 3. 分析函数调用
    mov rdi, [rbp-8]
    call strlen          // 调用 strlen

// 4. 分析返回值
    mov rax, [rbp-8]
    add rsp, 0x40
    pop rbp
    ret                  // 返回第一个参数

// 推断：这是一个处理字符串的函数
// 可能的原型：char* func(char* str, int len);
```

---

## 7. 反向调试 (Time-Travel Debugging)

### 7.1 rr (Record and Replay)

```bash
# 安装 (Linux)
sudo apt install rr

# 或从源码编译
git clone https://github.com/rr-debugger/rr.git
cd rr && mkdir build && cd build
cmake .. && make -j
sudo make install

# 录制
rr record ./MyApp

# 回放
rr replay

# 在 GDB 中反向执行
(rr) reverse-continue       # 反向继续
(rr) reverse-step           # 反向单步（进入函数）
(rr) reverse-next           # 反向单步（跳过函数）
(rr) reverse-finish         # 反向运行到函数入口

# 设置断点后反向运行
(rr) break some_function
(rr) reverse-continue       # 反向运行到断点

# 查看执行历史
(rr) when                   # 当前事件编号
(rr) run <event>            # 跳转到特定事件
```

### 7.2 WinDbg Time Travel Debugging

```
// Windows TTD (Time Travel Debugging)

// 1. 启动 TTD 录制
// WinDbg Preview → File → Start debugging → Launch executable (advanced)
// 勾选 "Record with Time Travel Debugging"

// 或命令行
tttracer -launch MyApp.exe -out trace.run

// 2. 回放
// WinDbg Preview 打开 .run 文件

// 3. 时间旅行命令
!tt 0            // 跳转到开始
!tt 100          // 跳转到 100% 位置
!tt 50:100       // 跳转到特定时间点

g-               // 反向执行
p-               // 反向步过
t-               // 反向步入

// 4. 查询历史
dx @$cursession.TTD.Calls("kernel32!CreateFileW")
// 查看所有 CreateFileW 调用

dx @$cursession.TTD.Memory(0x12345678, 0x12345680, "w")
// 查看内存写入历史
```

### 7.3 反向调试应用场景

```
场景：

1. 崩溃原因追溯
   - 从崩溃点反向执行
   - 找到导致崩溃的源头操作

2. 数据损坏分析
   - 发现损坏的数据
   - 反向找到谁修改了它

3. 竞态条件分析
   - 录制多次执行
   - 对比不同执行路径

4. 理解复杂逻辑
   - 正向反向交替执行
   - 理解程序状态变化

示例：分析 use-after-free

(rr) watch -l *0x12345678   // 监视内存
(rr) continue               // 运行到崩溃
(rr) reverse-continue       // 反向到释放点
// 找到 free() 调用
(rr) reverse-continue       // 继续反向到分配点
// 找到 malloc() 调用
// 分析生命周期
```

---

## 8. 面试深度问题

### Q1: 如何分析无符号的 dump 文件？

**答案**：

```
步骤：

1. 确定崩溃模块和偏移
   lm (查看模块)
   ? <crash_addr> - <module_base>

2. 反汇编崩溃位置
   u <address>
   分析崩溃指令

3. 分析寄存器和内存
   r (查看寄存器)
   dps @rsp (查看栈)
   识别内存模式

4. 手动栈回溯
   dps @rsp L50
   识别返回地址

5. 符号猜测
   - 导出函数最近的
   - 特征字符串
   - 调用模式

6. 关联带符号版本
   - 相同版本的调试版
   - 用偏移定位函数
```

### Q2: 如何识别虚函数调用？

**答案**：

```asm
; 虚函数调用模式

; 1. 获取 vtable 指针（对象第一个成员）
mov rax, [rcx]          ; rcx = this 指针，[rcx] = vtable

; 2. 从 vtable 获取函数地址
mov rax, [rax+0x10]     ; 偏移 0x10 = 第 3 个虚函数（0, 8, 16）

; 3. 调用
call rax

; 或一步完成
call qword ptr [rax+0x10]

; 识别方法：
; - 通过对象指针间接获取函数地址
; - 固定偏移访问 vtable
; - call 的目标是运行时确定的

; 虚析构函数通常是 vtable 第一个
call qword ptr [rax]    ; 可能是虚析构
```

### Q3: 如何判断调用约定？

**答案**：

```
判断方法：

1. 查看参数传递
   Linux/macOS: RDI, RSI, RDX, RCX, R8, R9
   Windows:     RCX, RDX, R8, R9

2. 查看栈清理
   cdecl:    调用者清理 (add rsp, N 在 call 之后)
   stdcall:  被调用者清理 (ret N)
   fastcall: 寄存器传参 + 被调用者清理

3. 查看影子空间
   Windows x64: call 前 sub rsp, 0x20 (32 字节)

4. 查看被保存寄存器
   函数入口保存哪些寄存器

示例：
; Windows x64
sub rsp, 28h          ; 影子空间 + 对齐
mov rcx, <arg1>       ; 第一个参数
mov rdx, <arg2>       ; 第二个参数
call func
add rsp, 28h
; Windows 特征：RCX/RDX 传参，32 字节影子空间
```

### Q4: 如何分析优化后的代码？

**答案**：

```
技巧：

1. 识别内联函数
   - 没有 call 指令
   - 代码模式重复出现
   - 使用调试版对比

2. 识别尾调用优化
   - jmp 代替 call+ret
   - 调用栈看起来缺少帧

3. 分析向量化循环
   - SIMD 指令 (movaps, addps 等)
   - 一次处理多个元素

4. 使用编译器输出
   gcc -S -O2 main.c  # 查看优化后汇编
   对比分析

5. 借助反编译器
   IDA/Ghidra 的反编译结果
   虽然可能不准确但有参考价值

6. 关注数据流
   跟踪变量在寄存器间的移动
   不纠结于控制流细节
```

### Q5: rr 的工作原理是什么？

**答案**：

```
rr 原理：

1. 录制阶段
   - 使用 ptrace 拦截系统调用
   - 记录所有非确定性输入：
     * 系统调用返回值
     * 信号
     * 共享内存访问
     * 时间戳

2. 回放阶段
   - 重新执行程序
   - 用录制的数据替代非确定性输入
   - 保证执行路径完全相同

3. 反向执行
   - 设置检查点（checkpoint）
   - 反向 = 重新从最近检查点开始
   - 执行到目标位置之前

4. 限制
   - 单核执行（确保确定性）
   - 不支持共享内存多进程
   - 录制有开销

原理图：
录制: 程序执行 → 拦截非确定性 → 记录日志
回放: 日志 → 提供给程序 → 确定性重放
```

---

## 9. 总结

```
┌─────────────────────────────────────────────────────────────┐
│                   逆向调试知识图谱                           │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  基础技能                                                   │
│  ├── x86-64 汇编阅读                                       │
│  ├── 调用约定理解                                           │
│  ├── 栈帧分析                                               │
│  └── 内存模式识别                                           │
│                                                             │
│  工具链                                                     │
│  ├── 调试器: WinDbg, GDB, LLDB, x64dbg                     │
│  ├── 反汇编: IDA Pro, Ghidra, objdump                      │
│  └── TTD: rr, WinDbg TTD                                   │
│                                                             │
│  无符号调试                                                  │
│  ├── 计算模块偏移                                           │
│  ├── 手动栈回溯                                             │
│  ├── 内存模式识别                                           │
│  └── 导出函数定位                                           │
│                                                             │
│  Release 调试                                               │
│  ├── 保留帧指针                                             │
│  ├── 保留调试信息                                           │
│  └── 关键日志                                               │
│                                                             │
│  面试重点                                                   │
│  ├── 无符号 dump 分析                                       │
│  ├── 虚函数调用识别                                         │
│  ├── 调用约定判断                                           │
│  └── 反向调试原理                                           │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```
