/**
 * @file print_alternately.cpp
 * @brief 交替打印 - 面试题
 *
 * 多线程交替打印是经典的线程同步问题，考察对 mutex、condition_variable 的掌握。
 */
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>

// 题目1: 两个线程交替打印奇偶数 1-100
// 线程A打印奇数，线程B打印偶数
class OddEvenPrinter {
public:
    OddEvenPrinter(int max) : max_(max), current_(1) {}

    void printOdd() { /* TODO: 打印奇数 */ }
    void printEven() { /* TODO: 打印偶数 */ }

private:
    int max_;
    int current_;
    std::mutex mutex_;
    std::condition_variable cv_;
};

// 题目2: 三个线程交替打印 ABC，打印n次 "ABCABC..."
class ABCPrinter {
public:
    ABCPrinter(int n) : n_(n), state_(0), count_(0) {}

    void printA() { /* TODO */ }
    void printB() { /* TODO */ }
    void printC() { /* TODO */ }

private:
    int n_;
    int state_;  // 0=A, 1=B, 2=C
    int count_;
    std::mutex mutex_;
    std::condition_variable cv_;
};

// 题目3: 按序打印 - 三个不同线程按顺序调用 first(), second(), third()
// 保证输出 "firstsecondthird"
class Foo {
public:
    Foo() {}

    void first(std::function<void()> printFirst) {
        // TODO: 保证最先执行
        printFirst();
    }

    void second(std::function<void()> printSecond) {
        // TODO: 保证第二个执行
        printSecond();
    }

    void third(std::function<void()> printThird) {
        // TODO: 保证第三个执行
        printThird();
    }

private:
    // TODO: 添加同步原语
};

// 题目4: 交替打印 FooBar，n次
// 两个线程交替执行 foo() 和 bar()，输出 "foobarfoobar..."
class FooBar {
public:
    FooBar(int n) : n_(n) {}

    void foo(std::function<void()> printFoo) {
        for (int i = 0; i < n_; ++i) {
            // TODO
            printFoo();
        }
    }

    void bar(std::function<void()> printBar) {
        for (int i = 0; i < n_; ++i) {
            // TODO
            printBar();
        }
    }

private:
    int n_;
    // TODO: 添加同步原语
};

// 题目5: 打印零与奇偶数
// 三个线程：一个打印0，一个打印奇数，一个打印偶数
// 输出序列: 0102030405...
class ZeroEvenOdd {
public:
    ZeroEvenOdd(int n) : n_(n) {}

    void zero(std::function<void(int)> printNumber) { /* TODO */ }
    void even(std::function<void(int)> printNumber) { /* TODO */ }
    void odd(std::function<void(int)> printNumber) { /* TODO */ }

private:
    int n_;
    // TODO: 添加同步原语
};

int main() {
    return 0;
}
