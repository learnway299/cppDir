# C++ 面向对象基础 (OOP Basic)

## 概述

本文档涵盖 C++ 面向对象编程的基础知识，包括类的定义、构造函数与析构函数、访问控制、成员函数、以及相关的核心概念。

---

## 1. 类的定义与对象创建

### 1.1 类的基本结构

**问题：如何定义一个类？类和结构体的区别是什么？**

```cpp
// 类的定义
class Student {
public:
    // 构造函数
    Student(std::string name, int age) : name_(name), age_(age) {}

    // 成员函数
    void introduce() const {
        std::cout << "Name: " << name_ << ", Age: " << age_ << std::endl;
    }

    // Getter
    std::string getName() const { return name_; }
    int getAge() const { return age_; }

private:
    std::string name_;
    int age_;
};

// 结构体定义（默认 public）
struct Point {
    int x;
    int y;

    Point(int x, int y) : x(x), y(y) {}
};

// 使用示例
int main() {
    Student s("Alice", 20);
    s.introduce();

    Point p(10, 20);
    std::cout << "Point: (" << p.x << ", " << p.y << ")" << std::endl;
}
```

**关键要点：**
- `class` 默认访问权限是 `private`，`struct` 默认是 `public`
- `class` 和 `struct` 在 C++ 中几乎完全相同，唯一区别是默认访问权限
- 推荐使用 `class` 用于复杂对象，`struct` 用于简单数据聚合（POD 类型）

**常见陷阱：**
- 忘记在类定义后加分号 `;`
- 混淆 `class` 和 `struct` 的默认访问权限

---

### 1.2 对象的创建方式

**问题：C++ 中有哪些创建对象的方式？栈对象和堆对象的区别？**

```cpp
class MyClass {
public:
    MyClass() { std::cout << "Constructor called\n"; }
    ~MyClass() { std::cout << "Destructor called\n"; }
};

int main() {
    // 方式1：栈上创建对象（自动管理生命周期）
    MyClass obj1;

    // 方式2：栈上创建对象（显式调用构造函数）
    MyClass obj2 = MyClass();

    // 方式3：堆上创建对象（手动管理生命周期）
    MyClass* obj3 = new MyClass();
    delete obj3;  // 必须手动释放

    // 方式4：使用智能指针（推荐）
    std::unique_ptr<MyClass> obj4 = std::make_unique<MyClass>();
    std::shared_ptr<MyClass> obj5 = std::make_shared<MyClass>();

    // 方式5：placement new（在指定内存位置构造对象）
    alignas(MyClass) char buffer[sizeof(MyClass)];
    MyClass* obj6 = new (buffer) MyClass();
    obj6->~MyClass();  // 显式调用析构函数

} // obj1, obj2 在此处自动销毁，智能指针自动释放
```

**关键要点：**
- 栈对象：自动管理生命周期，函数结束时自动销毁，速度快
- 堆对象：手动管理，灵活但容易内存泄漏，速度较慢
- 智能指针：结合了堆的灵活性和自动管理的安全性（推荐使用）
- Placement new：用于内存池、对象池等高级场景

**常见陷阱：**
- `new` 后忘记 `delete` 导致内存泄漏
- 重复 `delete` 导致 double free
- 栈上创建大对象可能导致栈溢出

---

## 2. 构造函数 (Constructor)

### 2.1 默认构造函数

**问题：什么是默认构造函数？编译器何时自动生成？**

```cpp
class Empty {
    // 编译器自动生成默认构造函数
};

class MyClass {
public:
    MyClass() {  // 用户定义的默认构造函数
        x_ = 0;
        y_ = 0;
    }

private:
    int x_;
    int y_;
};

class NoDefault {
public:
    NoDefault(int x) : x_(x) {}  // 提供了其他构造函数
    // 编译器不会生成默认构造函数

private:
    int x_;
};

int main() {
    Empty e;           // OK
    MyClass m;         // OK
    // NoDefault n;    // Error: 没有默认构造函数
    NoDefault n(10);   // OK
}
```

**关键要点：**
- 如果没有定义任何构造函数，编译器会自动生成默认构造函数
- 一旦定义了任何构造函数，编译器不再生成默认构造函数
- 可以使用 `= default` 显式要求编译器生成默认构造函数

**常见陷阱：**
- 定义了有参构造函数后，忘记定义默认构造函数，导致无法创建无参对象

---

### 2.2 初始化列表 (Initializer List)

**问题：为什么要使用初始化列表？它和在构造函数体内赋值有什么区别？**

```cpp
class MyClass {
public:
    // 推荐：使用初始化列表
    MyClass(int x, const std::string& s)
        : x_(x), s_(s), c_ref_(x_), c_const_(100) {
        // 构造函数体
    }

    // 不推荐：在构造函数体内赋值
    MyClass(int x) {
        x_ = x;  // 这是赋值，不是初始化
    }

private:
    int x_;
    std::string s_;
    int& c_ref_;        // 引用必须使用初始化列表
    const int c_const_; // const 成员必须使用初始化列表
};

class Base {
public:
    Base(int x) : x_(x) {}
private:
    int x_;
};

class Derived : public Base {
public:
    // 必须在初始化列表中调用基类构造函数
    Derived(int x, int y) : Base(x), y_(y) {}

private:
    int y_;
};
```

**关键要点：**
- 初始化列表是真正的初始化，构造函数体内是赋值
- 以下情况必须使用初始化列表：
  - `const` 成员
  - 引用成员
  - 没有默认构造函数的成员对象
  - 基类构造
- 初始化顺序由成员声明顺序决定，与初始化列表顺序无关

**常见陷阱：**
- 初始化列表顺序与声明顺序不一致，导致依赖关系错误
- 忘记初始化 `const` 或引用成员

---

### 2.3 委托构造函数 (Delegating Constructor, C++11)

**问题：如何在一个构造函数中调用另一个构造函数？**

```cpp
class MyClass {
public:
    // 主构造函数
    MyClass(int x, int y, int z) : x_(x), y_(y), z_(z) {
        std::cout << "Full constructor\n";
    }

    // 委托给主构造函数
    MyClass() : MyClass(0, 0, 0) {
        std::cout << "Delegating constructor\n";
    }

    // 委托给主构造函数
    MyClass(int x) : MyClass(x, 0, 0) {
        std::cout << "Partial delegating constructor\n";
    }

private:
    int x_, y_, z_;
};

int main() {
    MyClass m1;        // 输出两行
    MyClass m2(10);    // 输出两行
}
```

**关键要点：**
- C++11 引入委托构造函数，减少代码重复
- 委托构造函数不能同时使用初始化列表初始化成员
- 委托构造先执行，然后执行委托者的构造函数体

**常见陷阱：**
- 不能在委托构造函数中使用初始化列表
- 避免构造函数委托循环

---

### 2.4 拷贝构造函数 (Copy Constructor)

**问题：什么是拷贝构造函数？何时被调用？**

```cpp
class MyClass {
public:
    // 拷贝构造函数
    MyClass(const MyClass& other)
        : x_(other.x_), data_(new int(*other.data_)) {
        std::cout << "Copy constructor\n";
    }

    MyClass(int x) : x_(x), data_(new int(x)) {}

    ~MyClass() { delete data_; }

private:
    int x_;
    int* data_;  // 需要深拷贝
};

// 拷贝构造函数调用时机
void func(MyClass obj) {  // 1. 按值传递参数
    // ...
}

MyClass createObj() {
    MyClass temp(10);
    return temp;  // 2. 返回对象（可能被 RVO 优化掉）
}

int main() {
    MyClass m1(10);
    MyClass m2 = m1;   // 3. 用一个对象初始化另一个对象
    MyClass m3(m1);    // 4. 显式调用拷贝构造

    func(m1);          // 调用拷贝构造
}
```

**关键要点：**
- 如果类包含指针成员，必须自定义拷贝构造函数（深拷贝）
- 编译器生成的默认拷贝构造是浅拷贝（逐成员拷贝）
- 可以使用 `= delete` 禁用拷贝构造
- 现代编译器会进行 RVO (Return Value Optimization) 优化

**常见陷阱：**
- 忘记深拷贝指针成员，导致 double free
- 拷贝构造参数必须是 `const` 引用

---

### 2.5 移动构造函数 (Move Constructor, C++11)

**问题：什么是移动构造函数？和拷贝构造有什么区别？**

```cpp
class MyClass {
public:
    // 拷贝构造
    MyClass(const MyClass& other)
        : data_(new int[other.size_]), size_(other.size_) {
        std::copy(other.data_, other.data_ + size_, data_);
        std::cout << "Copy constructor\n";
    }

    // 移动构造
    MyClass(MyClass&& other) noexcept
        : data_(other.data_), size_(other.size_) {
        other.data_ = nullptr;  // 窃取资源，置空源对象
        other.size_ = 0;
        std::cout << "Move constructor\n";
    }

    MyClass(int size) : data_(new int[size]), size_(size) {}

    ~MyClass() { delete[] data_; }

private:
    int* data_;
    size_t size_;
};

int main() {
    MyClass m1(100);
    MyClass m2 = m1;              // 调用拷贝构造
    MyClass m3 = std::move(m1);   // 调用移动构造（m1 失效）

    // m1 现在处于有效但未定义状态，不应再使用
}
```

**关键要点：**
- 移动构造"窃取"资源，避免深拷贝，提高性能
- 移动后源对象处于有效但未定义状态
- 移动构造函数应标记为 `noexcept`
- 右值引用 `&&` 表示可以"移动"的对象

**常见陷阱：**
- 移动后仍然使用源对象
- 忘记置空源对象的指针，导致 double free
- 忘记添加 `noexcept`，影响 STL 容器性能优化

---

### 2.6 explicit 关键字

**问题：`explicit` 关键字的作用是什么？**

```cpp
class MyClass {
public:
    // 没有 explicit：允许隐式转换
    MyClass(int x) : x_(x) {}

private:
    int x_;
};

class SafeClass {
public:
    // 有 explicit：禁止隐式转换
    explicit SafeClass(int x) : x_(x) {}

private:
    int x_;
};

void func1(MyClass obj) {}
void func2(SafeClass obj) {}

int main() {
    MyClass m1 = 10;      // OK: 隐式转换 int -> MyClass
    func1(20);            // OK: 隐式转换

    // SafeClass s1 = 10; // Error: explicit 禁止隐式转换
    SafeClass s2(10);     // OK: 显式构造
    // func2(20);         // Error: 禁止隐式转换
    func2(SafeClass(20)); // OK: 显式构造
}
```

**关键要点：**
- `explicit` 禁止单参数构造函数的隐式类型转换
- 推荐所有单参数构造函数都加 `explicit`，除非确实需要隐式转换
- C++11 后也可用于多参数构造函数

**常见陷阱：**
- 忘记加 `explicit` 导致意外的隐式转换
- 过度使用 `explicit` 导致代码冗长

---

## 3. 析构函数 (Destructor)

### 3.1 析构函数基础

**问题：析构函数的作用是什么？何时被调用？**

```cpp
class MyClass {
public:
    MyClass(int size) : data_(new int[size]), size_(size) {
        std::cout << "Constructor: allocated " << size << " ints\n";
    }

    // 析构函数
    ~MyClass() {
        delete[] data_;
        std::cout << "Destructor: freed memory\n";
    }

private:
    int* data_;
    size_t size_;
};

void func() {
    MyClass m(10);
    // ...
} // m 在这里被销毁，调用析构函数

int main() {
    func();

    MyClass* p = new MyClass(20);
    delete p;  // 显式调用析构函数
}
```

**关键要点：**
- 析构函数名是 `~ClassName()`，无参数，无返回值
- 析构函数用于释放资源（内存、文件句柄、锁等）
- 栈对象离开作用域时自动调用析构函数
- 堆对象必须显式 `delete` 才会调用析构函数
- 析构顺序与构造顺序相反

**常见陷阱：**
- 析构函数中不要抛出异常
- 忘记在析构函数中释放资源导致内存泄漏

---

### 3.2 虚析构函数

**问题：什么时候需要虚析构函数？**

```cpp
class Base {
public:
    Base() { data_ = new int[10]; }

    // 非虚析构函数（危险！）
    ~Base() {
        delete[] data_;
        std::cout << "Base destructor\n";
    }

private:
    int* data_;
};

class Derived : public Base {
public:
    Derived() { more_data_ = new int[20]; }

    ~Derived() {
        delete[] more_data_;
        std::cout << "Derived destructor\n";
    }

private:
    int* more_data_;
};

int main() {
    Base* p = new Derived();
    delete p;  // 只调用 Base 析构函数！内存泄漏！
}

// 正确做法：使用虚析构函数
class SafeBase {
public:
    SafeBase() { data_ = new int[10]; }

    // 虚析构函数
    virtual ~SafeBase() {
        delete[] data_;
        std::cout << "SafeBase destructor\n";
    }

private:
    int* data_;
};

class SafeDerived : public SafeBase {
public:
    SafeDerived() { more_data_ = new int[20]; }

    ~SafeDerived() override {
        delete[] more_data_;
        std::cout << "SafeDerived destructor\n";
    }

private:
    int* more_data_;
};

int main() {
    SafeBase* p = new SafeDerived();
    delete p;  // 正确：先调用 SafeDerived，再调用 SafeBase
}
```

**关键要点：**
- 如果类被用作基类，析构函数必须是 `virtual`
- 虚析构函数确保通过基类指针删除派生类对象时，正确调用派生类析构函数
- 如果类不作为基类，不需要虚析构函数（避免虚表开销）

**常见陷阱：**
- 基类没有虚析构函数，导致派生类资源泄漏（非常常见的错误）
- 不必要地使用虚析构函数增加开销

---

## 4. 访问控制 (Access Control)

### 4.1 三种访问权限

**问题：`public`、`protected`、`private` 的区别是什么？**

```cpp
class Base {
public:
    int pub_data_;       // 公有：任何地方都可访问

protected:
    int prot_data_;      // 保护：本类和派生类可访问

private:
    int priv_data_;      // 私有：只有本类可访问

public:
    void accessTest() {
        pub_data_ = 1;   // OK
        prot_data_ = 2;  // OK
        priv_data_ = 3;  // OK
    }
};

class Derived : public Base {
public:
    void derivedAccess() {
        pub_data_ = 1;   // OK
        prot_data_ = 2;  // OK
        // priv_data_ = 3;  // Error: 私有成员不可访问
    }
};

int main() {
    Base b;
    b.pub_data_ = 1;     // OK
    // b.prot_data_ = 2; // Error: 保护成员不可访问
    // b.priv_data_ = 3; // Error: 私有成员不可访问

    Derived d;
    d.pub_data_ = 1;     // OK
    // d.prot_data_ = 2; // Error: 保护成员不可外部访问
}
```

**关键要点：**
- `public`：无限制访问
- `protected`：本类、友元、派生类可访问
- `private`：只有本类和友元可访问
- 推荐将数据成员设为 `private`，提供 `public` 接口

**常见陷阱：**
- 滥用 `public` 数据成员，破坏封装性
- `protected` 数据成员容易被派生类误用

---

### 4.2 友元 (Friend)

**问题：什么是友元？如何使用？**

```cpp
class MyClass {
private:
    int secret_data_;

public:
    MyClass(int data) : secret_data_(data) {}

    // 友元函数声明
    friend void showSecret(const MyClass& obj);

    // 友元类声明
    friend class Helper;
};

// 友元函数可以访问私有成员
void showSecret(const MyClass& obj) {
    std::cout << "Secret: " << obj.secret_data_ << std::endl;
}

// 友元类可以访问私有成员
class Helper {
public:
    void help(MyClass& obj) {
        obj.secret_data_ = 999;  // 可以修改私有成员
    }
};

int main() {
    MyClass m(42);
    showSecret(m);

    Helper h;
    h.help(m);
    showSecret(m);
}
```

**关键要点：**
- 友元可以访问类的私有和保护成员
- 友元关系不具有传递性（A 是 B 的友元，B 是 C 的友元，A 不是 C 的友元）
- 友元关系不具有继承性
- 友元破坏封装性，应谨慎使用

**常见陷阱：**
- 滥用友元导致耦合度过高
- 友元声明位置可以在类的任何访问权限区域

---

## 5. 成员函数

### 5.1 const 成员函数

**问题：什么是 `const` 成员函数？为什么要使用？**

```cpp
class MyClass {
public:
    MyClass(int x) : x_(x), count_(0) {}

    // const 成员函数：不修改对象状态
    int getX() const {
        // x_ = 10;  // Error: 不能修改成员变量
        return x_;
    }

    // mutable 成员可以在 const 函数中修改
    int getCount() const {
        count_++;  // OK: mutable 成员
        return count_;
    }

    // 非 const 成员函数
    void setX(int x) {
        x_ = x;
    }

private:
    int x_;
    mutable int count_;  // mutable: 可在 const 函数中修改
};

int main() {
    const MyClass cm(10);
    cm.getX();      // OK: const 对象调用 const 成员函数
    // cm.setX(20); // Error: const 对象不能调用非 const 函数

    MyClass m(20);
    m.getX();       // OK: 非 const 对象可以调用 const 函数
    m.setX(30);     // OK
}
```

**关键要点：**
- `const` 成员函数承诺不修改对象状态（逻辑 const）
- `const` 对象只能调用 `const` 成员函数
- `mutable` 关键字允许在 `const` 函数中修改成员（用于缓存、计数等）
- 推荐所有不修改状态的成员函数都声明为 `const`

**常见陷阱：**
- Getter 函数忘记加 `const`
- 滥用 `mutable` 破坏逻辑 const

---

### 5.2 静态成员函数

**问题：什么是静态成员函数？和普通成员函数有什么区别？**

```cpp
class MyClass {
public:
    MyClass() { instance_count_++; }
    ~MyClass() { instance_count_--; }

    // 静态成员函数
    static int getInstanceCount() {
        // x_ = 10;  // Error: 静态函数不能访问非静态成员
        return instance_count_;
    }

    // 非静态成员函数
    void doSomething() {
        x_ = 10;  // OK: 可以访问非静态成员
        instance_count_++;  // OK: 可以访问静态成员
    }

private:
    int x_;
    static int instance_count_;  // 静态成员变量声明
};

// 静态成员变量定义（必须在类外定义）
int MyClass::instance_count_ = 0;

int main() {
    // 通过类名调用静态函数
    std::cout << MyClass::getInstanceCount() << std::endl;  // 0

    MyClass m1;
    std::cout << MyClass::getInstanceCount() << std::endl;  // 1

    MyClass m2;
    std::cout << m2.getInstanceCount() << std::endl;  // 2（也可以通过对象调用）
}
```

**关键要点：**
- 静态成员函数属于类，不属于对象，没有 `this` 指针
- 静态函数只能访问静态成员，不能访问非静态成员
- 静态成员变量在所有对象间共享
- 静态成员变量必须在类外定义（C++17 可用 `inline static`）

**常见陷阱：**
- 忘记在类外定义静态成员变量
- 在静态函数中访问非静态成员

---

## 6. Rule of Three/Five/Zero

### 6.1 三五零法则

**问题：什么是 Rule of Three/Five/Zero？**

```cpp
// Rule of Zero: 不管理资源，依赖编译器生成的特殊成员函数
class RuleOfZero {
public:
    RuleOfZero(int x) : x_(x) {}
    // 编译器自动生成拷贝构造、移动构造、拷贝赋值、移动赋值、析构

private:
    int x_;
    std::string s_;
    std::vector<int> vec_;
};

// Rule of Three (C++98/03): 管理资源，必须定义三个函数
class RuleOfThree {
public:
    RuleOfThree(int size) : data_(new int[size]), size_(size) {}

    // 1. 析构函数
    ~RuleOfThree() {
        delete[] data_;
    }

    // 2. 拷贝构造函数
    RuleOfThree(const RuleOfThree& other)
        : data_(new int[other.size_]), size_(other.size_) {
        std::copy(other.data_, other.data_ + size_, data_);
    }

    // 3. 拷贝赋值运算符
    RuleOfThree& operator=(const RuleOfThree& other) {
        if (this != &other) {
            delete[] data_;
            size_ = other.size_;
            data_ = new int[size_];
            std::copy(other.data_, other.data_ + size_, data_);
        }
        return *this;
    }

private:
    int* data_;
    size_t size_;
};

// Rule of Five (C++11): 管理资源，定义五个函数
class RuleOfFive {
public:
    RuleOfFive(int size) : data_(new int[size]), size_(size) {}

    // 1. 析构函数
    ~RuleOfFive() {
        delete[] data_;
    }

    // 2. 拷贝构造函数
    RuleOfFive(const RuleOfFive& other)
        : data_(new int[other.size_]), size_(other.size_) {
        std::copy(other.data_, other.data_ + size_, data_);
    }

    // 3. 拷贝赋值运算符
    RuleOfFive& operator=(const RuleOfFive& other) {
        if (this != &other) {
            delete[] data_;
            size_ = other.size_;
            data_ = new int[size_];
            std::copy(other.data_, other.data_ + size_, data_);
        }
        return *this;
    }

    // 4. 移动构造函数
    RuleOfFive(RuleOfFive&& other) noexcept
        : data_(other.data_), size_(other.size_) {
        other.data_ = nullptr;
        other.size_ = 0;
    }

    // 5. 移动赋值运算符
    RuleOfFive& operator=(RuleOfFive&& other) noexcept {
        if (this != &other) {
            delete[] data_;
            data_ = other.data_;
            size_ = other.size_;
            other.data_ = nullptr;
            other.size_ = 0;
        }
        return *this;
    }

private:
    int* data_;
    size_t size_;
};
```

**关键要点：**
- **Rule of Zero**：优先使用，使用 RAII 容器（`std::vector`、`std::string`、智能指针）管理资源
- **Rule of Three**：如果需要自定义析构、拷贝构造、拷贝赋值之一，通常三个都需要
- **Rule of Five**：C++11 后，还需要移动构造和移动赋值
- 推荐使用 `= default` 或 `= delete` 显式控制

**常见陷阱：**
- 只定义了析构函数，忘记定义拷贝构造和拷贝赋值
- 拷贝赋值忘记检查自赋值
- 移动函数忘记 `noexcept`

---

## 7. 特殊成员函数控制

### 7.1 = default 和 = delete

**问题：如何显式控制编译器生成的特殊成员函数？**

```cpp
class NonCopyable {
public:
    NonCopyable() = default;  // 显式生成默认构造

    // 禁用拷贝
    NonCopyable(const NonCopyable&) = delete;
    NonCopyable& operator=(const NonCopyable&) = delete;

    // 允许移动
    NonCopyable(NonCopyable&&) = default;
    NonCopyable& operator=(NonCopyable&&) = default;
};

class OnlyHeapAlloc {
public:
    static OnlyHeapAlloc* create() {
        return new OnlyHeapAlloc();
    }

private:
    OnlyHeapAlloc() = default;  // 私有构造，只能堆分配
    ~OnlyHeapAlloc() = default;  // 私有析构，禁止栈分配
};

int main() {
    NonCopyable nc1;
    // NonCopyable nc2 = nc1;  // Error: 拷贝被禁用
    NonCopyable nc3 = std::move(nc1);  // OK: 移动允许

    // OnlyHeapAlloc oha;  // Error: 构造函数私有
    OnlyHeapAlloc* p = OnlyHeapAlloc::create();  // OK
    delete p;
}
```

**关键要点：**
- `= default`：显式要求编译器生成默认实现
- `= delete`：禁用该函数
- 可用于控制对象的复制、移动、构造方式

**常见陷阱：**
- `= delete` 后仍尝试调用该函数

---

## 8. this 指针

### 8.1 this 指针的使用

**问题：什么是 `this` 指针？如何使用？**

```cpp
class MyClass {
public:
    MyClass(int x) : x_(x) {}

    // 返回自身引用，支持链式调用
    MyClass& setX(int x) {
        this->x_ = x;  // this 指向当前对象
        return *this;  // 返回当前对象的引用
    }

    MyClass& setY(int y) {
        y_ = y;
        return *this;
    }

    // 比较是否是同一个对象
    bool isSame(const MyClass& other) const {
        return this == &other;
    }

    void print() const {
        std::cout << "x: " << x_ << ", y: " << y_ << std::endl;
    }

private:
    int x_;
    int y_ = 0;
};

int main() {
    MyClass m(10);

    // 链式调用
    m.setX(100).setY(200).print();

    MyClass m2(20);
    std::cout << m.isSame(m) << std::endl;   // true
    std::cout << m.isSame(m2) << std::endl;  // false
}
```

**关键要点：**
- `this` 是指向当前对象的指针
- 在成员函数中，`this` 指针隐式传递
- `*this` 表示当前对象本身
- 静态成员函数没有 `this` 指针

**常见陷阱：**
- 返回 `this` 而不是 `*this` 导致类型错误
- 在静态函数中使用 `this`

---

## 9. 总结

### 核心要点
1. **类与对象**：类是模板，对象是实例；栈对象自动管理，堆对象需手动释放
2. **构造函数**：初始化列表优于赋值；注意拷贝/移动构造；单参数构造加 `explicit`
3. **析构函数**：释放资源；基类析构必须是 `virtual`
4. **访问控制**：数据成员用 `private`，接口用 `public`，派生类用 `protected`
5. **特殊成员函数**：遵循 Rule of Zero/Three/Five
6. **const 正确性**：不修改状态的函数声明为 `const`

### 常见面试题
- 构造函数和析构函数的调用顺序？
- 深拷贝和浅拷贝的区别？
- 移动语义的优势？
- 什么时候需要虚析构函数？
- Rule of Three/Five/Zero 是什么？

---

## 参考资料
- *Effective C++* by Scott Meyers
- *C++ Primer* (5th Edition)
- ISO C++ Core Guidelines
