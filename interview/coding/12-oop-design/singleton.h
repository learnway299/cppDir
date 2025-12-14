/**
 * @file singleton.h
 * @brief 单例模式实现 - 头文件
 *
 * 单例模式确保一个类只有一个实例，并提供全局访问点
 */

#ifndef SINGLETON_H
#define SINGLETON_H

#include <mutex>
#include <atomic>
#include <string>

namespace Singleton {

/**
 * 题目1：实现饿汉式单例
 *
 * 要求：
 * 1. 程序启动时即创建实例
 * 2. 线程安全
 * 3. 禁止拷贝和赋值
 */
class EagerSingleton {
public:
    static EagerSingleton& getInstance();
    void doSomething();
    bool wasCalled() const;

    EagerSingleton(const EagerSingleton&) = delete;
    EagerSingleton& operator=(const EagerSingleton&) = delete;

private:
    EagerSingleton();
    bool called_;
};

/**
 * 题目2：实现懒汉式单例（双重检查锁定）
 *
 * 要求：
 * 1. 延迟初始化
 * 2. 线程安全
 * 3. 避免每次调用都加锁
 */
class LazySingleton {
public:
    static LazySingleton* getInstance();
    static void cleanup();
    void doSomething();
    bool wasCalled() const;

    LazySingleton(const LazySingleton&) = delete;
    LazySingleton& operator=(const LazySingleton&) = delete;

private:
    LazySingleton();

    static std::atomic<LazySingleton*> instance_;
    static std::mutex mutex_;
    bool called_;
};

/**
 * 题目3：实现 Meyers 单例（推荐方式）
 *
 * 要求：
 * 1. 利用局部静态变量的线程安全初始化
 * 2. 简洁高效
 */
class MeyersSingleton {
public:
    static MeyersSingleton& getInstance();
    void doSomething();
    bool wasCalled() const;

    MeyersSingleton(const MeyersSingleton&) = delete;
    MeyersSingleton& operator=(const MeyersSingleton&) = delete;

private:
    MeyersSingleton();
    ~MeyersSingleton() = default;
    bool called_;
};

/**
 * 题目4：实现可销毁单例
 *
 * 要求：
 * 1. 支持手动销毁实例
 * 2. 销毁后可重新创建
 */
class DestructibleSingleton {
public:
    static DestructibleSingleton* getInstance();
    static void destroyInstance();
    static bool exists();
    void doSomething();
    bool wasCalled() const;

    DestructibleSingleton(const DestructibleSingleton&) = delete;
    DestructibleSingleton& operator=(const DestructibleSingleton&) = delete;

private:
    DestructibleSingleton();
    ~DestructibleSingleton() = default;

    static DestructibleSingleton* instance_;
    static std::mutex mutex_;
    bool called_;
};

/**
 * 题目5：实现模板化单例（CRTP）
 *
 * 要求：
 * 1. 使用 CRTP 使任意类成为单例
 * 2. 避免代码重复
 */
template <typename T>
class SingletonBase {
public:
    static T& getInstance();

    SingletonBase(const SingletonBase&) = delete;
    SingletonBase& operator=(const SingletonBase&) = delete;

protected:
    SingletonBase() = default;
    virtual ~SingletonBase() = default;
};

// 使用示例
class Logger : public SingletonBase<Logger> {
    friend class SingletonBase<Logger>;

public:
    void log(const std::string& message);
    const std::string& getLastMessage() const;

private:
    Logger() = default;
    std::string lastMessage_;
};

class Config : public SingletonBase<Config> {
    friend class SingletonBase<Config>;

public:
    void loadConfig(const std::string& file);
    const std::string& getConfigFile() const;

private:
    Config();
    std::string configFile_;
};

// ==================== Solution 类 ====================
class EagerSingletonSolution {
public:
    static EagerSingletonSolution& getInstance();
    void doSomething();
    bool wasCalled() const;

    EagerSingletonSolution(const EagerSingletonSolution&) = delete;
    EagerSingletonSolution& operator=(const EagerSingletonSolution&) = delete;

private:
    EagerSingletonSolution();
    bool called_;
};

class LazySingletonSolution {
public:
    static LazySingletonSolution* getInstance();
    static void cleanup();
    void doSomething();
    bool wasCalled() const;

    LazySingletonSolution(const LazySingletonSolution&) = delete;
    LazySingletonSolution& operator=(const LazySingletonSolution&) = delete;

private:
    LazySingletonSolution();

    static std::atomic<LazySingletonSolution*> instance_;
    static std::mutex mutex_;
    bool called_;
};

class MeyersSingletonSolution {
public:
    static MeyersSingletonSolution& getInstance();
    void doSomething();
    bool wasCalled() const;

    MeyersSingletonSolution(const MeyersSingletonSolution&) = delete;
    MeyersSingletonSolution& operator=(const MeyersSingletonSolution&) = delete;

private:
    MeyersSingletonSolution();
    ~MeyersSingletonSolution() = default;
    bool called_;
};

class DestructibleSingletonSolution {
public:
    static DestructibleSingletonSolution* getInstance();
    static void destroyInstance();
    static bool exists();
    void doSomething();
    bool wasCalled() const;

    DestructibleSingletonSolution(const DestructibleSingletonSolution&) = delete;
    DestructibleSingletonSolution& operator=(const DestructibleSingletonSolution&) = delete;

private:
    DestructibleSingletonSolution();
    ~DestructibleSingletonSolution() = default;

    static DestructibleSingletonSolution* instance_;
    static std::mutex mutex_;
    bool called_;
};

template <typename T>
class SingletonBaseSolution {
public:
    static T& getInstance() {
        static T instance;
        return instance;
    }

    SingletonBaseSolution(const SingletonBaseSolution&) = delete;
    SingletonBaseSolution& operator=(const SingletonBaseSolution&) = delete;

protected:
    SingletonBaseSolution() = default;
    virtual ~SingletonBaseSolution() = default;
};

class LoggerSolution : public SingletonBaseSolution<LoggerSolution> {
    friend class SingletonBaseSolution<LoggerSolution>;

public:
    void log(const std::string& message);
    const std::string& getLastMessage() const;

private:
    LoggerSolution() = default;
    std::string lastMessage_;
};

class ConfigSolution : public SingletonBaseSolution<ConfigSolution> {
    friend class SingletonBaseSolution<ConfigSolution>;

public:
    void loadConfig(const std::string& file);
    const std::string& getConfigFile() const;

private:
    ConfigSolution();
    std::string configFile_;
};

// ==================== 测试函数声明 ====================

void testSingleton();          // 测试面试者实现
void testSingletonSolution();  // 测试解答实现

// runTests 在头文件 inline 定义，调用两个测试函数
inline void runTests() {
    testSingleton();
    testSingletonSolution();
}

} // namespace Singleton

// 模板实现
namespace Singleton {

template <typename T>
T& SingletonBase<T>::getInstance() {
    // TODO: 实现模板单例
    static T instance;
    return instance;
}

} // namespace Singleton

#endif // SINGLETON_H
