# 野指针排查 (Wild Pointer Debugging)

野指针是 C/C++ 程序最危险的问题之一，可能导致随机崩溃或数据损坏。

---

## 1. 野指针基础概念

### 1.1 什么是野指针

```
┌─────────────────────────────────────────────────────────────┐
│                      野指针定义                              │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  野指针 = 指向无效内存的指针                                 │
│                                                             │
│  三种类型：                                                  │
│                                                             │
│  1. 未初始化指针                                             │
│     int* p;        // p 指向随机地址                        │
│     *p = 42;       // 未定义行为                            │
│                                                             │
│  2. 悬垂指针 (Dangling Pointer)                             │
│     int* p = new int(42);                                   │
│     delete p;      // p 仍指向已释放地址                    │
│     *p = 100;      // use-after-free                        │
│                                                             │
│  3. 越界指针                                                 │
│     int arr[10];                                            │
│     int* p = &arr[10];  // 指向数组外                       │
│     *p = 42;            // 越界写入                         │
│                                                             │
│  危险性：                                                    │
│  • 可能不立即崩溃                                            │
│  • 破坏其他数据                                              │
│  • 难以重现和定位                                            │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

### 1.2 野指针的危害

```
┌─────────────────────────────────────────────────────────────┐
│                     野指针危害场景                           │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  场景一：立即崩溃（幸运情况）                                │
│  ┌─────────────────────────────────────────────────────┐   │
│  │  访问地址 0x00000000 → SIGSEGV                       │   │
│  │  原因：空指针，系统保护页                             │   │
│  │  结果：问题可以立即定位                               │   │
│  └─────────────────────────────────────────────────────┘   │
│                                                             │
│  场景二：静默损坏（最危险）                                  │
│  ┌─────────────────────────────────────────────────────┐   │
│  │  野指针指向有效内存区域                               │   │
│  │  写入覆盖了其他变量/堆元数据                          │   │
│  │  程序继续运行，但数据已损坏                           │   │
│  │  很久之后才崩溃，崩溃点不是问题点                     │   │
│  └─────────────────────────────────────────────────────┘   │
│                                                             │
│  场景三：间歇性问题                                          │
│  ┌─────────────────────────────────────────────────────┐   │
│  │  野指针指向的内存被重新分配                           │   │
│  │  有时是有效数据，有时已覆盖                           │   │
│  │  问题时有时无，难以重现                               │   │
│  └─────────────────────────────────────────────────────┘   │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

### 1.3 常见野指针代码模式

```cpp
// 1. 未初始化指针
void uninitialized_pointer() {
    int* p;          // 未初始化，值随机
    printf("%d\n", *p);  // UB：可能崩溃，可能输出垃圾
}

// 2. Use-After-Free
void use_after_free() {
    int* p = new int(42);
    delete p;
    *p = 100;  // UB：p 仍指向已释放内存
}

// 3. 返回局部变量地址
int* return_local_address() {
    int local = 42;
    return &local;  // 返回栈上地址，函数返回后无效
}

// 4. 多次释放后使用
void double_free_then_use() {
    int* p = new int(42);
    int* q = p;      // p 和 q 指向同一地址
    delete p;
    delete q;        // 双重释放
    *p = 100;        // use-after-free
}

// 5. 容器迭代器失效
void iterator_invalidation() {
    std::vector<int> vec = {1, 2, 3, 4, 5};
    auto it = vec.begin();

    vec.push_back(6);  // 可能触发 realloc，迭代器失效

    *it = 100;  // UB：it 可能已失效
}

// 6. 多线程竞争
std::shared_ptr<int> g_ptr;

void thread_a() {
    auto local = g_ptr;  // 可能读到正在被释放的指针
    *local = 42;
}

void thread_b() {
    g_ptr.reset();  // 释放
}

// 7. 析构后使用 this
class Widget {
public:
    void destroy() {
        delete this;
        member_ = 0;  // UB：this 已无效
    }
private:
    int member_;
};

// 8. 类型转换错误
void wrong_cast() {
    int x = 42;
    double* p = reinterpret_cast<double*>(&x);
    *p = 3.14;  // 越界写入（double 比 int 大）
}
```

---

## 2. 内存填充模式

### 2.1 调试堆内存模式 (MSVC)

```
┌─────────────────────────────────────────────────────────────┐
│                  MSVC Debug 内存填充模式                     │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  模式值      含义                    触发时机               │
│  ─────────────────────────────────────────────────────────  │
│  0xCD        Clean memory           new/malloc 后（堆）    │
│  0xDD        Dead memory            delete/free 后         │
│  0xFD        Fence (No man's land)  分配边界               │
│  0xCC        未初始化栈              函数入口               │
│  0xAB        用户保留                                       │
│                                                             │
│  示例：                                                     │
│  ┌─────────────────────────────────────────────────────┐   │
│  │  FD FD FD FD  │ CD CD CD CD │ FD FD FD FD           │   │
│  │   边界        │   用户数据   │   边界                │   │
│  │   (4 bytes)   │  (N bytes)  │  (4 bytes)            │   │
│  └─────────────────────────────────────────────────────┘   │
│                                                             │
│  检测：                                                     │
│  • 读到 0xCDCDCDCD → 使用未初始化堆内存                    │
│  • 读到 0xDDDDDDDD → use-after-free                        │
│  • 边界 0xFD 被修改 → 越界写入                              │
│  • 读到 0xCCCCCCCC → 使用未初始化栈变量                    │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

### 2.2 其他平台内存模式

```
┌─────────────────────────────────────────────────────────────┐
│                   各平台内存填充模式                         │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  平台/工具          分配后         释放后                    │
│  ─────────────────────────────────────────────────────────  │
│  MSVC Debug        0xCD           0xDD                      │
│  macOS MallocScribble  0xAA       0x55                      │
│  Valgrind          不改变          不改变（但跟踪状态）      │
│  ASan              0xbe           0xbe（redzone）           │
│  jemalloc debug    0xa5           0x5a                      │
│                                                             │
│  macOS 启用 MallocScribble：                                │
│  export MallocScribble=1                                    │
│  ./MyApp                                                    │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

### 2.3 自定义填充检测

```cpp
// 自定义 Debug 分配器
class DebugAllocator {
public:
    static constexpr uint8_t ALLOC_PATTERN = 0xCD;
    static constexpr uint8_t FREE_PATTERN = 0xDD;
    static constexpr uint8_t FENCE_PATTERN = 0xFD;
    static constexpr size_t FENCE_SIZE = 16;

    static void* allocate(size_t size) {
        size_t total = size + 2 * FENCE_SIZE;
        uint8_t* raw = static_cast<uint8_t*>(malloc(total));

        // 填充前边界
        memset(raw, FENCE_PATTERN, FENCE_SIZE);
        // 填充用户区域
        memset(raw + FENCE_SIZE, ALLOC_PATTERN, size);
        // 填充后边界
        memset(raw + FENCE_SIZE + size, FENCE_PATTERN, FENCE_SIZE);

        return raw + FENCE_SIZE;
    }

    static void deallocate(void* ptr, size_t size) {
        uint8_t* raw = static_cast<uint8_t*>(ptr) - FENCE_SIZE;

        // 检查边界是否被修改
        for (size_t i = 0; i < FENCE_SIZE; i++) {
            if (raw[i] != FENCE_PATTERN) {
                fprintf(stderr, "Buffer underflow detected!\n");
                abort();
            }
        }
        for (size_t i = 0; i < FENCE_SIZE; i++) {
            if (raw[FENCE_SIZE + size + i] != FENCE_PATTERN) {
                fprintf(stderr, "Buffer overflow detected!\n");
                abort();
            }
        }

        // 填充释放模式
        memset(ptr, FREE_PATTERN, size);

        free(raw);
    }

    // 检查是否访问已释放内存
    static bool is_freed_memory(void* ptr) {
        uint8_t* p = static_cast<uint8_t*>(ptr);
        // 检查连续 4 字节是否都是 FREE_PATTERN
        return p[0] == FREE_PATTERN && p[1] == FREE_PATTERN &&
               p[2] == FREE_PATTERN && p[3] == FREE_PATTERN;
    }
};
```

---

## 3. Windows 页堆调试

### 3.1 页堆 (Page Heap) 原理

```
┌─────────────────────────────────────────────────────────────┐
│                      页堆原理                                │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  普通堆分配：                                                │
│  ┌─────┬─────┬─────┬─────┬─────┬─────┐                     │
│  │Obj A│Obj B│Obj C│Obj D│Obj E│ ... │                     │
│  └─────┴─────┴─────┴─────┴─────┴─────┘                     │
│  越界写入 B 可能破坏 C，但不立即崩溃                        │
│                                                             │
│  页堆分配（Full Page Heap）：                                │
│  ┌────────────────┬─────┬──────────────────┐               │
│  │    可访问页     │Obj A│    保护页         │               │
│  │    填充        │     │   不可访问        │               │
│  └────────────────┴─────┴──────────────────┘               │
│  每个分配后紧跟保护页，越界立即崩溃                         │
│                                                             │
│  释放后：                                                    │
│  ┌────────────────────────────────────────┐                │
│  │            整个区域不可访问             │                │
│  └────────────────────────────────────────┘                │
│  use-after-free 立即崩溃                                    │
│                                                             │
│  开销：内存使用大幅增加（每个分配至少一页）                  │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

### 3.2 GFlags 配置页堆

```batch
:: 查看当前配置
gflags -i MyApp.exe

:: 启用完整页堆（推荐，能检测更多问题）
gflags /p /enable MyApp.exe /full

:: 启用标准页堆（开销较小）
gflags /p /enable MyApp.exe

:: 禁用页堆
gflags /p /disable MyApp.exe

:: 查看所有启用页堆的程序
gflags /p

:: 常用选项：
:: /full    完整页堆（每分配一页）
:: /unaligned  检测未对齐访问
:: /backwards  保护页在分配前面（检测 underflow）
```

### 3.3 Application Verifier

```
1. 安装 Application Verifier
   Windows SDK 包含

2. 启动 Application Verifier
   appverif.exe

3. 添加应用程序
   File → Add Application → 选择 .exe

4. 启用检测项目
   - Basics/Heaps: 堆损坏检测
   - Basics/Handles: 句柄验证
   - Basics/Memory: 内存错误检测
   - Basics/TLS: 线程本地存储
   - Low Resource Simulation: 资源不足模拟

5. 运行程序
   Application Verifier 自动 Hook

6. 查看日志
   Event Viewer → Windows Logs → Application
   或 Application Verifier 的 Logs 窗口
```

### 3.4 WinDbg 页堆分析

```
:: 分析页堆崩溃

:: 1. 加载 dump，查看异常
!analyze -v

:: 2. 查看堆信息
!heap -p -a <address>

:: 输出示例：
:: address 00ff1000 found in
:: _DPH_HEAP_ROOT @ 151000
:: in busy allocation (  DPH_HEAP_BLOCK:  UserAddr  UserSize)
::                          191a64:       ff1000       100
:: 7c918f01 ntdll!RtlAllocateHeap+0x00000e64
:: 00401234 MyApp!allocate_memory+0x0000001a
:: 00401567 MyApp!main+0x00000042

:: 3. 查看分配和释放历史
!heap -p -a <address>

:: 如果是 use-after-free：
:: freed allocation (  DPH_HEAP_BLOCK:  UserAddr  UserSize)
:: 显示释放时的调用栈
```

---

## 4. macOS/Linux 调试工具

### 4.1 Guard Malloc (macOS)

```bash
# 启用 Guard Malloc
export DYLD_INSERT_LIBRARIES=/usr/lib/libgmalloc.dylib
./MyApp

# 或通过 Xcode
# Scheme → Edit Scheme → Diagnostics → Guard Malloc

# 环境变量控制
export MALLOC_FILL_SPACE=1       # 填充分配的内存
export MALLOC_ALLOW_READS=1      # 允许读已释放内存（仅报告）
export MALLOC_PROTECT_BEFORE=1   # 保护页在分配前（检测 underflow）

# Guard Malloc 特性：
# - 每个分配使用独立的内存页
# - 分配后的页面设为不可访问
# - 释放后整个页面不可访问
# - 立即检测越界和 use-after-free
```

### 4.2 MallocScribble (macOS)

```bash
# 启用内存填充
export MallocScribble=1
./MyApp

# 分配后填充 0xAA
# 释放后填充 0x55

# 配合 MallocGuardEdges
export MallocGuardEdges=1  # 分配边界添加保护页
export MallocDoNotProtectPrelude=1  # 不保护前边界
export MallocDoNotProtectPostlude=1 # 不保护后边界

# 检查程序
export MallocCheckHeapStart=100   # 从第 100 次分配开始检查
export MallocCheckHeapEach=100    # 每 100 次分配检查一次
```

### 4.3 Valgrind 检测野指针

```bash
# 检测 use-after-free 和越界
valgrind --tool=memcheck \
    --track-origins=yes \
    --read-var-info=yes \
    ./MyApp

# 输出示例：
# ==12345== Invalid read of size 4
# ==12345==    at 0x400567: use_after_free (main.cpp:10)
# ==12345==    by 0x400589: main (main.cpp:20)
# ==12345==  Address 0x5204040 is 0 bytes inside a block of size 4 free'd
# ==12345==    at 0x4C2EDEB: free (vg_replace_malloc.c:530)
# ==12345==    by 0x400560: use_after_free (main.cpp:9)
# ==12345==    by 0x400589: main (main.cpp:20)
# ==12345==  Block was alloc'd at
# ==12345==    at 0x4C2DB8F: malloc (vg_replace_malloc.c:299)
# ==12345==    by 0x400545: use_after_free (main.cpp:8)
# ==12345==    by 0x400589: main (main.cpp:20)
```

### 4.4 Electric Fence

```bash
# 安装
sudo apt install electric-fence

# 使用（链接 libefence）
gcc -g main.c -lefence -o MyApp
./MyApp

# 或使用 LD_PRELOAD
LD_PRELOAD=/usr/lib/libefence.so ./MyApp

# 特性：
# - 每个 malloc 分配独立页
# - 越界访问立即段错误
# - 检测 use-after-free
```

---

## 5. AddressSanitizer 检测

### 5.1 ASan 编译和运行

```bash
# 编译启用 ASan
clang++ -fsanitize=address -g -O1 main.cpp -o MyApp
# 或
g++ -fsanitize=address -g -O1 main.cpp -o MyApp

# MSVC (Visual Studio 2019 16.9+)
cl /fsanitize=address main.cpp

# 运行
./MyApp

# 环境变量
export ASAN_OPTIONS="detect_leaks=1:halt_on_error=1:print_stats=1"
```

### 5.2 ASan 检测示例

```cpp
// use-after-free 示例
int main() {
    int* p = new int(42);
    delete p;
    return *p;  // ASan 报告
}

// ASan 输出：
// ==12345==ERROR: AddressSanitizer: heap-use-after-free on address 0x602000000010
// READ of size 4 at 0x602000000010 thread T0
//     #0 0x4005a7 in main main.cpp:4
//
// 0x602000000010 is located 0 bytes inside of 4-byte region [0x602000000010,0x602000000014)
// freed by thread T0 here:
//     #0 0x7f123456 in operator delete(void*)
//     #1 0x400590 in main main.cpp:3
//
// previously allocated by thread T0 here:
//     #0 0x7f123457 in operator new(unsigned long)
//     #1 0x400580 in main main.cpp:2
```

### 5.3 ASan 原理

```
┌─────────────────────────────────────────────────────────────┐
│                   ASan 工作原理                              │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  Shadow Memory（影子内存）：                                 │
│  8 字节应用内存 → 1 字节影子内存                            │
│                                                             │
│  内存布局：                                                  │
│  ┌────────────────────────────────────────────────────┐    │
│  │ 应用内存        │ Red Zone │ 应用内存       │ ...   │    │
│  │ 可访问          │ 不可访问  │ 可访问         │       │    │
│  └────────────────────────────────────────────────────┘    │
│         ↓                ↓            ↓                     │
│  ┌────────────────────────────────────────────────────┐    │
│  │ Shadow: 0      │ Shadow: -1│ Shadow: 0     │ ...   │    │
│  │ (可访问)       │ (禁止)    │ (可访问)      │       │    │
│  └────────────────────────────────────────────────────┘    │
│                                                             │
│  Shadow 值含义：                                             │
│  0:      全部 8 字节可访问                                  │
│  1-7:    前 1-7 字节可访问                                  │
│  负数:   不可访问（不同负值表示不同原因）                    │
│    -1 (0xf1): 堆 left redzone                               │
│    -2 (0xf2): 堆 right redzone                              │
│    -3 (0xf3): 已释放的堆内存                                │
│    -4 (0xf4): 栈 left redzone                               │
│    -5 (0xf5): 栈 mid redzone                                │
│                                                             │
│  编译时插桩：                                                │
│  每次内存访问前检查 Shadow 值                               │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

---

## 6. 预防野指针

### 6.1 编码规范

```cpp
// 1. 初始化所有指针
int* p = nullptr;  // 不要 int* p;

// 2. 释放后置空
delete p;
p = nullptr;

// 3. 使用智能指针
auto p = std::make_unique<int>(42);

// 4. 避免返回局部变量地址
std::string get_string() {
    std::string s = "hello";
    return s;  // 返回值，不是引用
}

// 5. 注意迭代器失效
std::vector<int> vec;
for (size_t i = 0; i < vec.size(); i++) {
    if (should_add()) {
        vec.push_back(x);  // 可能导致迭代器失效
        // 使用索引遍历避免问题
    }
}

// 6. 对象生命周期清晰
class Owner {
    std::unique_ptr<Resource> res_;  // 所有权明确
};

// 7. 避免 delete this
class Widget {
    static void destroy(Widget* w) {
        delete w;
        // 不要在这之后访问 w 的成员
    }
};
```

### 6.2 静态分析

```bash
# Clang Static Analyzer
clang --analyze main.cpp

# Clang-Tidy
clang-tidy main.cpp -checks='*'

# Coverity
cov-build --dir cov-int make
cov-analyze --dir cov-int

# PVS-Studio
pvs-studio-analyzer analyze -o report.log

# 常见检测：
# - 未初始化变量
# - use-after-free
# - 空指针解引用
# - 迭代器失效
```

### 6.3 运行时检查

```cpp
// 自定义 Checked Pointer
template<typename T>
class CheckedPtr {
public:
    CheckedPtr() : ptr_(nullptr), valid_(false) {}

    explicit CheckedPtr(T* p) : ptr_(p), valid_(true) {}

    ~CheckedPtr() {
        valid_ = false;
        ptr_ = reinterpret_cast<T*>(0xDEADBEEF);
    }

    T& operator*() const {
        check();
        return *ptr_;
    }

    T* operator->() const {
        check();
        return ptr_;
    }

    void reset(T* p = nullptr) {
        ptr_ = p;
        valid_ = (p != nullptr);
    }

private:
    void check() const {
        if (!valid_ || ptr_ == nullptr ||
            ptr_ == reinterpret_cast<T*>(0xDEADBEEF)) {
            fprintf(stderr, "Invalid pointer access!\n");
            std::abort();
        }
    }

    T* ptr_;
    bool valid_;
};

// 使用 Weak Reference 检测生命周期
class Observable {
public:
    std::weak_ptr<Observable> weak_self() {
        return weak_from_this();
    }
};

void safe_callback(std::weak_ptr<Observable> weak) {
    if (auto obj = weak.lock()) {
        obj->do_something();  // 安全
    } else {
        // 对象已销毁
    }
}
```

---

## 7. 调试技巧

### 7.1 条件断点

```
// WinDbg
bp MyApp!use_pointer "j (@eax == 0) ''; 'gc'"
// 当 eax 为 0 时中断

// GDB
break use_pointer if ptr == 0
// 当 ptr 为空时中断

// LLDB
breakpoint set --name use_pointer --condition 'ptr == 0'
```

### 7.2 数据断点（硬件断点）

```
// WinDbg - 写入断点
ba w4 0x00abc123
// 当地址 0x00abc123 被写入时中断

// GDB
watch *0x00abc123
// 或 watch 变量
watch my_variable

// LLDB
watchpoint set expression -- &my_variable
// 或
watchpoint set variable my_variable

// 用途：
// - 找出谁修改了某个变量
// - 检测内存踩踏
```

### 7.3 反向调试

```bash
# 使用 rr (Record and Replay)
# 安装
sudo apt install rr

# 录制
rr record ./MyApp

# 回放
rr replay

# 在 GDB 中反向执行
(rr) reverse-continue    # 反向继续
(rr) reverse-step        # 反向单步
(rr) reverse-next        # 反向下一步

# 从崩溃点反向追踪野指针来源
```

---

## 8. 面试深度问题

### Q1: 如何调试延迟崩溃的野指针问题？

**答案**：

```
延迟崩溃原因：
野指针写入破坏了堆元数据或其他数据，
在后续 malloc/free 或访问其他数据时才崩溃。

调试方法：

1. 启用页堆
   gflags /p /enable MyApp.exe /full
   每次分配使用独立页，越界立即崩溃

2. AddressSanitizer
   编译时启用，实时检测越界和 use-after-free

3. Valgrind
   运行时检测所有内存访问

4. 内存填充 + 检查
   - 自定义分配器填充特殊模式
   - 定期扫描检查填充是否被破坏

5. 二分法
   - 禁用部分代码
   - 逐步缩小范围

6. 反向调试 (rr)
   - 录制执行过程
   - 从崩溃点反向追踪

关键：让问题尽早暴露，而不是在最终崩溃点分析
```

### Q2: use-after-free 的典型场景和解决方案？

**答案**：

```
典型场景：

1. 手动内存管理
   int* p = new int;
   delete p;
   *p = 42;  // UAF

   解决：使用智能指针

2. 回调函数持有原始指针
   obj->register_callback([=]() {
       this->do_something();  // 如果 obj 已销毁
   });

   解决：weak_ptr + lock()

3. 容器存储原始指针
   std::vector<Object*> objects;
   objects.push_back(obj);
   delete obj;
   objects[0]->method();  // UAF

   解决：存储智能指针

4. 多线程竞争
   线程 A 释放，线程 B 使用

   解决：引用计数 + 原子操作

5. 迭代器/引用失效
   for (auto& item : container) {
       container.erase(...);  // 迭代器失效
   }

   解决：使用索引或收集后删除

6. 析构顺序问题
   class A 持有 B*，B 先于 A 析构

   解决：明确所有权，使用 shared_ptr/weak_ptr
```

### Q3: 如何在没有源码的情况下定位野指针？

**答案**：

```
方法：

1. 分析崩溃地址
   - 地址特征：0xCDCDCDCD（未初始化），0xDDDDDDDD（已释放）
   - 判断是读还是写

2. 反汇编分析
   - 定位崩溃指令
   - 分析寄存器来源
   - 追踪调用链

3. 内存分析
   - 查看崩溃地址周围的内存
   - 寻找有意义的数据（字符串、vtable）

4. 符号猜测
   - 根据函数偏移推测函数
   - 使用 IDA/Ghidra 分析

5. 二分定位
   - 注入代码记录调用
   - 逐步缩小范围

6. 使用工具
   - 页堆仍可工作（无需源码）
   - Application Verifier

7. 日志分析
   - 崩溃前的操作序列
   - 关联代码路径
```

### Q4: 多线程程序中的野指针问题如何排查？

**答案**：

```
挑战：
- 时序依赖，难以重现
- 可能只在特定线程交错下出现

排查方法：

1. ThreadSanitizer
   clang++ -fsanitize=thread main.cpp
   检测数据竞争

2. 增加同步
   在可疑位置添加锁
   如果问题消失，确认是竞争问题

3. 日志 + 时间戳
   记录关键操作和线程 ID
   分析交错情况

4. 引用计数
   原子引用计数跟踪对象生命周期
   析构时打印调用栈

5. 压力测试
   多线程高频操作
   增加问题重现概率

6. 延迟销毁
   不立即释放，放入延迟队列
   验证是否是生命周期问题

7. 代码审查
   重点检查：
   - 共享指针的读写
   - 对象生命周期管理
   - 锁的使用范围
```

### Q5: 页堆的实现原理是什么？

**答案**：

```
┌─────────────────────────────────────────────────────────────┐
│                     页堆实现原理                             │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  分配流程：                                                  │
│  1. 分配至少一个页（4KB）                                   │
│  2. 将用户数据放在页末尾                                    │
│  3. 在数据后分配一个保护页（不可访问）                      │
│                                                             │
│  ┌────────────────────┬────────┬────────────────┐          │
│  │   填充区域         │ 用户数据│   保护页        │          │
│  │   可访问          │        │   不可访问      │          │
│  └────────────────────┴────────┴────────────────┘          │
│                              ↑                              │
│                     用户得到的指针                           │
│                                                             │
│  检测机制：                                                  │
│  • 越界写入 → 访问保护页 → 立即 SIGSEGV                    │
│  • 释放后访问 → 整个区域设为不可访问 → 立即崩溃            │
│                                                             │
│  开销：                                                     │
│  • 每个分配至少两页（8KB）                                 │
│  • 小分配浪费严重                                          │
│  • 适合调试，不适合生产                                    │
│                                                             │
│  实现方式：                                                  │
│  • VirtualAlloc + VirtualProtect (Windows)                 │
│  • mmap + mprotect (Unix)                                  │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

---

## 9. 总结

```
┌─────────────────────────────────────────────────────────────┐
│                   野指针排查知识图谱                         │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  野指针类型                                                  │
│  ├── 未初始化指针                                           │
│  ├── 悬垂指针 (use-after-free)                             │
│  └── 越界指针                                               │
│                                                             │
│  检测工具                                                   │
│  ├── Windows: 页堆 (GFlags)、Application Verifier          │
│  ├── macOS: Guard Malloc、MallocScribble                   │
│  ├── Linux: Valgrind、Electric Fence                       │
│  └── 跨平台: AddressSanitizer                              │
│                                                             │
│  内存填充模式                                               │
│  ├── 0xCD: 未初始化堆 (MSVC)                               │
│  ├── 0xDD: 已释放内存                                      │
│  ├── 0xFD: 边界标记                                        │
│  └── 0xCC: 未初始化栈                                      │
│                                                             │
│  预防措施                                                   │
│  ├── 智能指针                                              │
│  ├── 释放后置空                                            │
│  ├── 静态分析                                              │
│  └── 代码规范                                              │
│                                                             │
│  面试重点                                                   │
│  ├── 延迟崩溃调试                                          │
│  ├── use-after-free 场景                                   │
│  ├── 多线程野指针                                          │
│  └── 页堆原理                                              │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```
