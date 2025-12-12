/**
 * @file type_erasure.cpp
 * @brief 类型擦除实现 - 面试题
 *
 * 类型擦除：隐藏具体类型，提供统一接口
 * 结合了运行时多态的灵活性和值语义的安全性
 */

#include <iostream>
#include <memory>
#include <string>
#include <vector>

/**
 * 题目1：实现简单的类型擦除容器
 *
 * 场景：存储任意可打印对象
 * 要求：
 * 1. 支持任意具有 print() 方法的类型
 * 2. 值语义（支持拷贝）
 */
class Printable {
public:
    // TODO: 实现构造函数（接受任意可打印类型）
    template <typename T>
    Printable(T obj) {
    }

    // TODO: 实现拷贝构造
    Printable(const Printable& other) {
    }

    // TODO: 实现拷贝赋值
    Printable& operator=(const Printable& other) {
        return *this;
    }

    // TODO: 实现打印
    void print() const {
    }

private:
    // TODO: 定义内部接口和模型
    struct Concept {
        virtual ~Concept() = default;
        virtual void print() const = 0;
        virtual std::unique_ptr<Concept> clone() const = 0;
    };

    template <typename T>
    struct Model : Concept {
        // TODO: 实现
    };

    std::unique_ptr<Concept> pImpl_;
};

/**
 * 题目2：实现 std::function 风格的类型擦除
 *
 * 要求：
 * 1. 可以存储任意可调用对象
 * 2. 支持 lambda、函数指针、仿函数
 */
template <typename Signature>
class Function;

template <typename R, typename... Args>
class Function<R(Args...)> {
public:
    Function() = default;

    // TODO: 接受任意可调用对象
    template <typename F>
    Function(F f) {
    }

    // TODO: 实现调用
    R operator()(Args... args) const {
        return R();
    }

    // TODO: 实现 bool 转换
    explicit operator bool() const {
        return false;
    }

private:
    // TODO: 定义内部结构
};

/**
 * 题目3：实现 std::any 风格的类型擦除
 *
 * 要求：
 * 1. 可以存储任意类型
 * 2. 支持类型安全的取值
 */
class Any {
public:
    Any() = default;

    // TODO: 接受任意类型
    template <typename T>
    Any(T value) {
    }

    // TODO: 判断是否有值
    bool hasValue() const {
        return false;
    }

    // TODO: 获取类型信息
    const std::type_info& type() const {
        return typeid(void);
    }

    // TODO: 类型安全的取值
    template <typename T>
    T& get() {
        throw std::bad_cast();
    }

private:
    // TODO: 定义内部结构
};

/**
 * 题目4：实现小对象优化（SBO）的类型擦除
 *
 * 要求：
 * 1. 小对象存储在栈上
 * 2. 大对象存储在堆上
 */
class AnyWithSBO {
public:
    // TODO: 实现带小对象优化的 Any

private:
    static constexpr size_t BufferSize = 32;
    alignas(std::max_align_t) char buffer_[BufferSize];
    // TODO: 其他成员
};

int main() {
    return 0;
}
