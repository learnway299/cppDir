/**
 * @file pimpl_solution.cpp
 * @brief Pimpl 惯用法实现 - 参考答案
 */

#include "pimpl.h"
#include <iostream>
#include <cassert>
#include <utility>
#include <stdexcept>

namespace PimplImpl {

namespace Solution {

// ==================== DatabaseConnection ====================

class DatabaseConnection {
public:
    DatabaseConnection() : pImpl_(std::make_unique<Impl>()) {}
    ~DatabaseConnection() = default;

    DatabaseConnection(DatabaseConnection&& other) noexcept = default;
    DatabaseConnection& operator=(DatabaseConnection&& other) noexcept = default;

    DatabaseConnection(const DatabaseConnection&) = delete;
    DatabaseConnection& operator=(const DatabaseConnection&) = delete;

    void connect(const std::string& connectionString) {
        pImpl_->connect(connectionString);
    }

    void disconnect() {
        pImpl_->disconnect();
    }

    bool isConnected() const {
        return pImpl_->isConnected();
    }

    void execute(const std::string& sql) {
        pImpl_->execute(sql);
    }

private:
    class Impl {
    public:
        Impl() : connected_(false) {}

        void connect(const std::string& connStr) {
            connectionString_ = connStr;
            connected_ = true;
        }

        void disconnect() {
            if (connected_) {
                connected_ = false;
            }
        }

        bool isConnected() const { return connected_; }

        void execute(const std::string& sql) {
            if (!connected_) {
                throw std::runtime_error("Not connected");
            }
            lastQuery_ = sql;
        }

    private:
        std::string connectionString_;
        std::string lastQuery_;
        bool connected_;
    };

    std::unique_ptr<Impl> pImpl_;
};

// ==================== Widget (支持拷贝) ====================

class Widget {
public:
    Widget() : pImpl_(std::make_unique<Impl>()) {}
    ~Widget() = default;

    Widget(const Widget& other)
        : pImpl_(std::make_unique<Impl>(*other.pImpl_)) {}

    Widget& operator=(const Widget& other) {
        if (this != &other) {
            pImpl_ = std::make_unique<Impl>(*other.pImpl_);
        }
        return *this;
    }

    Widget(Widget&& other) noexcept = default;
    Widget& operator=(Widget&& other) noexcept = default;

    void setData(const std::string& data) {
        pImpl_->data = data;
    }

    std::string getData() const {
        return pImpl_->data;
    }

private:
    class Impl {
    public:
        std::string data;
        Impl() = default;
        Impl(const Impl& other) : data(other.data) {}
    };

    std::unique_ptr<Impl> pImpl_;
};

// ==================== Document (Copy-on-Write) ====================

class Document {
public:
    Document() : pImpl_(std::make_shared<Impl>()) {}
    ~Document() = default;

    Document(const Document& other) : pImpl_(other.pImpl_) {}

    Document& operator=(const Document& other) {
        if (this != &other) {
            pImpl_ = other.pImpl_;
        }
        return *this;
    }

    void write(const std::string& content) {
        detach();
        pImpl_->content = content;
    }

    std::string read() const {
        return pImpl_->content;
    }

    long useCount() const {
        return pImpl_.use_count();
    }

private:
    class Impl {
    public:
        std::string content;
        Impl() = default;
        Impl(const Impl& other) : content(other.content) {}
    };

    std::shared_ptr<Impl> pImpl_;

    void detach() {
        if (pImpl_.use_count() > 1) {
            pImpl_ = std::make_shared<Impl>(*pImpl_);
        }
    }
};

// ==================== Base / Derived (继承) ====================

class Base {
public:
    Base() : pImpl_(std::make_unique<BaseImpl>()) {}
    virtual ~Base() = default;

    virtual int doSomething() {
        return pImpl_->doSomethingImpl();
    }

protected:
    class BaseImpl {
    public:
        int baseData = 0;
        virtual ~BaseImpl() = default;
        virtual int doSomethingImpl() {
            return baseData;
        }
    };

    std::unique_ptr<BaseImpl> pImpl_;

    explicit Base(std::unique_ptr<BaseImpl> impl) : pImpl_(std::move(impl)) {}
};

class Derived : public Base {
public:
    Derived() : Base(std::make_unique<DerivedImpl>()) {}
    ~Derived() override = default;

    int doSomething() override {
        return getDerivedImpl()->doSomethingImpl();
    }

    int doExtra() {
        return getDerivedImpl()->doExtraImpl();
    }

    void setDerivedData(int value) {
        getDerivedImpl()->derivedData = value;
    }

private:
    class DerivedImpl : public BaseImpl {
    public:
        int derivedData = 0;

        int doSomethingImpl() override {
            return baseData + derivedData;
        }

        int doExtraImpl() {
            return derivedData * 2;
        }
    };

    DerivedImpl* getDerivedImpl() {
        return static_cast<DerivedImpl*>(pImpl_.get());
    }
};

// ==================== Fast Pimpl ====================

template <size_t Size, size_t Alignment = alignof(std::max_align_t)>
class FastPimpl {
public:
    template <typename T, typename... Args>
    void construct(Args&&... args) {
        static_assert(sizeof(T) <= Size, "Buffer too small");
        static_assert(alignof(T) <= Alignment, "Alignment mismatch");
        new (buffer_) T(std::forward<Args>(args)...);
    }

    template <typename T>
    void destroy() {
        reinterpret_cast<T*>(buffer_)->~T();
    }

    template <typename T>
    T* get() {
        return reinterpret_cast<T*>(buffer_);
    }

    template <typename T>
    const T* get() const {
        return reinterpret_cast<const T*>(buffer_);
    }

private:
    alignas(Alignment) char buffer_[Size];
};

class FastWidget {
public:
    FastWidget() {
        storage_.construct<Impl>();
    }

    ~FastWidget() {
        storage_.destroy<Impl>();
    }

    void setValue(int v) { storage_.get<Impl>()->value = v; }
    int getValue() const { return storage_.get<Impl>()->value; }

private:
    struct Impl {
        int value = 0;
    };

    FastPimpl<sizeof(Impl), alignof(Impl)> storage_;
};

} // namespace Solution

// ==================== 测试函数 ====================

void runTests() {
    std::cout << "=== Pimpl Tests ===" << std::endl;

    // 测试基础 Pimpl
    {
        Solution::DatabaseConnection db;
        assert(!db.isConnected());

        db.connect("mysql://localhost:3306/test");
        assert(db.isConnected());

        db.execute("SELECT * FROM users");

        db.disconnect();
        assert(!db.isConnected());

        // 测试移动
        Solution::DatabaseConnection db2;
        db2.connect("postgres://localhost/db");
        Solution::DatabaseConnection db3 = std::move(db2);
        assert(db3.isConnected());
    }
    std::cout << "  DatabaseConnection: PASSED" << std::endl;

    // 测试可拷贝 Pimpl
    {
        Solution::Widget w1;
        w1.setData("Hello");

        Solution::Widget w2 = w1;  // 深拷贝
        assert(w2.getData() == "Hello");

        w2.setData("World");
        assert(w1.getData() == "Hello");  // w1 不受影响
        assert(w2.getData() == "World");

        Solution::Widget w3;
        w3 = w1;  // 赋值
        assert(w3.getData() == "Hello");
    }
    std::cout << "  Widget (copyable): PASSED" << std::endl;

    // 测试 Copy-on-Write Pimpl
    {
        Solution::Document d1;
        d1.write("Original content");

        Solution::Document d2 = d1;  // 浅拷贝，共享 Impl
        assert(d1.useCount() == 2);
        assert(d2.useCount() == 2);
        assert(d2.read() == "Original content");

        d2.write("Modified content");  // 触发 COW
        assert(d1.useCount() == 1);
        assert(d2.useCount() == 1);
        assert(d1.read() == "Original content");
        assert(d2.read() == "Modified content");
    }
    std::cout << "  Document (COW): PASSED" << std::endl;

    // 测试 Pimpl 与继承
    {
        Solution::Base base;
        assert(base.doSomething() == 0);

        Solution::Derived derived;
        derived.setDerivedData(10);
        assert(derived.doSomething() == 10);
        assert(derived.doExtra() == 20);

        // 多态
        Solution::Base* polyBase = new Solution::Derived();
        Solution::Derived* derivedPtr = dynamic_cast<Solution::Derived*>(polyBase);
        derivedPtr->setDerivedData(5);
        assert(polyBase->doSomething() == 5);
        delete polyBase;
    }
    std::cout << "  Inheritance Pimpl: PASSED" << std::endl;

    // 测试 FastPimpl
    {
        Solution::FastWidget fw;
        assert(fw.getValue() == 0);

        fw.setValue(42);
        assert(fw.getValue() == 42);
    }
    std::cout << "  FastWidget: PASSED" << std::endl;
}

} // namespace PimplImpl
