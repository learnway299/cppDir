/**
 * @file print_alternately_solution.cpp
 * @brief 交替打印 - 解答
 */
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <iostream>
#include <atomic>
#include <semaphore>

// ==================== 题目1: 奇偶数交替打印 ====================
class OddEvenPrinter {
public:
    OddEvenPrinter(int max) : max_(max), current_(1) {}

    void printOdd() {
        while (true) {
            std::unique_lock<std::mutex> lock(mutex_);
            cv_.wait(lock, [this] { return current_ > max_ || (current_ % 2 == 1); });
            if (current_ > max_) break;
            std::cout << "Odd: " << current_ << "\n";
            ++current_;
            cv_.notify_one();
        }
    }

    void printEven() {
        while (true) {
            std::unique_lock<std::mutex> lock(mutex_);
            cv_.wait(lock, [this] { return current_ > max_ || (current_ % 2 == 0); });
            if (current_ > max_) break;
            std::cout << "Even: " << current_ << "\n";
            ++current_;
            cv_.notify_one();
        }
    }

private:
    int max_;
    int current_;
    std::mutex mutex_;
    std::condition_variable cv_;
};

// ==================== 题目2: ABC 交替打印 ====================
class ABCPrinter {
public:
    ABCPrinter(int n) : n_(n), state_(0), count_(0) {}

    void printA() {
        for (int i = 0; i < n_; ++i) {
            std::unique_lock<std::mutex> lock(mutex_);
            cv_.wait(lock, [this] { return state_ == 0; });
            std::cout << "A";
            state_ = 1;
            cv_.notify_all();
        }
    }

    void printB() {
        for (int i = 0; i < n_; ++i) {
            std::unique_lock<std::mutex> lock(mutex_);
            cv_.wait(lock, [this] { return state_ == 1; });
            std::cout << "B";
            state_ = 2;
            cv_.notify_all();
        }
    }

    void printC() {
        for (int i = 0; i < n_; ++i) {
            std::unique_lock<std::mutex> lock(mutex_);
            cv_.wait(lock, [this] { return state_ == 2; });
            std::cout << "C";
            state_ = 0;
            cv_.notify_all();
        }
    }

private:
    int n_;
    int state_;
    int count_;
    std::mutex mutex_;
    std::condition_variable cv_;
};

// ==================== 题目3: 按序打印 ====================
class Foo {
public:
    Foo() : step_(0) {}

    void first(std::function<void()> printFirst) {
        printFirst();
        {
            std::lock_guard<std::mutex> lock(mutex_);
            step_ = 1;
        }
        cv_.notify_all();
    }

    void second(std::function<void()> printSecond) {
        std::unique_lock<std::mutex> lock(mutex_);
        cv_.wait(lock, [this] { return step_ >= 1; });
        printSecond();
        step_ = 2;
        cv_.notify_all();
    }

    void third(std::function<void()> printThird) {
        std::unique_lock<std::mutex> lock(mutex_);
        cv_.wait(lock, [this] { return step_ >= 2; });
        printThird();
    }

private:
    std::mutex mutex_;
    std::condition_variable cv_;
    int step_;
};

// 使用原子变量的无锁版本
class FooAtomic {
public:
    FooAtomic() : step_(0) {}

    void first(std::function<void()> printFirst) {
        printFirst();
        step_.store(1, std::memory_order_release);
    }

    void second(std::function<void()> printSecond) {
        while (step_.load(std::memory_order_acquire) < 1) {
            std::this_thread::yield();
        }
        printSecond();
        step_.store(2, std::memory_order_release);
    }

    void third(std::function<void()> printThird) {
        while (step_.load(std::memory_order_acquire) < 2) {
            std::this_thread::yield();
        }
        printThird();
    }

private:
    std::atomic<int> step_;
};

// ==================== 题目4: FooBar 交替打印 ====================
class FooBar {
public:
    FooBar(int n) : n_(n), fooTurn_(true) {}

    void foo(std::function<void()> printFoo) {
        for (int i = 0; i < n_; ++i) {
            std::unique_lock<std::mutex> lock(mutex_);
            cv_.wait(lock, [this] { return fooTurn_; });
            printFoo();
            fooTurn_ = false;
            cv_.notify_one();
        }
    }

    void bar(std::function<void()> printBar) {
        for (int i = 0; i < n_; ++i) {
            std::unique_lock<std::mutex> lock(mutex_);
            cv_.wait(lock, [this] { return !fooTurn_; });
            printBar();
            fooTurn_ = true;
            cv_.notify_one();
        }
    }

private:
    int n_;
    bool fooTurn_;
    std::mutex mutex_;
    std::condition_variable cv_;
};

// 使用信号量的版本 (C++20)
#if __cplusplus >= 202002L
class FooBarSemaphore {
public:
    FooBarSemaphore(int n) : n_(n), fooSem_(1), barSem_(0) {}

    void foo(std::function<void()> printFoo) {
        for (int i = 0; i < n_; ++i) {
            fooSem_.acquire();
            printFoo();
            barSem_.release();
        }
    }

    void bar(std::function<void()> printBar) {
        for (int i = 0; i < n_; ++i) {
            barSem_.acquire();
            printBar();
            fooSem_.release();
        }
    }

private:
    int n_;
    std::counting_semaphore<1> fooSem_;
    std::counting_semaphore<1> barSem_;
};
#endif

// ==================== 题目5: 打印零与奇偶数 ====================
class ZeroEvenOdd {
public:
    ZeroEvenOdd(int n) : n_(n), current_(0), state_(0) {}

    // state: 0=打印0, 1=打印奇数, 2=打印偶数
    void zero(std::function<void(int)> printNumber) {
        for (int i = 1; i <= n_; ++i) {
            std::unique_lock<std::mutex> lock(mutex_);
            cv_.wait(lock, [this] { return state_ == 0; });
            printNumber(0);
            current_ = i;
            state_ = (i % 2 == 1) ? 1 : 2;  // 奇数后打印奇数线程，偶数后打印偶数线程
            cv_.notify_all();
        }
    }

    void odd(std::function<void(int)> printNumber) {
        for (int i = 1; i <= n_; i += 2) {
            std::unique_lock<std::mutex> lock(mutex_);
            cv_.wait(lock, [this] { return state_ == 1; });
            printNumber(current_);
            state_ = 0;
            cv_.notify_all();
        }
    }

    void even(std::function<void(int)> printNumber) {
        for (int i = 2; i <= n_; i += 2) {
            std::unique_lock<std::mutex> lock(mutex_);
            cv_.wait(lock, [this] { return state_ == 2; });
            printNumber(current_);
            state_ = 0;
            cv_.notify_all();
        }
    }

private:
    int n_;
    int current_;
    int state_;
    std::mutex mutex_;
    std::condition_variable cv_;
};

// ==================== 测试代码 ====================
int main() {
    std::cout << "=== Test 1: OddEvenPrinter (1-10) ===\n";
    {
        OddEvenPrinter printer(10);
        std::thread t1(&OddEvenPrinter::printOdd, &printer);
        std::thread t2(&OddEvenPrinter::printEven, &printer);
        t1.join();
        t2.join();
    }

    std::cout << "\n=== Test 2: ABCPrinter (3 times) ===\n";
    {
        ABCPrinter printer(3);
        std::thread t1(&ABCPrinter::printA, &printer);
        std::thread t2(&ABCPrinter::printB, &printer);
        std::thread t3(&ABCPrinter::printC, &printer);
        t1.join();
        t2.join();
        t3.join();
        std::cout << "\n";
    }

    std::cout << "\n=== Test 3: Foo (ordered print) ===\n";
    {
        Foo foo;
        std::thread t1([&foo] { foo.first([] { std::cout << "first"; }); });
        std::thread t2([&foo] { foo.second([] { std::cout << "second"; }); });
        std::thread t3([&foo] { foo.third([] { std::cout << "third"; }); });
        t1.join();
        t2.join();
        t3.join();
        std::cout << "\n";
    }

    std::cout << "\n=== Test 4: FooBar (3 times) ===\n";
    {
        FooBar foobar(3);
        std::thread t1([&foobar] { foobar.foo([] { std::cout << "foo"; }); });
        std::thread t2([&foobar] { foobar.bar([] { std::cout << "bar"; }); });
        t1.join();
        t2.join();
        std::cout << "\n";
    }

    std::cout << "\n=== Test 5: ZeroEvenOdd (5) ===\n";
    {
        ZeroEvenOdd zeo(5);
        std::thread t1([&zeo] { zeo.zero([](int x) { std::cout << x; }); });
        std::thread t2([&zeo] { zeo.odd([](int x) { std::cout << x; }); });
        std::thread t3([&zeo] { zeo.even([](int x) { std::cout << x; }); });
        t1.join();
        t2.join();
        t3.join();
        std::cout << "\n";  // Output: 0102030405
    }

    return 0;
}

/**
 * 关键要点：
 *
 * 1. 条件变量使用模式：
 *    - wait(lock, predicate) 避免虚假唤醒
 *    - notify_one() vs notify_all() 的选择
 *    - 先修改状态，再通知
 *
 * 2. 状态机设计：
 *    - 用整数或枚举表示当前应该执行的线程
 *    - 每个线程等待自己的状态
 *    - 执行后切换到下一个状态
 *
 * 3. 信号量 (C++20)：
 *    - counting_semaphore 简化同步
 *    - acquire() 阻塞等待
 *    - release() 释放信号
 *
 * 4. 原子变量：
 *    - 简单场景可用 atomic + yield
 *    - 注意内存序的选择
 *
 * 5. 常见陷阱：
 *    - 忘记处理结束条件导致死锁
 *    - notify 在 unlock 之前可能导致效率问题
 *    - 虚假唤醒导致逻辑错误
 */
