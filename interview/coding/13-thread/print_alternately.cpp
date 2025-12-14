/**
 * @file print_alternately.cpp
 * @brief 交替打印 - 面试题实现
 */

#include "print_alternately.h"

namespace PrintAlternately {

// ==================== 面试者实现 ====================

// 题目1: 奇偶数交替打印
OddEvenPrinter::OddEvenPrinter(int max) : max_(max), current_(1) {}

void OddEvenPrinter::printOdd() {
    // TODO
}

void OddEvenPrinter::printEven() {
    // TODO
}

// 题目2: ABC交替打印
ABCPrinter::ABCPrinter(int n) : n_(n), state_(0), count_(0) {}

void ABCPrinter::printA() {
    // TODO
}

void ABCPrinter::printB() {
    // TODO
}

void ABCPrinter::printC() {
    // TODO
}

// 题目3: 按序打印
Foo::Foo() {}

void Foo::first(std::function<void()> printFirst) {
    // TODO
    printFirst();
}

void Foo::second(std::function<void()> printSecond) {
    // TODO
    printSecond();
}

void Foo::third(std::function<void()> printThird) {
    // TODO
    printThird();
}

// 题目4: FooBar交替打印
FooBar::FooBar(int n) : n_(n) {}

void FooBar::foo(std::function<void()> printFoo) {
    for (int i = 0; i < n_; ++i) {
        // TODO
        printFoo();
    }
}

void FooBar::bar(std::function<void()> printBar) {
    for (int i = 0; i < n_; ++i) {
        // TODO
        printBar();
    }
}

} // namespace PrintAlternately
