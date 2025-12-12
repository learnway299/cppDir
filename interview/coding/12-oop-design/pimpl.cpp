/**
 * @file pimpl.cpp
 * @brief Pimpl 惯用法实现 - 面试题
 *
 * Pointer to Implementation：将实现细节隐藏在独立的类中
 * 优点：编译防火墙、二进制兼容性、隐藏实现
 */

#include <memory>
#include <string>

/**
 * 题目1：实现基础 Pimpl
 *
 * 场景：数据库连接类
 * 要求：
 * 1. 头文件不暴露实现细节
 * 2. 使用 unique_ptr 管理实现
 * 3. 正确处理拷贝和移动
 */

// ===== DatabaseConnection.h =====
class DatabaseConnection {
public:
    DatabaseConnection();
    ~DatabaseConnection();

    // 移动操作
    DatabaseConnection(DatabaseConnection&& other) noexcept;
    DatabaseConnection& operator=(DatabaseConnection&& other) noexcept;

    // 禁止拷贝（或实现深拷贝）
    DatabaseConnection(const DatabaseConnection&) = delete;
    DatabaseConnection& operator=(const DatabaseConnection&) = delete;

    // 公共接口
    void connect(const std::string& connectionString);
    void disconnect();
    bool isConnected() const;
    void execute(const std::string& sql);

private:
    // 前向声明
    class Impl;
    std::unique_ptr<Impl> pImpl_;
};

// ===== DatabaseConnection.cpp =====
// TODO: 实现 Impl 类
// TODO: 实现 DatabaseConnection 的所有方法

/**
 * 题目2：实现支持拷贝的 Pimpl
 *
 * 要求：
 * 1. 支持深拷贝
 * 2. Copy-on-Write 优化（可选）
 */

// ===== Widget.h =====
class Widget {
public:
    Widget();
    ~Widget();

    // 支持拷贝
    Widget(const Widget& other);
    Widget& operator=(const Widget& other);

    // 支持移动
    Widget(Widget&& other) noexcept;
    Widget& operator=(Widget&& other) noexcept;

    void setData(const std::string& data);
    std::string getData() const;

private:
    class Impl;
    std::unique_ptr<Impl> pImpl_;
};

/**
 * 题目3：使用 shared_ptr 实现 Copy-on-Write
 *
 * 场景：大型数据结构，拷贝开销大
 */

// ===== Document.h =====
class Document {
public:
    Document();
    ~Document();

    // 拷贝是浅拷贝（共享 Impl）
    Document(const Document& other);
    Document& operator=(const Document& other);

    // 写入时才复制
    void write(const std::string& content);
    std::string read() const;

private:
    class Impl;
    std::shared_ptr<Impl> pImpl_;

    // 写时复制辅助函数
    void detach();
};

/**
 * 题目4：Pimpl 与继承
 *
 * 如何在继承体系中使用 Pimpl
 */
class Base {
public:
    Base();
    virtual ~Base();

    virtual void doSomething();

protected:
    class BaseImpl;
    std::unique_ptr<BaseImpl> pImpl_;

    // 供派生类使用
    Base(std::unique_ptr<BaseImpl> impl);
};

class Derived : public Base {
public:
    Derived();
    ~Derived() override;

    void doSomething() override;
    void doExtra();

private:
    class DerivedImpl;
    // 注意：派生类可以有自己的 Impl，也可以扩展基类的 Impl
};

int main() {
    return 0;
}
