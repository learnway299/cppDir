/**
 * @file type_erasure_solution.cpp
 * @brief 类型擦除实现 - 参考答案
 */

#include "type_erasure.h"
#include <iostream>
#include <cassert>
#include <vector>

namespace TypeErasureImpl {

namespace Solution {

// ==================== Printable ====================

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

    std::string print() const {
        if (pImpl_) {
            return pImpl_->print();
        }
        return "";
    }

private:
    struct Concept {
        virtual ~Concept() = default;
        virtual std::string print() const = 0;
        virtual std::unique_ptr<Concept> clone() const = 0;
    };

    template <typename T>
    struct Model : Concept {
        T data_;

        explicit Model(T obj) : data_(std::move(obj)) {}

        std::string print() const override {
            return data_.print();
        }

        std::unique_ptr<Concept> clone() const override {
            return std::make_unique<Model>(*this);
        }
    };

    std::unique_ptr<Concept> pImpl_;
};

struct Document {
    std::string content;
    std::string print() const {
        return "Document: " + content;
    }
};

struct Image {
    int width, height;
    std::string print() const {
        return "Image: " + std::to_string(width) + "x" + std::to_string(height);
    }
};

// ==================== Function ====================

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

// ==================== Any ====================

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

template <typename T>
T any_cast(Any& a) {
    return a.get<std::remove_reference_t<T>>();
}

template <typename T>
T any_cast(const Any& a) {
    return a.get<std::remove_reference_t<T>>();
}

// ==================== AnyWithSBO ====================

class AnyWithSBO {
public:
    AnyWithSBO() : vtable_(nullptr) {}

    template <typename T>
    AnyWithSBO(T value) {
        using DecayT = std::decay_t<T>;
        if constexpr (sizeof(DecayT) <= BufferSize &&
                      alignof(DecayT) <= alignof(std::max_align_t) &&
                      std::is_nothrow_move_constructible_v<DecayT>) {
            new (buffer_) DecayT(std::move(value));
            vtable_ = &getVTable<DecayT, true>();
        } else {
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

} // namespace Solution

// ==================== 测试函数 ====================

void runTests() {
    std::cout << "=== Type Erasure Tests ===" << std::endl;

    // 测试 Printable 类型擦除
    {
        std::vector<Solution::Printable> items;
        items.push_back(Solution::Document{"Hello World"});
        items.push_back(Solution::Image{800, 600});

        assert(items[0].print() == "Document: Hello World");
        assert(items[1].print() == "Image: 800x600");

        // 测试拷贝
        Solution::Printable copy = items[0];
        assert(copy.print() == "Document: Hello World");
    }
    std::cout << "  Printable: PASSED" << std::endl;

    // 测试 Function 类型擦除
    {
        Solution::Function<int(int, int)> add = [](int a, int b) { return a + b; };
        Solution::Function<int(int, int)> mul = [](int a, int b) { return a * b; };

        assert(add(3, 4) == 7);
        assert(mul(3, 4) == 12);

        // 拷贝
        Solution::Function<int(int, int)> addCopy = add;
        assert(addCopy(5, 6) == 11);

        // 空函数检查
        Solution::Function<void()> empty;
        assert(!empty);

        Solution::Function<void()> nonEmpty = [](){};
        assert(nonEmpty);
    }
    std::cout << "  Function: PASSED" << std::endl;

    // 测试 Any 类型擦除
    {
        Solution::Any a = 42;
        assert(a.type() == typeid(int));
        assert(Solution::any_cast<int>(a) == 42);

        a = std::string("Hello");
        assert(a.type() == typeid(std::string));
        assert(Solution::any_cast<std::string>(a) == "Hello");

        // 类型不匹配
        bool caught = false;
        try {
            int x = Solution::any_cast<int>(a);
            (void)x;
        } catch (const std::bad_cast&) {
            caught = true;
        }
        assert(caught);

        // hasValue 和 reset
        assert(a.hasValue());
        a.reset();
        assert(!a.hasValue());
    }
    std::cout << "  Any: PASSED" << std::endl;

    // 测试 AnyWithSBO 类型擦除
    {
        // 小对象（栈存储）
        Solution::AnyWithSBO small = 42;
        assert(small.hasValue());
        assert(small.type() == typeid(int));
        assert(*small.tryGet<int>() == 42);

        // 大对象（堆存储）
        struct LargeObject {
            char data[100];
            int value;
        };
        Solution::AnyWithSBO large = LargeObject{{}, 123};
        assert(large.hasValue());

        // 拷贝
        Solution::AnyWithSBO copy = small;
        assert(*copy.tryGet<int>() == 42);

        // 移动
        Solution::AnyWithSBO moved = std::move(copy);
        assert(*moved.tryGet<int>() == 42);
        assert(!copy.hasValue());
    }
    std::cout << "  AnyWithSBO: PASSED" << std::endl;
}

} // namespace TypeErasureImpl
