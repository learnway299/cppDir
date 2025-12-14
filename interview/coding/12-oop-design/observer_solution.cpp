/**
 * @file observer_solution.cpp
 * @brief 观察者模式实现 - 参考答案
 */

#include "observer.h"
#include <iostream>
#include <cassert>
#include <algorithm>
#include <typeinfo>

namespace ObserverImpl {

namespace Solution {

// ==================== 基础观察者模式 ====================

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
    void attach(Observer* observer) override {
        observers_.push_back(observer);
    }

    void detach(Observer* observer) override {
        observers_.erase(
            std::remove(observers_.begin(), observers_.end(), observer),
            observers_.end()
        );
    }

    void notify() override {
        for (auto* observer : observers_) {
            observer->update(state_);
        }
    }

    void setState(const std::string& state) {
        state_ = state;
        notify();
    }

    const std::string& getState() const { return state_; }
    size_t observerCount() const { return observers_.size(); }

private:
    std::vector<Observer*> observers_;
    std::string state_;
};

class ConcreteObserver : public Observer {
public:
    explicit ConcreteObserver(const std::string& name) : name_(name) {}

    void update(const std::string& message) override {
        lastMessage_ = message;
    }

    const std::string& getLastMessage() const { return lastMessage_; }
    const std::string& getName() const { return name_; }

private:
    std::string name_;
    std::string lastMessage_;
};

// ==================== 类型安全的观察者 ====================

template <typename EventType>
class TypedObserver {
public:
    virtual ~TypedObserver() = default;
    virtual void onEvent(const EventType& event) = 0;
};

template <typename EventType>
class TypedSubject {
public:
    void attach(TypedObserver<EventType>* observer) {
        observers_.push_back(observer);
    }

    void detach(TypedObserver<EventType>* observer) {
        observers_.erase(
            std::remove(observers_.begin(), observers_.end(), observer),
            observers_.end()
        );
    }

    void notify(const EventType& event) {
        for (auto* observer : observers_) {
            observer->onEvent(event);
        }
    }

    size_t observerCount() const { return observers_.size(); }

private:
    std::vector<TypedObserver<EventType>*> observers_;
};

struct PriceChangeEvent {
    std::string symbol;
    double oldPrice;
    double newPrice;
};

class PriceObserver : public TypedObserver<PriceChangeEvent> {
public:
    explicit PriceObserver(const std::string& name) : name_(name) {}

    void onEvent(const PriceChangeEvent& event) override {
        lastEvent_ = event;
    }

    const PriceChangeEvent& getLastEvent() const { return lastEvent_; }

private:
    std::string name_;
    PriceChangeEvent lastEvent_;
};

// ==================== Signal/Slot 模式 ====================

template <typename... Args>
class Signal {
public:
    using Slot = std::function<void(Args...)>;
    using SlotId = size_t;

    SlotId connect(Slot slot) {
        SlotId id = nextId_++;
        slots_[id] = std::move(slot);
        return id;
    }

    void disconnect(SlotId id) {
        slots_.erase(id);
    }

    void emit(Args... args) {
        for (auto& [id, slot] : slots_) {
            slot(args...);
        }
    }

    void disconnectAll() {
        slots_.clear();
    }

    size_t slotCount() const { return slots_.size(); }

private:
    std::unordered_map<SlotId, Slot> slots_;
    SlotId nextId_ = 0;
};

// RAII 连接管理
template <typename... Args>
class ScopedConnection {
public:
    ScopedConnection(Signal<Args...>& signal, typename Signal<Args...>::SlotId id)
        : signal_(&signal), id_(id) {}

    ~ScopedConnection() {
        if (signal_) {
            signal_->disconnect(id_);
        }
    }

    ScopedConnection(const ScopedConnection&) = delete;
    ScopedConnection& operator=(const ScopedConnection&) = delete;

    ScopedConnection(ScopedConnection&& other) noexcept
        : signal_(other.signal_), id_(other.id_) {
        other.signal_ = nullptr;
    }

    void release() { signal_ = nullptr; }

private:
    Signal<Args...>* signal_;
    typename Signal<Args...>::SlotId id_;
};

// ==================== 弱引用观察者 ====================

class WeakObserver : public std::enable_shared_from_this<WeakObserver> {
public:
    virtual ~WeakObserver() = default;
    virtual void update(const std::string& message) = 0;
};

class WeakSubject {
public:
    void attach(std::shared_ptr<WeakObserver> observer) {
        observers_.push_back(observer);
    }

    void notify(const std::string& message) {
        auto it = observers_.begin();
        while (it != observers_.end()) {
            if (auto observer = it->lock()) {
                observer->update(message);
                ++it;
            } else {
                it = observers_.erase(it);
            }
        }
    }

    size_t observerCount() const {
        size_t count = 0;
        for (const auto& weak : observers_) {
            if (!weak.expired()) ++count;
        }
        return count;
    }

private:
    std::vector<std::weak_ptr<WeakObserver>> observers_;
};

class ConcreteWeakObserver : public WeakObserver {
public:
    explicit ConcreteWeakObserver(const std::string& name) : name_(name) {}

    void update(const std::string& message) override {
        lastMessage_ = message;
    }

    const std::string& getLastMessage() const { return lastMessage_; }

private:
    std::string name_;
    std::string lastMessage_;
};

// ==================== 事件总线 ====================

class EventBus {
public:
    static EventBus& getInstance() {
        static EventBus instance;
        return instance;
    }

    template <typename Event>
    void subscribe(std::function<void(const Event&)> handler) {
        auto wrapper = [handler](const void* data) {
            handler(*static_cast<const Event*>(data));
        };
        handlers_[typeid(Event).name()].push_back(wrapper);
    }

    template <typename Event>
    void publish(const Event& event) {
        auto it = handlers_.find(typeid(Event).name());
        if (it != handlers_.end()) {
            for (auto& handler : it->second) {
                handler(&event);
            }
        }
    }

    void clear() {
        handlers_.clear();
    }

private:
    EventBus() = default;
    std::unordered_map<std::string, std::vector<std::function<void(const void*)>>> handlers_;
};

} // namespace Solution

// ==================== 测试函数 ====================

void runTests() {
    std::cout << "=== Observer Tests ===" << std::endl;

    // 测试基础观察者模式
    {
        Solution::ConcreteSubject subject;
        Solution::ConcreteObserver obs1("Observer1");
        Solution::ConcreteObserver obs2("Observer2");

        subject.attach(&obs1);
        subject.attach(&obs2);
        assert(subject.observerCount() == 2);

        subject.setState("State A");
        assert(obs1.getLastMessage() == "State A");
        assert(obs2.getLastMessage() == "State A");

        subject.detach(&obs1);
        assert(subject.observerCount() == 1);

        subject.setState("State B");
        assert(obs1.getLastMessage() == "State A");  // obs1 未更新
        assert(obs2.getLastMessage() == "State B");
    }
    std::cout << "  Basic Observer: PASSED" << std::endl;

    // 测试类型安全观察者
    {
        Solution::TypedSubject<Solution::PriceChangeEvent> stock;
        Solution::PriceObserver trader1("Trader1");
        Solution::PriceObserver trader2("Trader2");

        stock.attach(&trader1);
        stock.attach(&trader2);
        assert(stock.observerCount() == 2);

        stock.notify({"AAPL", 150.0, 155.0});
        assert(trader1.getLastEvent().symbol == "AAPL");
        assert(trader1.getLastEvent().newPrice == 155.0);

        stock.detach(&trader1);
        stock.notify({"GOOGL", 2800.0, 2750.0});
        assert(trader1.getLastEvent().symbol == "AAPL");  // 未更新
        assert(trader2.getLastEvent().symbol == "GOOGL");
    }
    std::cout << "  TypedObserver: PASSED" << std::endl;

    // 测试 Signal/Slot 模式
    {
        Solution::Signal<int, std::string> signal;
        int receivedCode = 0;
        std::string receivedMsg;

        auto id1 = signal.connect([&](int code, const std::string& msg) {
            receivedCode = code;
            receivedMsg = msg;
        });
        assert(signal.slotCount() == 1);

        signal.emit(200, "OK");
        assert(receivedCode == 200);
        assert(receivedMsg == "OK");

        signal.disconnect(id1);
        assert(signal.slotCount() == 0);

        signal.emit(404, "Not Found");
        assert(receivedCode == 200);  // 未更新
    }
    std::cout << "  Signal/Slot: PASSED" << std::endl;

    // 测试 RAII 连接管理
    {
        Solution::Signal<std::string> signal;
        std::string received;

        {
            Solution::ScopedConnection conn(signal, signal.connect([&](const std::string& s) {
                received = s;
            }));

            signal.emit("Hello");
            assert(received == "Hello");
            assert(signal.slotCount() == 1);
        }

        assert(signal.slotCount() == 0);
        signal.emit("World");
        assert(received == "Hello");  // 未更新
    }
    std::cout << "  ScopedConnection: PASSED" << std::endl;

    // 测试弱引用观察者
    {
        Solution::WeakSubject subject;

        auto obs1 = std::make_shared<Solution::ConcreteWeakObserver>("WeakObs1");
        auto obs2 = std::make_shared<Solution::ConcreteWeakObserver>("WeakObs2");

        subject.attach(obs1);
        subject.attach(obs2);
        assert(subject.observerCount() == 2);

        subject.notify("First notification");
        assert(obs1->getLastMessage() == "First notification");
        assert(obs2->getLastMessage() == "First notification");

        obs1.reset();  // 销毁 obs1
        assert(subject.observerCount() == 1);

        subject.notify("Second notification");
        assert(obs2->getLastMessage() == "Second notification");
    }
    std::cout << "  WeakObserver: PASSED" << std::endl;

    // 测试事件总线
    {
        struct UserLoggedIn { std::string username; };
        struct OrderPlaced { int orderId; double amount; };

        Solution::EventBus::getInstance().clear();

        std::string loggedUser;
        int orderId = 0;

        Solution::EventBus::getInstance().subscribe<UserLoggedIn>([&](const UserLoggedIn& e) {
            loggedUser = e.username;
        });

        Solution::EventBus::getInstance().subscribe<OrderPlaced>([&](const OrderPlaced& e) {
            orderId = e.orderId;
        });

        Solution::EventBus::getInstance().publish(UserLoggedIn{"Alice"});
        assert(loggedUser == "Alice");

        Solution::EventBus::getInstance().publish(OrderPlaced{12345, 99.99});
        assert(orderId == 12345);

        Solution::EventBus::getInstance().clear();
    }
    std::cout << "  EventBus: PASSED" << std::endl;
}

} // namespace ObserverImpl
