/**
 * @file observer.h
 * @brief 观察者模式实现 - 题目说明与声明
 *
 * 包含题目：
 * 1. 基础观察者模式
 * 2. 类型安全的观察者（模板）
 * 3. Signal/Slot 模式（现代 C++ 风格）
 * 4. 弱引用观察者
 * 5. 事件总线
 */

#ifndef OBSERVER_H
#define OBSERVER_H

#include <memory>
#include <string>
#include <vector>
#include <functional>
#include <unordered_map>

namespace ObserverImpl {

// ==================== 题目1: 基础观察者模式 ====================

/**
 * 题目1: 实现基础观察者模式
 *
 * 定义对象间一对多的依赖关系，当一个对象状态改变时，
 * 所有依赖它的对象都得到通知并自动更新
 *
 * 示例：
 *   ConcreteSubject subject;
 *   ConcreteObserver obs("Observer1");
 *   subject.attach(&obs);
 *   subject.setState("new state");  // obs.update 被调用
 */
class Observer {
public:
    virtual ~Observer() = default;
    virtual void update(const std::string& message) = 0;
};

class Subject {
public:
    virtual ~Subject() = default;
    virtual void attach(Observer* observer) = 0;
    virtual void detach(Observer* observer) = 0;
    virtual void notify() = 0;
};

class ConcreteSubject : public Subject {
public:
    void attach(Observer* observer) override;
    void detach(Observer* observer) override;
    void notify() override;

    void setState(const std::string& state);
    const std::string& getState() const;
    size_t observerCount() const;

private:
    std::vector<Observer*> observers_;
    std::string state_;
};

class ConcreteObserver : public Observer {
public:
    explicit ConcreteObserver(const std::string& name);
    void update(const std::string& message) override;

    const std::string& getLastMessage() const;
    const std::string& getName() const;

private:
    std::string name_;
    std::string lastMessage_;
};

// ==================== 题目2: 类型安全的观察者 ====================

/**
 * 题目2: 实现类型安全的观察者（使用模板）
 *
 * 编译期类型检查，避免运行时错误
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
    void attach(TypedObserver<EventType>* observer);
    void detach(TypedObserver<EventType>* observer);
    void notify(const EventType& event);
    size_t observerCount() const;

private:
    std::vector<TypedObserver<EventType>*> observers_;
};

// 示例事件类型
struct PriceChangeEvent {
    std::string symbol;
    double oldPrice;
    double newPrice;
};

class PriceObserver : public TypedObserver<PriceChangeEvent> {
public:
    explicit PriceObserver(const std::string& name);
    void onEvent(const PriceChangeEvent& event) override;
    const PriceChangeEvent& getLastEvent() const;

private:
    std::string name_;
    PriceChangeEvent lastEvent_;
};

// ==================== 题目3: Signal/Slot 模式 ====================

/**
 * 题目3: 实现基于函数的观察者（现代 C++ 风格）
 *
 * 使用 std::function 作为回调，支持 lambda 表达式
 *
 * 示例：
 *   Signal<int, std::string> signal;
 *   auto id = signal.connect([](int code, const std::string& msg) {
 *       std::cout << code << ": " << msg << std::endl;
 *   });
 *   signal.emit(200, "OK");
 *   signal.disconnect(id);
 */
template <typename... Args>
class Signal {
public:
    using Slot = std::function<void(Args...)>;
    using SlotId = size_t;

    SlotId connect(Slot slot);
    void disconnect(SlotId id);
    void emit(Args... args);
    void disconnectAll();
    size_t slotCount() const;

private:
    std::unordered_map<SlotId, Slot> slots_;
    SlotId nextId_ = 0;
};

// RAII 连接管理
template <typename... Args>
class ScopedConnection {
public:
    ScopedConnection(Signal<Args...>& signal, typename Signal<Args...>::SlotId id);
    ~ScopedConnection();

    ScopedConnection(const ScopedConnection&) = delete;
    ScopedConnection& operator=(const ScopedConnection&) = delete;
    ScopedConnection(ScopedConnection&& other) noexcept;

    void release();

private:
    Signal<Args...>* signal_;
    typename Signal<Args...>::SlotId id_;
};

// ==================== 题目4: 弱引用观察者 ====================

/**
 * 题目4: 实现弱引用观察者
 *
 * 使用 weak_ptr 避免循环引用，自动清理已销毁的观察者
 */
class WeakObserver : public std::enable_shared_from_this<WeakObserver> {
public:
    virtual ~WeakObserver() = default;
    virtual void update(const std::string& message) = 0;
};

class WeakSubject {
public:
    void attach(std::shared_ptr<WeakObserver> observer);
    void notify(const std::string& message);
    size_t observerCount() const;

private:
    std::vector<std::weak_ptr<WeakObserver>> observers_;
};

class ConcreteWeakObserver : public WeakObserver {
public:
    explicit ConcreteWeakObserver(const std::string& name);
    void update(const std::string& message) override;
    const std::string& getLastMessage() const;

private:
    std::string name_;
    std::string lastMessage_;
};

// ==================== 题目5: 事件总线 ====================

/**
 * 题目5: 实现事件总线
 *
 * 全局事件发布/订阅系统
 */
class EventBus {
public:
    static EventBus& getInstance();

    template <typename Event>
    void subscribe(std::function<void(const Event&)> handler);

    template <typename Event>
    void publish(const Event& event);

    void clear();

private:
    EventBus() = default;
    std::unordered_map<std::string, std::vector<std::function<void(const void*)>>> handlers_;
};

// ==================== 测试函数 ====================

void runTests();

} // namespace ObserverImpl

#endif // OBSERVER_H
