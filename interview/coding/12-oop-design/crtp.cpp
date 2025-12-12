/**
 * @file crtp.cpp
 * @brief CRTP (Curiously Recurring Template Pattern) 实现 - 面试题
 *
 * CRTP：基类模板以派生类作为模板参数
 * 实现静态多态，零运行时开销
 */

#include <iostream>
#include <string>
#include <chrono>
#include <vector>

/**
 * 题目1：实现静态多态（替代虚函数）
 *
 * 要求：
 * 1. 基类提供接口
 * 2. 派生类提供实现
 * 3. 编译期绑定，无虚函数开销
 */
template <typename Derived>
class Shape {
public:
    // TODO: 实现静态多态接口
    void draw() const {
        // 调用派生类的实现
    }

    double area() const {
        return 0.0;
    }

protected:
    // 防止直接实例化基类
    Shape() = default;
};

// TODO: 实现派生类
class Circle : public Shape<Circle> {
public:
    Circle(double radius) : radius_(radius) {}

    void drawImpl() const {
        // TODO
    }

    double areaImpl() const {
        return 0.0;
    }

private:
    double radius_;
};

class Rectangle : public Shape<Rectangle> {
public:
    Rectangle(double width, double height) : width_(width), height_(height) {}

    void drawImpl() const {
        // TODO
    }

    double areaImpl() const {
        return 0.0;
    }

private:
    double width_, height_;
};

/**
 * 题目2：实现静态接口约束（编译期检查）
 *
 * 要求：
 * 1. 派生类必须实现特定方法
 * 2. 编译期报错提示
 */
template <typename Derived>
class Serializable {
public:
    std::string serialize() const {
        // TODO: 调用派生类的 serializeImpl
        return "";
    }

    void deserialize(const std::string& data) {
        // TODO
    }

private:
    // 编译期检查辅助
    Serializable() {
        // TODO: 静态断言检查派生类是否实现了必要方法
    }
    friend Derived;
};

/**
 * 题目3：实现对象计数器
 *
 * 要求：
 * 1. 每个类有独立的计数器
 * 2. 跟踪创建和销毁
 */
template <typename Derived>
class ObjectCounter {
public:
    static size_t getCount() {
        return count_;
    }

protected:
    ObjectCounter() {
        // TODO
    }

    ~ObjectCounter() {
        // TODO
    }

    // 拷贝也计数
    ObjectCounter(const ObjectCounter&) {
        // TODO
    }

private:
    static size_t count_;
};

template <typename Derived>
size_t ObjectCounter<Derived>::count_ = 0;

/**
 * 题目4：实现 Mixin（混入）模式
 *
 * 通过 CRTP 组合多个功能
 */
template <typename Derived>
class Printable {
public:
    void print() const {
        // TODO
    }
};

template <typename Derived>
class Comparable {
public:
    bool operator<(const Derived& other) const {
        // TODO
        return false;
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

// 组合使用
class Person : public Printable<Person>, public Comparable<Person> {
public:
    Person(const std::string& name, int age) : name_(name), age_(age) {}

    // TODO: 实现 Printable 需要的方法
    // TODO: 实现 Comparable 需要的方法

private:
    std::string name_;
    int age_;
};

/**
 * 题目5：实现表达式模板（性能优化）
 *
 * 场景：向量运算，避免临时对象
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

// TODO: 实现 Vec 类
// TODO: 实现 VecSum 表达式类
// TODO: 实现 operator+

int main() {
    return 0;
}
