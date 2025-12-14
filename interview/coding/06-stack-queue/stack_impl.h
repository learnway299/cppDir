/**
 * @file stack_impl.h
 * @brief 栈和队列实现问题 - 函数声明
 */

#ifndef STACK_IMPL_H
#define STACK_IMPL_H

#include <vector>
#include <string>
#include "../../util/Util.h"

namespace StackImplProblems {

// 题目1: 用栈实现队列
class MyQueue {
public:
    MyQueue();
    void push(int x);
    int pop();
    int peek();
    bool empty();
};

class MyQueueSolution {
public:
    MyQueueSolution();
    void push(int x);
    int pop();
    int peek();
    bool empty();
};

// 题目2: 用队列实现栈
class MyStack {
public:
    MyStack();
    void push(int x);
    int pop();
    int top();
    bool empty();
};

class MyStackSolution {
public:
    MyStackSolution();
    void push(int x);
    int pop();
    int top();
    bool empty();
};

// 题目3: 最小栈
class MinStack {
public:
    MinStack();
    void push(int val);
    void pop();
    int top();
    int getMin();
};

class MinStackSolution {
public:
    MinStackSolution();
    void push(int val);
    void pop();
    int top();
    int getMin();
};

// 题目4: 有效的括号
bool isValid(std::string s);
bool isValidSolution(std::string s);

// 题目5: 基本计算器
int calculate(std::string s);
int calculateSolution(std::string s);

// 题目6: 逆波兰表达式求值
int evalRPN(std::vector<std::string>& tokens);
int evalRPNSolution(std::vector<std::string>& tokens);

// 题目7: 简化路径
std::string simplifyPath(std::string path);
std::string simplifyPathSolution(std::string path);

// 题目8: 字符串解码
std::string decodeString(std::string s);
std::string decodeStringSolution(std::string s);

// 题目9: 每日温度
std::vector<int> dailyTemperatures(std::vector<int>& temperatures);
std::vector<int> dailyTemperaturesSolution(std::vector<int>& temperatures);

// 题目10: 验证栈序列
bool validateStackSequences(std::vector<int>& pushed, std::vector<int>& popped);
bool validateStackSequencesSolution(std::vector<int>& pushed, std::vector<int>& popped);

// ==================== 测试函数声明 ====================

void testStackImpl();          // 测试面试者实现
void testStackImplSolution();  // 测试解答实现

// runTests 在头文件 inline 定义，调用两个测试函数
inline void runTests() {
    testStackImpl();
    testStackImplSolution();
}

} // namespace StackImplProblems

#endif // STACK_IMPL_H
