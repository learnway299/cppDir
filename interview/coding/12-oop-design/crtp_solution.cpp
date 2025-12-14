/**
 * @file crtp_solution.cpp
 * @brief CRTP (Curiously Recurring Template Pattern) - 参考答案
 */

#include "crtp.h"
#include <iostream>
#include <cassert>
#include <sstream>

namespace CRTPImpl {

namespace Solution {

// ==================== Circle ====================

class Circle : public Shape<Circle> {
public:
    explicit Circle(double radius) : radius_(radius) {}

    std::string nameImpl() const { return "Circle"; }

    double areaImpl() const {
        return M_PI * radius_ * radius_;
    }

    double getRadius() const { return radius_; }

private:
    double radius_;
};

// ==================== Rectangle ====================

class Rectangle : public Shape<Rectangle> {
public:
    Rectangle(double width, double height) : width_(width), height_(height) {}

    std::string nameImpl() const { return "Rectangle"; }

    double areaImpl() const {
        return width_ * height_;
    }

private:
    double width_, height_;
};

// ==================== Config ====================

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

    int getPort() const { return port_; }
    const std::string& getHost() const { return host_; }

private:
    int port_;
    std::string host_;
};

// ==================== ObjectCounter ====================

template <typename Derived>
class ObjectCounter {
public:
    static size_t getCount() { return count_; }
    static size_t getCreated() { return created_; }
    static void resetStats() { count_ = 0; created_ = 0; }

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
    explicit Entity(int id) : id_(id) {}
    int getId() const { return id_; }
private:
    int id_;
};

class Component : public ObjectCounter<Component> {
public:
    explicit Component(const std::string& name) : name_(name) {}
    const std::string& getName() const { return name_; }
private:
    std::string name_;
};

// ==================== Mixin 模式 ====================

template <typename Derived>
class Printable {
public:
    std::string print() const {
        return static_cast<const Derived*>(this)->toString();
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

// ==================== 表达式模板 ====================

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
    explicit Vec(size_t n) : data_(n) {}

    Vec(std::initializer_list<double> list) : data_(list) {}

    template <typename E>
    Vec(const VecExpression<E>& expr) : data_(expr.size()) {
        for (size_t i = 0; i < expr.size(); ++i) {
            data_[i] = expr[i];
        }
    }

    double operator[](size_t i) const { return data_[i]; }
    double& operator[](size_t i) { return data_[i]; }
    size_t size() const { return data_.size(); }

private:
    std::vector<double> data_;
};

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

template <typename E1, typename E2>
VecSum<E1, E2> vecAdd(const VecExpression<E1>& a, const VecExpression<E2>& b) {
    return VecSum<E1, E2>(static_cast<const E1&>(a), static_cast<const E2&>(b));
}

template <typename E>
VecScale<E> vecScale(double scalar, const VecExpression<E>& expr) {
    return VecScale<E>(scalar, static_cast<const E&>(expr));
}

} // namespace Solution

// ==================== 测试函数 ====================

void runTests() {
    std::cout << "=== CRTP Tests ===" << std::endl;

    // 测试静态多态
    {
        Solution::Circle circle(5.0);
        Solution::Rectangle rect(4.0, 3.0);

        assert(circle.name() == "Circle");
        assert(rect.name() == "Rectangle");

        double circleArea = processShape(circle);
        assert(circleArea > 78.5 && circleArea < 78.6);

        double rectArea = processShape(rect);
        assert(rectArea == 12.0);
    }
    std::cout << "  Static Polymorphism: PASSED" << std::endl;

    // 测试静态接口约束
    {
        Solution::Config config;
        std::string serialized = config.serialize();
        assert(serialized == "localhost:8080");

        Solution::Config config2;
        config2.deserialize("192.168.1.1:9000");
        assert(config2.getHost() == "192.168.1.1");
        assert(config2.getPort() == 9000);
    }
    std::cout << "  Serializable: PASSED" << std::endl;

    // 测试对象计数器
    Solution::Entity::resetStats();
    Solution::Component::resetStats();
    {
        assert(Solution::Entity::getCount() == 0);
        assert(Solution::Component::getCount() == 0);

        Solution::Entity e1(1), e2(2);
        Solution::Component c1("Physics");

        assert(Solution::Entity::getCount() == 2);
        assert(Solution::Component::getCount() == 1);

        {
            Solution::Entity e3(3);
            Solution::Component c2("Graphics");
            assert(Solution::Entity::getCount() == 3);
            assert(Solution::Component::getCount() == 2);
        }

        assert(Solution::Entity::getCount() == 2);
        assert(Solution::Component::getCount() == 1);
        assert(Solution::Entity::getCreated() == 3);
        assert(Solution::Component::getCreated() == 2);
    }
    std::cout << "  ObjectCounter: PASSED" << std::endl;

    // 测试 Mixin 模式
    {
        Solution::Person alice("Alice", 30);
        Solution::Person bob("Bob", 25);
        Solution::Person charlie("Charlie", 30);

        assert(alice.print() == "Person(Alice, 30)");
        assert(bob.print() == "Person(Bob, 25)");

        assert(alice > bob);  // alice 年龄大
        assert(bob < alice);
        assert(!(alice == charlie));  // 名字不同

        assert(alice.hash() != bob.hash());
    }
    std::cout << "  Mixin Pattern: PASSED" << std::endl;

    // 测试表达式模板
    {
        Solution::Vec a = {1.0, 2.0, 3.0};
        Solution::Vec b = {4.0, 5.0, 6.0};
        Solution::Vec c = {7.0, 8.0, 9.0};

        // a + b + c
        Solution::Vec result = Solution::vecAdd(Solution::vecAdd(a, b), c);
        assert(result[0] == 12.0);
        assert(result[1] == 15.0);
        assert(result[2] == 18.0);

        // 2.0 * a + b
        Solution::Vec scaled = Solution::vecAdd(Solution::vecScale(2.0, a), b);
        assert(scaled[0] == 6.0);
        assert(scaled[1] == 9.0);
        assert(scaled[2] == 12.0);
    }
    std::cout << "  Expression Template: PASSED" << std::endl;
}

} // namespace CRTPImpl
