# 18 - C++ 设计模式 (Design Patterns)

## 简介

设计模式是面向对象编程中的经典解决方案。本章重点介绍 C++ 特有的惯用法：RAII、Pimpl、CRTP。

**核心模式**：
- **RAII**：资源获取即初始化
- **Pimpl**：指向实现的指针
- **CRTP**：奇异递归模板模式

---

## 1. RAII (Resource Acquisition Is Initialization)

### 1.1 RAII 的核心思想？

**原则**：资源的生命周期与对象的生命周期绑定
- 构造时获取资源
- 析构时释放资源

**示例**：

```cpp
// ❌ 手动管理：容易忘记释放
void bad_resource() {
    int* ptr = new int[100];
    process(ptr);  // 可能抛异常
    delete[] ptr;  // ❌ 异常时不会执行
}

// ✅ RAII：自动释放
void good_resource() {
    std::unique_ptr<int[]> ptr(new int[100]);
    process(ptr.get());
    // 自动释放，即使抛异常
}
```

### 1.2 RAII 的应用？

**1. 内存管理**：
```cpp
std::unique_ptr<int> ptr(new int(10));
std::shared_ptr<int> sptr = std::make_shared<int>(20);
std::vector<int> vec{1, 2, 3};  // 自动管理内存
```

**2. 文件管理**：
```cpp
class FileHandle {
    FILE* file;
public:
    FileHandle(const char* path, const char* mode) {
        file = fopen(path, mode);
        if (!file) throw std::runtime_error("Failed to open file");
    }

    ~FileHandle() {
        if (file) fclose(file);
    }

    FILE* get() { return file; }

    FileHandle(const FileHandle&) = delete;
    FileHandle& operator=(const FileHandle&) = delete;
};

void use_file() {
    FileHandle f("data.txt", "r");
    // 使用 f.get()
} // 自动关闭文件
```

**3. 锁管理**：
```cpp
std::mutex mtx;

void thread_safe() {
    std::lock_guard<std::mutex> lock(mtx);
    // 临界区
} // 自动释放锁
```

**4. 自定义资源**：
```cpp
class GLContext {
    GLuint context_id;
public:
    GLContext() {
        context_id = glCreateContext();
    }

    ~GLContext() {
        glDeleteContext(context_id);
    }

    GLuint get() { return context_id; }
};
```

---

## 2. Pimpl (Pointer to Implementation)

### 2.1 Pimpl 的作用？

**目的**：
- 隐藏实现细节
- 减少编译依赖
- 保持 ABI 稳定

**基本实现**：

```cpp
// widget.h（接口）
class Widget {
public:
    Widget();
    ~Widget();

    void do_something();

private:
    class Impl;  // 前向声明
    std::unique_ptr<Impl> pimpl;  // 指向实现
};

// widget.cpp（实现）
class Widget::Impl {
public:
    void do_something_impl() {
        // 实际实现
    }

private:
    // 私有成员（不在头文件中）
    int private_data;
    std::vector<int> more_data;
};

Widget::Widget() : pimpl(std::make_unique<Impl>()) {}

Widget::~Widget() = default;  // 必须在 Impl 定义后

void Widget::do_something() {
    pimpl->do_something_impl();
}
```

### 2.2 Pimpl 的优点？

1. **编译时间**：修改实现不需要重新编译使用者
2. **二进制兼容**：可以改变实现而不破坏 ABI
3. **隐藏细节**：私有成员不在头文件中

### 2.3 Pimpl 的注意事项？

```cpp
// ❌ 错误：析构函数在头文件定义
class Widget {
public:
    ~Widget() = default;  // ❌ Impl 类型不完整
private:
    class Impl;
    std::unique_ptr<Impl> pimpl;
};

// ✅ 正确：析构函数在 cpp 文件定义
// widget.h
class Widget {
public:
    ~Widget();  // 仅声明
private:
    class Impl;
    std::unique_ptr<Impl> pimpl;
};

// widget.cpp
Widget::~Widget() = default;  // Impl 已完整定义
```

---

## 3. CRTP (Curiously Recurring Template Pattern)

### 3.1 CRTP 的原理？

**定义**：派生类将自己作为模板参数传给基类

```cpp
template<typename Derived>
class Base {
public:
    void interface() {
        // 调用派生类的实现
        static_cast<Derived*>(this)->implementation();
    }
};

class Derived : public Base<Derived> {
public:
    void implementation() {
        std::cout << "Derived implementation\n";
    }
};

void crtp_demo() {
    Derived d;
    d.interface();  // 编译期多态
}
```

### 3.2 CRTP 的应用？

**1. 静态多态（避免虚函数开销）**：

```cpp
template<typename Derived>
class Shape {
public:
    double area() const {
        return static_cast<const Derived*>(this)->area_impl();
    }
};

class Circle : public Shape<Circle> {
    double radius;
public:
    Circle(double r) : radius(r) {}

    double area_impl() const {
        return 3.14159 * radius * radius;
    }
};

class Rectangle : public Shape<Rectangle> {
    double width, height;
public:
    Rectangle(double w, double h) : width(w), height(h) {}

    double area_impl() const {
        return width * height;
    }
};

// 泛型函数
template<typename Shape>
void print_area(const Shape& shape) {
    std::cout << "Area: " << shape.area() << "\n";
}

void static_polymorphism() {
    Circle c(5.0);
    Rectangle r(4.0, 6.0);

    print_area(c);  // 无虚函数调用
    print_area(r);
}
```

**2. 混入（Mixin）**：

```cpp
template<typename Derived>
class Comparable {
public:
    bool operator!=(const Derived& other) const {
        return !static_cast<const Derived*>(this)->operator==(other);
    }

    bool operator>(const Derived& other) const {
        return other < *static_cast<const Derived*>(this);
    }

    bool operator<=(const Derived& other) const {
        return !(*static_cast<const Derived*>(this) > other);
    }

    bool operator>=(const Derived& other) const {
        return !(*static_cast<const Derived*>(this) < other);
    }
};

class Person : public Comparable<Person> {
    int age;
public:
    Person(int a) : age(a) {}

    bool operator==(const Person& other) const {
        return age == other.age;
    }

    bool operator<(const Person& other) const {
        return age < other.age;
    }

    // 其他比较运算符自动生成
};
```

**3. 计数器（统计对象数量）**：

```cpp
template<typename T>
class Counter {
protected:
    static int count;

    Counter() { ++count; }
    Counter(const Counter&) { ++count; }
    ~Counter() { --count; }

public:
    static int get_count() { return count; }
};

template<typename T>
int Counter<T>::count = 0;

class MyClass : public Counter<MyClass> {
    // ...
};

void counter_demo() {
    MyClass a, b, c;
    std::cout << "Count: " << MyClass::get_count() << "\n";  // 3
}
```

### 3.3 CRTP vs 虚函数？

| 特性 | CRTP | 虚函数 |
|------|------|--------|
| 多态类型 | 编译期 | 运行期 |
| 性能 | 无开销 | 虚表开销 |
| 灵活性 | 低（编译期确定） | 高（运行期绑定） |
| 类型安全 | 编译期检查 | 运行期错误 |

---

## 4. 其他 C++ 惯用法

### 4.1 Copy-and-Swap？

```cpp
class MyClass {
    int* data;
public:
    MyClass& operator=(const MyClass& other) {
        MyClass temp(other);  // 拷贝
        swap(temp);           // 交换
        return *this;
    }  // temp 析构，释放旧资源

    void swap(MyClass& other) noexcept {
        std::swap(data, other.data);
    }
};
```

### 4.2 SFINAE 与 Tag Dispatch？

```cpp
// Tag Dispatch
template<typename Iterator>
void advance_impl(Iterator& it, int n, std::random_access_iterator_tag) {
    it += n;  // 快速
}

template<typename Iterator>
void advance_impl(Iterator& it, int n, std::input_iterator_tag) {
    while (n--) ++it;  // 慢速
}

template<typename Iterator>
void advance(Iterator& it, int n) {
    advance_impl(it, n, typename std::iterator_traits<Iterator>::iterator_category{});
}
```

### 4.3 NVI (Non-Virtual Interface)？

```cpp
class Base {
public:
    void process() {  // 公共非虚接口
        pre_process();
        do_process();  // 调用虚函数
        post_process();
    }

private:
    virtual void do_process() = 0;  // 私有虚函数

    void pre_process() { /* ... */ }
    void post_process() { /* ... */ }
};

class Derived : public Base {
private:
    void do_process() override {
        // 实现
    }
};
```

---

## 5. 设计原则

### 5.1 SOLID 原则

1. **S - 单一职责**：一个类只做一件事
2. **O - 开闭原则**：对扩展开放，对修改关闭
3. **L - 里氏替换**：子类可以替换基类
4. **I - 接口隔离**：多个专用接口优于单一通用接口
5. **D - 依赖倒置**：依赖抽象，不依赖具体

### 5.2 C++ 核心准则

1. **优先使用 RAII**
2. **优先使用值语义**
3. **避免裸指针**
4. **使用智能指针管理所有权**
5. **优先使用标准库**

---

## 总结

### 关键模式速查

| 模式 | 用途 | 特点 |
|------|------|------|
| **RAII** | 资源管理 | 自动释放，异常安全 |
| **Pimpl** | 隐藏实现 | 减少编译依赖，ABI 稳定 |
| **CRTP** | 静态多态 | 零开销，编译期绑定 |
| **Copy-and-Swap** | 异常安全赋值 | 强异常保证 |

### 关键记忆点

1. **RAII 是 C++ 的核心**：资源管理的基石
2. **Pimpl 用于接口稳定**：隐藏实现细节
3. **CRTP 用于性能优化**：静态多态无虚函数开销
4. **优先使用值语义和智能指针**
5. **设计模式服务于需求**：不要过度设计
