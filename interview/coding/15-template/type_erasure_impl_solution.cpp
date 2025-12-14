/**
 * @file type_erasure_impl_solution.cpp
 * @brief 类型擦除实现 - 参考答案
 */

#include "type_erasure_impl.h"
#include <iostream>
#include <vector>
#include <cassert>

namespace TypeErasureTemplateImpl {

namespace Solution {

// ==================== Function ====================

template <typename Signature>
class Function;

template <typename R, typename... Args>
class Function<R(Args...)> {
public:
    Function() = default;

    template <typename F,
              typename = std::enable_if_t<!std::is_same_v<std::decay_t<F>, Function>>>
    Function(F&& f) : callable_(std::make_unique<CallableImpl<std::decay_t<F>>>(std::forward<F>(f))) {}

    Function(const Function& other) {
        if (other.callable_) {
            callable_ = other.callable_->clone();
        }
    }

    Function(Function&&) noexcept = default;

    Function& operator=(const Function& other) {
        if (this != &other) {
            callable_ = other.callable_ ? other.callable_->clone() : nullptr;
        }
        return *this;
    }

    Function& operator=(Function&&) noexcept = default;

    R operator()(Args... args) const {
        if (!callable_) {
            throw std::bad_function_call();
        }
        return callable_->invoke(std::forward<Args>(args)...);
    }

    explicit operator bool() const noexcept {
        return callable_ != nullptr;
    }

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

// ==================== Any ====================

class Any {
public:
    Any() = default;

    template <typename T,
              typename = std::enable_if_t<!std::is_same_v<std::decay_t<T>, Any>>>
    Any(T&& value) : holder_(std::make_unique<Holder<std::decay_t<T>>>(std::forward<T>(value))) {}

    Any(const Any& other) {
        if (other.holder_) {
            holder_ = other.holder_->clone();
        }
    }

    Any(Any&&) noexcept = default;

    Any& operator=(const Any& other) {
        if (this != &other) {
            holder_ = other.holder_ ? other.holder_->clone() : nullptr;
        }
        return *this;
    }

    Any& operator=(Any&&) noexcept = default;

    const std::type_info& type() const noexcept {
        return holder_ ? holder_->type() : typeid(void);
    }

    bool has_value() const noexcept {
        return holder_ != nullptr;
    }

    void reset() noexcept {
        holder_.reset();
    }

    template <typename T>
    T* get_if() noexcept {
        if (!holder_ || holder_->type() != typeid(T)) {
            return nullptr;
        }
        return &static_cast<Holder<T>*>(holder_.get())->value;
    }

    template <typename T>
    const T* get_if() const noexcept {
        if (!holder_ || holder_->type() != typeid(T)) {
            return nullptr;
        }
        return &static_cast<const Holder<T>*>(holder_.get())->value;
    }

    template <typename T>
    T& get() {
        T* ptr = get_if<T>();
        if (!ptr) {
            throw std::bad_cast();
        }
        return *ptr;
    }

    template <typename T>
    const T& get() const {
        const T* ptr = get_if<T>();
        if (!ptr) {
            throw std::bad_cast();
        }
        return *ptr;
    }

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

// ==================== Drawable ====================

class Drawable {
public:
    template <typename T>
    Drawable(T x) : self_(std::make_unique<Model<T>>(std::move(x))) {}

    Drawable(const Drawable& other) : self_(other.self_->clone()) {}

    Drawable(Drawable&&) noexcept = default;

    Drawable& operator=(const Drawable& other) {
        self_ = other.self_->clone();
        return *this;
    }

    Drawable& operator=(Drawable&&) noexcept = default;

    void draw() const {
        self_->draw();
    }

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
    void draw() const { std::cout << "Drawing Circle (r=" << radius << ")\n"; }
};

struct Square {
    int side = 5;
    void draw() const { std::cout << "Drawing Square (s=" << side << ")\n"; }
};

int add(int a, int b) { return a + b; }

} // namespace Solution

// ==================== 测试函数 ====================

void runTests() {
    std::cout << "=== Type Erasure Impl Tests ===" << std::endl;

    // Test Function
    {
        Solution::Function<int(int, int)> f1 = Solution::add;
        assert(f1(3, 4) == 7);

        Solution::Function<int(int)> f2 = [](int x) { return x * x; };
        assert(f2(5) == 25);

        // Copy test
        auto f3 = f1;
        assert(f3(2, 3) == 5);
    }
    std::cout << "  Function: PASSED" << std::endl;

    // Test Any
    {
        Solution::Any a1 = 42;
        assert(a1.has_value());
        assert(a1.get<int>() == 42);

        Solution::Any a2 = std::string("hello");
        assert(a2.get<std::string>() == "hello");

        Solution::Any a3 = 3.14;
        assert(a3.get_if<int>() == nullptr);
        assert(a3.get_if<double>() != nullptr);

        // Copy test
        Solution::Any a4 = a1;
        assert(a4.get<int>() == 42);
    }
    std::cout << "  Any: PASSED" << std::endl;

    // Test Drawable
    {
        Solution::Drawable d1 = Solution::Circle{};
        Solution::Drawable d2 = Solution::Square{};

        std::vector<Solution::Drawable> shapes;
        shapes.push_back(Solution::Circle{});
        shapes.push_back(Solution::Square{});

        // Copy test
        Solution::Drawable d3 = d1;
        assert(shapes.size() == 2);
    }
    std::cout << "  Drawable: PASSED" << std::endl;
}

} // namespace TypeErasureTemplateImpl

/**
 * 关键要点：
 *
 * 1. 类型擦除的核心模式：
 *    - 抽象基类定义接口
 *    - 模板派生类持有具体类型
 *    - 通过基类指针实现多态
 *
 * 2. 实现要点：
 *    - 构造函数接受任意类型
 *    - 复制语义需要 clone()
 *    - 移动语义提高效率
 *
 * 3. Small Buffer Optimization：
 *    - 小对象存储在栈上的 buffer
 *    - 大对象分配在堆上
 *    - 避免小对象的堆分配开销
 *    - 需要处理构造、析构、复制、移动
 *
 * 4. 类型安全：
 *    - any_cast 检查类型匹配
 *    - 类型不匹配抛出 bad_cast
 *    - 指针版本返回 nullptr
 *
 * 5. External Polymorphism：
 *    - 不需要继承关系
 *    - 通过 concept/model 模式实现
 *    - 允许对外部类型进行多态处理
 *
 * 6. 标准库实现：
 *    - std::function：可调用对象包装
 *    - std::any：任意类型容器
 *    - std::shared_ptr 的 deleter：类型擦除删除器
 */
