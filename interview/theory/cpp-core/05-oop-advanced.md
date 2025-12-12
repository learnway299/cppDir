# C++ 面向对象进阶 (OOP Advanced)

## 概述

本文档涵盖 C++ 面向对象编程的进阶知识，包括继承、多态、虚函数、虚函数表、抽象类、接口、RTTI（运行时类型识别）等核心概念。

---

## 1. 继承 (Inheritance)

### 1.1 继承的基本概念

**问题：什么是继承？C++ 支持哪些继承方式？**

```cpp
// 基类
class Animal {
public:
    Animal(std::string name) : name_(name) {}

    void eat() {
        std::cout << name_ << " is eating" << std::endl;
    }

protected:
    std::string name_;
};

// 派生类：public 继承
class Dog : public Animal {
public:
    Dog(std::string name, std::string breed)
        : Animal(name), breed_(breed) {}

    void bark() {
        std::cout << name_ << " is barking" << std::endl;
    }

private:
    std::string breed_;
};

// 派生类：protected 继承
class Cat : protected Animal {
public:
    Cat(std::string name) : Animal(name) {}

    void meow() {
        eat();  // protected 继承，eat() 变为 protected
    }
};

// 派生类：private 继承
class Bird : private Animal {
public:
    Bird(std::string name) : Animal(name) {}

    void chirp() {
        eat();  // private 继承，eat() 变为 private
    }
};

int main() {
    Dog dog("Buddy", "Golden Retriever");
    dog.eat();   // OK: public 继承，基类 public 成员仍为 public
    dog.bark();

    Cat cat("Whiskers");
    // cat.eat();  // Error: protected 继承，基类 public 成员变为 protected
    cat.meow();

    Bird bird("Tweety");
    // bird.eat();  // Error: private 继承，基类 public 成员变为 private
    bird.chirp();
}
```

**关键要点：**
- **public 继承**："is-a" 关系，最常用，基类接口对外界保持可见
- **protected 继承**：较少使用，基类 public 成员变为 protected
- **private 继承**："implemented-in-terms-of" 关系，基类接口对外界不可见
- 派生类继承基类的所有成员（除构造、析构、赋值运算符）

**常见陷阱：**
- 混淆三种继承方式的访问权限变化
- 忘记在派生类构造函数中初始化基类

---

### 1.2 多重继承 (Multiple Inheritance)

**问题：C++ 如何实现多重继承？有什么问题？**

```cpp
class Flyable {
public:
    void fly() {
        std::cout << "Flying..." << std::endl;
    }
};

class Swimmable {
public:
    void swim() {
        std::cout << "Swimming..." << std::endl;
    }
};

// 多重继承
class Duck : public Flyable, public Swimmable {
public:
    void quack() {
        std::cout << "Quack!" << std::endl;
    }
};

int main() {
    Duck duck;
    duck.fly();
    duck.swim();
    duck.quack();
}
```

**关键要点：**
- C++ 支持多重继承（从多个基类继承）
- 多重继承可能导致菱形继承问题
- 多重继承增加复杂度，应谨慎使用

**常见陷阱：**
- 菱形继承导致的二义性和数据冗余（见下节）

---

### 1.3 菱形继承与虚继承

**问题：什么是菱形继承？如何使用虚继承解决？**

```cpp
// 菱形继承问题
class Animal {
public:
    int age_;
    Animal() : age_(0) {
        std::cout << "Animal constructor\n";
    }
};

class Mammal : public Animal {
public:
    Mammal() {
        std::cout << "Mammal constructor\n";
    }
};

class Bird : public Animal {
public:
    Bird() {
        std::cout << "Bird constructor\n";
    }
};

// 问题：Bat 有两份 Animal 数据
class Bat : public Mammal, public Bird {
public:
    Bat() {
        std::cout << "Bat constructor\n";
    }
};

int main() {
    Bat bat;
    // bat.age_ = 5;  // Error: 二义性，不知道是 Mammal::age_ 还是 Bird::age_
    bat.Mammal::age_ = 5;  // 必须显式指定
    bat.Bird::age_ = 3;
}

// 解决方案：虚继承
class AnimalV {
public:
    int age_;
    AnimalV() : age_(0) {
        std::cout << "AnimalV constructor\n";
    }
};

class MammalV : virtual public AnimalV {  // 虚继承
public:
    MammalV() {
        std::cout << "MammalV constructor\n";
    }
};

class BirdV : virtual public AnimalV {  // 虚继承
public:
    BirdV() {
        std::cout << "BirdV constructor\n";
    }
};

class BatV : public MammalV, public BirdV {
public:
    BatV() {
        std::cout << "BatV constructor\n";
    }
};

int main() {
    BatV bat;
    bat.age_ = 5;  // OK: 只有一份 AnimalV 数据
    std::cout << "Age: " << bat.age_ << std::endl;
}
```

**关键要点：**
- **菱形继承**：派生类从多条路径继承同一个基类，导致数据冗余和二义性
- **虚继承**：使用 `virtual` 关键字，确保只有一份共享基类实例
- 虚继承有性能开销（额外的指针和间接访问）
- 最派生类负责初始化虚基类

**常见陷阱：**
- 不理解虚继承的构造顺序（虚基类最先构造）
- 滥用多重继承导致菱形问题

---

## 2. 多态 (Polymorphism)

### 2.1 编译时多态（静态多态）

**问题：什么是编译时多态？有哪些实现方式？**

```cpp
// 1. 函数重载
class Calculator {
public:
    int add(int a, int b) {
        return a + b;
    }

    double add(double a, double b) {
        return a + b;
    }

    int add(int a, int b, int c) {
        return a + b + c;
    }
};

// 2. 模板（泛型编程）
template<typename T>
T max(T a, T b) {
    return a > b ? a : b;
}

int main() {
    Calculator calc;
    std::cout << calc.add(1, 2) << std::endl;        // 调用 int 版本
    std::cout << calc.add(1.5, 2.5) << std::endl;    // 调用 double 版本

    std::cout << max(10, 20) << std::endl;           // T = int
    std::cout << max(3.14, 2.71) << std::endl;       // T = double
}
```

**关键要点：**
- 编译时多态在编译期确定调用的函数，无运行时开销
- 实现方式：函数重载、运算符重载、模板
- 性能优秀，但缺乏运行时灵活性

---

### 2.2 运行时多态（动态多态）

**问题：什么是运行时多态？如何实现？**

```cpp
class Shape {
public:
    // 虚函数
    virtual void draw() const {
        std::cout << "Drawing a shape" << std::endl;
    }

    virtual double area() const = 0;  // 纯虚函数

    virtual ~Shape() = default;  // 虚析构函数
};

class Circle : public Shape {
public:
    Circle(double radius) : radius_(radius) {}

    // 重写虚函数
    void draw() const override {
        std::cout << "Drawing a circle" << std::endl;
    }

    double area() const override {
        return 3.14159 * radius_ * radius_;
    }

private:
    double radius_;
};

class Rectangle : public Shape {
public:
    Rectangle(double width, double height)
        : width_(width), height_(height) {}

    void draw() const override {
        std::cout << "Drawing a rectangle" << std::endl;
    }

    double area() const override {
        return width_ * height_;
    }

private:
    double width_;
    double height_;
};

// 多态使用
void render(const Shape& shape) {
    shape.draw();  // 运行时动态绑定
    std::cout << "Area: " << shape.area() << std::endl;
}

int main() {
    Circle circle(5.0);
    Rectangle rect(4.0, 6.0);

    render(circle);   // 调用 Circle::draw()
    render(rect);     // 调用 Rectangle::draw()

    // 通过基类指针实现多态
    Shape* shapes[] = {
        new Circle(3.0),
        new Rectangle(2.0, 4.0)
    };

    for (Shape* shape : shapes) {
        shape->draw();
        delete shape;
    }
}
```

**关键要点：**
- 运行时多态通过虚函数实现，在运行时动态绑定
- 必须通过基类指针或引用调用虚函数才能实现多态
- 虚函数有运行时开销（虚表查找）
- 使用 `override` 关键字（C++11）可防止拼写错误

**常见陷阱：**
- 通过对象（而非指针/引用）调用虚函数，导致静态绑定（对象切片）
- 忘记在基类中声明虚析构函数

---

## 3. 虚函数 (Virtual Function)

### 3.1 虚函数的工作原理

**问题：虚函数是如何实现的？什么是虚函数表（vtable）？**

```cpp
class Base {
public:
    virtual void func1() {
        std::cout << "Base::func1()" << std::endl;
    }

    virtual void func2() {
        std::cout << "Base::func2()" << std::endl;
    }

    void nonVirtual() {
        std::cout << "Base::nonVirtual()" << std::endl;
    }
};

class Derived : public Base {
public:
    void func1() override {
        std::cout << "Derived::func1()" << std::endl;
    }

    void func2() override {
        std::cout << "Derived::func2()" << std::endl;
    }

    void nonVirtual() {  // 隐藏，不是重写
        std::cout << "Derived::nonVirtual()" << std::endl;
    }
};

int main() {
    Base* ptr = new Derived();

    ptr->func1();       // 输出: Derived::func1() (动态绑定)
    ptr->func2();       // 输出: Derived::func2() (动态绑定)
    ptr->nonVirtual();  // 输出: Base::nonVirtual() (静态绑定)

    delete ptr;
}

// 虚表结构（概念示意）
/*
Base 对象内存布局:
+-------------------+
| vptr (指向 vtable)|  <-- 虚表指针
+-------------------+
| 其他成员变量       |
+-------------------+

Base vtable:
+-------------------+
| &Base::func1      |
+-------------------+
| &Base::func2      |
+-------------------+

Derived vtable:
+-------------------+
| &Derived::func1   |  <-- 重写，指向派生类版本
+-------------------+
| &Derived::func2   |  <-- 重写，指向派生类版本
+-------------------+
*/
```

**关键要点：**
- 每个包含虚函数的类都有一个虚函数表（vtable）
- 每个对象包含一个虚表指针（vptr），指向该类的 vtable
- 虚函数调用通过 vptr 查找 vtable，然后调用对应函数（运行时开销）
- 虚表在编译期生成，vptr 在对象构造时初始化

**常见陷阱：**
- 混淆虚函数重写（override）和函数隐藏（hide）
- 在构造函数或析构函数中调用虚函数，导致非预期行为

---

### 3.2 纯虚函数与抽象类

**问题：什么是纯虚函数？什么是抽象类？**

```cpp
// 抽象类（接口）
class IDrawable {
public:
    // 纯虚函数
    virtual void draw() const = 0;

    virtual ~IDrawable() = default;
};

class ISerializable {
public:
    virtual std::string serialize() const = 0;

    virtual ~ISerializable() = default;
};

// 具体类：实现所有纯虚函数
class Circle : public IDrawable, public ISerializable {
public:
    Circle(double radius) : radius_(radius) {}

    void draw() const override {
        std::cout << "Drawing circle with radius " << radius_ << std::endl;
    }

    std::string serialize() const override {
        return "Circle(" + std::to_string(radius_) + ")";
    }

private:
    double radius_;
};

// 仍然是抽象类：只实现了部分纯虚函数
class PartialImpl : public IDrawable, public ISerializable {
public:
    void draw() const override {
        std::cout << "Partial implementation" << std::endl;
    }

    // 没有实现 serialize()，仍是抽象类
};

int main() {
    // IDrawable drawable;  // Error: 抽象类不能实例化

    Circle circle(5.0);
    circle.draw();
    std::cout << circle.serialize() << std::endl;

    // PartialImpl partial;  // Error: 仍是抽象类
}
```

**关键要点：**
- **纯虚函数**：`virtual void func() = 0;`，没有实现（也可以有实现）
- **抽象类**：包含至少一个纯虚函数的类，不能实例化
- 抽象类用于定义接口，强制派生类实现特定行为
- 派生类必须实现所有纯虚函数才能实例化

**常见陷阱：**
- 尝试实例化抽象类
- 派生类忘记实现所有纯虚函数

---

### 3.3 虚函数的特殊情况

**问题：构造函数和析构函数中调用虚函数会发生什么？**

```cpp
class Base {
public:
    Base() {
        std::cout << "Base constructor\n";
        init();  // 危险：在构造函数中调用虚函数
    }

    virtual ~Base() {
        std::cout << "Base destructor\n";
        cleanup();  // 危险：在析构函数中调用虚函数
    }

    virtual void init() {
        std::cout << "Base::init()\n";
    }

    virtual void cleanup() {
        std::cout << "Base::cleanup()\n";
    }
};

class Derived : public Base {
public:
    Derived() : data_(new int(42)) {
        std::cout << "Derived constructor\n";
    }

    ~Derived() override {
        std::cout << "Derived destructor\n";
        delete data_;
    }

    void init() override {
        std::cout << "Derived::init()\n";
        // 如果在 Base 构造时调用，data_ 还未初始化！
        // *data_ = 100;  // 危险！
    }

    void cleanup() override {
        std::cout << "Derived::cleanup()\n";
        // 如果在 Base 析构时调用，data_ 已被销毁！
        // std::cout << *data_ << std::endl;  // 危险！
    }

private:
    int* data_;
};

int main() {
    Derived d;
    /*
    输出：
    Base constructor
    Base::init()         <-- 注意：调用的是 Base 版本，不是 Derived 版本
    Derived constructor
    Derived destructor
    Base destructor
    Base::cleanup()      <-- 注意：调用的是 Base 版本
    */
}
```

**关键要点：**
- 在构造函数中调用虚函数，只会调用当前类的版本，不会多态
- 在析构函数中调用虚函数，只会调用当前类的版本
- 原因：构造/析构期间，对象的类型是当前正在构造/析构的类
- 避免在构造/析构函数中调用虚函数

**常见陷阱：**
- 期望在构造函数中调用派生类的虚函数
- 在析构函数中访问已销毁的派生类成员

---

### 3.4 final 和 override 关键字 (C++11)

**问题：`override` 和 `final` 关键字的作用是什么？**

```cpp
class Base {
public:
    virtual void func1() {}
    virtual void func2() {}
    virtual void func3() final {}  // final: 不能被重写
};

class Derived : public Base {
public:
    void func1() override {}  // override: 明确表示重写基类虚函数

    // void func2(int x) override {}  // Error: 签名不匹配，不是重写

    // void func3() override {}  // Error: func3 是 final，不能重写

    void func4() {}  // 新函数，不是重写
};

// final 类：不能被继承
class FinalClass final {
public:
    void func() {}
};

// class CannotDerive : public FinalClass {};  // Error: FinalClass 是 final

int main() {
    Derived d;
    d.func1();
}
```

**关键要点：**
- `override`：显式表示重写虚函数，编译器会检查是否真的重写
- `final`：禁止函数被进一步重写，或禁止类被继承
- 使用 `override` 可以避免拼写错误和签名不匹配

**常见陷阱：**
- 忘记使用 `override`，导致拼写错误未被发现
- 滥用 `final` 限制了扩展性

---

## 4. RTTI (Run-Time Type Identification)

### 4.1 dynamic_cast

**问题：什么是 `dynamic_cast`？如何安全地向下转型？**

```cpp
class Base {
public:
    virtual ~Base() = default;  // 必须有虚函数才能使用 dynamic_cast
    virtual void baseFunc() {}
};

class Derived : public Base {
public:
    void derivedFunc() {
        std::cout << "Derived function" << std::endl;
    }
};

class AnotherDerived : public Base {
public:
    void anotherFunc() {
        std::cout << "Another derived function" << std::endl;
    }
};

int main() {
    // 向上转型（安全）
    Derived d;
    Base* base_ptr = &d;

    // 向下转型：使用 dynamic_cast（安全，运行时检查）
    Derived* derived_ptr = dynamic_cast<Derived*>(base_ptr);
    if (derived_ptr) {
        derived_ptr->derivedFunc();  // OK
    }

    // 错误的向下转型
    AnotherDerived* another_ptr = dynamic_cast<AnotherDerived*>(base_ptr);
    if (another_ptr == nullptr) {
        std::cout << "Cast failed: not an AnotherDerived" << std::endl;
    }

    // dynamic_cast 用于引用（失败时抛出异常）
    try {
        Base& base_ref = d;
        Derived& derived_ref = dynamic_cast<Derived&>(base_ref);  // OK

        AnotherDerived& another_ref = dynamic_cast<AnotherDerived&>(base_ref);  // 抛出异常
    } catch (const std::bad_cast& e) {
        std::cout << "Bad cast: " << e.what() << std::endl;
    }
}
```

**关键要点：**
- `dynamic_cast` 用于多态类型的安全向下转型（基类指针/引用 -> 派生类指针/引用）
- 转型失败时，指针返回 `nullptr`，引用抛出 `std::bad_cast` 异常
- 只能用于包含虚函数的类（多态类）
- 有运行时开销（需要查找类型信息）

**常见陷阱：**
- 对非多态类使用 `dynamic_cast`
- 忘记检查转型结果是否为 `nullptr`
- 过度使用 `dynamic_cast` 表明设计可能有问题

---

### 4.2 typeid

**问题：如何在运行时获取对象的类型信息？**

```cpp
#include <typeinfo>

class Base {
public:
    virtual ~Base() = default;
};

class Derived : public Base {};

void printType(const Base& obj) {
    std::cout << "Type: " << typeid(obj).name() << std::endl;

    // 比较类型
    if (typeid(obj) == typeid(Derived)) {
        std::cout << "This is a Derived object" << std::endl;
    } else if (typeid(obj) == typeid(Base)) {
        std::cout << "This is a Base object" << std::endl;
    }
}

int main() {
    Base base;
    Derived derived;

    printType(base);     // Type: 4Base (具体名称取决于编译器)
    printType(derived);  // Type: 7Derived

    // 通过基类指针
    Base* ptr = &derived;
    std::cout << "Through pointer: " << typeid(*ptr).name() << std::endl;  // 7Derived

    // 编译时类型 vs 运行时类型
    std::cout << "Compile-time: " << typeid(ptr).name() << std::endl;  // P4Base (指针类型)
    std::cout << "Runtime: " << typeid(*ptr).name() << std::endl;      // 7Derived (实际对象类型)
}
```

**关键要点：**
- `typeid` 返回 `std::type_info` 对象，包含类型信息
- 对于多态类，`typeid` 返回实际类型（运行时类型）
- 对于非多态类，`typeid` 返回静态类型（编译时类型）
- `type_info::name()` 返回的字符串格式由编译器定义

**常见陷阱：**
- 依赖 `name()` 的具体格式（不可移植）
- 对指针使用 `typeid` 得到的是指针类型，而非指向对象的类型

---

## 5. 对象切片 (Object Slicing)

### 5.1 对象切片问题

**问题：什么是对象切片？如何避免？**

```cpp
class Base {
public:
    int base_value_;

    Base(int val = 0) : base_value_(val) {}

    virtual void print() const {
        std::cout << "Base: " << base_value_ << std::endl;
    }
};

class Derived : public Base {
public:
    int derived_value_;

    Derived(int base_val, int derived_val)
        : Base(base_val), derived_value_(derived_val) {}

    void print() const override {
        std::cout << "Derived: " << base_value_
                  << ", " << derived_value_ << std::endl;
    }
};

// 错误：按值传递，发生对象切片
void printByValue(Base obj) {
    obj.print();  // 总是调用 Base::print()，丢失多态性
}

// 正确：按引用传递，保持多态性
void printByReference(const Base& obj) {
    obj.print();  // 根据实际类型调用对应的 print()
}

int main() {
    Derived d(10, 20);

    // 对象切片：派生类部分被"切掉"
    Base b = d;  // derived_value_ 丢失
    b.print();   // 输出: Base: 10

    printByValue(d);      // 输出: Base: 10 (切片)
    printByReference(d);  // 输出: Derived: 10, 20 (多态)

    // 容器中的对象切片
    std::vector<Base> vec;
    vec.push_back(d);  // 切片！只保存 Base 部分
    vec[0].print();    // 输出: Base: 10

    // 正确做法：使用指针或智能指针
    std::vector<std::unique_ptr<Base>> vec_ptr;
    vec_ptr.push_back(std::make_unique<Derived>(10, 20));
    vec_ptr[0]->print();  // 输出: Derived: 10, 20 (多态)
}
```

**关键要点：**
- 对象切片：将派生类对象赋值给基类对象，派生类部分被"切掉"
- 对象切片会丢失多态性和派生类特有成员
- 避免方法：使用指针或引用传递多态对象

**常见陷阱：**
- 按值传递多态对象
- 在 STL 容器中直接存储多态对象（应使用智能指针）

---

## 6. 协变返回类型 (Covariant Return Type)

### 6.1 协变返回类型

**问题：什么是协变返回类型？**

```cpp
class Base {
public:
    virtual ~Base() = default;

    // 返回基类指针
    virtual Base* clone() const {
        return new Base(*this);
    }

    virtual void print() const {
        std::cout << "Base" << std::endl;
    }
};

class Derived : public Base {
public:
    // 协变返回类型：返回派生类指针
    Derived* clone() const override {
        return new Derived(*this);
    }

    void print() const override {
        std::cout << "Derived" << std::endl;
    }

    void derivedOnlyFunc() {
        std::cout << "Derived only" << std::endl;
    }
};

int main() {
    Derived d;

    // 通过派生类对象调用，得到派生类指针
    Derived* d_clone = d.clone();
    d_clone->derivedOnlyFunc();  // OK
    delete d_clone;

    // 通过基类指针调用，得到基类指针（但实际是 Derived*）
    Base* b_ptr = &d;
    Base* b_clone = b_ptr->clone();  // 实际返回 Derived*
    b_clone->print();  // 输出: Derived (多态)
    delete b_clone;
}
```

**关键要点：**
- 协变返回类型：重写虚函数时，返回类型可以是基类版本返回类型的派生类
- 仅适用于返回指针或引用
- 保持类型安全，避免 `dynamic_cast`

**常见陷阱：**
- 返回值类型不满足协变条件（必须是指针或引用）

---

## 7. 虚继承的底层实现

### 7.1 虚继承的内存布局

**问题：虚继承如何实现？内存布局是怎样的？**

```cpp
class Base {
public:
    int base_data_;
    Base() : base_data_(0) {}
};

// 普通继承
class Derived1 : public Base {
public:
    int derived1_data_;
};

class Derived2 : public Base {
public:
    int derived2_data_;
};

// 菱形继承（无虚继承）
class Bottom : public Derived1, public Derived2 {
public:
    int bottom_data_;
};

// 虚继承
class VDerived1 : virtual public Base {
public:
    int vderived1_data_;
};

class VDerived2 : virtual public Base {
public:
    int vderived2_data_;
};

// 菱形继承（虚继承）
class VBottom : public VDerived1, public VDerived2 {
public:
    int vbottom_data_;
};

int main() {
    // 普通继承：Bottom 有两份 Base
    Bottom b;
    std::cout << "sizeof(Bottom): " << sizeof(Bottom) << std::endl;
    // 内存布局：[Derived1: Base + derived1_data_][Derived2: Base + derived2_data_][bottom_data_]

    // 虚继承：VBottom 只有一份 Base
    VBottom vb;
    std::cout << "sizeof(VBottom): " << sizeof(VBottom) << std::endl;
    // 内存布局：[VDerived1: vptr + vderived1_data_][VDerived2: vptr + vderived2_data_][vbottom_data_][Base]
}
```

**关键要点：**
- 虚继承通过虚基类指针（vbptr）实现
- 虚基类子对象放在最派生类对象的末尾
- 虚继承有额外的空间和时间开销
- 最派生类负责初始化虚基类

**常见陷阱：**
- 不理解虚继承的性能开销
- 虚基类初始化顺序混淆

---

## 8. 最佳实践与设计原则

### 8.1 何时使用继承

**问题：什么时候应该使用继承？**

```cpp
// 好的继承使用：is-a 关系
class Vehicle {
public:
    virtual void move() = 0;
    virtual ~Vehicle() = default;
};

class Car : public Vehicle {
public:
    void move() override {
        std::cout << "Car is driving" << std::endl;
    }
};

// 不好的继承使用：has-a 关系应该用组合
class Engine {
public:
    void start() {
        std::cout << "Engine started" << std::endl;
    }
};

// 错误：Car is-a Engine? 不合理
// class Car : public Engine {};

// 正确：Car has-a Engine（组合）
class BetterCar : public Vehicle {
public:
    void move() override {
        engine_.start();
        std::cout << "Car is driving" << std::endl;
    }

private:
    Engine engine_;  // 组合
};
```

**关键要点：**
- 继承表示 "is-a" 关系，组合表示 "has-a" 关系
- 优先使用组合（composition over inheritance）
- 继承用于实现多态，组合用于代码复用

---

### 8.2 Liskov 替换原则

**问题：什么是 Liskov 替换原则？**

```cpp
// 遵循 LSP：派生类可以替换基类
class Rectangle {
public:
    virtual void setWidth(int w) { width_ = w; }
    virtual void setHeight(int h) { height_ = h; }
    virtual int area() const { return width_ * height_; }

protected:
    int width_ = 0;
    int height_ = 0;
};

// 违反 LSP：正方形不能完全替换矩形
class Square : public Rectangle {
public:
    void setWidth(int w) override {
        width_ = w;
        height_ = w;  // 破坏了 Rectangle 的行为
    }

    void setHeight(int h) override {
        width_ = h;
        height_ = h;  // 破坏了 Rectangle 的行为
    }
};

void testRectangle(Rectangle& rect) {
    rect.setWidth(5);
    rect.setHeight(4);
    assert(rect.area() == 20);  // 对 Rectangle 成立，对 Square 失败！
}

int main() {
    Rectangle r;
    testRectangle(r);  // OK

    Square s;
    // testRectangle(s);  // 断言失败！违反 LSP
}
```

**关键要点：**
- Liskov 替换原则：子类对象应该能够替换父类对象而不影响程序正确性
- 违反 LSP 表明继承设计有问题
- 解决方法：重新设计类层次，或使用组合代替继承

---

## 9. 总结

### 核心要点
1. **继承**：public 继承表示 is-a 关系；避免多重继承，必要时使用虚继承
2. **多态**：运行时多态通过虚函数实现；必须通过指针/引用调用虚函数
3. **虚函数**：通过虚表（vtable）实现动态绑定；基类析构函数必须是 virtual
4. **抽象类**：包含纯虚函数的类不能实例化，用于定义接口
5. **RTTI**：`dynamic_cast` 安全向下转型，`typeid` 获取类型信息
6. **对象切片**：避免按值传递多态对象，使用指针/引用
7. **设计原则**：优先组合，遵循 LSP，使用 `override` 和 `final`

### 常见面试题
- 虚函数的实现原理？虚表在哪里？
- 构造函数/析构函数中调用虚函数会怎样？
- 什么时候需要虚析构函数？
- 菱形继承的问题及解决方法？
- `dynamic_cast` 和 `static_cast` 的区别？
- 什么是对象切片？如何避免？
- Liskov 替换原则是什么？

### 性能考虑
- 虚函数有运行时开销（虚表查找），但通常可以忽略
- 虚继承有额外的空间和时间开销
- 过度的继承层次会降低可维护性

---

## 参考资料
- *Effective C++* by Scott Meyers (Item 7, 32-40)
- *More Effective C++* by Scott Meyers (Item 24)
- *C++ Primer* (5th Edition), Chapter 15
- ISO C++ Core Guidelines - Class Hierarchies
