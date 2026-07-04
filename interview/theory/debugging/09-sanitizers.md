# Sanitizer 工具集 (Sanitizers)

Sanitizer 是编译器级别的动态检测工具，用于发现内存错误、数据竞争等问题。

---

## 1. Sanitizer 概述

### 1.1 Sanitizer 家族

```
┌─────────────────────────────────────────────────────────────┐
│                    Sanitizer 工具集                          │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  工具                  检测内容                  开销        │
│  ─────────────────────────────────────────────────────────  │
│  AddressSanitizer     内存错误                  2-3x        │
│  (ASan)               • 越界访问                            │
│                       • use-after-free                     │
│                       • use-after-return                   │
│                       • 内存泄漏                            │
│                                                             │
│  ThreadSanitizer      线程问题                  5-15x       │
│  (TSan)               • 数据竞争                            │
│                       • 死锁                                │
│                                                             │
│  MemorySanitizer      未初始化内存              3x          │
│  (MSan)               • 使用未初始化的值                    │
│                                                             │
│  UndefinedBehavior    未定义行为                1.2-2x      │
│  Sanitizer (UBSan)    • 整数溢出                            │
│                       • 空指针解引用                        │
│                       • 类型错误                            │
│                                                             │
│  LeakSanitizer        内存泄漏                  几乎无      │
│  (LSan)               • 程序结束时检测泄漏                  │
│                       （ASan 内置）                         │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

### 1.2 Sanitizer 工作原理

```
┌─────────────────────────────────────────────────────────────┐
│                   Sanitizer 工作原理                         │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  编译时插桩 (Compile-time Instrumentation)                  │
│  ┌─────────┐     ┌─────────┐     ┌─────────────┐           │
│  │  源代码  │ ──→ │  编译器  │ ──→ │ 插桩后代码   │           │
│  └─────────┘     │  + San  │     └─────────────┘           │
│                  └─────────┘                                │
│                                                             │
│  内存访问插桩示例：                                          │
│  原始代码：                                                  │
│    int x = arr[i];                                         │
│                                                             │
│  插桩后：                                                    │
│    __asan_check(&arr[i], sizeof(int));  // 检查访问         │
│    int x = arr[i];                                         │
│                                                             │
│  运行时库 (Runtime Library)                                 │
│  ┌─────────────────────────────────────────────────────┐   │
│  │  • 影子内存管理                                       │   │
│  │  • 替换 malloc/free                                  │   │
│  │  • 错误报告                                          │   │
│  │  • 调用栈记录                                        │   │
│  └─────────────────────────────────────────────────────┘   │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

---

## 2. AddressSanitizer (ASan)

### 2.1 ASan 基础使用

```bash
# 编译启用 ASan
# Clang
clang++ -fsanitize=address -g -O1 main.cpp -o MyApp

# GCC
g++ -fsanitize=address -g -O1 main.cpp -o MyApp

# MSVC (Visual Studio 2019 16.9+)
cl /fsanitize=address /Zi main.cpp

# CMake
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -g")

# 运行
./MyApp
# 或设置环境变量
ASAN_OPTIONS="detect_leaks=1:halt_on_error=0" ./MyApp
```

### 2.2 ASan 检测能力

```cpp
// 1. 堆缓冲区溢出
void heap_buffer_overflow() {
    int* arr = new int[10];
    arr[10] = 42;  // 越界写入
    delete[] arr;
}
// ASan 报告: heap-buffer-overflow

// 2. 栈缓冲区溢出
void stack_buffer_overflow() {
    int arr[10];
    arr[10] = 42;  // 越界写入
}
// ASan 报告: stack-buffer-overflow

// 3. 全局缓冲区溢出
int global_arr[10];
void global_buffer_overflow() {
    global_arr[10] = 42;
}
// ASan 报告: global-buffer-overflow

// 4. Use-after-free
void use_after_free() {
    int* p = new int(42);
    delete p;
    *p = 100;  // 访问已释放内存
}
// ASan 报告: heap-use-after-free

// 5. Use-after-return（需要启用）
int* use_after_return() {
    int local = 42;
    return &local;
}
// ASan 报告: stack-use-after-return
// 需要: ASAN_OPTIONS=detect_stack_use_after_return=1

// 6. Double-free
void double_free() {
    int* p = new int(42);
    delete p;
    delete p;
}
// ASan 报告: double-free

// 7. 内存泄漏（LeakSanitizer）
void memory_leak() {
    int* p = new int(42);
    // 忘记 delete
}
// ASan 报告: detected memory leaks
```

### 2.3 ASan 输出解读

```
=================================================================
==12345==ERROR: AddressSanitizer: heap-buffer-overflow on address 0x602000000038 at pc 0x000000400567 bp 0x7ffd12345678 sp 0x7ffd12345670
WRITE of size 4 at 0x602000000038 thread T0
    #0 0x400566 in heap_buffer_overflow() main.cpp:5
    #1 0x400589 in main main.cpp:10
    #2 0x7f1234567890 in __libc_start_main

0x602000000038 is located 0 bytes to the right of 40-byte region [0x602000000010,0x602000000038)
allocated by thread T0 here:
    #0 0x7f123456 in operator new[](unsigned long)
    #1 0x400550 in heap_buffer_overflow() main.cpp:4
    #2 0x400589 in main main.cpp:10

SUMMARY: AddressSanitizer: heap-buffer-overflow main.cpp:5 in heap_buffer_overflow()
Shadow bytes around the buggy address:
  0x0c047fff7fb0: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
  0x0c047fff7fc0: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
  0x0c047fff7fd0: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
=>0x0c047fff7fe0: fa fa 00 00 00 00 00[fa]fa fa fa fa fa fa fa fa
  0x0c047fff7ff0: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
Shadow byte legend:
  Addressable:           00
  Heap left redzone:     fa
  Freed heap region:     fd
  ...

解读：
• heap-buffer-overflow: 越界类型
• WRITE of size 4: 4 字节写入
• 第一个调用栈: 错误发生位置
• allocated by: 内存分配位置
• Shadow bytes: 内存状态图
  - 00: 可访问
  - fa: 堆 redzone（边界）
  - fd: 已释放
```

### 2.4 ASan 环境变量

```bash
# ASAN_OPTIONS 常用选项
export ASAN_OPTIONS="option1=value1:option2=value2"

# 常用选项
detect_leaks=1              # 启用泄漏检测（默认启用）
detect_stack_use_after_return=1  # 检测栈上 use-after-return
halt_on_error=0             # 发现错误后继续运行
abort_on_error=1            # 发现错误后 abort
log_path=/tmp/asan.log      # 输出到文件
suppressions=asan.supp      # 抑制文件
malloc_context_size=30      # 分配调用栈深度
fast_unwind_on_malloc=0     # 更准确的栈回溯（更慢）
check_initialization_order=1 # 检测全局初始化顺序问题
strict_init_order=1         # 严格初始化顺序检测
print_stats=1               # 打印统计信息
```

### 2.5 ASan 原理：Shadow Memory

```
┌─────────────────────────────────────────────────────────────┐
│                   ASan Shadow Memory 原理                    │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  映射关系：8 字节应用内存 → 1 字节影子内存                   │
│                                                             │
│  Shadow 地址计算：                                           │
│  Shadow = (Addr >> 3) + ShadowOffset                        │
│                                                             │
│  Linux x64: ShadowOffset = 0x7fff8000                       │
│                                                             │
│  Shadow 值含义：                                             │
│  ┌──────────────────────────────────────────────────────┐  │
│  │  值        含义                                       │  │
│  │  0         全部 8 字节可访问                          │  │
│  │  1-7       前 1-7 字节可访问                          │  │
│  │  负数      不可访问：                                 │  │
│  │    0xf1    堆左边界 (heap left redzone)              │  │
│  │    0xf2    堆右边界 (heap right redzone)             │  │
│  │    0xf3    已释放堆内存 (freed heap)                 │  │
│  │    0xf5    栈左边界 (stack left redzone)             │  │
│  │    0xf6    栈中间边界 (stack mid redzone)            │  │
│  │    0xf7    栈右边界 (stack right redzone)            │  │
│  │    0xf8    use-after-return 栈                       │  │
│  └──────────────────────────────────────────────────────┘  │
│                                                             │
│  内存访问检查（插桩代码）：                                  │
│  if (Shadow[(addr >> 3)] != 0) {                           │
│      if (Shadow[(addr >> 3)] < 0 ||                        │
│          (addr & 7) + size > Shadow[(addr >> 3)]) {        │
│          ReportError(addr, size);                          │
│      }                                                      │
│  }                                                          │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

---

## 3. ThreadSanitizer (TSan)

### 3.1 TSan 基础使用

```bash
# 编译启用 TSan
clang++ -fsanitize=thread -g -O1 main.cpp -o MyApp
# 或
g++ -fsanitize=thread -g -O1 main.cpp -o MyApp

# TSan 和 ASan 不能同时使用！

# 运行
./MyApp

# 环境变量
export TSAN_OPTIONS="halt_on_error=0:second_deadlock_stack=1"
```

### 3.2 TSan 检测能力

```cpp
#include <thread>
#include <mutex>

// 1. 数据竞争
int counter = 0;

void data_race() {
    std::thread t1([]{ counter++; });
    std::thread t2([]{ counter++; });
    t1.join();
    t2.join();
}
// TSan 报告: data race

// 2. 锁顺序问题（可能死锁）
std::mutex m1, m2;

void thread1() {
    std::lock_guard<std::mutex> l1(m1);
    std::lock_guard<std::mutex> l2(m2);
}

void thread2() {
    std::lock_guard<std::mutex> l1(m2);  // 不同顺序
    std::lock_guard<std::mutex> l2(m1);
}
// TSan 报告: lock-order-inversion (potential deadlock)

// 3. 信号处理中的竞争
volatile int flag = 0;

void signal_handler(int) {
    flag = 1;  // 与主线程竞争
}

void signal_race() {
    signal(SIGUSR1, signal_handler);
    while (!flag) {
        // spin
    }
}
// TSan 报告: data race
```

### 3.3 TSan 输出解读

```
==================
WARNING: ThreadSanitizer: data race (pid=12345)
  Write of size 4 at 0x000000601078 by thread T1:
    #0 increment() main.cpp:10 (MyApp+0x4a5678)
    #1 thread_func() main.cpp:20 (MyApp+0x4a5789)

  Previous write of size 4 at 0x000000601078 by thread T2:
    #0 increment() main.cpp:10 (MyApp+0x4a5678)
    #1 thread_func() main.cpp:20 (MyApp+0x4a5789)

  Location is global 'counter' of size 4 at 0x000000601078 (MyApp+0x601078)

  Thread T1 (tid=12346, running) created by main thread at:
    #0 pthread_create (libtsan.so.0+0x12345)
    #1 std::thread::thread<...> main.cpp:25 (MyApp+0x4a5890)
    #2 main main.cpp:30 (MyApp+0x4a5901)

  Thread T2 (tid=12347, running) created by main thread at:
    #0 pthread_create (libtsan.so.0+0x12345)
    #1 std::thread::thread<...> main.cpp:26 (MyApp+0x4a5912)
    #2 main main.cpp:30 (MyApp+0x4a5901)

SUMMARY: ThreadSanitizer: data race main.cpp:10 in increment()
==================

解读：
• data race: 数据竞争
• Write of size 4: 4 字节写入
• 显示两个冲突访问的调用栈
• Location: 竞争的内存位置
• Thread T1/T2: 冲突的两个线程
• created by: 线程创建位置
```

### 3.4 TSan Suppressions

```
# tsan_suppressions.txt

# 忽略特定函数
race:known_benign_function

# 忽略特定文件
race_top:third_party/*

# 忽略死锁检测
deadlock:legacy_code

# 忽略信号处理
signal:signal_handler_func

# 使用
TSAN_OPTIONS="suppressions=tsan_suppressions.txt" ./MyApp
```

---

## 4. MemorySanitizer (MSan)

### 4.1 MSan 基础使用

```bash
# 只支持 Clang
clang++ -fsanitize=memory -g -O1 main.cpp -o MyApp

# 需要用 MSan 编译所有链接的库！
# 否则会有误报

# 使用 msan-instrumented libc++
clang++ -fsanitize=memory -stdlib=libc++ \
    -fsanitize-memory-track-origins=2 \
    -g -O1 main.cpp -o MyApp

# 环境变量
export MSAN_OPTIONS="halt_on_error=0"
```

### 4.2 MSan 检测能力

```cpp
// 1. 使用未初始化的局部变量
void uninitialized_local() {
    int x;
    if (x > 0) {  // 使用未初始化的 x
        printf("positive\n");
    }
}
// MSan 报告: use-of-uninitialized-value

// 2. 使用未初始化的堆内存
void uninitialized_heap() {
    int* p = (int*)malloc(sizeof(int));
    if (*p > 0) {  // 未初始化
        printf("positive\n");
    }
    free(p);
}
// MSan 报告: use-of-uninitialized-value

// 3. 传递未初始化值
void pass_uninitialized() {
    int arr[10];
    memcpy(dest, arr, sizeof(arr));  // 拷贝未初始化数据
}
// MSan 报告: use-of-uninitialized-value

// 4. 条件依赖未初始化值
void conditional_uninitialized() {
    int x;
    int y = x ? 1 : 0;  // 条件依赖未初始化值
}
// MSan 报告: use-of-uninitialized-value
```

### 4.3 MSan 输出解读

```
==12345==WARNING: MemorySanitizer: use-of-uninitialized-value
    #0 0x4a5678 in uninitialized_local() main.cpp:5
    #1 0x4a5789 in main main.cpp:10

  Uninitialized value was created by an allocation of 'x' in the stack frame
  of function 'uninitialized_local'
    #0 0x4a5670 in uninitialized_local() main.cpp:3

SUMMARY: MemorySanitizer: use-of-uninitialized-value main.cpp:5 in uninitialized_local()
```

---

## 5. UndefinedBehaviorSanitizer (UBSan)

### 5.1 UBSan 基础使用

```bash
# 启用所有检查
clang++ -fsanitize=undefined -g main.cpp -o MyApp

# 启用特定检查
clang++ -fsanitize=signed-integer-overflow,null -g main.cpp

# 常用检查组合
clang++ -fsanitize=undefined,float-divide-by-zero,nullability -g main.cpp

# 与 ASan 组合使用
clang++ -fsanitize=address,undefined -g main.cpp

# 环境变量
export UBSAN_OPTIONS="print_stacktrace=1:halt_on_error=0"
```

### 5.2 UBSan 检测能力

```cpp
// 1. 有符号整数溢出
void signed_overflow() {
    int x = INT_MAX;
    x++;  // 溢出
}
// UBSan: signed integer overflow

// 2. 无符号整数溢出（需要显式启用）
void unsigned_overflow() {
    unsigned x = UINT_MAX;
    x++;  // 回绕
}
// -fsanitize=unsigned-integer-overflow

// 3. 除零
void divide_by_zero() {
    int x = 1 / 0;
}
// UBSan: division by zero

// 4. 空指针解引用
void null_deref() {
    int* p = nullptr;
    *p = 42;
}
// UBSan: null pointer passed

// 5. 数组越界（需要 VLA 或显式边界信息）
void array_bounds() {
    int arr[10];
    arr[20] = 42;
}
// UBSan: index out of bounds

// 6. 类型双关（type punning）
void type_mismatch() {
    float f = 3.14f;
    int* p = reinterpret_cast<int*>(&f);
    *p = 42;  // 类型不匹配
}
// UBSan: type mismatch

// 7. 对齐错误
void misaligned_access() {
    char buf[10];
    int* p = reinterpret_cast<int*>(buf + 1);  // 未对齐
    *p = 42;
}
// UBSan: misaligned address

// 8. 移位错误
void shift_error() {
    int x = 1 << 32;  // 超过位宽
    int y = -1 << 1;  // 负数左移
}
// UBSan: shift exponent is too large

// 9. 无效的布尔值
void invalid_bool() {
    bool* p = (bool*)malloc(1);
    *p = 2;  // 无效的 bool 值
    if (*p) {}
}
// UBSan: load of value 2, which is not a valid value for type 'bool'

// 10. 虚调用错误
void virtual_call_error() {
    struct Base {
        virtual void foo() = 0;
        Base() { foo(); }  // 构造中调用纯虚函数
    };
}
// UBSan: call to pure virtual function
```

### 5.3 UBSan 检查类型

```bash
# 可用的检查类型
-fsanitize=alignment              # 对齐检查
-fsanitize=bool                   # 无效布尔值
-fsanitize=bounds                 # 数组越界
-fsanitize=enum                   # 无效枚举值
-fsanitize=float-cast-overflow    # 浮点转整数溢出
-fsanitize=float-divide-by-zero   # 浮点除零
-fsanitize=function               # 函数指针类型错误
-fsanitize=integer-divide-by-zero # 整数除零
-fsanitize=null                   # 空指针
-fsanitize=object-size            # 对象大小检查
-fsanitize=pointer-overflow       # 指针运算溢出
-fsanitize=return                 # 无返回值
-fsanitize=returns-nonnull-attribute # 返回非空属性
-fsanitize=shift                  # 移位检查
-fsanitize=signed-integer-overflow # 有符号整数溢出
-fsanitize=unreachable            # 不可达代码
-fsanitize=unsigned-integer-overflow # 无符号整数溢出
-fsanitize=vla-bound              # VLA 边界
-fsanitize=vptr                   # 虚指针检查

# 组合
-fsanitize=undefined              # 所有默认检查
-fsanitize=integer                # 所有整数检查
```

---

## 6. Sanitizer 组合使用

### 6.1 兼容性

```
┌─────────────────────────────────────────────────────────────┐
│                   Sanitizer 兼容性矩阵                       │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│            ASan    TSan    MSan    UBSan   LSan            │
│  ASan       -       ✗       ✗       ✓       内置           │
│  TSan       ✗       -       ✗       ✓       ✗              │
│  MSan       ✗       ✗       -       ✓       ✗              │
│  UBSan      ✓       ✓       ✓       -       ✓              │
│  LSan      内置     ✗       ✗       ✓       -              │
│                                                             │
│  ✓ = 可以同时使用                                           │
│  ✗ = 不能同时使用                                           │
│                                                             │
│  推荐组合：                                                  │
│  • 开发阶段: ASan + UBSan                                  │
│  • 多线程测试: TSan + UBSan                                │
│  • 深度测试: 分别运行各 Sanitizer                          │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

### 6.2 构建配置

```cmake
# CMakeLists.txt

option(ENABLE_ASAN "Enable AddressSanitizer" OFF)
option(ENABLE_TSAN "Enable ThreadSanitizer" OFF)
option(ENABLE_MSAN "Enable MemorySanitizer" OFF)
option(ENABLE_UBSAN "Enable UndefinedBehaviorSanitizer" OFF)

if(ENABLE_ASAN)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -fno-omit-frame-pointer")
    set(CMAKE_LINKER_FLAGS "${CMAKE_LINKER_FLAGS} -fsanitize=address")
endif()

if(ENABLE_TSAN)
    if(ENABLE_ASAN OR ENABLE_MSAN)
        message(FATAL_ERROR "TSan cannot be used with ASan or MSan")
    endif()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=thread")
    set(CMAKE_LINKER_FLAGS "${CMAKE_LINKER_FLAGS} -fsanitize=thread")
endif()

if(ENABLE_MSAN)
    if(ENABLE_ASAN OR ENABLE_TSAN)
        message(FATAL_ERROR "MSan cannot be used with ASan or TSan")
    endif()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=memory -fsanitize-memory-track-origins")
    set(CMAKE_LINKER_FLAGS "${CMAKE_LINKER_FLAGS} -fsanitize=memory")
endif()

if(ENABLE_UBSAN)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=undefined")
    set(CMAKE_LINKER_FLAGS "${CMAKE_LINKER_FLAGS} -fsanitize=undefined")
endif()

# 构建
# cmake -B build -DENABLE_ASAN=ON -DENABLE_UBSAN=ON
# cmake --build build
```

---

## 7. CI/CD 集成

### 7.1 GitHub Actions 示例

```yaml
# .github/workflows/sanitizers.yml
name: Sanitizers

on: [push, pull_request]

jobs:
  asan:
    name: AddressSanitizer
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v3

      - name: Build with ASan
        run: |
          cmake -B build -DCMAKE_CXX_FLAGS="-fsanitize=address,undefined -g"
          cmake --build build

      - name: Run tests with ASan
        env:
          ASAN_OPTIONS: "detect_leaks=1:halt_on_error=1"
          UBSAN_OPTIONS: "print_stacktrace=1:halt_on_error=1"
        run: |
          cd build && ctest --output-on-failure

  tsan:
    name: ThreadSanitizer
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v3

      - name: Build with TSan
        run: |
          cmake -B build -DCMAKE_CXX_FLAGS="-fsanitize=thread -g"
          cmake --build build

      - name: Run tests with TSan
        env:
          TSAN_OPTIONS: "halt_on_error=1"
        run: |
          cd build && ctest --output-on-failure
```

### 7.2 Jenkins Pipeline 示例

```groovy
// Jenkinsfile
pipeline {
    agent any

    stages {
        stage('ASan Build') {
            steps {
                sh '''
                    cmake -B build-asan \
                        -DCMAKE_CXX_FLAGS="-fsanitize=address,undefined -g"
                    cmake --build build-asan
                '''
            }
        }

        stage('ASan Test') {
            environment {
                ASAN_OPTIONS = 'detect_leaks=1:halt_on_error=1:log_path=asan.log'
            }
            steps {
                sh 'cd build-asan && ctest --output-on-failure'
            }
            post {
                always {
                    archiveArtifacts artifacts: 'asan.log*', allowEmptyArchive: true
                }
            }
        }

        stage('TSan Build') {
            steps {
                sh '''
                    cmake -B build-tsan \
                        -DCMAKE_CXX_FLAGS="-fsanitize=thread -g"
                    cmake --build build-tsan
                '''
            }
        }

        stage('TSan Test') {
            environment {
                TSAN_OPTIONS = 'halt_on_error=1:log_path=tsan.log'
            }
            steps {
                sh 'cd build-tsan && ctest --output-on-failure'
            }
        }
    }
}
```

---

## 8. 面试深度问题

### Q1: ASan 的性能开销来自哪里？

**答案**：

```
开销来源：

1. 内存开销（~3x）
   - 影子内存：1/8 应用内存
   - Redzone：每次分配额外空间
   - 隔离区：已释放内存暂不复用

2. 运行时开销（~2x）
   - 每次内存访问检查影子内存
   - malloc/free 额外操作
   - 调用栈记录

3. 降低开销方法
   - 使用 -O1 或 -O2
   - 减少 malloc_context_size
   - fast_unwind_on_malloc=1

性能敏感场景：
- 使用采样检测
- 只在测试环境启用
- 针对性地检测特定模块
```

### Q2: TSan 如何检测数据竞争？

**答案**：

```
检测原理：

1. 向量时钟（Vector Clock）
   每个线程维护一个向量时钟
   记录与其他线程的同步关系

2. 访问历史
   每个内存位置记录最近访问
   - 线程 ID
   - 向量时钟
   - 访问类型（读/写）

3. 竞争检测
   新访问与历史比较：
   - 不同线程？
   - 至少一个写？
   - 向量时钟不可比（无同步）？
   → 报告竞争

4. 同步识别
   Hook 同步原语：
   - pthread_mutex_lock/unlock
   - std::mutex
   - atomic 操作
   正确传递向量时钟

局限：
- 只能检测执行到的竞争
- 高性能开销
- 可能有误报
```

### Q3: 如何处理 Sanitizer 误报？

**答案**：

```
处理方法：

1. 确认是否真是误报
   - 仔细分析报告
   - 检查代码逻辑
   - 第三方库问题？

2. 使用 Suppressions
   # suppression 文件
   race:known_benign_function
   leak:third_party_lib

3. 代码注解
   // ASan
   __attribute__((no_sanitize("address")))
   void func() { ... }

   // TSan
   __attribute__((no_sanitize("thread")))

   // 或宏
   #if defined(__has_feature)
   #  if __has_feature(address_sanitizer)
   #    define ASAN_DISABLE __attribute__((no_sanitize("address")))
   #  endif
   #endif

4. 编译器指令
   #pragma clang diagnostic ignored "-Wuninitialized"

5. 报告给维护者
   如果是第三方库问题
```

### Q4: 为什么 MSan 需要编译所有库？

**答案**：

```
原因：

MSan 通过影子内存跟踪未初始化状态
如果调用未插桩的库：
1. 库返回数据没有对应的影子内存
2. MSan 无法判断是否初始化
3. 可能产生误报或漏报

示例：
// 未插桩的 libc
char* str = strdup("hello");  // strdup 未插桩
// MSan 认为 str 内容未初始化 → 误报

解决方案：

1. 使用 instrumented libc++
   clang++ -fsanitize=memory -stdlib=libc++

2. 使用 MSan-instrumented 系统库
   # 需要自己编译

3. 标记已知安全的函数
   // 告诉 MSan 返回值是初始化的
   __msan_unpoison(ptr, size);

4. 使用 intercept
   MSan 已拦截常见 libc 函数
```

### Q5: Sanitizer 在生产环境能用吗？

**答案**：

```
不建议直接在生产环境使用：

原因：
1. 性能开销大（2-15x）
2. 内存开销大（2-3x）
3. 检测到问题会终止程序

生产环境策略：

1. 全量测试覆盖
   - CI/CD 必须通过 Sanitizer 测试
   - 覆盖率高的测试用例

2. 金丝雀发布
   - 少量实例启用 Sanitizer
   - 采样式部署

3. 线上采样
   - 概率性启用检测
   - 只检测新代码路径

4. 轻量级检测
   - 只启用 UBSan（开销小）
   - 使用 -fsanitize=undefined-trap

5. 崩溃分析
   - 结合 crash dump 分析
   - 问题重现时启用 Sanitizer

实践：
- 开发测试必启
- 性能测试不启
- 生产采样或不启
```

---

## 9. 总结

```
┌─────────────────────────────────────────────────────────────┐
│                   Sanitizer 知识图谱                         │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  工具类型                                                   │
│  ├── ASan: 内存错误（越界、UAF、泄漏）                      │
│  ├── TSan: 数据竞争、死锁                                  │
│  ├── MSan: 未初始化内存                                    │
│  └── UBSan: 未定义行为                                     │
│                                                             │
│  使用方式                                                   │
│  ├── 编译选项: -fsanitize=<type>                           │
│  ├── 环境变量: ASAN_OPTIONS, TSAN_OPTIONS                  │
│  └── 抑制文件: suppressions                                │
│                                                             │
│  工作原理                                                   │
│  ├── 编译时插桩                                             │
│  ├── 影子内存（ASan/MSan）                                  │
│  ├── 向量时钟（TSan）                                       │
│  └── 运行时检查                                             │
│                                                             │
│  CI/CD 集成                                                 │
│  ├── 单独构建各 Sanitizer 版本                             │
│  ├── 必须通过才能合并                                       │
│  └── 定期运行完整测试                                       │
│                                                             │
│  面试重点                                                   │
│  ├── ASan 原理（影子内存）                                  │
│  ├── TSan 原理（向量时钟）                                  │
│  ├── 性能开销分析                                           │
│  ├── 误报处理                                               │
│  └── 生产环境策略                                           │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```
