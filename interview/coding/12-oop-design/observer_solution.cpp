/**
 * @file observer_solution.cpp
 * @brief 观察者模式实现 - 解答
 */

#include <iostream>
#include <vector>
#include <memory>
#include <string>
#include <functional>
#include <algorithm>
#include <unordered_map>

/**
 * 题目1：基础观察者模式
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

private:
    std::vector<Observer*> observers_;
    std::string state_;
};

class ConcreteObserver : public Observer {
public:
    explicit ConcreteObserver(const std::string& name) : name_(name) {}

    void update(const std::string& message) override {
        std::cout << "[" << name_ << "] Received: " << message << "\n";
    }

private:
    std::string name_;
};

/**
 * 题目2：类型安全的观察者（使用模板）
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

private:
    std::vector<TypedObserver<EventType>*> observers_;
};

// 事件类型示例
struct PriceChangeEvent {
    std::string symbol;
    double oldPrice;
    double newPrice;
};

class PriceObserver : public TypedObserver<PriceChangeEvent> {
public:
    explicit PriceObserver(const std::string& name) : name_(name) {}

    void onEvent(const PriceChangeEvent& event) override {
        std::cout << "[" << name_ << "] " << event.symbol
                  << " price changed: " << event.oldPrice
                  << " -> " << event.newPrice << "\n";
    }

private:
    std::string name_;
};

/**
 * 题目3：基于函数的观察者（现代 C++ 风格）
 *
 * 类似于 Qt 的 Signal/Slot 或 Boost.Signals2
 */
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

    // 禁止拷贝
    ScopedConnection(const ScopedConnection&) = delete;
    ScopedConnection& operator=(const ScopedConnection&) = delete;

    // 允许移动
    ScopedConnection(ScopedConnection&& other) noexcept
        : signal_(other.signal_), id_(other.id_) {
        other.signal_ = nullptr;
    }

    void release() { signal_ = nullptr; }

private:
    Signal<Args...>* signal_;
    typename Signal<Args...>::SlotId id_;
};

/**
 * 题目4：弱引用观察者
 *
 * 避免循环引用，自动清理已销毁的观察者
 */
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
        // 清理已销毁的观察者并通知存活的
        auto it = observers_.begin();
        while (it != observers_.end()) {
            if (auto observer = it->lock()) {
                observer->update(message);
                ++it;
            } else {
                // 观察者已销毁，移除
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
    explicit ConcreteWeakObserver(const std::string& name) : name_(name) {
        std::cout << name_ << " created\n";
    }

    ~ConcreteWeakObserver() {
        std::cout << name_ << " destroyed\n";
    }

    void update(const std::string& message) override {
        std::cout << "[" << name_ << "] Received: " << message << "\n";
    }

private:
    std::string name_;
};

/**
 * 扩展：事件总线（全局事件系统）
 */
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

private:
    EventBus() = default;
    std::unordered_map<std::string, std::vector<std::function<void(const void*)>>> handlers_;
};

int main() {
    std::cout << "=== 基础观察者模式 ===\n";
    {
        ConcreteSubject subject;
        ConcreteObserver obs1("Observer1");
        ConcreteObserver obs2("Observer2");

        subject.attach(&obs1);
        subject.attach(&obs2);

        subject.setState("State A");

        subject.detach(&obs1);
        subject.setState("State B");
    }

    std::cout << "\n=== 类型安全观察者 ===\n";
    {
        TypedSubject<PriceChangeEvent> stock;
        PriceObserver trader1("Trader1");
        PriceObserver trader2("Trader2");

        stock.attach(&trader1);
        stock.attach(&trader2);

        stock.notify({"AAPL", 150.0, 155.0});
        stock.notify({"GOOGL", 2800.0, 2750.0});
    }

    std::cout << "\n=== Signal/Slot 模式 ===\n";
    {
        Signal<int, std::string> signal;

        auto id1 = signal.connect([](int code, const std::string& msg) {
            std::cout << "Slot1: code=" << code << ", msg=" << msg << "\n";
        });

        auto id2 = signal.connect([](int code, const std::string& msg) {
            std::cout << "Slot2: code=" << code << ", msg=" << msg << "\n";
        });

        signal.emit(200, "OK");

        signal.disconnect(id1);
        std::cout << "After disconnect:\n";
        signal.emit(404, "Not Found");
    }

    std::cout << "\n=== RAII 连接管理 ===\n";
    {
        Signal<std::string> signal;

        {
            ScopedConnection conn(signal, signal.connect([](const std::string& s) {
                std::cout << "Scoped: " << s << "\n";
            }));

            signal.emit("Hello");
            std::cout << "Slots: " << signal.slotCount() << "\n";
        }

        std::cout << "After scope, slots: " << signal.slotCount() << "\n";
        signal.emit("World");
    }

    std::cout << "\n=== 弱引用观察者 ===\n";
    {
        WeakSubject subject;

        auto obs1 = std::make_shared<ConcreteWeakObserver>("WeakObs1");
        auto obs2 = std::make_shared<ConcreteWeakObserver>("WeakObs2");

        subject.attach(obs1);
        subject.attach(obs2);

        std::cout << "Observer count: " << subject.observerCount() << "\n";
        subject.notify("First notification");

        obs1.reset();  // 销毁 obs1
        std::cout << "After destroying obs1:\n";
        std::cout << "Observer count: " << subject.observerCount() << "\n";
        subject.notify("Second notification");
    }

    std::cout << "\n=== 事件总线 ===\n";
    {
        struct UserLoggedIn { std::string username; };
        struct OrderPlaced { int orderId; double amount; };

        EventBus::getInstance().subscribe<UserLoggedIn>([](const UserLoggedIn& e) {
            std::cout << "User logged in: " << e.username << "\n";
        });

        EventBus::getInstance().subscribe<OrderPlaced>([](const OrderPlaced& e) {
            std::cout << "Order placed: #" << e.orderId << ", $" << e.amount << "\n";
        });

        EventBus::getInstance().publish(UserLoggedIn{"Alice"});
        EventBus::getInstance().publish(OrderPlaced{12345, 99.99});
    }

    return 0;
}
