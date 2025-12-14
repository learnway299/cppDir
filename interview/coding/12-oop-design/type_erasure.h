/**
 * @file type_erasure.h
 * @brief 类型擦除实现 - 题目说明与声明
 *
 * 类型擦除：隐藏具体类型，提供统一接口
 * 结合了运行时多态的灵活性和值语义的安全性
 *
 * 包含题目：
 * 1. 简单类型擦除容器（Printable）
 * 2. std::function 风格的类型擦除
 * 3. std::any 风格的类型擦除
 * 4. 小对象优化（SBO）的类型擦除
 */

#ifndef TYPE_ERASURE_H
#define TYPE_ERASURE_H

#include <memory>
#include <string>
#include <typeinfo>
#include <stdexcept>
#include <functional>

namespace TypeErasureImpl {

// ==================== 题目1: 简单类型擦除容器 ====================

/**
 * 题目1: 实现简单的类型擦除容器
 *
 * 场景：存储任意可打印对象
 * 要求：
 * 1. 支持任意具有 print() 方法的类型
 * 2. 值语义（支持拷贝）
 *
 * 示例：
 *   struct Document {
 *       std::string content;
 *       std::string print() const { return "Document: " + content; }
 *   };
 *   Printable p = Document{"Hello"};
 *   std::string s = p.print();  // "Document: Hello"
 */
class Printable {
public:
    template <typename T>
    Printable(T obj);

    Printable(const Printable& other);
    Printable(Printable&& other) noexcept;

    Printable& operator=(const Printable& other);
    Printable& operator=(Printable&& other) noexcept;

    std::string print() const;

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

// 测试用结构体
struct Document {
    std::string content;
    std::string print() const;
};

struct Image {
    int width, height;
    std::string print() const;
};

// ==================== 题目2: Function 类型擦除 ====================

/**
 * 题目2: 实现 std::function 风格的类型擦除
 *
 * 要求：
 * 1. 可以存储任意可调用对象
 * 2. 支持 lambda、函数指针、仿函数
 *
 * 示例：
 *   Function<int(int, int)> add = [](int a, int b) { return a + b; };
 *   int result = add(3, 4);  // 7
 */
template <typename Signature>
class Function;

template <typename R, typename... Args>
class Function<R(Args...)> {
public:
    Function() = default;

    template <typename F>
    Function(F f);

    Function(const Function& other);
    Function(Function&&) noexcept = default;

    Function& operator=(const Function& other);
    Function& operator=(Function&&) noexcept = default;

    R operator()(Args... args) const;

    explicit operator bool() const;

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

// ==================== 题目3: Any 类型擦除 ====================

/**
 * 题目3: 实现 std::any 风格的类型擦除
 *
 * 要求：
 * 1. 可以存储任意类型
 * 2. 支持类型安全的取值
 *
 * 示例：
 *   Any a = 42;
 *   int x = a.get<int>();  // 42
 *   a = std::string("Hello");
 *   std::string s = a.get<std::string>();  // "Hello"
 */
class Any {
public:
    Any() = default;

    template <typename T>
    Any(T value);

    Any(const Any& other);
    Any(Any&&) noexcept = default;

    Any& operator=(const Any& other);
    Any& operator=(Any&&) noexcept = default;

    template <typename T>
    Any& operator=(T value);

    bool hasValue() const;
    const std::type_info& type() const;

    template <typename T>
    T* tryGet();

    template <typename T>
    T& get();

    template <typename T>
    const T& get() const;

    void reset();

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
T any_cast(Any& a);

template <typename T>
T any_cast(const Any& a);

// ==================== 题目4: AnyWithSBO 类型擦除 ====================

/**
 * 题目4: 实现小对象优化（SBO）的类型擦除
 *
 * 要求：
 * 1. 小对象存储在栈上（避免堆分配）
 * 2. 大对象存储在堆上
 *
 * 示例：
 *   AnyWithSBO small = 42;  // 栈存储
 *   AnyWithSBO large = LargeStruct{};  // 堆存储
 */
class AnyWithSBO {
public:
    AnyWithSBO();

    template <typename T>
    AnyWithSBO(T value);

    ~AnyWithSBO();

    AnyWithSBO(const AnyWithSBO& other);
    AnyWithSBO(AnyWithSBO&& other) noexcept;

    AnyWithSBO& operator=(const AnyWithSBO& other);
    AnyWithSBO& operator=(AnyWithSBO&& other) noexcept;

    void swap(AnyWithSBO& other) noexcept;

    bool hasValue() const;
    const std::type_info& type() const;

    template <typename T>
    T* tryGet();

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
    static const VTable& getVTable();

    alignas(std::max_align_t) char buffer_[BufferSize];
    const VTable* vtable_;
};

// ==================== 测试函数 ====================

void runTests();

} // namespace TypeErasureImpl

#endif // TYPE_ERASURE_H
