/**
 * @file pimpl_solution.cpp
 * @brief Pimpl 惯用法实现 - 解答
 */

#include <memory>
#include <string>
#include <iostream>
#include <utility>

/**
 * 题目1：基础 Pimpl - 数据库连接类
 */

// ===== DatabaseConnection.h =====
class DatabaseConnection {
public:
    DatabaseConnection();
    ~DatabaseConnection();

    DatabaseConnection(DatabaseConnection&& other) noexcept;
    DatabaseConnection& operator=(DatabaseConnection&& other) noexcept;

    DatabaseConnection(const DatabaseConnection&) = delete;
    DatabaseConnection& operator=(const DatabaseConnection&) = delete;

    void connect(const std::string& connectionString);
    void disconnect();
    bool isConnected() const;
    void execute(const std::string& sql);

private:
    class Impl;
    std::unique_ptr<Impl> pImpl_;
};

// ===== DatabaseConnection.cpp =====
class DatabaseConnection::Impl {
public:
    Impl() : connected_(false) {}

    void connect(const std::string& connStr) {
        connectionString_ = connStr;
        connected_ = true;
        std::cout << "Connected to: " << connStr << "\n";
    }

    void disconnect() {
        if (connected_) {
            std::cout << "Disconnected from: " << connectionString_ << "\n";
            connected_ = false;
        }
    }

    bool isConnected() const { return connected_; }

    void execute(const std::string& sql) {
        if (!connected_) {
            throw std::runtime_error("Not connected");
        }
        std::cout << "Executing: " << sql << "\n";
    }

private:
    std::string connectionString_;
    bool connected_;
};

DatabaseConnection::DatabaseConnection()
    : pImpl_(std::make_unique<Impl>()) {}

DatabaseConnection::~DatabaseConnection() = default;

DatabaseConnection::DatabaseConnection(DatabaseConnection&& other) noexcept = default;
DatabaseConnection& DatabaseConnection::operator=(DatabaseConnection&& other) noexcept = default;

void DatabaseConnection::connect(const std::string& connectionString) {
    pImpl_->connect(connectionString);
}

void DatabaseConnection::disconnect() {
    pImpl_->disconnect();
}

bool DatabaseConnection::isConnected() const {
    return pImpl_->isConnected();
}

void DatabaseConnection::execute(const std::string& sql) {
    pImpl_->execute(sql);
}

/**
 * 题目2：支持拷贝的 Pimpl
 */

// ===== Widget.h =====
class Widget {
public:
    Widget();
    ~Widget();

    Widget(const Widget& other);
    Widget& operator=(const Widget& other);

    Widget(Widget&& other) noexcept;
    Widget& operator=(Widget&& other) noexcept;

    void setData(const std::string& data);
    std::string getData() const;

private:
    class Impl;
    std::unique_ptr<Impl> pImpl_;
};

// ===== Widget.cpp =====
class Widget::Impl {
public:
    std::string data;

    Impl() = default;
    Impl(const Impl& other) : data(other.data) {}
};

Widget::Widget() : pImpl_(std::make_unique<Impl>()) {}

Widget::~Widget() = default;

Widget::Widget(const Widget& other)
    : pImpl_(std::make_unique<Impl>(*other.pImpl_)) {}

Widget& Widget::operator=(const Widget& other) {
    if (this != &other) {
        pImpl_ = std::make_unique<Impl>(*other.pImpl_);
    }
    return *this;
}

Widget::Widget(Widget&& other) noexcept = default;
Widget& Widget::operator=(Widget&& other) noexcept = default;

void Widget::setData(const std::string& data) {
    pImpl_->data = data;
}

std::string Widget::getData() const {
    return pImpl_->data;
}

/**
 * 题目3：Copy-on-Write Pimpl
 */

// ===== Document.h =====
class Document {
public:
    Document();
    ~Document();

    Document(const Document& other);
    Document& operator=(const Document& other);

    void write(const std::string& content);
    std::string read() const;

    // 调试用
    long useCount() const;

private:
    class Impl;
    std::shared_ptr<Impl> pImpl_;

    void detach();
};

// ===== Document.cpp =====
class Document::Impl {
public:
    std::string content;

    Impl() = default;
    Impl(const Impl& other) : content(other.content) {}
};

Document::Document() : pImpl_(std::make_shared<Impl>()) {}
Document::~Document() = default;

Document::Document(const Document& other) : pImpl_(other.pImpl_) {}

Document& Document::operator=(const Document& other) {
    if (this != &other) {
        pImpl_ = other.pImpl_;
    }
    return *this;
}

void Document::detach() {
    if (pImpl_.use_count() > 1) {
        pImpl_ = std::make_shared<Impl>(*pImpl_);
    }
}

void Document::write(const std::string& content) {
    detach();  // 写入前分离
    pImpl_->content = content;
}

std::string Document::read() const {
    return pImpl_->content;
}

long Document::useCount() const {
    return pImpl_.use_count();
}

/**
 * 题目4：Pimpl 与继承
 */

// ===== Base =====
class Base {
public:
    Base();
    virtual ~Base();

    virtual void doSomething();

protected:
    class BaseImpl {
    public:
        int baseData = 0;
        virtual ~BaseImpl() = default;
        virtual void doSomethingImpl() {
            std::cout << "Base::doSomething, data=" << baseData << "\n";
        }
    };

    std::unique_ptr<BaseImpl> pImpl_;

    explicit Base(std::unique_ptr<BaseImpl> impl);
};

Base::Base() : pImpl_(std::make_unique<BaseImpl>()) {}
Base::Base(std::unique_ptr<BaseImpl> impl) : pImpl_(std::move(impl)) {}
Base::~Base() = default;

void Base::doSomething() {
    pImpl_->doSomethingImpl();
}

// ===== Derived =====
class Derived : public Base {
public:
    Derived();
    ~Derived() override;

    void doSomething() override;
    void doExtra();

private:
    class DerivedImpl : public BaseImpl {
    public:
        int derivedData = 0;

        void doSomethingImpl() override {
            std::cout << "Derived::doSomething, baseData=" << baseData
                      << ", derivedData=" << derivedData << "\n";
        }

        void doExtraImpl() {
            std::cout << "Derived::doExtra, derivedData=" << derivedData << "\n";
        }
    };

    DerivedImpl* getDerivedImpl() {
        return static_cast<DerivedImpl*>(pImpl_.get());
    }
};

Derived::Derived() : Base(std::make_unique<DerivedImpl>()) {}
Derived::~Derived() = default;

void Derived::doSomething() {
    getDerivedImpl()->doSomethingImpl();
}

void Derived::doExtra() {
    getDerivedImpl()->doExtraImpl();
}

/**
 * 扩展：使用 fast-pimpl 减少堆分配
 *
 * 在对象内部预留空间，避免小对象的堆分配
 */
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

// 使用 FastPimpl 的类
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

int main() {
    std::cout << "=== 基础 Pimpl: DatabaseConnection ===\n";
    {
        DatabaseConnection db;
        db.connect("mysql://localhost:3306/test");
        db.execute("SELECT * FROM users");
        db.disconnect();
    }

    std::cout << "\n=== 可拷贝 Pimpl: Widget ===\n";
    {
        Widget w1;
        w1.setData("Hello");

        Widget w2 = w1;  // 深拷贝
        w2.setData("World");

        std::cout << "w1: " << w1.getData() << "\n";
        std::cout << "w2: " << w2.getData() << "\n";
    }

    std::cout << "\n=== Copy-on-Write Pimpl: Document ===\n";
    {
        Document d1;
        d1.write("Original content");

        Document d2 = d1;  // 浅拷贝，共享 Impl
        std::cout << "After copy: d1 use_count=" << d1.useCount()
                  << ", d2 use_count=" << d2.useCount() << "\n";

        d2.write("Modified content");  // 触发 COW
        std::cout << "After write: d1 use_count=" << d1.useCount()
                  << ", d2 use_count=" << d2.useCount() << "\n";

        std::cout << "d1: " << d1.read() << "\n";
        std::cout << "d2: " << d2.read() << "\n";
    }

    std::cout << "\n=== Pimpl 与继承 ===\n";
    {
        Base base;
        base.doSomething();

        Derived derived;
        derived.doSomething();
        derived.doExtra();

        Base* polyBase = new Derived();
        polyBase->doSomething();  // 多态调用
        delete polyBase;
    }

    std::cout << "\n=== FastPimpl ===\n";
    {
        FastWidget fw;
        fw.setValue(42);
        std::cout << "FastWidget value: " << fw.getValue() << "\n";
    }

    return 0;
}
