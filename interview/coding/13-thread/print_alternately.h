/**
 * @file print_alternately.h
 * @brief 交替打印 - 声明
 *
 * 多线程交替打印是经典的线程同步问题
 */

#ifndef PRINT_ALTERNATELY_H
#define PRINT_ALTERNATELY_H

#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <atomic>

namespace PrintAlternately {

// ==================== 面试者实现 ====================

class OddEvenPrinter {
public:
    OddEvenPrinter(int max);
    void printOdd();
    void printEven();

private:
    int max_;
    int current_;
    std::mutex mutex_;
    std::condition_variable cv_;
};

class ABCPrinter {
public:
    ABCPrinter(int n);
    void printA();
    void printB();
    void printC();

private:
    int n_;
    int state_;
    int count_;
    std::mutex mutex_;
    std::condition_variable cv_;
};

class Foo {
public:
    Foo();
    void first(std::function<void()> printFirst);
    void second(std::function<void()> printSecond);
    void third(std::function<void()> printThird);

private:
    // TODO
};

class FooBar {
public:
    FooBar(int n);
    void foo(std::function<void()> printFoo);
    void bar(std::function<void()> printBar);

private:
    int n_;
    // TODO
};

// ==================== 参考答案 ====================

class OddEvenPrinterSolution {
public:
    OddEvenPrinterSolution(int max);
    void printOdd();
    void printEven();
    int getCurrent() const;

private:
    int max_;
    int current_;
    std::mutex mutex_;
    std::condition_variable cv_;
};

class ABCPrinterSolution {
public:
    ABCPrinterSolution(int n);
    void printA(std::string& output);
    void printB(std::string& output);
    void printC(std::string& output);

private:
    int n_;
    int state_;
    int count_;
    std::mutex mutex_;
    std::condition_variable cv_;
};

class FooSolution {
public:
    FooSolution();
    void first(std::function<void()> printFirst);
    void second(std::function<void()> printSecond);
    void third(std::function<void()> printThird);

private:
    std::mutex mutex_;
    std::condition_variable cv_;
    int step_;
};

class FooAtomicSolution {
public:
    FooAtomicSolution();
    void first(std::function<void()> printFirst);
    void second(std::function<void()> printSecond);
    void third(std::function<void()> printThird);

private:
    std::atomic<int> step_;
};

class FooBarSolution {
public:
    FooBarSolution(int n);
    void foo(std::function<void()> printFoo);
    void bar(std::function<void()> printBar);

private:
    int n_;
    bool fooTurn_;
    std::mutex mutex_;
    std::condition_variable cv_;
};

// 测试函数
void runTests();

} // namespace PrintAlternately

#endif // PRINT_ALTERNATELY_H
