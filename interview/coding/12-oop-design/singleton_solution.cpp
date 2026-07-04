/**
 * @file singleton_solution.cpp
 * @brief 单例模式实现 - 参考答案
 */

#include "singleton.h"
#include <iostream>
#include <cassert>

namespace Singleton {

// ==================== EagerSingletonSolution ====================

/**
 * 题目1: EagerSingleton - 饿汉式单例（Meyers实现）
 * getInstance: O(1) - 静态局部变量（C++11线程安全）
 * 空间复杂度: O(1) - 单个实例
 * 特点: 线程安全，无需加锁
 */
EagerSingletonSolution::EagerSingletonSolution() : called_(false) {}

EagerSingletonSolution& EagerSingletonSolution::getInstance() {
    static EagerSingletonSolution instance_;
    return instance_;
}

void EagerSingletonSolution::doSomething() {
    called_ = true;
}

bool EagerSingletonSolution::wasCalled() const {
    return called_;
}

// ==================== LazySingletonSolution ====================

/**
 * 题目2: LazySingleton - 懒汉式单例（双重检查锁定）
 * getInstance: O(1) - 首次需要加锁，之后无锁
 * cleanup: O(1) - 原子交换+删除
 * 空间复杂度: O(1) - 单个实例 + 原子指针 + 互斥锁
 * 特点: 延迟初始化，双重检查避免重复加锁
 */
std::atomic<LazySingletonSolution*> LazySingletonSolution::instance_{nullptr};
std::mutex LazySingletonSolution::mutex_;

LazySingletonSolution::LazySingletonSolution() : called_(false) {}

LazySingletonSolution* LazySingletonSolution::getInstance() {
    // 第一次检查（无锁）
    LazySingletonSolution* tmp = instance_.load(std::memory_order_acquire);
    if (tmp == nullptr) {
        std::lock_guard<std::mutex> lock(mutex_);
        // 第二次检查（有锁）
        tmp = instance_.load(std::memory_order_relaxed);
        if (tmp == nullptr) {
            tmp = new LazySingletonSolution();
            instance_.store(tmp, std::memory_order_release);
        }
    }
    return tmp;
}

void LazySingletonSolution::cleanup() {
    delete instance_.exchange(nullptr);
}

void LazySingletonSolution::doSomething() {
    called_ = true;
}

bool LazySingletonSolution::wasCalled() const {
    return called_;
}

// ==================== MeyersSingletonSolution ====================

/**
 * 题目3: MeyersSingleton - Meyers单例
 * getInstance: O(1) - 静态局部变量（C++11保证线程安全）
 * 空间复杂度: O(1) - 单个实例
 * 特点: 最简洁的线程安全实现
 */
MeyersSingletonSolution::MeyersSingletonSolution() : called_(false) {}

MeyersSingletonSolution& MeyersSingletonSolution::getInstance() {
    static MeyersSingletonSolution instance;
    return instance;
}

void MeyersSingletonSolution::doSomething() {
    called_ = true;
}

bool MeyersSingletonSolution::wasCalled() const {
    return called_;
}

// ==================== DestructibleSingletonSolution ====================

/**
 * 题目4: DestructibleSingleton - 可销毁单例
 * getInstance/destroyInstance: O(1) - 加锁操作
 * exists: O(1) - 加锁检查
 * 空间复杂度: O(1) - 单个实例 + 互斥锁
 * 特点: 支持显式销毁和重新创建
 */
DestructibleSingletonSolution* DestructibleSingletonSolution::instance_ = nullptr;
std::mutex DestructibleSingletonSolution::mutex_;

DestructibleSingletonSolution::DestructibleSingletonSolution() : called_(false) {}

DestructibleSingletonSolution* DestructibleSingletonSolution::getInstance() {
    std::lock_guard<std::mutex> lock(mutex_);
    if (!instance_) {
        instance_ = new DestructibleSingletonSolution();
    }
    return instance_;
}

void DestructibleSingletonSolution::destroyInstance() {
    std::lock_guard<std::mutex> lock(mutex_);
    delete instance_;
    instance_ = nullptr;
}

bool DestructibleSingletonSolution::exists() {
    std::lock_guard<std::mutex> lock(mutex_);
    return instance_ != nullptr;
}

void DestructibleSingletonSolution::doSomething() {
    called_ = true;
}

bool DestructibleSingletonSolution::wasCalled() const {
    return called_;
}

// ==================== LoggerSolution ====================

/**
 * 题目5: CRTP Singleton - CRTP模板单例
 * getInstance: O(1) - 静态局部变量
 * 空间复杂度: O(1) - 每个派生类一个实例
 * 特点: 可复用的单例基类模板
 */
void LoggerSolution::log(const std::string& message) {
    lastMessage_ = message;
}

const std::string& LoggerSolution::getLastMessage() const {
    return lastMessage_;
}

// ==================== ConfigSolution ====================
ConfigSolution::ConfigSolution() : configFile_("default.conf") {}

void ConfigSolution::loadConfig(const std::string& file) {
    configFile_ = file;
}

const std::string& ConfigSolution::getConfigFile() const {
    return configFile_;
}

// ==================== 测试函数 ====================
void testSingletonSolution() {
    std::cout << "=== Singleton Tests (Solution) ===" << std::endl;

    // 测试饿汉式单例
    {
        auto& s1 = EagerSingletonSolution::getInstance();
        auto& s2 = EagerSingletonSolution::getInstance();
        assert(&s1 == &s2);

        s1.doSomething();
        assert(s2.wasCalled());
    }
    std::cout << "  EagerSingleton: PASSED" << std::endl;

    // 测试懒汉式单例
    {
        auto* s1 = LazySingletonSolution::getInstance();
        auto* s2 = LazySingletonSolution::getInstance();
        assert(s1 == s2);

        s1->doSomething();
        assert(s2->wasCalled());

        LazySingletonSolution::cleanup();
    }
    std::cout << "  LazySingleton: PASSED" << std::endl;

    // 测试 Meyers 单例
    {
        auto& s1 = MeyersSingletonSolution::getInstance();
        auto& s2 = MeyersSingletonSolution::getInstance();
        assert(&s1 == &s2);

        s1.doSomething();
        assert(s2.wasCalled());
    }
    std::cout << "  MeyersSingleton: PASSED" << std::endl;

    // 测试可销毁单例
    {
        auto* s1 = DestructibleSingletonSolution::getInstance();
        assert(DestructibleSingletonSolution::exists());

        s1->doSomething();
        assert(s1->wasCalled());

        DestructibleSingletonSolution::destroyInstance();
        assert(!DestructibleSingletonSolution::exists());

        // 重新创建
        auto* s2 = DestructibleSingletonSolution::getInstance();
        assert(DestructibleSingletonSolution::exists());
        assert(!s2->wasCalled());  // 新实例

        DestructibleSingletonSolution::destroyInstance();
    }
    std::cout << "  DestructibleSingleton: PASSED" << std::endl;

    // 测试 CRTP 单例
    {
        auto& logger1 = LoggerSolution::getInstance();
        auto& logger2 = LoggerSolution::getInstance();
        assert(&logger1 == &logger2);

        logger1.log("test message");
        assert(logger2.getLastMessage() == "test message");

        auto& config1 = ConfigSolution::getInstance();
        auto& config2 = ConfigSolution::getInstance();
        assert(&config1 == &config2);

        assert(config1.getConfigFile() == "default.conf");
        config1.loadConfig("app.conf");
        assert(config2.getConfigFile() == "app.conf");
    }
    std::cout << "  CRTP Singleton: PASSED" << std::endl;

    std::cout << "All tests passed!" << std::endl;
}

} // namespace Singleton
