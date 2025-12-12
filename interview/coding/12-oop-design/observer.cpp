/**
 * @file observer.cpp
 * @brief 观察者模式实现 - 面试题
 *
 * 定义对象间一对多的依赖关系，当一个对象状态改变时，
 * 所有依赖它的对象都得到通知并自动更新
 */

#include <iostream>
#include <vector>
#include <memory>
#include <string>
#include <functional>
#include <algorithm>

/**
 * 题目1：实现基础观察者模式
 *
 * 要求：
 * 1. Subject（主题）可以添加/移除 Observer
 * 2. 状态变化时通知所有 Observer
 * 3. Observer 收到通知后更新自己
 */

// TODO: 定义观察者接口
class Observer {
public:
    virtual ~Observer() = default;
    virtual void update(const std::string& message) = 0;
};

// TODO: 定义主题接口
class Subject {
public:
    virtual ~Subject() = default;
    virtual void attach(Observer* observer) = 0;
    virtual void detach(Observer* observer) = 0;
    virtual void notify() = 0;
};

// TODO: 实现具体主题
class ConcreteSubject : public Subject {
public:
    void attach(Observer* observer) override {
    }

    void detach(Observer* observer) override {
    }

    void notify() override {
    }

    void setState(const std::string& state) {
    }

private:
    // TODO: 定义成员变量
};

// TODO: 实现具体观察者
class ConcreteObserver : public Observer {
public:
    explicit ConcreteObserver(const std::string& name) {
    }

    void update(const std::string& message) override {
    }

private:
    std::string name_;
};

/**
 * 题目2：实现类型安全的观察者（使用模板）
 *
 * 要求：
 * 1. 观察者可以接收特定类型的事件
 * 2. 编译期类型检查
 */
template <typename EventType>
class TypedObserver {
public:
    virtual ~TypedObserver() = default;
    virtual void onEvent(const EventType& event) = 0;
};

template <typename EventType>
class TypedSubject {
public:
    // TODO: 实现类型安全的主题
};

/**
 * 题目3：实现基于函数的观察者（现代 C++ 风格）
 *
 * 要求：
 * 1. 使用 std::function 作为回调
 * 2. 支持 lambda 表达式
 * 3. 返回可用于取消订阅的句柄
 */
template <typename... Args>
class Signal {
public:
    using Slot = std::function<void(Args...)>;
    using SlotId = size_t;

    // TODO: 实现订阅
    SlotId connect(Slot slot) {
        return 0;
    }

    // TODO: 实现取消订阅
    void disconnect(SlotId id) {
    }

    // TODO: 实现触发
    void emit(Args... args) {
    }

private:
    // TODO: 定义成员变量
};

/**
 * 题目4：实现弱引用观察者
 *
 * 要求：
 * 1. 使用 weak_ptr 避免循环引用
 * 2. 自动清理已销毁的观察者
 */
class WeakObserver {
public:
    virtual ~WeakObserver() = default;
    virtual void update() = 0;
};

class WeakSubject {
public:
    // TODO: 实现使用 weak_ptr 的主题
    void attach(std::shared_ptr<WeakObserver> observer) {
    }

    void notify() {
    }

private:
    // std::vector<std::weak_ptr<WeakObserver>> observers_;
};

int main() {
    return 0;
}
