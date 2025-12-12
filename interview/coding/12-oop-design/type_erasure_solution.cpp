/**
 * @file type_erasure_solution.cpp
 * @brief 类型擦除实现 - 解答
 */

#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <typeinfo>
#include <stdexcept>
#include <functional>

/**
 * 题目1：简单类型擦除容器
 *
 * 实现思路：
 * 1. Concept（抽象基类）定义接口
 * 2. Model<T>（模板派生类）持有具体类型并实现接口
 */
class Printable {
public:
    template <typename T>
    Printable(T obj) : pImpl_(std::make_unique<Model<T>>(std::move(obj))) {}

    Printable(const Printable& other)
        : pImpl_(other.pImpl_ ? other.pImpl_->clone() : nullptr) {}

    Printable(Printable&& other) noexcept = default;

    Printable& operator=(const Printable& other) {
        if (this != &other) {
            pImpl_ = other.pImpl_ ? other.pImpl_->clone() : nullptr;
        }
        return *this;
    }

    Printable& operator=(Printable&& other) noexcept = default;

    void print() const {
        if (pImpl_) {
            pImpl_->print();
        }
    }

private:
    struct Concept {
        virtual ~Concept() = default;
        virtual void print() const = 0;
        virtual std::unique_ptr<Concept> clone() const = 0;
    };

    template <typename T>
    struct Model : Concept {
        T data_;

        explicit Model(T obj) : data_(std::move(obj)) {}

        void print() const override {
            data_.print();
        }

        std::unique_ptr<Concept> clone() const override {
            return std::make_unique<Model>(*this);
        }
    };

    std::unique_ptr<Concept> pImpl_;
};

// 测试用的可打印类
struct Document {
    std::string content;
    void print() const {
        std::cout << "Document: " << content << "\n";
    }
};

struct Image {
    int width, height;
    void print() const {
        std::cout << "Image: " << width << "x" << height << "\n";
    }
};

/**
 * 题目2：实现 std::function 风格的类型擦除
 */
template <typename Signature>
class Function;

template <typename R, typename... Args>
class Function<R(Args...)> {
public:
    Function() = default;

    template <typename F>
    Function(F f) : pImpl_(std::make_unique<Model<F>>(std::move(f))) {}

    Function(const Function& other)
        : pImpl_(other.pImpl_ ? other.pImpl_->clone() : nullptr) {}

    Function(Function&&) noexcept = default;

    Function& operator=(const Function& other) {
        if (this != &other) {
            pImpl_ = other.pImpl_ ? other.pImpl_->clone() : nullptr;
        }
        return *this;
    }

    Function& operator=(Function&&) noexcept = default;

    R operator()(Args... args) const {
        if (!pImpl_) {
            throw std::bad_function_call();
        }
        return pImpl_->invoke(std::forward<Args>(args)...);
    }

    explicit operator bool() const {
        return pImpl_ != nullptr;
    }

private:
    struct Concept {
        virtual ~Concept() = default;
        virtual R invoke(Args...) const = 0;
        virtual std::unique_ptr<Concept> clone() const = 0;
    };

    template <typename F>
    struct Model : Concept {
        F func_;

        explicit Model(F f) : func_(std::move(f)) {}

        R invoke(Args... args) const override {
            return func_(std::forward<Args>(args)...);
        }

        std::unique_ptr<Concept> clone() const override {
            return std::make_unique<Model>(*this);
        }
    };

    std::unique_ptr<Concept> pImpl_;
};

/**
 * 题目3：实现 std::any 风格的类型擦除
 */
class Any {
public:
    Any() = default;

    template <typename T>
    Any(T value) : pImpl_(std::make_unique<Model<std::decay_t<T>>>(std::move(value))) {}

    Any(const Any& other)
        : pImpl_(other.pImpl_ ? other.pImpl_->clone() : nullptr) {}

    Any(Any&&) noexcept = default;

    Any& operator=(const Any& other) {
        if (this != &other) {
            pImpl_ = other.pImpl_ ? other.pImpl_->clone() : nullptr;
        }
        return *this;
    }

    Any& operator=(Any&&) noexcept = default;

    template <typename T>
    Any& operator=(T value) {
        pImpl_ = std::make_unique<Model<std::decay_t<T>>>(std::move(value));
        return *this;
    }

    bool hasValue() const {
        return pImpl_ != nullptr;
    }

    const std::type_info& type() const {
        return pImpl_ ? pImpl_->type() : typeid(void);
    }

    template <typename T>
    T* tryGet() {
        if (pImpl_ && pImpl_->type() == typeid(T)) {
            return &static_cast<Model<T>*>(pImpl_.get())->data_;
        }
        return nullptr;
    }

    template <typename T>
    T& get() {
        T* ptr = tryGet<T>();
        if (!ptr) {
            throw std::bad_cast();
        }
        return *ptr;
    }

    template <typename T>
    const T& get() const {
        return const_cast<Any*>(this)->get<T>();
    }

    void reset() {
        pImpl_.reset();
    }

private:
    struct Concept {
        virtual ~Concept() = default;
        virtual const std::type_info& type() const = 0;
        virtual std::unique_ptr<Concept> clone() const = 0;
    };

    template <typename T>
    struct Model : Concept {
        T data_;

        explicit Model(T value) : data_(std::move(value)) {}

        const std::type_info& type() const override {
            return typeid(T);
        }

        std::unique_ptr<Concept> clone() const override {
            return std::make_unique<Model>(*this);
        }
    };

    std::unique_ptr<Concept> pImpl_;
};

// any_cast 辅助函数
template <typename T>
T any_cast(Any& a) {
    return a.get<std::remove_reference_t<T>>();
}

template <typename T>
T any_cast(const Any& a) {
    return a.get<std::remove_reference_t<T>>();
}

/**
 * 题目4：带小对象优化（SBO）的类型擦除
 */
class AnyWithSBO {
public:
    AnyWithSBO() : vtable_(nullptr) {}

    template <typename T>
    AnyWithSBO(T value) {
        using DecayT = std::decay_t<T>;
        if constexpr (sizeof(DecayT) <= BufferSize &&
                      alignof(DecayT) <= alignof(std::max_align_t) &&
                      std::is_nothrow_move_constructible_v<DecayT>) {
            // 小对象：存储在栈上
            new (buffer_) DecayT(std::move(value));
            vtable_ = &getVTable<DecayT, true>();
        } else {
            // 大对象：存储在堆上
            *reinterpret_cast<DecayT**>(buffer_) = new DecayT(std::move(value));
            vtable_ = &getVTable<DecayT, false>();
        }
    }

    ~AnyWithSBO() {
        if (vtable_) {
            vtable_->destroy(buffer_);
        }
    }

    AnyWithSBO(const AnyWithSBO& other) : vtable_(other.vtable_) {
        if (vtable_) {
            vtable_->copy(other.buffer_, buffer_);
        }
    }

    AnyWithSBO(AnyWithSBO&& other) noexcept : vtable_(other.vtable_) {
        if (vtable_) {
            vtable_->move(other.buffer_, buffer_);
            other.vtable_ = nullptr;
        }
    }

    AnyWithSBO& operator=(const AnyWithSBO& other) {
        if (this != &other) {
            AnyWithSBO tmp(other);
            swap(tmp);
        }
        return *this;
    }

    AnyWithSBO& operator=(AnyWithSBO&& other) noexcept {
        if (this != &other) {
            if (vtable_) {
                vtable_->destroy(buffer_);
            }
            vtable_ = other.vtable_;
            if (vtable_) {
                vtable_->move(other.buffer_, buffer_);
                other.vtable_ = nullptr;
            }
        }
        return *this;
    }

    void swap(AnyWithSBO& other) noexcept {
        std::swap(vtable_, other.vtable_);
        std::swap(buffer_, other.buffer_);
    }

    bool hasValue() const { return vtable_ != nullptr; }

    const std::type_info& type() const {
        return vtable_ ? vtable_->type() : typeid(void);
    }

    template <typename T>
    T* tryGet() {
        if (vtable_ && vtable_->type() == typeid(T)) {
            if (vtable_->isSmall) {
                return reinterpret_cast<T*>(buffer_);
            } else {
                return *reinterpret_cast<T**>(buffer_);
            }
        }
        return nullptr;
    }

private:
    static constexpr size_t BufferSize = 32;

    struct VTable {
        const std::type_info& (*type)();
        void (*destroy)(char*);
        void (*copy)(const char*, char*);
        void (*move)(char*, char*);
        bool isSmall;
    };

    template <typename T, bool IsSmall>
    static const VTable& getVTable() {
        static const VTable vtable = {
            []() -> const std::type_info& { return typeid(T); },
            [](char* buf) {
                if constexpr (IsSmall) {
                    reinterpret_cast<T*>(buf)->~T();
                } else {
                    delete *reinterpret_cast<T**>(buf);
                }
            },
            [](const char* src, char* dst) {
                if constexpr (IsSmall) {
                    new (dst) T(*reinterpret_cast<const T*>(src));
                } else {
                    *reinterpret_cast<T**>(dst) = new T(**reinterpret_cast<T* const*>(src));
                }
            },
            [](char* src, char* dst) {
                if constexpr (IsSmall) {
                    new (dst) T(std::move(*reinterpret_cast<T*>(src)));
                    reinterpret_cast<T*>(src)->~T();
                } else {
                    *reinterpret_cast<T**>(dst) = *reinterpret_cast<T**>(src);
                    *reinterpret_cast<T**>(src) = nullptr;
                }
            },
            IsSmall
        };
        return vtable;
    }

    alignas(std::max_align_t) char buffer_[BufferSize];
    const VTable* vtable_;
};

int main() {
    std::cout << "=== 简单类型擦除：Printable ===\n";
    {
        std::vector<Printable> items;
        items.push_back(Document{"Hello World"});
        items.push_back(Image{800, 600});

        for (const auto& item : items) {
            item.print();
        }
    }

    std::cout << "\n=== Function 类型擦除 ===\n";
    {
        Function<int(int, int)> add = [](int a, int b) { return a + b; };
        Function<int(int, int)> mul = [](int a, int b) { return a * b; };

        std::cout << "add(3, 4) = " << add(3, 4) << "\n";
        std::cout << "mul(3, 4) = " << mul(3, 4) << "\n";

        // 拷贝
        Function<int(int, int)> addCopy = add;
        std::cout << "addCopy(5, 6) = " << addCopy(5, 6) << "\n";

        // 空函数检查
        Function<void()> empty;
        std::cout << "empty is valid: " << (empty ? "yes" : "no") << "\n";
    }

    std::cout << "\n=== Any 类型擦除 ===\n";
    {
        Any a = 42;
        std::cout << "a holds int: " << (a.type() == typeid(int)) << "\n";
        std::cout << "a value: " << any_cast<int>(a) << "\n";

        a = std::string("Hello");
        std::cout << "a holds string: " << (a.type() == typeid(std::string)) << "\n";
        std::cout << "a value: " << any_cast<std::string>(a) << "\n";

        // 类型不匹配
        try {
            int x = any_cast<int>(a);
            (void)x;
        } catch (const std::bad_cast&) {
            std::cout << "Bad cast caught!\n";
        }

        // 容器中使用
        std::vector<Any> vec;
        vec.push_back(1);
        vec.push_back(3.14);
        vec.push_back(std::string("world"));

        for (auto& elem : vec) {
            std::cout << "Type: " << elem.type().name() << "\n";
        }
    }

    std::cout << "\n=== AnyWithSBO 类型擦除 ===\n";
    {
        // 小对象（栈存储）
        AnyWithSBO small = 42;
        std::cout << "small type: " << small.type().name() << "\n";
        std::cout << "small value: " << *small.tryGet<int>() << "\n";

        // 大对象（堆存储）
        struct LargeObject {
            char data[100];
            int value;
        };
        AnyWithSBO large = LargeObject{};
        std::cout << "large has value: " << large.hasValue() << "\n";

        // 拷贝和移动
        AnyWithSBO copy = small;
        AnyWithSBO moved = std::move(copy);
        std::cout << "moved value: " << *moved.tryGet<int>() << "\n";
        std::cout << "copy has value after move: " << copy.hasValue() << "\n";
    }

    return 0;
}
