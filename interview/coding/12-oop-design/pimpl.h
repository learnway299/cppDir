/**
 * @file pimpl.h
 * @brief Pimpl 惯用法实现 - 题目说明与声明
 *
 * Pointer to Implementation：将实现细节隐藏在独立的类中
 *
 * 包含题目：
 * 1. 基础 Pimpl（数据库连接）
 * 2. 支持拷贝的 Pimpl
 * 3. Copy-on-Write Pimpl
 * 4. Pimpl 与继承
 * 5. Fast Pimpl（栈上分配）
 */

#ifndef PIMPL_H
#define PIMPL_H

#include <memory>
#include <string>

namespace PimplImpl {

// ==================== 题目1: 基础 Pimpl ====================

/**
 * 题目1: 实现基础 Pimpl - 数据库连接类
 *
 * 优点：
 * 1. 编译防火墙 - 修改实现不需要重新编译用户代码
 * 2. 二进制兼容性 - 可以添加私有成员而不破坏 ABI
 * 3. 隐藏实现细节
 *
 * 示例：
 *   DatabaseConnection db;
 *   db.connect("mysql://localhost:3306/test");
 *   db.execute("SELECT * FROM users");
 *   db.disconnect();
 */
class DatabaseConnection {
public:
    DatabaseConnection();
    ~DatabaseConnection();

    // 移动操作
    DatabaseConnection(DatabaseConnection&& other) noexcept;
    DatabaseConnection& operator=(DatabaseConnection&& other) noexcept;

    // 禁止拷贝
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

// ==================== 题目2: 支持拷贝的 Pimpl ====================

/**
 * 题目2: 实现支持深拷贝的 Pimpl
 *
 * 拷贝构造时需要创建新的 Impl 对象
 */
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

// ==================== 题目3: Copy-on-Write Pimpl ====================

/**
 * 题目3: 实现 Copy-on-Write Pimpl
 *
 * 拷贝时共享 Impl，只在写入时才复制
 * 适用于大型数据结构，减少拷贝开销
 */
class Document {
public:
    Document();
    ~Document();

    Document(const Document& other);
    Document& operator=(const Document& other);

    void write(const std::string& content);
    std::string read() const;
    long useCount() const;

private:
    class Impl;
    std::shared_ptr<Impl> pImpl_;

    void detach();
};

// ==================== 题目4: Pimpl 与继承 ====================

/**
 * 题目4: Pimpl 与继承
 *
 * 基类和派生类如何共享 Pimpl 模式
 */
class Base {
public:
    Base();
    virtual ~Base();

    virtual int doSomething();

protected:
    class BaseImpl;
    std::unique_ptr<BaseImpl> pImpl_;

    explicit Base(std::unique_ptr<BaseImpl> impl);
};

class Derived : public Base {
public:
    Derived();
    ~Derived() override;

    int doSomething() override;
    int doExtra();
    void setDerivedData(int value);

private:
    class DerivedImpl;

    DerivedImpl* getDerivedImpl();
};

// ==================== 题目5: Fast Pimpl ====================

/**
 * 题目5: Fast Pimpl - 避免堆分配
 *
 * 使用栈上缓冲区存储 Impl 对象
 */
template <size_t Size, size_t Alignment = alignof(std::max_align_t)>
class FastPimpl {
public:
    template <typename T, typename... Args>
    void construct(Args&&... args);

    template <typename T>
    void destroy();

    template <typename T>
    T* get();

    template <typename T>
    const T* get() const;

private:
    alignas(Alignment) char buffer_[Size];
};

class FastWidget {
public:
    FastWidget();
    ~FastWidget();

    void setValue(int v);
    int getValue() const;

private:
    struct Impl;
    FastPimpl<sizeof(int) * 2, alignof(int)> storage_;
};

// ==================== 测试函数 ====================

void runTests();

} // namespace PimplImpl

#endif // PIMPL_H
