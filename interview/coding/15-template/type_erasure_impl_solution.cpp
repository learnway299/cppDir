/**
 * @file type_erasure_impl_solution.cpp
 * @brief 类型擦除实现 - 解答
 */
#include <memory>
#include <iostream>
#include <typeinfo>
#include <stdexcept>
#include <cstring>
#include <type_traits>

// ==================== 题目1: 简化版 std::function ====================
template <typename Signature>
class Function;

template <typename R, typename... Args>
class Function<R(Args...)> {
public:
    Function() = default;

    // 接受任意可调用对象
    template <typename F,
              typename = std::enable_if_t<!std::is_same_v<std::decay_t<F>, Function>>>
    Function(F&& f) : callable_(std::make_unique<CallableImpl<std::decay_t<F>>>(std::forward<F>(f))) {}

    // 复制构造
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
    // 类型擦除的抽象基类
    struct CallableBase {
        virtual ~CallableBase() = default;
        virtual R invoke(Args... args) = 0;
        virtual std::unique_ptr<CallableBase> clone() const = 0;
    };

    // 具体实现类
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
    friend T any_cast(const Any& a);

    template <typename T>
    friend T any_cast(Any& a);

    template <typename T>
    friend T* any_cast(Any* a) noexcept;

    template <typename T>
    friend const T* any_cast(const Any* a) noexcept;

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

// any_cast 实现
template <typename T>
T any_cast(const Any& a) {
    if (a.type() != typeid(std::remove_reference_t<T>)) {
        throw std::bad_cast();
    }
    return static_cast<Any::Holder<std::remove_reference_t<T>>*>(a.holder_.get())->value;
}

template <typename T>
T any_cast(Any& a) {
    if (a.type() != typeid(std::remove_reference_t<T>)) {
        throw std::bad_cast();
    }
    return static_cast<Any::Holder<std::remove_reference_t<T>>*>(a.holder_.get())->value;
}

template <typename T>
T* any_cast(Any* a) noexcept {
    if (!a || a->type() != typeid(T)) {
        return nullptr;
    }
    return &static_cast<Any::Holder<T>*>(a->holder_.get())->value;
}

template <typename T>
const T* any_cast(const Any* a) noexcept {
    if (!a || a->type() != typeid(T)) {
        return nullptr;
    }
    return &static_cast<Any::Holder<T>*>(a->holder_.get())->value;
}

// ==================== 题目3: 类型安全的多态容器 ====================
// External Polymorphism Pattern
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
            data.draw();  // 要求 T 有 draw() 成员函数
        }

        std::unique_ptr<Concept> clone() const override {
            return std::make_unique<Model>(data);
        }
    };

    std::unique_ptr<Concept> self_;
};

// ==================== 题目4: 带 SBO 的 Function ====================
template <typename Signature, std::size_t BufferSize = 32>
class SmallFunction;

template <typename R, typename... Args, std::size_t BufferSize>
class SmallFunction<R(Args...), BufferSize> {
public:
    SmallFunction() : invoke_(nullptr), manage_(nullptr) {}

    template <typename F,
              typename = std::enable_if_t<!std::is_same_v<std::decay_t<F>, SmallFunction>>>
    SmallFunction(F&& f) {
        using FuncType = std::decay_t<F>;
        if constexpr (sizeof(FuncType) <= BufferSize && std::is_nothrow_move_constructible_v<FuncType>) {
            // Small buffer optimization
            new (&storage_) FuncType(std::forward<F>(f));
            invoke_ = [](const void* ptr, Args... args) -> R {
                return (*static_cast<const FuncType*>(ptr))(std::forward<Args>(args)...);
            };
            manage_ = [](void* dst, const void* src, Operation op) {
                switch (op) {
                    case Operation::Clone:
                        new (dst) FuncType(*static_cast<const FuncType*>(src));
                        break;
                    case Operation::Destroy:
                        static_cast<FuncType*>(dst)->~FuncType();
                        break;
                    case Operation::Move:
                        new (dst) FuncType(std::move(*static_cast<FuncType*>(const_cast<void*>(src))));
                        static_cast<FuncType*>(const_cast<void*>(src))->~FuncType();
                        break;
                }
            };
            isSmall_ = true;
        } else {
            // Heap allocation
            heapPtr_ = new FuncType(std::forward<F>(f));
            invoke_ = [](const void* ptr, Args... args) -> R {
                return (*static_cast<const FuncType*>(ptr))(std::forward<Args>(args)...);
            };
            manage_ = [](void* dst, const void* src, Operation op) {
                switch (op) {
                    case Operation::Clone:
                        *static_cast<void**>(dst) = new FuncType(*static_cast<const FuncType*>(src));
                        break;
                    case Operation::Destroy:
                        delete static_cast<FuncType*>(*static_cast<void**>(dst));
                        break;
                    case Operation::Move:
                        *static_cast<void**>(dst) = *static_cast<void* const*>(src);
                        *static_cast<void**>(const_cast<void*>(src)) = nullptr;
                        break;
                }
            };
            isSmall_ = false;
        }
    }

    ~SmallFunction() {
        if (manage_) {
            manage_(isSmall_ ? &storage_ : &heapPtr_, nullptr, Operation::Destroy);
        }
    }

    SmallFunction(const SmallFunction& other) : invoke_(other.invoke_), manage_(other.manage_), isSmall_(other.isSmall_) {
        if (manage_) {
            const void* src = other.isSmall_ ? &other.storage_ : other.heapPtr_;
            manage_(isSmall_ ? &storage_ : &heapPtr_, src, Operation::Clone);
        }
    }

    SmallFunction(SmallFunction&& other) noexcept
        : invoke_(other.invoke_), manage_(other.manage_), isSmall_(other.isSmall_) {
        if (manage_) {
            void* src = other.isSmall_ ? &other.storage_ : &other.heapPtr_;
            manage_(isSmall_ ? &storage_ : &heapPtr_, src, Operation::Move);
        }
        other.invoke_ = nullptr;
        other.manage_ = nullptr;
    }

    R operator()(Args... args) const {
        if (!invoke_) {
            throw std::bad_function_call();
        }
        const void* ptr = isSmall_ ? &storage_ : heapPtr_;
        return invoke_(ptr, std::forward<Args>(args)...);
    }

    explicit operator bool() const noexcept {
        return invoke_ != nullptr;
    }

private:
    enum class Operation { Clone, Destroy, Move };

    using InvokePtr = R(*)(const void*, Args...);
    using ManagePtr = void(*)(void*, const void*, Operation);

    InvokePtr invoke_;
    ManagePtr manage_;
    bool isSmall_;

    union {
        std::aligned_storage_t<BufferSize> storage_;
        void* heapPtr_;
    };
};

// ==================== 测试代码 ====================
struct Circle {
    void draw() const { std::cout << "Drawing Circle\n"; }
};

struct Square {
    void draw() const { std::cout << "Drawing Square\n"; }
};

int add(int a, int b) { return a + b; }

int main() {
    std::cout << "=== Type Erasure Tests ===\n\n";

    // Test Function
    std::cout << "--- Function ---\n";
    Function<int(int, int)> f1 = add;
    std::cout << "f1(3, 4) = " << f1(3, 4) << "\n";

    Function<int(int)> f2 = [](int x) { return x * x; };
    std::cout << "f2(5) = " << f2(5) << "\n";

    // Test Any
    std::cout << "\n--- Any ---\n";
    Any a1 = 42;
    std::cout << "a1 type: " << a1.type().name() << ", value: " << any_cast<int>(a1) << "\n";

    Any a2 = std::string("hello");
    std::cout << "a2 type: " << a2.type().name() << ", value: " << any_cast<std::string>(a2) << "\n";

    Any a3 = 3.14;
    if (int* p = any_cast<int>(&a3)) {
        std::cout << "a3 is int: " << *p << "\n";
    } else {
        std::cout << "a3 is not int\n";
    }

    // Test Drawable
    std::cout << "\n--- Drawable ---\n";
    Drawable d1 = Circle{};
    Drawable d2 = Square{};
    d1.draw();
    d2.draw();

    std::vector<Drawable> shapes;
    shapes.push_back(Circle{});
    shapes.push_back(Square{});
    for (const auto& s : shapes) {
        s.draw();
    }

    // Test SmallFunction
    std::cout << "\n--- SmallFunction ---\n";
    SmallFunction<int(int, int)> sf1 = add;
    std::cout << "sf1(3, 4) = " << sf1(3, 4) << "\n";

    int capture = 10;
    SmallFunction<int(int)> sf2 = [capture](int x) { return x + capture; };
    std::cout << "sf2(5) = " << sf2(5) << "\n";

    return 0;
}

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
