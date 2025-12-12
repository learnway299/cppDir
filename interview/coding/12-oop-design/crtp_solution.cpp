/**
 * @file crtp_solution.cpp
 * @brief CRTP 实现 - 解答
 */

#include <iostream>
#include <string>
#include <cmath>
#include <vector>
#include <sstream>

/**
 * 题目1：静态多态
 *
 * 通过 CRTP 实现编译期多态，避免虚函数开销
 */
template <typename Derived>
class Shape {
public:
    void draw() const {
        static_cast<const Derived*>(this)->drawImpl();
    }

    double area() const {
        return static_cast<const Derived*>(this)->areaImpl();
    }

    // 统一接口
    void describe() const {
        std::cout << "Shape with area: " << area() << "\n";
        draw();
    }

protected:
    Shape() = default;
};

class Circle : public Shape<Circle> {
public:
    explicit Circle(double radius) : radius_(radius) {}

    void drawImpl() const {
        std::cout << "Drawing Circle with radius " << radius_ << "\n";
    }

    double areaImpl() const {
        return M_PI * radius_ * radius_;
    }

private:
    double radius_;
};

class Rectangle : public Shape<Rectangle> {
public:
    Rectangle(double width, double height) : width_(width), height_(height) {}

    void drawImpl() const {
        std::cout << "Drawing Rectangle " << width_ << "x" << height_ << "\n";
    }

    double areaImpl() const {
        return width_ * height_;
    }

private:
    double width_, height_;
};

// 模板函数可以处理所有 Shape 派生类
template <typename T>
void processShape(const Shape<T>& shape) {
    shape.describe();
}

/**
 * 题目2：静态接口约束
 *
 * 编译期检查派生类是否实现了必要方法
 */
template <typename Derived>
class Serializable {
public:
    std::string serialize() const {
        return static_cast<const Derived*>(this)->serializeImpl();
    }

    void deserialize(const std::string& data) {
        static_cast<Derived*>(this)->deserializeImpl(data);
    }

protected:
    Serializable() = default;

    // 使用 C++20 概念或 static_assert 进行编译期检查
    ~Serializable() {
        // 编译期检查（如果 Derived 没有实现 serializeImpl，这里会报错）
        auto check = &Derived::serializeImpl;
        (void)check;
    }
};

class Config : public Serializable<Config> {
public:
    Config() : port_(8080), host_("localhost") {}

    std::string serializeImpl() const {
        return host_ + ":" + std::to_string(port_);
    }

    void deserializeImpl(const std::string& data) {
        size_t pos = data.find(':');
        if (pos != std::string::npos) {
            host_ = data.substr(0, pos);
            port_ = std::stoi(data.substr(pos + 1));
        }
    }

    void print() const {
        std::cout << "Config: " << host_ << ":" << port_ << "\n";
    }

private:
    int port_;
    std::string host_;
};

/**
 * 题目3：对象计数器
 *
 * 每个类有独立的静态计数器
 */
template <typename Derived>
class ObjectCounter {
public:
    static size_t getCount() { return count_; }
    static size_t getCreated() { return created_; }

protected:
    ObjectCounter() {
        ++count_;
        ++created_;
    }

    ~ObjectCounter() {
        --count_;
    }

    ObjectCounter(const ObjectCounter&) {
        ++count_;
        ++created_;
    }

    ObjectCounter(ObjectCounter&&) noexcept {
        ++count_;
        ++created_;
    }

    ObjectCounter& operator=(const ObjectCounter&) = default;
    ObjectCounter& operator=(ObjectCounter&&) = default;

private:
    static size_t count_;
    static size_t created_;
};

template <typename Derived>
size_t ObjectCounter<Derived>::count_ = 0;

template <typename Derived>
size_t ObjectCounter<Derived>::created_ = 0;

class Entity : public ObjectCounter<Entity> {
public:
    Entity(int id) : id_(id) {}
    int getId() const { return id_; }
private:
    int id_;
};

class Component : public ObjectCounter<Component> {
public:
    Component(const std::string& name) : name_(name) {}
    const std::string& getName() const { return name_; }
private:
    std::string name_;
};

/**
 * 题目4：Mixin 模式
 *
 * 组合多个 CRTP 基类实现功能混入
 */
template <typename Derived>
class Printable {
public:
    void print(std::ostream& os = std::cout) const {
        os << static_cast<const Derived*>(this)->toString() << "\n";
    }
};

template <typename Derived>
class Comparable {
public:
    bool operator==(const Derived& other) const {
        return static_cast<const Derived*>(this)->compareImpl(other) == 0;
    }

    bool operator!=(const Derived& other) const {
        return !(*this == other);
    }

    bool operator<(const Derived& other) const {
        return static_cast<const Derived*>(this)->compareImpl(other) < 0;
    }

    bool operator>(const Derived& other) const {
        return other < static_cast<const Derived&>(*this);
    }

    bool operator<=(const Derived& other) const {
        return !(static_cast<const Derived&>(*this) > other);
    }

    bool operator>=(const Derived& other) const {
        return !(static_cast<const Derived&>(*this) < other);
    }
};

template <typename Derived>
class Hashable {
public:
    size_t hash() const {
        return static_cast<const Derived*>(this)->hashImpl();
    }
};

class Person : public Printable<Person>,
               public Comparable<Person>,
               public Hashable<Person> {
public:
    Person(const std::string& name, int age) : name_(name), age_(age) {}

    std::string toString() const {
        return "Person(" + name_ + ", " + std::to_string(age_) + ")";
    }

    int compareImpl(const Person& other) const {
        if (age_ != other.age_) return age_ - other.age_;
        return name_.compare(other.name_);
    }

    size_t hashImpl() const {
        return std::hash<std::string>{}(name_) ^ (std::hash<int>{}(age_) << 1);
    }

private:
    std::string name_;
    int age_;
};

/**
 * 题目5：表达式模板
 *
 * 惰性求值，避免临时对象
 */
template <typename E>
class VecExpression {
public:
    double operator[](size_t i) const {
        return static_cast<const E&>(*this)[i];
    }

    size_t size() const {
        return static_cast<const E&>(*this).size();
    }
};

class Vec : public VecExpression<Vec> {
public:
    Vec(size_t n) : data_(n) {}

    Vec(std::initializer_list<double> list) : data_(list) {}

    // 从任意表达式构造
    template <typename E>
    Vec(const VecExpression<E>& expr) : data_(expr.size()) {
        for (size_t i = 0; i < expr.size(); ++i) {
            data_[i] = expr[i];
        }
    }

    double operator[](size_t i) const { return data_[i]; }
    double& operator[](size_t i) { return data_[i]; }
    size_t size() const { return data_.size(); }

    void print() const {
        std::cout << "[";
        for (size_t i = 0; i < data_.size(); ++i) {
            if (i > 0) std::cout << ", ";
            std::cout << data_[i];
        }
        std::cout << "]\n";
    }

private:
    std::vector<double> data_;
};

// 表达式类：向量加法
template <typename E1, typename E2>
class VecSum : public VecExpression<VecSum<E1, E2>> {
public:
    VecSum(const E1& a, const E2& b) : a_(a), b_(b) {}

    double operator[](size_t i) const {
        return a_[i] + b_[i];
    }

    size_t size() const { return a_.size(); }

private:
    const E1& a_;
    const E2& b_;
};

// 表达式类：标量乘法
template <typename E>
class VecScale : public VecExpression<VecScale<E>> {
public:
    VecScale(double scalar, const E& expr) : scalar_(scalar), expr_(expr) {}

    double operator[](size_t i) const {
        return scalar_ * expr_[i];
    }

    size_t size() const { return expr_.size(); }

private:
    double scalar_;
    const E& expr_;
};

// 运算符重载
template <typename E1, typename E2>
VecSum<E1, E2> operator+(const VecExpression<E1>& a, const VecExpression<E2>& b) {
    return VecSum<E1, E2>(static_cast<const E1&>(a), static_cast<const E2&>(b));
}

template <typename E>
VecScale<E> operator*(double scalar, const VecExpression<E>& expr) {
    return VecScale<E>(scalar, static_cast<const E&>(expr));
}

int main() {
    std::cout << "=== 静态多态 ===\n";
    {
        Circle circle(5.0);
        Rectangle rect(4.0, 3.0);

        processShape(circle);
        processShape(rect);
    }

    std::cout << "\n=== 静态接口约束 ===\n";
    {
        Config config;
        std::string serialized = config.serialize();
        std::cout << "Serialized: " << serialized << "\n";

        Config config2;
        config2.deserialize("192.168.1.1:9000");
        config2.print();
    }

    std::cout << "\n=== 对象计数器 ===\n";
    {
        std::cout << "Initial - Entity: " << Entity::getCount()
                  << ", Component: " << Component::getCount() << "\n";

        Entity e1(1), e2(2);
        Component c1("Physics");

        std::cout << "After creation - Entity: " << Entity::getCount()
                  << ", Component: " << Component::getCount() << "\n";

        {
            Entity e3(3);
            Component c2("Graphics");
            std::cout << "Inside scope - Entity: " << Entity::getCount()
                      << ", Component: " << Component::getCount() << "\n";
        }

        std::cout << "After scope - Entity: " << Entity::getCount()
                  << ", Component: " << Component::getCount() << "\n";
        std::cout << "Total created - Entity: " << Entity::getCreated()
                  << ", Component: " << Component::getCreated() << "\n";
    }

    std::cout << "\n=== Mixin 模式 ===\n";
    {
        Person alice("Alice", 30);
        Person bob("Bob", 25);
        Person charlie("Charlie", 30);

        alice.print();
        bob.print();

        std::cout << "alice < bob: " << (alice < bob) << "\n";
        std::cout << "alice > bob: " << (alice > bob) << "\n";
        std::cout << "alice == charlie: " << (alice == charlie) << "\n";

        std::cout << "Hash of alice: " << alice.hash() << "\n";
    }

    std::cout << "\n=== 表达式模板 ===\n";
    {
        Vec a = {1.0, 2.0, 3.0};
        Vec b = {4.0, 5.0, 6.0};
        Vec c = {7.0, 8.0, 9.0};

        std::cout << "a = "; a.print();
        std::cout << "b = "; b.print();
        std::cout << "c = "; c.print();

        // a + b + c 不会产生临时 Vec 对象
        // 表达式在赋值时才真正计算
        Vec result = a + b + c;
        std::cout << "a + b + c = "; result.print();

        // 2.0 * a + b
        Vec scaled = 2.0 * a + b;
        std::cout << "2*a + b = "; scaled.print();
    }

    return 0;
}
