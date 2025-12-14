/**
 * @file crtp.h
 * @brief CRTP (Curiously Recurring Template Pattern) - 题目说明与声明
 *
 * CRTP：基类模板以派生类作为模板参数
 * 实现静态多态，零运行时开销
 *
 * 包含题目：
 * 1. 静态多态（替代虚函数）
 * 2. 静态接口约束（编译期检查）
 * 3. 对象计数器
 * 4. Mixin（混入）模式
 * 5. 表达式模板（性能优化）
 */

#ifndef CRTP_H
#define CRTP_H

#include <string>
#include <vector>
#include <functional>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace CRTPImpl {

// ==================== 题目1: 静态多态 ====================

/**
 * 题目1: 实现静态多态（替代虚函数）
 *
 * 要求：
 * 1. 基类提供接口
 * 2. 派生类提供实现
 * 3. 编译期绑定，无虚函数开销
 *
 * 示例：
 *   Circle circle(5.0);
 *   double area = circle.area();  // M_PI * 25
 *   std::string name = circle.name();  // "Circle"
 */
template <typename Derived>
class Shape {
public:
    double area() const {
        return static_cast<const Derived*>(this)->areaImpl();
    }

    std::string name() const {
        return static_cast<const Derived*>(this)->nameImpl();
    }

protected:
    Shape() = default;
};

class Circle : public Shape<Circle> {
public:
    explicit Circle(double radius);

    std::string nameImpl() const;
    double areaImpl() const;
    double getRadius() const;

private:
    double radius_;
};

class Rectangle : public Shape<Rectangle> {
public:
    Rectangle(double width, double height);

    std::string nameImpl() const;
    double areaImpl() const;

private:
    double width_, height_;
};

template <typename T>
double processShape(const Shape<T>& shape) {
    return shape.area();
}

// ==================== 题目2: 静态接口约束 ====================

/**
 * 题目2: 实现静态接口约束
 *
 * 要求：
 * 1. 派生类必须实现 serializeImpl / deserializeImpl
 * 2. 编译期检查
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
};

class Config : public Serializable<Config> {
public:
    Config();

    std::string serializeImpl() const;
    void deserializeImpl(const std::string& data);

    int getPort() const;
    const std::string& getHost() const;

private:
    int port_;
    std::string host_;
};

// ==================== 题目3: 对象计数器 ====================

/**
 * 题目3: 实现对象计数器
 *
 * 要求：
 * 1. 每个类有独立的计数器
 * 2. 跟踪创建和销毁
 * 3. 支持拷贝和移动
 */
template <typename Derived>
class ObjectCounter {
public:
    static size_t getCount() { return count_; }
    static size_t getCreated() { return created_; }
    static void resetStats() { count_ = 0; created_ = 0; }

protected:
    ObjectCounter();
    ~ObjectCounter();
    ObjectCounter(const ObjectCounter&);
    ObjectCounter(ObjectCounter&&) noexcept;
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
    explicit Entity(int id);
    int getId() const;

private:
    int id_;
};

class Component : public ObjectCounter<Component> {
public:
    explicit Component(const std::string& name);
    const std::string& getName() const;

private:
    std::string name_;
};

// ==================== 题目4: Mixin 模式 ====================

/**
 * 题目4: 实现 Mixin（混入）模式
 *
 * 通过 CRTP 组合多个功能
 * 派生类需要实现对应的 xxxImpl 方法
 */
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
    Person(const std::string& name, int age);

    std::string toString() const;
    int compareImpl(const Person& other) const;
    size_t hashImpl() const;

private:
    std::string name_;
    int age_;
};

// ==================== 题目5: 表达式模板 ====================

/**
 * 题目5: 实现表达式模板（性能优化）
 *
 * 场景：向量运算，避免临时对象
 * 通过延迟求值优化 a + b + c 这样的链式运算
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
    explicit Vec(size_t n);
    Vec(std::initializer_list<double> list);

    template <typename E>
    Vec(const VecExpression<E>& expr) : data_(expr.size()) {
        for (size_t i = 0; i < expr.size(); ++i) {
            data_[i] = expr[i];
        }
    }

    double operator[](size_t i) const;
    double& operator[](size_t i);
    size_t size() const;

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

// ==================== 测试函数 ====================

void runTests();

} // namespace CRTPImpl

#endif // CRTP_H
