# 14 - 异常与错误处理 (Exception Handling)

## 简介

异常处理是 C++ 处理运行时错误的机制，允许将错误处理代码与正常逻辑分离。理解异常的工作原理、性能影响和最佳实践对于编写健壮的代码至关重要。

**核心特点**：
- **try-catch-throw**：异常处理三要素
- **异常安全性**：保证程序状态一致
- **RAII**：资源管理与异常
- **noexcept**：无异常保证

---

## 1. 异常基础

### 1.1 异常的基本语法？

**问题描述**：如何抛出和捕获异常？

**基本用法**：

```cpp
#include <iostream>
#include <stdexcept>

double divide(double a, double b) {
    if (b == 0) {
        throw std::invalid_argument("Division by zero");
    }
    return a / b;
}

void exception_basic() {
    try {
        double result = divide(10, 0);
        std::cout << "Result: " << result << "\n";
    } catch (const std::invalid_argument& e) {
        std::cout << "Error: " << e.what() << "\n";
    }
}
```

**多个 catch 块**：

```cpp
void multiple_catch() {
    try {
        // 可能抛出不同类型的异常
        throw std::runtime_error("Runtime error");
    } catch (const std::invalid_argument& e) {
        std::cout << "Invalid argument: " << e.what() << "\n";
    } catch (const std::runtime_error& e) {
        std::cout << "Runtime error: " << e.what() << "\n";
    } catch (const std::exception& e) {
        // 捕获所有标准异常
        std::cout << "Standard exception: " << e.what() << "\n";
    } catch (...) {
        // 捕获所有异常（包括非标准异常）
        std::cout << "Unknown exception\n";
    }
}
```

**捕获顺序很重要**：

```cpp
// ❌ 错误：基类在前，派生类的 catch 永远不会执行
try {
    // ...
} catch (const std::exception& e) {
    // 捕获所有标准异常
} catch (const std::runtime_error& e) {
    // ❌ 永远不会执行（被上面的 catch 捕获）
}

// ✅ 正确：派生类在前，基类在后
try {
    // ...
} catch (const std::runtime_error& e) {
    // 先捕获特定异常
} catch (const std::exception& e) {
    // 再捕获通用异常
}
```

**重新抛出异常**：

```cpp
void rethrow_demo() {
    try {
        throw std::runtime_error("Error occurred");
    } catch (const std::exception& e) {
        std::cout << "Caught: " << e.what() << "\n";
        // 重新抛出当前异常
        throw;  // 不是 throw e;
    }
}

// throw vs throw e 的区别
void throw_vs_throw_e() {
    try {
        throw std::runtime_error("Original error");
    } catch (std::exception& e) {
        // throw e;    // ❌ 对象切片（如果 e 是派生类）
        throw;         // ✅ 重新抛出原始异常（保留类型）
    }
}
```

**关键要点**：
- **throw**：抛出异常
- **try**：标记可能抛出异常的代码块
- **catch**：捕获并处理异常
- **捕获顺序**：派生类在前，基类在后
- **重新抛出**：使用 `throw;` 而不是 `throw e;`

---

### 1.2 标准异常类层次？

**问题描述**：C++ 标准库提供哪些异常类？

**异常类层次**：

```
std::exception
├── std::logic_error
│   ├── std::invalid_argument
│   ├── std::domain_error
│   ├── std::length_error
│   ├── std::out_of_range
│   └── std::future_error
├── std::runtime_error
│   ├── std::range_error
│   ├── std::overflow_error
│   ├── std::underflow_error
│   └── std::system_error
├── std::bad_alloc
├── std::bad_cast
├── std::bad_typeid
├── std::bad_exception
└── std::bad_function_call
```

**使用示例**：

```cpp
#include <stdexcept>
#include <iostream>

void standard_exceptions() {
    // logic_error：程序逻辑错误（可预防）
    try {
        throw std::invalid_argument("Invalid argument");
    } catch (const std::logic_error& e) {
        std::cout << "Logic error: " << e.what() << "\n";
    }

    // runtime_error：运行时错误（难以预防）
    try {
        throw std::runtime_error("Runtime error");
    } catch (const std::runtime_error& e) {
        std::cout << "Runtime error: " << e.what() << "\n";
    }

    // bad_alloc：内存分配失败
    try {
        int* huge_array = new int[999999999999];
    } catch (const std::bad_alloc& e) {
        std::cout << "Memory allocation failed: " << e.what() << "\n";
    }

    // out_of_range：越界访问
    try {
        std::vector<int> vec = {1, 2, 3};
        vec.at(10);  // 抛出 out_of_range
    } catch (const std::out_of_range& e) {
        std::cout << "Out of range: " << e.what() << "\n";
    }
}
```

**选择合适的异常类**：

| 异常类 | 使用场景 |
|--------|---------|
| `invalid_argument` | 函数参数无效 |
| `out_of_range` | 索引越界 |
| `length_error` | 容器大小超限 |
| `runtime_error` | 通用运行时错误 |
| `bad_alloc` | 内存分配失败 |
| `bad_cast` | `dynamic_cast` 失败 |

---

### 1.3 自定义异常类？

**问题描述**：如何创建自定义异常？

**基本自定义异常**：

```cpp
// 继承 std::exception
class MyException : public std::exception {
private:
    std::string message;

public:
    explicit MyException(const std::string& msg) : message(msg) {}

    const char* what() const noexcept override {
        return message.c_str();
    }
};

void custom_exception_demo() {
    try {
        throw MyException("Something went wrong");
    } catch (const MyException& e) {
        std::cout << "Custom exception: " << e.what() << "\n";
    }
}
```

**继承标准异常类（推荐）**：

```cpp
// 继承 std::runtime_error
class FileOpenError : public std::runtime_error {
private:
    std::string filename;

public:
    explicit FileOpenError(const std::string& fname)
        : std::runtime_error("Failed to open file: " + fname),
          filename(fname) {}

    const std::string& get_filename() const {
        return filename;
    }
};

void file_exception_demo() {
    try {
        throw FileOpenError("data.txt");
    } catch (const FileOpenError& e) {
        std::cout << "Error: " << e.what() << "\n";
        std::cout << "File: " << e.get_filename() << "\n";
    }
}
```

**异常类层次设计**：

```cpp
// 基础异常
class DatabaseException : public std::runtime_error {
    using std::runtime_error::runtime_error;
};

// 特定异常
class ConnectionError : public DatabaseException {
    using DatabaseException::DatabaseException;
};

class QueryError : public DatabaseException {
private:
    std::string query;

public:
    QueryError(const std::string& msg, const std::string& q)
        : DatabaseException(msg), query(q) {}

    const std::string& get_query() const { return query; }
};

void exception_hierarchy_demo() {
    try {
        throw QueryError("Invalid SQL", "SELECT * FROM non_existent_table");
    } catch (const QueryError& e) {
        std::cout << "Query error: " << e.what() << "\n";
        std::cout << "Query: " << e.get_query() << "\n";
    } catch (const DatabaseException& e) {
        std::cout << "Database error: " << e.what() << "\n";
    }
}
```

**关键要点**：
- **继承 `std::exception`** 或其派生类
- **重写 `what()`** 方法，返回错误信息
- **`what()` 必须是 `noexcept`**
- **使用 `using` 继承构造函数**（C++11）

---

## 2. 异常安全性 (Exception Safety)

### 2.1 什么是异常安全性？

**问题描述**：异常抛出后，程序状态如何保证？

**三个级别的异常安全保证**：

1. **基本保证 (Basic Guarantee)**
   - 不泄漏资源
   - 对象处于有效（但可能不一致）状态

2. **强保证 (Strong Guarantee)**
   - 操作要么成功，要么失败
   - 失败时状态回滚到操作前（事务性）

3. **不抛异常保证 (No-throw Guarantee)**
   - 保证不抛出异常
   - 通常用 `noexcept` 标记

**示例**：

```cpp
class Container {
    int* data;
    size_t size;

public:
    // ❌ 无异常安全：内存泄漏
    void bad_push(int value) {
        int* new_data = new int[size + 1];  // 可能抛出 bad_alloc
        // 如果这里抛异常，new_data 泄漏
        std::copy(data, data + size, new_data);
        new_data[size] = value;
        delete[] data;
        data = new_data;
        ++size;
    }

    // ✅ 基本保证：不泄漏资源
    void basic_push(int value) {
        int* new_data = new int[size + 1];
        try {
            std::copy(data, data + size, new_data);
            new_data[size] = value;
        } catch (...) {
            delete[] new_data;  // 清理资源
            throw;
        }
        delete[] data;
        data = new_data;
        ++size;
    }

    // ✅ 强保证：失败时状态不变
    void strong_push(int value) {
        int* new_data = new int[size + 1];
        std::copy(data, data + size, new_data);
        new_data[size] = value;
        // 到这里都没有修改对象状态
        delete[] data;  // noexcept
        data = new_data;
        ++size;
    }

    // ✅ 不抛异常保证
    size_t get_size() const noexcept {
        return size;
    }
};
```

**关键要点**：
- **基本保证**：最低要求，不泄漏资源
- **强保证**：理想目标，"全或无"语义
- **不抛异常**：析构函数、移动操作、swap 等

---

### 2.2 Copy-and-Swap 惯用法？

**问题描述**：如何实现强异常安全的赋值运算符？

```cpp
class MyClass {
    int* data;
    size_t size;

public:
    MyClass(size_t n) : data(new int[n]), size(n) {}

    ~MyClass() {
        delete[] data;
    }

    // ❌ 不安全的赋值
    MyClass& operator=(const MyClass& other) {
        if (this != &other) {
            delete[] data;  // ⚠️ 删除旧数据
            data = new int[other.size];  // ❌ 如果这里抛异常，data 是悬空指针
            size = other.size;
            std::copy(other.data, other.data + size, data);
        }
        return *this;
    }

    // ✅ 强异常安全的赋值（Copy-and-Swap）
    MyClass& operator=(const MyClass& other) {
        MyClass temp(other);  // 拷贝构造（可能抛异常）
        swap(temp);           // noexcept
        return *this;
    }  // temp 析构，自动清理旧资源

    void swap(MyClass& other) noexcept {
        std::swap(data, other.data);
        std::swap(size, other.size);
    }
};
```

**关键要点**：
- **先拷贝，再交换**
- 拷贝可能失败，但不影响原对象
- 交换操作是 `noexcept`

---

### 2.3 RAII 与异常安全？

**问题描述**：RAII 如何保证异常安全？

**RAII (Resource Acquisition Is Initialization)**：
- 资源在构造时获取
- 资源在析构时释放
- 异常抛出时自动调用析构函数

**示例**：

```cpp
// ❌ 不安全：忘记释放资源
void bad_resource_management() {
    int* ptr = new int[100];
    process_data(ptr);  // 可能抛异常
    delete[] ptr;       // ❌ 异常时不会执行
}

// ✅ 安全：使用 RAII
void good_resource_management() {
    std::unique_ptr<int[]> ptr(new int[100]);
    process_data(ptr.get());  // 可能抛异常
    // ✅ 无论是否抛异常，ptr 析构时自动释放资源
}

// RAII 包装器
class FileHandle {
    FILE* file;

public:
    explicit FileHandle(const char* filename) {
        file = fopen(filename, "r");
        if (!file) {
            throw std::runtime_error("Failed to open file");
        }
    }

    ~FileHandle() {
        if (file) {
            fclose(file);  // 自动关闭文件
        }
    }

    FILE* get() { return file; }

    // 禁止拷贝
    FileHandle(const FileHandle&) = delete;
    FileHandle& operator=(const FileHandle&) = delete;
};

void raii_demo() {
    FileHandle file("data.txt");
    // ... 使用文件
    // 异常时也会自动关闭文件
}
```

**标准库的 RAII 类**：

```cpp
void standard_raii() {
    // 智能指针
    std::unique_ptr<int> ptr(new int(10));
    std::shared_ptr<int> sptr = std::make_shared<int>(20);

    // 锁
    std::mutex mtx;
    std::lock_guard<std::mutex> lock(mtx);

    // 文件流
    std::ifstream file("data.txt");

    // ... 使用资源
    // 作用域结束时自动释放
}
```

**关键要点**：
- **RAII 是异常安全的基石**
- 使用智能指针、`lock_guard`、文件流等
- **析构函数不应抛出异常**

---

## 3. noexcept 规范

### 3.1 `noexcept` 的作用？

**问题描述**：`noexcept` 关键字有什么用？

**基本用法**：

```cpp
// 声明函数不抛出异常
int safe_func() noexcept {
    return 42;
}

// 条件 noexcept
template<typename T>
void swap(T& a, T& b) noexcept(noexcept(T(std::move(a)))) {
    T temp(std::move(a));
    a = std::move(b);
    b = std::move(temp);
}
```

**检查是否 noexcept**：

```cpp
void func1() noexcept {}
void func2() {}

static_assert(noexcept(func1()), "func1 is noexcept");
static_assert(!noexcept(func2()), "func2 may throw");
```

**noexcept 的好处**：

1. **性能优化**：
```cpp
std::vector<MyClass> vec;
vec.push_back(obj);

// 如果移动构造是 noexcept，vector 扩容时用移动
// 否则，使用拷贝（更安全但更慢）
```

2. **编译期检查**：
```cpp
void critical_func() noexcept {
    // 如果这里调用可能抛异常的函数，编译器警告
    may_throw_func();  // 警告
}
```

3. **接口契约**：
```cpp
// 明确告诉调用者：不会抛异常
void destroy() noexcept;
```

**何时使用 noexcept**：

| 场景 | 是否 noexcept |
|------|--------------|
| 析构函数 | ✅ 是（默认） |
| 移动构造/赋值 | ✅ 尽量是 |
| `swap` | ✅ 是 |
| 默认构造 | ⚠️ 如果简单 |
| 拷贝构造/赋值 | ❌ 通常否 |
| 一般函数 | ⚠️ 视情况 |

**常见陷阱**：

```cpp
// ❌ 错误：noexcept 函数抛出异常 → std::terminate
void bad_noexcept() noexcept {
    throw std::runtime_error("Error");  // ❌ 程序终止
}

// ❌ 错误：析构函数抛异常
class BadClass {
    ~BadClass() {  // 默认是 noexcept
        throw std::runtime_error("Error in destructor");  // ❌ std::terminate
    }
};
```

**关键要点**：
- **noexcept** 声明函数不抛异常
- 违反 noexcept 会调用 `std::terminate`
- **析构函数默认是 noexcept**
- **移动操作应该是 noexcept**

---

## 4. 异常 vs 错误码

### 4.1 何时使用异常？何时使用错误码？

**问题描述**：异常和错误码各有什么优缺点？

**异常的优点**：
- 错误处理与业务逻辑分离
- 强制处理错误（不能忽略）
- 自动传播到调用栈
- 支持 RAII，异常安全

**异常的缺点**：
- 性能开销（抛出时）
- 控制流不明显
- 二进制体积增大
- 不适合嵌入式/实时系统

**错误码的优点**：
- 性能可预测
- 控制流明显
- 零开销（无异常）
- 适合 C 互操作

**错误码的缺点**：
- 容易忽略错误
- 污染正常逻辑
- 需要逐层传递
- 资源管理困难

**使用指南**：

```cpp
// ✅ 使用异常：构造函数失败
class File {
public:
    File(const std::string& path) {
        // 构造函数无法返回错误码
        if (!open(path)) {
            throw std::runtime_error("Failed to open file");
        }
    }
};

// ✅ 使用异常：深层调用栈的错误传播
void level3() {
    throw std::runtime_error("Error in level3");
}

void level2() {
    level3();  // 异常自动传播
}

void level1() {
    try {
        level2();
    } catch (const std::exception& e) {
        // 在合适的层级处理
    }
}

// ✅ 使用错误码：预期的失败（不是异常）
bool find_user(int user_id, User& out) {
    // 找不到用户是预期的情况，不是异常
    if (database.find(user_id, out)) {
        return true;
    }
    return false;
}

// ✅ 使用错误码：性能敏感路径
bool fast_parse(const char* str, int& result) noexcept {
    // 解析失败很常见，不用异常
    return try_parse(str, result);
}
```

**C++17/20 的替代方案**：

```cpp
#include <optional>
#include <expected>  // C++23

// std::optional：可能没有值
std::optional<int> parse_int(const std::string& str) {
    try {
        return std::stoi(str);
    } catch (...) {
        return std::nullopt;
    }
}

// std::expected (C++23)：值或错误
std::expected<int, std::string> safe_divide(int a, int b) {
    if (b == 0) {
        return std::unexpected("Division by zero");
    }
    return a / b;
}

void modern_error_handling() {
    // optional
    if (auto result = parse_int("123")) {
        std::cout << "Parsed: " << *result << "\n";
    } else {
        std::cout << "Parse failed\n";
    }

    // expected
    auto result = safe_divide(10, 0);
    if (result) {
        std::cout << "Result: " << *result << "\n";
    } else {
        std::cout << "Error: " << result.error() << "\n";
    }
}
```

---

## 5. 异常的性能

### 5.1 异常的性能影响？

**问题描述**：异常有多慢？

**零开销原则**：
- **不抛异常时**：几乎零开销
- **抛异常时**：非常慢（1000+ 倍）

**实现方式**：

1. **表驱动（Zero-Cost Exception）**：
   - GCC、Clang 使用
   - 不抛异常时：零开销
   - 抛异常时：慢（栈展开需要查表）

2. **Setjmp/Longjmp**：
   - 老式实现
   - 不抛异常时：有开销（保存上下文）
   - 抛异常时：较快

**性能测试**：

```cpp
#include <chrono>

// 无异常版本
int normal_path(int n) {
    int sum = 0;
    for (int i = 0; i < n; ++i) {
        sum += i;
    }
    return sum;
}

// 异常版本
int exception_path(int n) {
    int sum = 0;
    for (int i = 0; i < n; ++i) {
        try {
            sum += i;
        } catch (...) {
            // 不会进入
        }
    }
    return sum;
}

void performance_test() {
    const int N = 1000000;

    auto start = std::chrono::high_resolution_clock::now();
    int result1 = normal_path(N);
    auto end = std::chrono::high_resolution_clock::now();
    auto duration1 = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

    start = std::chrono::high_resolution_clock::now();
    int result2 = exception_path(N);
    end = std::chrono::high_resolution_clock::now();
    auto duration2 = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

    std::cout << "Normal: " << duration1.count() << " us\n";
    std::cout << "Exception: " << duration2.count() << " us\n";
    // 结果：几乎相同（零开销）
}
```

**抛出异常的开销**：

```cpp
void throw_test() {
    try {
        throw std::runtime_error("Error");
    } catch (...) {
        // 捕获异常：非常慢（数千纳秒）
    }
}
// 抛出异常比正常返回慢 1000+ 倍
```

**关键要点**：
- **正常路径**：零开销
- **异常路径**：非常慢
- **不要用异常控制流**（如循环退出）
- 异常用于**异常情况**，不是常规逻辑

---

## 6. 常见陷阱

### 6.1 析构函数抛异常？

**问题描述**：析构函数抛异常会怎样？

```cpp
// ❌ 致命错误：析构函数抛异常
class BadClass {
public:
    ~BadClass() {  // 默认 noexcept(true)
        throw std::runtime_error("Error in destructor");  // ❌
    }
};

void destructor_exception() {
    try {
        BadClass obj;
        throw std::runtime_error("First exception");
        // obj 析构时抛出第二个异常 → std::terminate
    } catch (...) {
        // 永远不会到达这里
    }
}
```

**为什么析构函数不能抛异常？**
- 栈展开时调用析构函数
- 如果析构函数抛异常 → 两个异常同时存在 → `std::terminate`

**正确做法**：

```cpp
class GoodClass {
public:
    ~GoodClass() noexcept {
        try {
            risky_cleanup();
        } catch (...) {
            // 记录错误，但不抛出
            std::cerr << "Error during cleanup\n";
        }
    }
};
```

---

### 6.2 资源泄漏？

**问题描述**：异常导致资源未释放。

```cpp
// ❌ 错误：异常导致内存泄漏
void bad_resource() {
    int* ptr = new int[100];
    process_data(ptr);  // 可能抛异常
    delete[] ptr;       // ❌ 异常时不会执行
}

// ✅ 正确：使用 RAII
void good_resource() {
    std::unique_ptr<int[]> ptr(new int[100]);
    process_data(ptr.get());
    // ✅ 异常时自动释放
}

// ✅ 正确：手动 try-catch
void manual_cleanup() {
    int* ptr = new int[100];
    try {
        process_data(ptr);
    } catch (...) {
        delete[] ptr;
        throw;
    }
    delete[] ptr;
}
```

---

### 6.3 异常规范不匹配？

**问题描述**：`noexcept` 函数调用可能抛异常的函数。

```cpp
// ❌ 危险：noexcept 函数调用可能抛异常的函数
void risky() {
    throw std::runtime_error("Error");
}

void bad_noexcept() noexcept {
    risky();  // ⚠️ 如果 risky() 抛异常 → std::terminate
}

// ✅ 正确：捕获所有异常
void good_noexcept() noexcept {
    try {
        risky();
    } catch (...) {
        // 处理或记录错误
    }
}
```

---

## 7. 最佳实践

### 7.1 异常使用指南

**DO（应该做）**：

1. **构造函数失败时抛异常**
```cpp
class File {
public:
    File(const std::string& path) {
        if (!open(path)) {
            throw std::runtime_error("Failed to open file");
        }
    }
};
```

2. **使用 RAII 管理资源**
```cpp
void good_practice() {
    std::unique_ptr<int> ptr(new int(10));
    std::lock_guard<std::mutex> lock(mtx);
    // 异常安全
}
```

3. **按引用捕获异常**
```cpp
try {
    // ...
} catch (const std::exception& e) {  // ✅ const&
    // ...
}
```

4. **重新抛出使用 `throw;`**
```cpp
try {
    // ...
} catch (const std::exception& e) {
    log_error(e);
    throw;  // ✅ 保留原始类型
}
```

5. **析构函数、移动操作标记 noexcept**
```cpp
~MyClass() noexcept;
MyClass(MyClass&&) noexcept;
MyClass& operator=(MyClass&&) noexcept;
```

**DON'T（不应该做）**：

1. **不要在析构函数抛异常**
```cpp
~BadClass() {
    throw std::runtime_error("Error");  // ❌
}
```

2. **不要用异常控制流**
```cpp
// ❌ 错误：用异常作为循环退出条件
for (int i = 0; ; ++i) {
    try {
        vec.at(i);
    } catch (const std::out_of_range&) {
        break;  // ❌ 性能差
    }
}

// ✅ 正确：用正常逻辑
for (size_t i = 0; i < vec.size(); ++i) {
    // ...
}
```

3. **不要捕获不处理的异常**
```cpp
try {
    // ...
} catch (...) {
    // ❌ 空捕获，隐藏错误
}
```

4. **不要按值抛出指针**
```cpp
throw new std::runtime_error("Error");  // ❌ 内存泄漏
throw std::runtime_error("Error");      // ✅ 按值抛出
```

5. **不要在 noexcept 函数调用可能抛异常的函数**
```cpp
void bad_noexcept() noexcept {
    may_throw();  // ❌ 没有 try-catch
}
```

---

### 7.2 异常安全代码模式

**模式 1：RAII**

```cpp
void raii_pattern() {
    std::lock_guard<std::mutex> lock(mtx);
    std::unique_ptr<Resource> res(new Resource());
    // 异常安全
}
```

**模式 2：Copy-and-Swap**

```cpp
MyClass& operator=(const MyClass& other) {
    MyClass temp(other);
    swap(temp);
    return *this;
}
```

**模式 3：两阶段操作**

```cpp
// 第一阶段：准备（可能失败）
Resource* temp = new Resource();
prepare(temp);  // 可能抛异常

// 第二阶段：提交（noexcept）
old_resource = resource;
resource = temp;  // noexcept
delete old_resource;
```

---

## 总结

### 关键概念速查

| 概念 | 说明 |
|------|------|
| **try-catch-throw** | 异常处理三要素 |
| **异常类层次** | `std::exception` 及派生类 |
| **异常安全性** | 基本、强、不抛异常 |
| **RAII** | 资源管理与异常安全 |
| **noexcept** | 声明不抛异常 |
| **Copy-and-Swap** | 强异常安全的赋值 |

### 关键记忆点

1. **异常用于异常情况**，不是常规逻辑
2. **析构函数不能抛异常**（默认 noexcept）
3. **使用 RAII** 保证资源释放
4. **按 const 引用捕获**异常
5. **重新抛出使用 `throw;`**，不是 `throw e;`
6. **移动操作应该 noexcept**
7. **正常路径零开销，异常路径很慢**
8. **构造函数失败用异常**，预期失败用错误码

### 常见陷阱

1. 析构函数抛异常 → `std::terminate`
2. 忘记释放资源（不用 RAII）
3. 按值捕获异常（对象切片）
4. 捕获顺序错误（基类在前）
5. 用异常控制流（性能差）
6. noexcept 函数抛异常 → `std::terminate`

### 最佳实践

1. **优先使用 RAII** 管理资源
2. **构造函数失败抛异常**
3. **析构函数、移动操作标记 noexcept**
4. **异常用于异常，错误码用于预期失败**
5. **C++17/20/23**：使用 `std::optional`、`std::expected`
6. **性能敏感代码**：避免异常或用错误码
