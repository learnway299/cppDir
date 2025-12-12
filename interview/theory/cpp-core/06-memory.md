# C++ 内存管理 (Memory Management)

## 概述

本文档涵盖 C++ 内存管理的核心知识，包括堆栈内存、动态内存分配（new/delete）、智能指针、RAII 原则、内存泄漏、内存对齐等重要概念。

---

## 1. 内存布局

### 1.1 程序的内存分区

**问题：C++ 程序的内存是如何组织的？**

```cpp
#include <iostream>

int global_var = 100;           // 全局变量：数据段（.data）
static int static_var = 200;    // 静态变量：数据段（.data）
const int const_var = 300;      // 常量：只读数据段（.rodata）

int uninitialized_global;       // 未初始化全局变量：BSS 段

void func() {
    static int static_local = 400;  // 静态局部变量：数据段
}

int main() {
    int local_var = 10;             // 局部变量：栈
    int* heap_var = new int(20);    // 动态分配：堆

    std::cout << "Stack address (local_var): " << &local_var << std::endl;
    std::cout << "Heap address (heap_var): " << heap_var << std::endl;
    std::cout << "Data segment (global_var): " << &global_var << std::endl;
    std::cout << "BSS segment (uninitialized): " << &uninitialized_global << std::endl;
    std::cout << "Text segment (function): " << (void*)&func << std::endl;

    delete heap_var;
    return 0;
}

/*
内存布局（从低地址到高地址）：
+------------------+
| Text Segment     | 代码段：存放程序指令
+------------------+
| .rodata          | 只读数据段：const 常量、字符串字面量
+------------------+
| .data            | 数据段：已初始化的全局/静态变量
+------------------+
| .bss             | BSS 段：未初始化的全局/静态变量（自动初始化为 0）
+------------------+
| Heap             | 堆：动态分配的内存（向上增长）
|     ↑            |
+------------------+
|     ↓            |
| Stack            | 栈：局部变量、函数调用（向下增长）
+------------------+
| Kernel Space     | 内核空间
+------------------+
*/
```

**关键要点：**
- **栈 (Stack)**：自动管理，速度快，空间有限（通常几 MB）
- **堆 (Heap)**：手动管理，灵活，空间大，速度较慢
- **数据段 (.data)**：已初始化的全局/静态变量
- **BSS 段**：未初始化的全局/静态变量（自动初始化为 0）
- **代码段 (Text)**：程序指令，只读

**常见陷阱：**
- 混淆栈和堆的特性
- 栈空间有限，大对象或递归过深会导致栈溢出

---

## 2. 栈内存 (Stack)

### 2.1 栈的特性

**问题：栈内存有什么特点？如何使用？**

```cpp
#include <iostream>

void func() {
    int a = 10;       // 栈上分配
    int arr[1000];    // 栈上分配数组

    std::cout << "Inside func, a = " << a << std::endl;
}  // a 和 arr 在这里自动销毁

class MyClass {
public:
    MyClass() { std::cout << "Constructor\n"; }
    ~MyClass() { std::cout << "Destructor\n"; }
};

int main() {
    {
        MyClass obj;  // 栈上创建对象
        // ...
    }  // obj 离开作用域，自动调用析构函数

    func();
    // func 返回后，a 和 arr 的内存自动回收

    return 0;
}
```

**关键要点：**
- 栈内存自动管理，遵循 LIFO (Last In First Out) 原则
- 变量离开作用域时自动销毁
- 速度快（指针移动即可分配/释放）
- 空间有限（通常 1-8 MB，取决于系统）
- 适合小对象和临时变量

**常见陷阱：**
- 返回局部变量的指针/引用（悬垂指针）
- 递归过深导致栈溢出
- 在栈上分配大数组导致栈溢出

---

### 2.2 栈溢出

**问题：什么是栈溢出？如何避免？**

```cpp
// 错误示例1：递归过深
void infiniteRecursion() {
    infiniteRecursion();  // 无限递归，栈溢出
}

// 错误示例2：栈上分配大对象
void largeArray() {
    int huge_array[10000000];  // 可能导致栈溢出
}

// 正确做法1：使用尾递归优化或迭代
int factorial(int n, int acc = 1) {
    if (n <= 1) return acc;
    return factorial(n - 1, n * acc);  // 尾递归
}

// 正确做法2：大对象放在堆上
void largeArrayHeap() {
    int* huge_array = new int[10000000];  // 堆上分配
    // ...
    delete[] huge_array;
}

// 正确做法3：使用容器
void largeVector() {
    std::vector<int> vec(10000000);  // 数据在堆上
}
```

**关键要点：**
- 栈溢出原因：递归过深、大对象、大数组
- 解决方法：使用堆、减少递归深度、使用容器

**常见陷阱：**
- 不检查递归终止条件
- 低估栈空间限制

---

## 3. 堆内存 (Heap)

### 3.1 new 和 delete

**问题：如何使用 new 和 delete 进行动态内存分配？**

```cpp
#include <iostream>

int main() {
    // 1. 分配单个对象
    int* p1 = new int;           // 未初始化
    int* p2 = new int(42);       // 初始化为 42
    int* p3 = new int{100};      // C++11 统一初始化

    delete p1;
    delete p2;
    delete p3;

    // 2. 分配数组
    int* arr1 = new int[10];           // 未初始化
    int* arr2 = new int[10]{};         // 值初始化（全为 0）
    int* arr3 = new int[5]{1, 2, 3, 4, 5};  // 列表初始化

    delete[] arr1;  // 注意：必须使用 delete[]
    delete[] arr2;
    delete[] arr3;

    // 3. 分配对象
    class MyClass {
    public:
        MyClass(int x) : x_(x) {
            std::cout << "Constructor: " << x_ << std::endl;
        }
        ~MyClass() {
            std::cout << "Destructor: " << x_ << std::endl;
        }
    private:
        int x_;
    };

    MyClass* obj = new MyClass(10);  // 调用构造函数
    delete obj;  // 调用析构函数

    // 4. 分配对象数组
    MyClass* obj_arr = new MyClass[3]{MyClass(1), MyClass(2), MyClass(3)};
    delete[] obj_arr;  // 依次调用每个对象的析构函数

    return 0;
}
```

**关键要点：**
- `new` 分配内存并调用构造函数
- `delete` 调用析构函数并释放内存
- `new[]` 和 `delete[]` 用于数组
- `new` 失败时抛出 `std::bad_alloc` 异常（除非使用 `new(std::nothrow)`）

**常见陷阱：**
- `new[]` 配 `delete` 或 `new` 配 `delete[]`（未定义行为）
- `new` 后忘记 `delete`（内存泄漏）
- 重复 `delete`（double free）

---

### 3.2 new 和 malloc 的区别

**问题：new/delete 和 malloc/free 有什么区别？**

```cpp
#include <cstdlib>
#include <iostream>

class MyClass {
public:
    MyClass(int x) : x_(x) {
        std::cout << "Constructor: " << x_ << std::endl;
    }
    ~MyClass() {
        std::cout << "Destructor: " << x_ << std::endl;
    }
private:
    int x_;
};

int main() {
    // 1. malloc/free：只分配/释放内存，不调用构造/析构函数
    MyClass* p1 = (MyClass*)malloc(sizeof(MyClass));
    // 没有调用构造函数！对象未初始化
    free(p1);
    // 没有调用析构函数！

    // 2. new/delete：分配内存 + 调用构造/析构函数
    MyClass* p2 = new MyClass(10);  // 调用构造函数
    delete p2;  // 调用析构函数

    // 3. malloc 需要手动计算大小，new 自动
    int* arr1 = (int*)malloc(10 * sizeof(int));
    int* arr2 = new int[10];

    free(arr1);
    delete[] arr2;

    // 4. new 失败抛出异常，malloc 返回 nullptr
    try {
        int* huge = new int[1000000000000];  // 抛出 std::bad_alloc
    } catch (const std::bad_alloc& e) {
        std::cout << "Allocation failed: " << e.what() << std::endl;
    }

    int* huge2 = (int*)malloc(1000000000000 * sizeof(int));
    if (huge2 == nullptr) {
        std::cout << "malloc failed" << std::endl;
    }

    return 0;
}
```

**关键要点：**

| 特性 | new/delete | malloc/free |
|------|------------|-------------|
| 类型 | 操作符 | 函数 |
| 构造/析构 | 调用 | 不调用 |
| 类型安全 | 有 | 无（需要转型） |
| 大小计算 | 自动 | 手动 |
| 失败处理 | 抛异常 | 返回 nullptr |
| 重载 | 可以 | 不可以 |
| 使用场景 | C++ 对象 | C 兼容、POD 类型 |

**常见陷阱：**
- 混用 `new`/`free` 或 `malloc`/`delete`（未定义行为）
- 对 C++ 对象使用 `malloc`/`free`

---

### 3.3 placement new

**问题：什么是 placement new？何时使用？**

```cpp
#include <iostream>
#include <new>

class MyClass {
public:
    MyClass(int x) : x_(x) {
        std::cout << "Constructor: " << x_ << std::endl;
    }
    ~MyClass() {
        std::cout << "Destructor: " << x_ << std::endl;
    }
    void print() {
        std::cout << "x = " << x_ << std::endl;
    }
private:
    int x_;
};

int main() {
    // 1. 在预分配的内存上构造对象
    char buffer[sizeof(MyClass)];  // 预分配内存

    // placement new：在指定位置构造对象
    MyClass* obj = new (buffer) MyClass(42);
    obj->print();

    // 必须显式调用析构函数
    obj->~MyClass();
    // 不需要 delete（内存不是通过 new 分配的）

    // 2. 用途：内存池
    const size_t pool_size = 1024;
    char* memory_pool = new char[pool_size];

    MyClass* obj1 = new (memory_pool) MyClass(1);
    MyClass* obj2 = new (memory_pool + sizeof(MyClass)) MyClass(2);

    obj1->~MyClass();
    obj2->~MyClass();

    delete[] memory_pool;

    return 0;
}
```

**关键要点：**
- Placement new 在指定内存位置构造对象，不分配新内存
- 用途：内存池、对象池、共享内存
- 必须显式调用析构函数
- 不能对 placement new 的对象使用 `delete`

**常见陷阱：**
- 忘记显式调用析构函数
- 对 placement new 对象使用 `delete`
- 内存对齐问题

---

## 4. 智能指针 (Smart Pointers)

### 4.1 为什么需要智能指针

**问题：为什么要使用智能指针？**

```cpp
#include <iostream>

// 问题1：忘记 delete 导致内存泄漏
void memoryLeak() {
    int* p = new int(42);
    // 忘记 delete p;
}  // 内存泄漏

// 问题2：异常导致资源未释放
void exceptionLeak() {
    int* p = new int(42);

    // 如果这里抛出异常，delete 不会执行
    if (some_condition) {
        throw std::runtime_error("Error");
    }

    delete p;  // 可能不会执行
}

// 问题3：多次 delete
void doubleFree() {
    int* p = new int(42);
    delete p;
    // delete p;  // 未定义行为
}

// 解决方案：智能指针（RAII）
void smartPointerSolution() {
    std::unique_ptr<int> p(new int(42));

    if (some_condition) {
        throw std::runtime_error("Error");
    }

    // 不需要 delete，智能指针自动释放
}  // p 离开作用域，自动释放内存
```

**关键要点：**
- 智能指针自动管理内存，避免内存泄漏
- 异常安全：无论如何退出作用域，都会释放资源
- 符合 RAII 原则

---

### 4.2 unique_ptr

**问题：unique_ptr 有什么特点？如何使用？**

```cpp
#include <memory>
#include <iostream>

class MyClass {
public:
    MyClass(int x) : x_(x) {
        std::cout << "Constructor: " << x_ << std::endl;
    }
    ~MyClass() {
        std::cout << "Destructor: " << x_ << std::endl;
    }
    void print() {
        std::cout << "x = " << x_ << std::endl;
    }
private:
    int x_;
};

int main() {
    // 1. 创建 unique_ptr
    std::unique_ptr<int> p1(new int(42));
    std::unique_ptr<int> p2 = std::make_unique<int>(100);  // C++14，推荐

    // 2. 访问对象
    std::cout << *p1 << std::endl;
    *p2 = 200;

    // 3. unique_ptr 独占所有权，不能拷贝
    // std::unique_ptr<int> p3 = p1;  // Error: 拷贝被禁用

    // 4. 可以移动
    std::unique_ptr<int> p3 = std::move(p1);  // p1 变为 nullptr
    if (p1 == nullptr) {
        std::cout << "p1 is now nullptr" << std::endl;
    }

    // 5. 管理数组
    std::unique_ptr<int[]> arr = std::make_unique<int[]>(10);
    arr[0] = 100;
    arr[1] = 200;

    // 6. 自定义删除器
    auto deleter = [](MyClass* p) {
        std::cout << "Custom deleter\n";
        delete p;
    };
    std::unique_ptr<MyClass, decltype(deleter)> p4(new MyClass(10), deleter);

    // 7. 手动释放
    std::unique_ptr<MyClass> p5 = std::make_unique<MyClass>(20);
    p5.reset();  // 立即释放
    p5.reset(new MyClass(30));  // 释放旧对象，管理新对象

    // 8. 获取原始指针
    std::unique_ptr<int> p6 = std::make_unique<int>(42);
    int* raw_ptr = p6.get();  // 不转移所有权
    std::cout << *raw_ptr << std::endl;

    // 9. 释放所有权
    int* raw_ptr2 = p6.release();  // p6 不再管理内存
    delete raw_ptr2;  // 必须手动 delete

    return 0;
}  // 所有 unique_ptr 离开作用域，自动释放资源
```

**关键要点：**
- `unique_ptr` 独占所有权，不能拷贝，只能移动
- 使用 `std::make_unique`（C++14）创建，更安全、更高效
- 零开销：和原始指针性能相同
- 适用场景：独占资源、函数返回值

**常见陷阱：**
- 尝试拷贝 `unique_ptr`
- 从 `get()` 获取的指针创建新的 `unique_ptr`（导致 double free）

---

### 4.3 shared_ptr

**问题：shared_ptr 如何实现共享所有权？**

```cpp
#include <memory>
#include <iostream>

class MyClass {
public:
    MyClass(int x) : x_(x) {
        std::cout << "Constructor: " << x_ << std::endl;
    }
    ~MyClass() {
        std::cout << "Destructor: " << x_ << std::endl;
    }
    int x_;
};

int main() {
    // 1. 创建 shared_ptr
    std::shared_ptr<MyClass> p1 = std::make_shared<MyClass>(42);
    std::cout << "use_count: " << p1.use_count() << std::endl;  // 1

    // 2. 拷贝：共享所有权，引用计数 +1
    std::shared_ptr<MyClass> p2 = p1;
    std::cout << "use_count: " << p1.use_count() << std::endl;  // 2
    std::cout << "use_count: " << p2.use_count() << std::endl;  // 2

    {
        std::shared_ptr<MyClass> p3 = p1;
        std::cout << "use_count: " << p1.use_count() << std::endl;  // 3
    }  // p3 离开作用域，引用计数 -1

    std::cout << "use_count: " << p1.use_count() << std::endl;  // 2

    // 3. reset：释放当前对象，引用计数 -1
    p2.reset();
    std::cout << "use_count: " << p1.use_count() << std::endl;  // 1

    // 4. 手动减少引用计数
    p1 = nullptr;
    std::cout << "Object destroyed" << std::endl;

    // 5. 循环引用问题（见下节）

    return 0;
}
```

**关键要点：**
- `shared_ptr` 使用引用计数实现共享所有权
- 拷贝时引用计数 +1，销毁时引用计数 -1
- 引用计数为 0 时自动释放资源
- 使用 `std::make_shared` 更高效（一次分配）
- 有开销：引用计数的空间和时间开销

**常见陷阱：**
- 循环引用导致内存泄漏（需要 `weak_ptr`）
- 不要从原始指针创建多个 `shared_ptr`

---

### 4.4 weak_ptr

**问题：weak_ptr 解决什么问题？如何使用？**

```cpp
#include <memory>
#include <iostream>

// 循环引用问题
class B;

class A {
public:
    std::shared_ptr<B> b_ptr;
    ~A() { std::cout << "A destroyed\n"; }
};

class B {
public:
    std::shared_ptr<A> a_ptr;  // 循环引用！
    ~B() { std::cout << "B destroyed\n"; }
};

void circularReference() {
    std::shared_ptr<A> a = std::make_shared<A>();
    std::shared_ptr<B> b = std::make_shared<B>();

    a->b_ptr = b;  // a 持有 b
    b->a_ptr = a;  // b 持有 a

    // 循环引用：a 和 b 互相持有，引用计数不会为 0，内存泄漏！
}

// 解决方案：使用 weak_ptr
class B_fixed;

class A_fixed {
public:
    std::shared_ptr<B_fixed> b_ptr;
    ~A_fixed() { std::cout << "A_fixed destroyed\n"; }
};

class B_fixed {
public:
    std::weak_ptr<A_fixed> a_ptr;  // 使用 weak_ptr 打破循环
    ~B_fixed() { std::cout << "B_fixed destroyed\n"; }
};

void fixedReference() {
    std::shared_ptr<A_fixed> a = std::make_shared<A_fixed>();
    std::shared_ptr<B_fixed> b = std::make_shared<B_fixed>();

    a->b_ptr = b;
    b->a_ptr = a;  // weak_ptr 不增加引用计数

    // a 和 b 都会被正确释放
}

// weak_ptr 使用
int main() {
    std::shared_ptr<int> sp = std::make_shared<int>(42);
    std::weak_ptr<int> wp = sp;  // weak_ptr 不增加引用计数

    std::cout << "use_count: " << sp.use_count() << std::endl;  // 1
    std::cout << "weak use_count: " << wp.use_count() << std::endl;  // 1

    // 使用 weak_ptr：先转换为 shared_ptr
    if (std::shared_ptr<int> sp2 = wp.lock()) {  // lock() 返回 shared_ptr
        std::cout << "Value: " << *sp2 << std::endl;
        std::cout << "use_count: " << sp.use_count() << std::endl;  // 2
    }

    sp.reset();  // 释放资源

    // 对象已销毁，lock() 返回空指针
    if (std::shared_ptr<int> sp3 = wp.lock()) {
        std::cout << "Object exists" << std::endl;
    } else {
        std::cout << "Object destroyed" << std::endl;
    }

    std::cout << "expired: " << wp.expired() << std::endl;  // true

    return 0;
}
```

**关键要点：**
- `weak_ptr` 不增加引用计数，用于观察 `shared_ptr` 管理的对象
- 用途：打破循环引用、缓存、观察者模式
- 使用前必须通过 `lock()` 转换为 `shared_ptr`
- `expired()` 检查对象是否已销毁

**常见陷阱：**
- 不检查 `lock()` 返回值就使用
- 混淆 `weak_ptr` 和 `shared_ptr` 的用途

---

### 4.5 智能指针的选择

**问题：何时使用哪种智能指针？**

```cpp
// 1. unique_ptr：独占所有权（默认选择）
std::unique_ptr<MyClass> createObject() {
    return std::make_unique<MyClass>(42);
}

// 2. shared_ptr：共享所有权
class Cache {
    std::unordered_map<std::string, std::shared_ptr<Data>> cache_;

    std::shared_ptr<Data> get(const std::string& key) {
        return cache_[key];  // 共享所有权
    }
};

// 3. weak_ptr：观察者模式
class Subject;

class Observer {
    std::weak_ptr<Subject> subject_;  // 不持有 Subject

    void notify() {
        if (auto sub = subject_.lock()) {  // 检查 Subject 是否存活
            // 使用 sub
        }
    }
};
```

**选择指南：**

| 场景 | 智能指针 | 原因 |
|------|----------|------|
| 默认选择 | `unique_ptr` | 性能最好，语义明确 |
| 多个所有者 | `shared_ptr` | 需要共享所有权 |
| 打破循环引用 | `weak_ptr` | 不增加引用计数 |
| 观察对象 | `weak_ptr` | 不影响对象生命周期 |
| 数组 | `unique_ptr<T[]>` 或 `vector` | 优先使用 `vector` |

---

## 5. RAII (Resource Acquisition Is Initialization)

### 5.1 RAII 原则

**问题：什么是 RAII？为什么重要？**

```cpp
#include <iostream>
#include <fstream>
#include <mutex>

// 反例：手动管理资源（容易出错）
void badResourceManagement() {
    std::FILE* file = std::fopen("data.txt", "r");
    if (!file) {
        return;  // 忘记关闭？不，这里还没打开
    }

    // 处理文件
    if (error_occurred) {
        // 忘记 fclose(file)！内存泄漏
        return;
    }

    std::fclose(file);  // 可能不会执行
}

// 正确：RAII 封装资源
class FileGuard {
public:
    FileGuard(const char* filename, const char* mode) {
        file_ = std::fopen(filename, mode);
        if (!file_) {
            throw std::runtime_error("Failed to open file");
        }
    }

    ~FileGuard() {
        if (file_) {
            std::fclose(file_);
            std::cout << "File closed\n";
        }
    }

    std::FILE* get() { return file_; }

    // 禁用拷贝
    FileGuard(const FileGuard&) = delete;
    FileGuard& operator=(const FileGuard&) = delete;

private:
    std::FILE* file_;
};

void goodResourceManagement() {
    FileGuard file("data.txt", "r");

    // 处理文件
    if (error_occurred) {
        return;  // FileGuard 析构函数自动关闭文件
    }

    // ...
}  // 无论如何退出，文件都会被正确关闭

// 更好：使用标准库的 RAII 类
void bestPractice() {
    std::ifstream file("data.txt");  // RAII

    if (error_occurred) {
        return;  // 自动关闭
    }

    // ...
}  // 自动关闭

// 互斥锁的 RAII
std::mutex mtx;

void threadSafeFunction() {
    std::lock_guard<std::mutex> lock(mtx);  // RAII 锁

    // 临界区
    if (error_occurred) {
        return;  // 自动解锁
    }

    // ...
}  // 自动解锁
```

**关键要点：**
- **RAII**：资源获取即初始化，资源释放即销毁
- 在构造函数中获取资源，在析构函数中释放资源
- 利用 C++ 的自动析构机制，确保资源总是被释放
- 异常安全：无论如何退出，资源都会被正确释放

**RAII 管理的资源：**
- 内存：智能指针
- 文件：`std::fstream`
- 锁：`std::lock_guard`、`std::unique_lock`
- 数据库连接、网络套接字等

**常见陷阱：**
- 忘记遵循 RAII 原则，手动管理资源
- 在构造函数中获取资源但忘记在析构函数中释放

---

## 6. 内存泄漏检测

### 6.1 常见内存泄漏场景

**问题：有哪些常见的内存泄漏场景？**

```cpp
#include <iostream>
#include <memory>

// 1. 忘记 delete
void leak1() {
    int* p = new int(42);
    // 忘记 delete p;
}

// 2. 异常导致未 delete
void leak2() {
    int* p = new int(42);
    throw std::runtime_error("Error");  // p 未被 delete
    delete p;  // 不会执行
}

// 3. 循环引用
class Node {
public:
    std::shared_ptr<Node> next;
    std::shared_ptr<Node> prev;  // 循环引用
    ~Node() { std::cout << "Node destroyed\n"; }
};

void leak3() {
    auto node1 = std::make_shared<Node>();
    auto node2 = std::make_shared<Node>();
    node1->next = node2;
    node2->prev = node1;  // 循环引用，内存泄漏
}

// 4. 容器中的原始指针
void leak4() {
    std::vector<int*> vec;
    vec.push_back(new int(1));
    vec.push_back(new int(2));
    // vector 销毁时不会 delete 指针指向的对象
}

// 5. 虚析构函数缺失
class Base {
public:
    ~Base() { std::cout << "Base destroyed\n"; }  // 非虚析构
};

class Derived : public Base {
public:
    Derived() { data_ = new int[100]; }
    ~Derived() {
        delete[] data_;
        std::cout << "Derived destroyed\n";
    }
private:
    int* data_;
};

void leak5() {
    Base* p = new Derived();
    delete p;  // 只调用 Base 析构，Derived 析构不执行，泄漏！
}

// 正确做法
void fixed() {
    // 1. 使用智能指针
    auto p = std::make_unique<int>(42);

    // 2. 异常安全
    try {
        auto p2 = std::make_unique<int>(42);
        throw std::runtime_error("Error");
    } catch (...) {
        // p2 自动释放
    }

    // 3. 使用 weak_ptr 打破循环
    class NodeFixed {
    public:
        std::shared_ptr<NodeFixed> next;
        std::weak_ptr<NodeFixed> prev;  // weak_ptr
    };

    // 4. 容器使用智能指针
    std::vector<std::unique_ptr<int>> vec;
    vec.push_back(std::make_unique<int>(1));

    // 5. 虚析构函数
    class BaseFixed {
    public:
        virtual ~BaseFixed() = default;
    };
}
```

**内存泄漏检测工具：**
- **Valgrind**（Linux）：`valgrind --leak-check=full ./program`
- **AddressSanitizer**（GCC/Clang）：编译时加 `-fsanitize=address`
- **Visual Studio**（Windows）：内置内存泄漏检测
- **Dr. Memory**（跨平台）

---

## 7. 内存对齐

### 7.1 内存对齐基础

**问题：什么是内存对齐？为什么需要？**

```cpp
#include <iostream>

struct A {
    char c;    // 1 字节
    int i;     // 4 字节
    short s;   // 2 字节
};

struct B {
    char c;    // 1 字节
    char pad1[3];  // 填充 3 字节
    int i;     // 4 字节
    short s;   // 2 字节
    char pad2[2];  // 填充 2 字节
};

struct C {
    int i;     // 4 字节
    char c;    // 1 字节
    short s;   // 2 字节
    char pad[1];  // 填充 1 字节
};

int main() {
    std::cout << "sizeof(char): " << sizeof(char) << std::endl;    // 1
    std::cout << "sizeof(short): " << sizeof(short) << std::endl;  // 2
    std::cout << "sizeof(int): " << sizeof(int) << std::endl;      // 4

    std::cout << "sizeof(A): " << sizeof(A) << std::endl;  // 12 (不是 7)
    std::cout << "sizeof(B): " << sizeof(B) << std::endl;  // 12
    std::cout << "sizeof(C): " << sizeof(C) << std::endl;  // 8

    std::cout << "alignof(A): " << alignof(A) << std::endl;  // 4

    return 0;
}

/*
A 的内存布局（按声明顺序）：
+---+---+---+---+---+---+---+---+---+---+---+---+
| c |pad|pad|pad| i | i | i | i | s | s |pad|pad|
+---+---+---+---+---+---+---+---+---+---+---+---+
  1   1   1   1   4   4   4   4   2   2   1   1  = 12 字节

C 的内存布局（优化后）：
+---+---+---+---+---+---+---+---+
| i | i | i | i | c | s | s |pad|
+---+---+---+---+---+---+---+---+
  4   4   4   4   1   2   2   1  = 8 字节
*/
```

**关键要点：**
- **内存对齐**：数据地址必须是其大小的整数倍
- 对齐原因：CPU 访问对齐的数据更高效，某些架构要求对齐
- 结构体大小是最大成员对齐值的整数倍
- 优化：将大字段放在前面，减少填充

**常见陷阱：**
- 忽略内存对齐，导致结构体大小超出预期
- 跨平台时对齐规则不同

---

### 7.2 控制内存对齐

**问题：如何控制内存对齐？**

```cpp
#include <iostream>

// 1. 默认对齐
struct Default {
    char c;
    int i;
};

// 2. 指定对齐（C++11）
struct alignas(16) Aligned16 {
    char c;
    int i;
};

// 3. 取消对齐（编译器特定）
#pragma pack(push, 1)
struct Packed {
    char c;
    int i;
    short s;
};
#pragma pack(pop)

// 4. 指定成员对齐
struct MemberAligned {
    char c;
    alignas(8) int i;  // i 对齐到 8 字节
};

int main() {
    std::cout << "sizeof(Default): " << sizeof(Default) << std::endl;       // 8
    std::cout << "sizeof(Aligned16): " << sizeof(Aligned16) << std::endl;   // 16
    std::cout << "sizeof(Packed): " << sizeof(Packed) << std::endl;         // 7
    std::cout << "sizeof(MemberAligned): " << sizeof(MemberAligned) << std::endl;  // 16

    return 0;
}
```

**关键要点：**
- `alignas` 指定对齐值（C++11）
- `#pragma pack` 取消对齐（编译器特定）
- 取消对齐会降低性能，但减少内存占用

**常见陷阱：**
- 过度使用 `#pragma pack` 导致性能下降
- `alignas` 只能增加对齐，不能减少

---

## 8. 内存优化技巧

### 8.1 对象池 (Object Pool)

**问题：如何减少频繁 new/delete 的开销？**

```cpp
#include <iostream>
#include <vector>
#include <memory>

template<typename T>
class ObjectPool {
public:
    ObjectPool(size_t size) {
        pool_.reserve(size);
    }

    template<typename... Args>
    T* allocate(Args&&... args) {
        if (pool_.empty()) {
            return new T(std::forward<Args>(args)...);
        } else {
            T* obj = pool_.back();
            pool_.pop_back();
            new (obj) T(std::forward<Args>(args)...);  // placement new
            return obj;
        }
    }

    void deallocate(T* obj) {
        obj->~T();  // 显式调用析构函数
        pool_.push_back(obj);
    }

    ~ObjectPool() {
        for (T* obj : pool_) {
            delete obj;
        }
    }

private:
    std::vector<T*> pool_;
};

class MyClass {
public:
    MyClass(int x) : x_(x) {}
    int x_;
};

int main() {
    ObjectPool<MyClass> pool(10);

    MyClass* obj1 = pool.allocate(42);
    MyClass* obj2 = pool.allocate(100);

    pool.deallocate(obj1);  // 回收到池中
    pool.deallocate(obj2);

    MyClass* obj3 = pool.allocate(200);  // 从池中获取

    pool.deallocate(obj3);

    return 0;
}
```

**关键要点：**
- 对象池预分配对象，减少 new/delete 开销
- 适用于频繁创建/销毁的小对象
- 使用 placement new 在池中重用内存

---

## 9. 总结

### 核心要点
1. **内存分区**：栈（自动）、堆（手动）、数据段、代码段
2. **栈 vs 堆**：栈快但有限，堆灵活但慢，需手动管理
3. **new/delete**：调用构造/析构函数，优于 malloc/free
4. **智能指针**：`unique_ptr`（独占）、`shared_ptr`（共享）、`weak_ptr`（观察）
5. **RAII**：资源获取即初始化，确保资源总是被释放
6. **内存泄漏**：使用智能指针、虚析构函数、打破循环引用
7. **内存对齐**：提高性能，注意结构体大小

### 最佳实践
- 优先使用栈内存和智能指针
- 遵循 RAII 原则
- 避免裸 new/delete
- 使用 `make_unique` 和 `make_shared`
- 基类必须有虚析构函数
- 使用工具检测内存泄漏

### 常见面试题
- 栈和堆的区别？
- new 和 malloc 的区别？
- 智能指针的实现原理？
- 如何避免内存泄漏？
- 什么是 RAII？
- 循环引用如何解决？
- 内存对齐的作用？

---

## 参考资料
- *Effective C++* by Scott Meyers (Item 13-17)
- *Effective Modern C++* by Scott Meyers (Item 18-22)
- *C++ Primer* (5th Edition), Chapter 12
- ISO C++ Core Guidelines - Resource Management
