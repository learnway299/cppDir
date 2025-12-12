/**
 * @file singleton.cpp
 * @brief 单例模式实现 - 面试题
 *
 * 单例模式确保一个类只有一个实例，并提供全局访问点
 */

#include <iostream>
#include <mutex>
#include <memory>

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
    // TODO: 实现 getInstance()
    static EagerSingleton& getInstance() {
        // TODO
        static EagerSingleton instance;
        return instance;
    }

    void doSomething() {
        std::cout << "EagerSingleton working\n";
    }

private:
    // TODO: 私有构造函数
    EagerSingleton() {}

    // TODO: 禁止拷贝
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
    // TODO: 实现双重检查锁定
    static LazySingleton* getInstance() {
        return nullptr;
    }

    void doSomething() {
        std::cout << "LazySingleton working\n";
    }

private:
    LazySingleton() {}

    // TODO: 定义成员变量
    // 提示：需要静态实例指针和互斥锁
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
    // TODO: 实现 getInstance()
    static MeyersSingleton& getInstance() {
        // TODO
        static MeyersSingleton instance;
        return instance;
    }

private:
    MeyersSingleton() {}
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
    // TODO: 实现可销毁单例
    static DestructibleSingleton* getInstance() {
        return nullptr;
    }

    static void destroyInstance() {
    }

private:
    DestructibleSingleton() {}
};

/**
 * 题目5：实现模板化单例（CRTP）
 *
 * 要求：
 * 1. 使用 CRTP 使任意类成为单例
 * 2. 避免代码重复
 */
template <typename T>
class Singleton {
public:
    // TODO: 实现模板单例基类
    static T& getInstance() {
        static T instance;
        return instance;
    }

protected:
    Singleton() = default;
    ~Singleton() = default;

private:
    Singleton(const Singleton&) = delete;
    Singleton& operator=(const Singleton&) = delete;
};

// 使用示例
class MySingletonClass : public Singleton<MySingletonClass> {
    friend class Singleton<MySingletonClass>;
private:
    MySingletonClass() {}
public:
    void doWork() {
        std::cout << "MySingletonClass working\n";
    }
};

int main() {
    return 0;
}
