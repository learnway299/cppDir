/**
 * @file print_alternately_solution.cpp
 * @brief 交替打印 - 参考答案
 */

#include "print_alternately.h"
#include <iostream>
#include <sstream>
#include <cassert>

namespace PrintAlternately {

// ==================== 参考答案实现 ====================

// 奇偶数交替打印
OddEvenPrinterSolution::OddEvenPrinterSolution(int max) : max_(max), current_(1) {}

void OddEvenPrinterSolution::printOdd() {
    while (true) {
        std::unique_lock<std::mutex> lock(mutex_);
        cv_.wait(lock, [this] { return current_ > max_ || (current_ % 2 == 1); });
        if (current_ > max_) break;
        ++current_;
        cv_.notify_one();
    }
}

void OddEvenPrinterSolution::printEven() {
    while (true) {
        std::unique_lock<std::mutex> lock(mutex_);
        cv_.wait(lock, [this] { return current_ > max_ || (current_ % 2 == 0); });
        if (current_ > max_) break;
        ++current_;
        cv_.notify_one();
    }
}

int OddEvenPrinterSolution::getCurrent() const { return current_; }

// ABC 交替打印
ABCPrinterSolution::ABCPrinterSolution(int n) : n_(n), state_(0), count_(0) {}

void ABCPrinterSolution::printA(std::string& output) {
    for (int i = 0; i < n_; ++i) {
        std::unique_lock<std::mutex> lock(mutex_);
        cv_.wait(lock, [this] { return state_ == 0; });
        output += "A";
        state_ = 1;
        cv_.notify_all();
    }
}

void ABCPrinterSolution::printB(std::string& output) {
    for (int i = 0; i < n_; ++i) {
        std::unique_lock<std::mutex> lock(mutex_);
        cv_.wait(lock, [this] { return state_ == 1; });
        output += "B";
        state_ = 2;
        cv_.notify_all();
    }
}

void ABCPrinterSolution::printC(std::string& output) {
    for (int i = 0; i < n_; ++i) {
        std::unique_lock<std::mutex> lock(mutex_);
        cv_.wait(lock, [this] { return state_ == 2; });
        output += "C";
        state_ = 0;
        cv_.notify_all();
    }
}

// 按序打印
FooSolution::FooSolution() : step_(0) {}

void FooSolution::first(std::function<void()> printFirst) {
    printFirst();
    {
        std::lock_guard<std::mutex> lock(mutex_);
        step_ = 1;
    }
    cv_.notify_all();
}

void FooSolution::second(std::function<void()> printSecond) {
    std::unique_lock<std::mutex> lock(mutex_);
    cv_.wait(lock, [this] { return step_ >= 1; });
    printSecond();
    step_ = 2;
    cv_.notify_all();
}

void FooSolution::third(std::function<void()> printThird) {
    std::unique_lock<std::mutex> lock(mutex_);
    cv_.wait(lock, [this] { return step_ >= 2; });
    printThird();
}

// 无锁版本
FooAtomicSolution::FooAtomicSolution() : step_(0) {}

void FooAtomicSolution::first(std::function<void()> printFirst) {
    printFirst();
    step_.store(1, std::memory_order_release);
}

void FooAtomicSolution::second(std::function<void()> printSecond) {
    while (step_.load(std::memory_order_acquire) < 1) {
        std::this_thread::yield();
    }
    printSecond();
    step_.store(2, std::memory_order_release);
}

void FooAtomicSolution::third(std::function<void()> printThird) {
    while (step_.load(std::memory_order_acquire) < 2) {
        std::this_thread::yield();
    }
    printThird();
}

// FooBar 交替打印
FooBarSolution::FooBarSolution(int n) : n_(n), fooTurn_(true) {}

void FooBarSolution::foo(std::function<void()> printFoo) {
    for (int i = 0; i < n_; ++i) {
        std::unique_lock<std::mutex> lock(mutex_);
        cv_.wait(lock, [this] { return fooTurn_; });
        printFoo();
        fooTurn_ = false;
        cv_.notify_one();
    }
}

void FooBarSolution::bar(std::function<void()> printBar) {
    for (int i = 0; i < n_; ++i) {
        std::unique_lock<std::mutex> lock(mutex_);
        cv_.wait(lock, [this] { return !fooTurn_; });
        printBar();
        fooTurn_ = true;
        cv_.notify_one();
    }
}

// ==================== 测试函数 ====================

void runTests() {
    std::cout << "=== Print Alternately Tests ===" << std::endl;

    // 测试ABC打印
    {
        ABCPrinterSolution printer(3);
        std::string output;

        std::thread t1([&printer, &output] { printer.printA(output); });
        std::thread t2([&printer, &output] { printer.printB(output); });
        std::thread t3([&printer, &output] { printer.printC(output); });

        t1.join();
        t2.join();
        t3.join();

        assert(output == "ABCABCABC");
    }
    std::cout << "  ABC Printer: PASSED" << std::endl;

    // 测试按序打印
    {
        FooSolution foo;
        std::string output;

        std::thread t3([&foo, &output] {
            foo.third([&output] { output += "third"; });
        });
        std::thread t1([&foo, &output] {
            foo.first([&output] { output += "first"; });
        });
        std::thread t2([&foo, &output] {
            foo.second([&output] { output += "second"; });
        });

        t1.join();
        t2.join();
        t3.join();

        assert(output == "firstsecondthird");
    }
    std::cout << "  Foo (Ordered Print): PASSED" << std::endl;

    // 测试FooBar
    {
        FooBarSolution foobar(3);
        std::string output;

        std::thread t1([&foobar, &output] {
            foobar.foo([&output] { output += "foo"; });
        });
        std::thread t2([&foobar, &output] {
            foobar.bar([&output] { output += "bar"; });
        });

        t1.join();
        t2.join();

        assert(output == "foobarfoobarfoobar");
    }
    std::cout << "  FooBar: PASSED" << std::endl;
}

} // namespace PrintAlternately
