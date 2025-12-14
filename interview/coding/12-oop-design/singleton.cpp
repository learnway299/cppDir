/**
 * @file singleton.cpp
 * @brief 单例模式实现 - 面试题（空实现）
 */

#include "singleton.h"
#include <iostream>

namespace Singleton {

// ==================== EagerSingleton ====================
EagerSingleton::EagerSingleton() : called_(false) {}

EagerSingleton& EagerSingleton::getInstance() {
    // TODO: 实现饿汉式单例
    static EagerSingleton instance;
    return instance;
}

void EagerSingleton::doSomething() {
    // TODO
}

bool EagerSingleton::wasCalled() const {
    // TODO
    return false;
}

// ==================== LazySingleton ====================
std::atomic<LazySingleton*> LazySingleton::instance_{nullptr};
std::mutex LazySingleton::mutex_;

LazySingleton::LazySingleton() : called_(false) {}

LazySingleton* LazySingleton::getInstance() {
    // TODO: 实现懒汉式单例（双重检查锁定）
    return nullptr;
}

void LazySingleton::cleanup() {
    // TODO
}

void LazySingleton::doSomething() {
    // TODO
}

bool LazySingleton::wasCalled() const {
    // TODO
    return false;
}

// ==================== MeyersSingleton ====================
MeyersSingleton::MeyersSingleton() : called_(false) {}

MeyersSingleton& MeyersSingleton::getInstance() {
    // TODO: 实现 Meyers 单例
    static MeyersSingleton instance;
    return instance;
}

void MeyersSingleton::doSomething() {
    // TODO
}

bool MeyersSingleton::wasCalled() const {
    // TODO
    return false;
}

// ==================== DestructibleSingleton ====================
DestructibleSingleton* DestructibleSingleton::instance_ = nullptr;
std::mutex DestructibleSingleton::mutex_;

DestructibleSingleton::DestructibleSingleton() : called_(false) {}

DestructibleSingleton* DestructibleSingleton::getInstance() {
    // TODO: 实现可销毁单例
    return nullptr;
}

void DestructibleSingleton::destroyInstance() {
    // TODO
}

bool DestructibleSingleton::exists() {
    // TODO
    return false;
}

void DestructibleSingleton::doSomething() {
    // TODO
}

bool DestructibleSingleton::wasCalled() const {
    // TODO
    return false;
}

// ==================== Logger ====================
void Logger::log(const std::string& message) {
    // TODO
}

const std::string& Logger::getLastMessage() const {
    // TODO
    static std::string empty;
    return empty;
}

// ==================== Config ====================
Config::Config() : configFile_("default.conf") {}

void Config::loadConfig(const std::string& file) {
    // TODO
}

const std::string& Config::getConfigFile() const {
    // TODO
    return configFile_;
}

// ==================== 测试函数 ====================

void testSingleton() {
    std::cout << "=== Singleton Tests (User Implementation) ===\n";
    // TODO: 添加面试者实现的测试
    std::cout << "  (No tests yet - implement your solutions first)\n";
}

} // namespace Singleton
