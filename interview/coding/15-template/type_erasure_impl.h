/**
 * @file type_erasure_impl.h
 * @brief 类型擦除实现 - 题目说明与声明
 *
 * 类型擦除是一种技术，允许在运行时处理不同类型的对象，
 * 同时隐藏具体类型信息。std::function 和 std::any 是典型例子。
 *
 * 包含题目：
 * 1. 简化版 std::function
 * 2. 简化版 std::any
 * 3. 类型安全的多态容器 (External Polymorphism)
 * 4. 带 SBO 的类型擦除
 */

#ifndef TYPE_ERASURE_IMPL_H
#define TYPE_ERASURE_IMPL_H

#include <memory>
#include <typeinfo>
#include <stdexcept>
#include <type_traits>
#include <functional>

namespace TypeErasureTemplateImpl {

// ==================== 题目1: 简化版 std::function ====================

/**
 * 题目1: 实现简化版 std::function<R(Args...)>
 *
 * 可以存储任何可调用对象（函数、lambda、函数对象）
 *
 * 示例：
 *   Function<int(int, int)> f = [](int a, int b) { return a + b; };
 *   int result = f(3, 4);  // 7
 */
template <typename Signature>
class Function;

template <typename R, typename... Args>
class Function<R(Args...)> {
public:
    Function() = default;

    template <typename F,
              typename = std::enable_if_t<!std::is_same_v<std::decay_t<F>, Function>>>
    Function(F&& f);

    Function(const Function& other);
    Function(Function&&) noexcept = default;

    Function& operator=(const Function& other);
    Function& operator=(Function&&) noexcept = default;

    R operator()(Args... args) const;

    explicit operator bool() const noexcept;

private:
    struct CallableBase {
        virtual ~CallableBase() = default;
        virtual R invoke(Args... args) = 0;
        virtual std::unique_ptr<CallableBase> clone() const = 0;
    };

    template <typename F>
    struct CallableImpl : CallableBase {
        F func;

        CallableImpl(F f) : func(std::move(f)) {}

        R invoke(Args... args) override {
            return func(std::forward<Args>(args)...);
        }

        std::unique_ptr<CallableBase> clone() const override {
            return std::make_unique<CallableImpl>(func);
        }
    };

    std::unique_ptr<CallableBase> callable_;
};

// ==================== 题目2: 简化版 std::any ====================

/**
 * 题目2: 实现简化版 std::any
 *
 * 可以存储任何类型的值
 *
 * 示例：
 *   Any a = 42;
 *   int x = a.get<int>();  // 42
 *   a = std::string("hello");
 *   std::string s = a.get<std::string>();  // "hello"
 */
class Any {
public:
    Any() = default;

    template <typename T,
              typename = std::enable_if_t<!std::is_same_v<std::decay_t<T>, Any>>>
    Any(T&& value);

    Any(const Any& other);
    Any(Any&&) noexcept = default;

    Any& operator=(const Any& other);
    Any& operator=(Any&&) noexcept = default;

    const std::type_info& type() const noexcept;
    bool has_value() const noexcept;
    void reset() noexcept;

    template <typename T>
    T* get_if() noexcept;

    template <typename T>
    const T* get_if() const noexcept;

    template <typename T>
    T& get();

    template <typename T>
    const T& get() const;

private:
    struct HolderBase {
        virtual ~HolderBase() = default;
        virtual const std::type_info& type() const noexcept = 0;
        virtual std::unique_ptr<HolderBase> clone() const = 0;
    };

    template <typename T>
    struct Holder : HolderBase {
        T value;

        Holder(T v) : value(std::move(v)) {}

        const std::type_info& type() const noexcept override {
            return typeid(T);
        }

        std::unique_ptr<HolderBase> clone() const override {
            return std::make_unique<Holder>(value);
        }
    };

    std::unique_ptr<HolderBase> holder_;
};

// ==================== 题目3: 类型安全的多态容器 ====================

/**
 * 题目3: 实现类型安全的多态容器 (External Polymorphism)
 *
 * 存储不同类型但满足相同概念的对象
 * 例如：所有可以 draw() 的对象
 *
 * 示例：
 *   struct Circle { void draw() const { ... } };
 *   struct Square { void draw() const { ... } };
 *   std::vector<Drawable> shapes;
 *   shapes.push_back(Circle{});
 *   shapes.push_back(Square{});
 */
class Drawable {
public:
    template <typename T>
    Drawable(T x);

    Drawable(const Drawable& other);
    Drawable(Drawable&&) noexcept = default;

    Drawable& operator=(const Drawable& other);
    Drawable& operator=(Drawable&&) noexcept = default;

    void draw() const;

private:
    struct Concept {
        virtual ~Concept() = default;
        virtual void draw() const = 0;
        virtual std::unique_ptr<Concept> clone() const = 0;
    };

    template <typename T>
    struct Model : Concept {
        T data;

        Model(T x) : data(std::move(x)) {}

        void draw() const override {
            data.draw();
        }

        std::unique_ptr<Concept> clone() const override {
            return std::make_unique<Model>(data);
        }
    };

    std::unique_ptr<Concept> self_;
};

// ==================== 测试辅助类型 ====================

struct Circle {
    int radius = 10;
    void draw() const;
};

struct Square {
    int side = 5;
    void draw() const;
};

int add(int a, int b);

// ==================== 测试函数 ====================

void runTests();

} // namespace TypeErasureTemplateImpl

#endif // TYPE_ERASURE_IMPL_H
