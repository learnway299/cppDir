/**
 * @file stack_impl_solution.cpp
 * @brief 栈和队列实现问题 - 解答文件
 */

#include <iostream>
#include <vector>
#include <stack>
#include <queue>
#include <string>
#include <unordered_map>
#include <sstream>
#include <climits>

/**
 * 题目1: 用栈实现队列
 *
 * 解法: 双栈
 * - 入队栈：用于 push 操作
 * - 出队栈：用于 pop/peek 操作
 *
 * 均摊时间复杂度: O(1)
 * 空间复杂度: O(n)
 */
class MyQueue {
private:
    std::stack<int> inStack;   // 入队栈
    std::stack<int> outStack;  // 出队栈

    void transfer() {
        if (outStack.empty()) {
            while (!inStack.empty()) {
                outStack.push(inStack.top());
                inStack.pop();
            }
        }
    }

public:
    MyQueue() {}

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

/**
 * 题目2: 用队列实现栈
 *
 * 解法1: 双队列
 * 解法2: 单队列（更优）
 *
 * 单队列解法：push 时将新元素插入后，把前面的元素依次出队再入队
 *
 * push: O(n), pop/top: O(1)
 */
class MyStack {
private:
    std::queue<int> q;

public:
    MyStack() {}

    void push(int x) {
        int n = q.size();
        q.push(x);
        // 将前面的元素移到后面
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

/**
 * 双队列版本
 */
class MyStackTwoQueues {
private:
    std::queue<int> q1, q2;

public:
    void push(int x) {
        q2.push(x);
        while (!q1.empty()) {
            q2.push(q1.front());
            q1.pop();
        }
        std::swap(q1, q2);
    }

    int pop() {
        int val = q1.front();
        q1.pop();
        return val;
    }

    int top() {
        return q1.front();
    }

    bool empty() {
        return q1.empty();
    }
};

/**
 * 题目3: 最小栈
 *
 * 解法1: 辅助栈
 * 解法2: 单栈存储差值（空间优化）
 *
 * 时间复杂度: O(1)
 * 空间复杂度: O(n)
 */
class MinStack {
private:
    std::stack<int> dataStack;
    std::stack<int> minStack;

public:
    MinStack() {}

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

/**
 * 空间优化版本：只在最小值变化时入栈
 */
class MinStackOptimized {
private:
    std::stack<int> dataStack;
    std::stack<int> minStack;

public:
    MinStackOptimized() {}

    void push(int val) {
        dataStack.push(val);
        if (minStack.empty() || val <= minStack.top()) {
            minStack.push(val);
        }
    }

    void pop() {
        if (dataStack.top() == minStack.top()) {
            minStack.pop();
        }
        dataStack.pop();
    }

    int top() {
        return dataStack.top();
    }

    int getMin() {
        return minStack.top();
    }
};

/**
 * 题目4: 有效的括号
 *
 * 解法: 栈
 * 时间复杂度: O(n)
 * 空间复杂度: O(n)
 */
bool isValid(std::string s) {
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

/**
 * 题目5: 基本计算器
 *
 * 解法: 栈处理符号和括号
 * 时间复杂度: O(n)
 * 空间复杂度: O(n)
 */
int calculate(std::string s) {
    std::stack<int> signs;  // 存储当前括号层级的符号
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
            sign = signs.top();  // 当前层级的符号
        } else if (c == '-') {
            result += sign * num;
            num = 0;
            sign = -signs.top();  // 取反
        } else if (c == '(') {
            signs.push(sign);  // 进入新层级，保存当前符号
        } else if (c == ')') {
            signs.pop();  // 退出当前层级
        }
    }

    result += sign * num;
    return result;
}

/**
 * 题目6: 逆波兰表达式求值
 *
 * 解法: 栈
 * 时间复杂度: O(n)
 * 空间复杂度: O(n)
 */
int evalRPN(std::vector<std::string>& tokens) {
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

/**
 * 题目7: 简化路径
 *
 * 解法: 栈
 * 时间复杂度: O(n)
 * 空间复杂度: O(n)
 */
std::string simplifyPath(std::string path) {
    std::vector<std::string> stk;  // 用 vector 方便最后拼接
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

/**
 * 题目8: 字符串解码
 *
 * 解法: 栈
 * 时间复杂度: O(解码后字符串长度)
 * 空间复杂度: O(n)
 */
std::string decodeString(std::string s) {
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

/**
 * 递归解法
 */
class DecoderRecursive {
private:
    int idx = 0;

public:
    std::string decode(const std::string& s) {
        std::string result;

        while (idx < static_cast<int>(s.size()) && s[idx] != ']') {
            if (std::isdigit(s[idx])) {
                int num = 0;
                while (idx < static_cast<int>(s.size()) && std::isdigit(s[idx])) {
                    num = num * 10 + (s[idx] - '0');
                    ++idx;
                }
                ++idx;  // 跳过 '['
                std::string sub = decode(s);
                ++idx;  // 跳过 ']'

                for (int i = 0; i < num; ++i) {
                    result += sub;
                }
            } else {
                result += s[idx++];
            }
        }

        return result;
    }
};

/**
 * 题目9: 每日温度
 *
 * 解法: 单调递减栈
 * 时间复杂度: O(n)
 * 空间复杂度: O(n)
 */
std::vector<int> dailyTemperatures(std::vector<int>& temperatures) {
    int n = temperatures.size();
    std::vector<int> answer(n, 0);
    std::stack<int> stk;  // 存储索引，维护单调递减

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

/**
 * 题目10: 验证栈序列
 *
 * 解法: 模拟
 * 时间复杂度: O(n)
 * 空间复杂度: O(n)
 */
bool validateStackSequences(std::vector<int>& pushed, std::vector<int>& popped) {
    std::stack<int> stk;
    int j = 0;

    for (int x : pushed) {
        stk.push(x);
        // 尽可能多地出栈
        while (!stk.empty() && stk.top() == popped[j]) {
            stk.pop();
            ++j;
        }
    }

    return stk.empty();
}


// ==================== 测试代码 ====================
#include <cassert>

void testMyQueue() {
    MyQueue myQueue;
    myQueue.push(1);
    myQueue.push(2);
    assert(myQueue.peek() == 1);
    assert(myQueue.pop() == 1);
    assert(myQueue.empty() == false);

    std::cout << "MyQueue: PASSED\n";
}

void testMyStack() {
    MyStack myStack;
    myStack.push(1);
    myStack.push(2);
    assert(myStack.top() == 2);
    assert(myStack.pop() == 2);
    assert(myStack.empty() == false);

    std::cout << "MyStack: PASSED\n";
}

void testMinStack() {
    MinStack minStack;
    minStack.push(-2);
    minStack.push(0);
    minStack.push(-3);
    assert(minStack.getMin() == -3);
    minStack.pop();
    assert(minStack.top() == 0);
    assert(minStack.getMin() == -2);

    std::cout << "MinStack: PASSED\n";
}

void testIsValid() {
    assert(isValid("()") == true);
    assert(isValid("()[]{}") == true);
    assert(isValid("(]") == false);
    assert(isValid("([)]") == false);

    std::cout << "isValid: PASSED\n";
}

void testCalculate() {
    assert(calculate("1 + 1") == 2);
    assert(calculate(" 2-1 + 2 ") == 3);
    assert(calculate("(1+(4+5+2)-3)+(6+8)") == 23);

    std::cout << "calculate: PASSED\n";
}

void testEvalRPN() {
    std::vector<std::string> tokens1 = {"2", "1", "+", "3", "*"};
    assert(evalRPN(tokens1) == 9);

    std::vector<std::string> tokens2 = {"4", "13", "5", "/", "+"};
    assert(evalRPN(tokens2) == 6);

    std::cout << "evalRPN: PASSED\n";
}

void testSimplifyPath() {
    assert(simplifyPath("/home/") == "/home");
    assert(simplifyPath("/../") == "/");
    assert(simplifyPath("/home//foo/") == "/home/foo");
    assert(simplifyPath("/a/./b/../../c/") == "/c");

    std::cout << "simplifyPath: PASSED\n";
}

void testDecodeString() {
    assert(decodeString("3[a]2[bc]") == "aaabcbc");
    assert(decodeString("3[a2[c]]") == "accaccacc");
    assert(decodeString("2[abc]3[cd]ef") == "abcabccdcdcdef");

    std::cout << "decodeString: PASSED\n";
}

void testDailyTemperatures() {
    std::vector<int> temps = {73, 74, 75, 71, 69, 72, 76, 73};
    std::vector<int> expected = {1, 1, 4, 2, 1, 1, 0, 0};
    assert(dailyTemperatures(temps) == expected);

    std::cout << "dailyTemperatures: PASSED\n";
}

void testValidateStackSequences() {
    std::vector<int> pushed1 = {1, 2, 3, 4, 5};
    std::vector<int> popped1 = {4, 5, 3, 2, 1};
    assert(validateStackSequences(pushed1, popped1) == true);

    std::vector<int> pushed2 = {1, 2, 3, 4, 5};
    std::vector<int> popped2 = {4, 3, 5, 1, 2};
    assert(validateStackSequences(pushed2, popped2) == false);

    std::cout << "validateStackSequences: PASSED\n";
}

int main() {
    std::cout << "=== Stack Implementation Solution Tests ===\n";

    testMyQueue();
    testMyStack();
    testMinStack();
    testIsValid();
    testCalculate();
    testEvalRPN();
    testSimplifyPath();
    testDecodeString();
    testDailyTemperatures();
    testValidateStackSequences();

    std::cout << "\nAll tests passed!\n";
    return 0;
}
