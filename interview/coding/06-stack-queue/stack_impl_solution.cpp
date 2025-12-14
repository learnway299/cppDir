/**
 * @file stack_impl_solution.cpp
 * @brief 栈和队列实现问题 - 解答文件
 */

#include "stack_impl.h"
#include <iostream>
#include <stack>
#include <queue>
#include <unordered_map>
#include <sstream>
#include <cassert>

namespace StackImplProblems {

// 题目1: 用栈实现队列（解答版本）
class MyQueueSolutionImpl {
private:
    std::stack<int> inStack;
    std::stack<int> outStack;

    void transfer() {
        if (outStack.empty()) {
            while (!inStack.empty()) {
                outStack.push(inStack.top());
                inStack.pop();
            }
        }
    }

public:
    MyQueueSolutionImpl() {}

    void push(int x) {
        inStack.push(x);
    }

    int pop() {
        transfer();
        int val = outStack.top();
        outStack.pop();
        return val;
    }

    int peek() {
        transfer();
        return outStack.top();
    }

    bool empty() {
        return inStack.empty() && outStack.empty();
    }
};

MyQueueSolution::MyQueueSolution() {}
void MyQueueSolution::push(int x) {}
int MyQueueSolution::pop() { return 0; }
int MyQueueSolution::peek() { return 0; }
bool MyQueueSolution::empty() { return true; }

// 题目2: 用队列实现栈（解答版本）
class MyStackSolutionImpl {
private:
    std::queue<int> q;

public:
    MyStackSolutionImpl() {}

    void push(int x) {
        int n = static_cast<int>(q.size());
        q.push(x);
        for (int i = 0; i < n; ++i) {
            q.push(q.front());
            q.pop();
        }
    }

    int pop() {
        int val = q.front();
        q.pop();
        return val;
    }

    int top() {
        return q.front();
    }

    bool empty() {
        return q.empty();
    }
};

MyStackSolution::MyStackSolution() {}
void MyStackSolution::push(int x) {}
int MyStackSolution::pop() { return 0; }
int MyStackSolution::top() { return 0; }
bool MyStackSolution::empty() { return true; }

// 题目3: 最小栈（解答版本）
class MinStackSolutionImpl {
private:
    std::stack<int> dataStack;
    std::stack<int> minStack;

public:
    MinStackSolutionImpl() {}

    void push(int val) {
        dataStack.push(val);
        if (minStack.empty() || val <= minStack.top()) {
            minStack.push(val);
        } else {
            minStack.push(minStack.top());
        }
    }

    void pop() {
        dataStack.pop();
        minStack.pop();
    }

    int top() {
        return dataStack.top();
    }

    int getMin() {
        return minStack.top();
    }
};

MinStackSolution::MinStackSolution() {}
void MinStackSolution::push(int val) {}
void MinStackSolution::pop() {}
int MinStackSolution::top() { return 0; }
int MinStackSolution::getMin() { return 0; }

// 题目4: 有效的括号
bool isValidSolution(std::string s) {
    std::stack<char> stk;
    std::unordered_map<char, char> pairs = {
        {')', '('}, {']', '['}, {'}', '{'}
    };

    for (char c : s) {
        if (pairs.count(c)) {
            if (stk.empty() || stk.top() != pairs[c]) {
                return false;
            }
            stk.pop();
        } else {
            stk.push(c);
        }
    }

    return stk.empty();
}

// 题目5: 基本计算器
int calculateSolution(std::string s) {
    std::stack<int> signs;
    signs.push(1);

    int result = 0;
    int num = 0;
    int sign = 1;

    for (int i = 0; i < static_cast<int>(s.size()); ++i) {
        char c = s[i];

        if (std::isdigit(c)) {
            num = num * 10 + (c - '0');
        } else if (c == '+') {
            result += sign * num;
            num = 0;
            sign = signs.top();
        } else if (c == '-') {
            result += sign * num;
            num = 0;
            sign = -signs.top();
        } else if (c == '(') {
            signs.push(sign);
        } else if (c == ')') {
            signs.pop();
        }
    }

    result += sign * num;
    return result;
}

// 题目6: 逆波兰表达式求值
int evalRPNSolution(std::vector<std::string>& tokens) {
    std::stack<int> stk;

    for (const std::string& token : tokens) {
        if (token == "+" || token == "-" || token == "*" || token == "/") {
            int b = stk.top(); stk.pop();
            int a = stk.top(); stk.pop();

            if (token == "+") stk.push(a + b);
            else if (token == "-") stk.push(a - b);
            else if (token == "*") stk.push(a * b);
            else stk.push(a / b);
        } else {
            stk.push(std::stoi(token));
        }
    }

    return stk.top();
}

// 题目7: 简化路径
std::string simplifyPathSolution(std::string path) {
    std::vector<std::string> stk;
    std::stringstream ss(path);
    std::string token;

    while (std::getline(ss, token, '/')) {
        if (token == "" || token == ".") {
            continue;
        } else if (token == "..") {
            if (!stk.empty()) {
                stk.pop_back();
            }
        } else {
            stk.push_back(token);
        }
    }

    std::string result;
    for (const std::string& dir : stk) {
        result += "/" + dir;
    }

    return result.empty() ? "/" : result;
}

// 题目8: 字符串解码
std::string decodeStringSolution(std::string s) {
    std::stack<int> numStack;
    std::stack<std::string> strStack;
    std::string current;
    int num = 0;

    for (char c : s) {
        if (std::isdigit(c)) {
            num = num * 10 + (c - '0');
        } else if (c == '[') {
            numStack.push(num);
            strStack.push(current);
            num = 0;
            current = "";
        } else if (c == ']') {
            int repeatCount = numStack.top();
            numStack.pop();
            std::string prev = strStack.top();
            strStack.pop();

            std::string repeated;
            for (int i = 0; i < repeatCount; ++i) {
                repeated += current;
            }
            current = prev + repeated;
        } else {
            current += c;
        }
    }

    return current;
}

// 题目9: 每日温度
std::vector<int> dailyTemperaturesSolution(std::vector<int>& temperatures) {
    int n = static_cast<int>(temperatures.size());
    std::vector<int> answer(n, 0);
    std::stack<int> stk;

    for (int i = 0; i < n; ++i) {
        while (!stk.empty() && temperatures[i] > temperatures[stk.top()]) {
            int prevIdx = stk.top();
            stk.pop();
            answer[prevIdx] = i - prevIdx;
        }
        stk.push(i);
    }

    return answer;
}

// 题目10: 验证栈序列
bool validateStackSequencesSolution(std::vector<int>& pushed, std::vector<int>& popped) {
    std::stack<int> stk;
    int j = 0;

    for (int x : pushed) {
        stk.push(x);
        while (!stk.empty() && stk.top() == popped[j]) {
            stk.pop();
            ++j;
        }
    }

    return stk.empty();
}

// ==================== 测试函数 ====================

void testStackImplSolution() {
    std::cout << "=== Stack Implementation Tests (Solution) ===\n";

    // MyQueue
    MyQueueSolutionImpl myQueue;
    myQueue.push(1);
    myQueue.push(2);
    assert(myQueue.peek() == 1);
    assert(myQueue.pop() == 1);
    assert(myQueue.empty() == false);
    std::cout << "  MyQueue: PASSED\n";

    // MyStack
    MyStackSolutionImpl myStack;
    myStack.push(1);
    myStack.push(2);
    assert(myStack.top() == 2);
    assert(myStack.pop() == 2);
    assert(myStack.empty() == false);
    std::cout << "  MyStack: PASSED\n";

    // MinStack
    MinStackSolutionImpl minStack;
    minStack.push(-2);
    minStack.push(0);
    minStack.push(-3);
    assert(minStack.getMin() == -3);
    minStack.pop();
    assert(minStack.top() == 0);
    assert(minStack.getMin() == -2);
    std::cout << "  MinStack: PASSED\n";

    // isValid
    assert(isValidSolution("()") == true);
    assert(isValidSolution("()[]{}") == true);
    assert(isValidSolution("(]") == false);
    assert(isValidSolution("([)]") == false);
    std::cout << "  isValid: PASSED\n";

    // calculate
    assert(calculateSolution("1 + 1") == 2);
    assert(calculateSolution(" 2-1 + 2 ") == 3);
    assert(calculateSolution("(1+(4+5+2)-3)+(6+8)") == 23);
    std::cout << "  calculate: PASSED\n";

    // evalRPN
    std::vector<std::string> tokens1 = {"2", "1", "+", "3", "*"};
    assert(evalRPNSolution(tokens1) == 9);
    std::vector<std::string> tokens2 = {"4", "13", "5", "/", "+"};
    assert(evalRPNSolution(tokens2) == 6);
    std::cout << "  evalRPN: PASSED\n";

    // simplifyPath
    assert(simplifyPathSolution("/home/") == "/home");
    assert(simplifyPathSolution("/../") == "/");
    assert(simplifyPathSolution("/home//foo/") == "/home/foo");
    assert(simplifyPathSolution("/a/./b/../../c/") == "/c");
    std::cout << "  simplifyPath: PASSED\n";

    // decodeString
    assert(decodeStringSolution("3[a]2[bc]") == "aaabcbc");
    assert(decodeStringSolution("3[a2[c]]") == "accaccacc");
    assert(decodeStringSolution("2[abc]3[cd]ef") == "abcabccdcdcdef");
    std::cout << "  decodeString: PASSED\n";

    // dailyTemperatures
    std::vector<int> temps = {73, 74, 75, 71, 69, 72, 76, 73};
    std::vector<int> expected = {1, 1, 4, 2, 1, 1, 0, 0};
    assert(dailyTemperaturesSolution(temps) == expected);
    std::cout << "  dailyTemperatures: PASSED\n";

    // validateStackSequences
    std::vector<int> pushed1 = {1, 2, 3, 4, 5};
    std::vector<int> popped1 = {4, 5, 3, 2, 1};
    assert(validateStackSequencesSolution(pushed1, popped1) == true);
    std::vector<int> pushed2 = {1, 2, 3, 4, 5};
    std::vector<int> popped2 = {4, 3, 5, 1, 2};
    assert(validateStackSequencesSolution(pushed2, popped2) == false);
    std::cout << "  validateStackSequences: PASSED\n";

    std::cout << "All Stack Implementation tests passed!\n";
}

} // namespace StackImplProblems
