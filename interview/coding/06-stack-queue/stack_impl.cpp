/**
 * @file stack_impl.cpp
 * @brief 栈和队列实现问题 - 题目文件
 *
 * 栈：后进先出 (LIFO)
 * 队列：先进先出 (FIFO)
 * 常见应用：表达式求值、括号匹配、BFS/DFS
 */

#include <iostream>
#include <vector>
#include <stack>
#include <queue>
#include <string>
#include <climits>

/**
 * 题目1: 用栈实现队列
 *
 * 请你仅使用两个栈实现先入先出队列。队列应当支持一般队列支持的所有操作：
 *   - push(x) 将元素 x 推到队列的末尾
 *   - pop() 从队列的开头移除并返回元素
 *   - peek() 返回队列开头的元素
 *   - empty() 如果队列为空，返回 true；否则返回 false
 *
 * 示例:
 *   MyQueue myQueue = new MyQueue();
 *   myQueue.push(1);
 *   myQueue.push(2);
 *   myQueue.peek();  // 返回 1
 *   myQueue.pop();   // 返回 1
 *   myQueue.empty(); // 返回 false
 *
 * 思路: 使用两个栈，一个用于入队，一个用于出队
 */
class MyQueue {
public:
    MyQueue() {
        // TODO: 初始化
    }

    void push(int x) {
        // TODO: 实现入队
    }

    int pop() {
        // TODO: 实现出队
        return 0;
    }

    int peek() {
        // TODO: 返回队首元素
        return 0;
    }

    bool empty() {
        // TODO: 判断是否为空
        return true;
    }
};

/**
 * 题目2: 用队列实现栈
 *
 * 请你仅使用两个队列实现一个后入先出（LIFO）的栈。
 *
 * 实现 MyStack 类：
 *   - void push(int x) 将元素 x 压入栈顶
 *   - int pop() 移除并返回栈顶元素
 *   - int top() 返回栈顶元素
 *   - boolean empty() 如果栈是空的，返回 true；否则返回 false
 *
 * 进阶：只用一个队列实现
 *
 * 示例:
 *   MyStack myStack = new MyStack();
 *   myStack.push(1);
 *   myStack.push(2);
 *   myStack.top();   // 返回 2
 *   myStack.pop();   // 返回 2
 *   myStack.empty(); // 返回 false
 */
class MyStack {
public:
    MyStack() {
        // TODO: 初始化
    }

    void push(int x) {
        // TODO: 实现入栈
    }

    int pop() {
        // TODO: 实现出栈
        return 0;
    }

    int top() {
        // TODO: 返回栈顶元素
        return 0;
    }

    bool empty() {
        // TODO: 判断是否为空
        return true;
    }
};

/**
 * 题目3: 最小栈
 *
 * 设计一个支持 push，pop，top 操作，并能在常数时间内检索到最小元素的栈。
 *
 * 实现 MinStack 类:
 *   - MinStack() 初始化堆栈对象
 *   - void push(int val) 将元素 val 推入堆栈
 *   - void pop() 删除堆栈顶部的元素
 *   - int top() 获取堆栈顶部的元素
 *   - int getMin() 获取堆栈中的最小元素
 *
 * 示例:
 *   MinStack minStack = new MinStack();
 *   minStack.push(-2);
 *   minStack.push(0);
 *   minStack.push(-3);
 *   minStack.getMin(); // 返回 -3
 *   minStack.pop();
 *   minStack.top();    // 返回 0
 *   minStack.getMin(); // 返回 -2
 *
 * 思路: 辅助栈记录每个状态下的最小值
 */
class MinStack {
public:
    MinStack() {
        // TODO: 初始化
    }

    void push(int val) {
        // TODO: 实现
    }

    void pop() {
        // TODO: 实现
    }

    int top() {
        // TODO: 实现
        return 0;
    }

    int getMin() {
        // TODO: 实现
        return 0;
    }
};

/**
 * 题目4: 有效的括号
 *
 * 给定一个只包括 '('，')'，'{'，'}'，'['，']' 的字符串 s，判断字符串是否有效。
 *
 * 有效字符串需满足：
 *   1. 左括号必须用相同类型的右括号闭合
 *   2. 左括号必须以正确的顺序闭合
 *
 * 示例:
 *   输入: s = "()"
 *   输出: true
 *
 *   输入: s = "()[]{}"
 *   输出: true
 *
 *   输入: s = "(]"
 *   输出: false
 *
 * @param s 括号字符串
 * @return 是否有效
 */
bool isValid(std::string s) {
    // TODO: 实现你的解法
    return false;
}

/**
 * 题目5: 基本计算器
 *
 * 给你一个字符串表达式 s，请你实现一个基本计算器来计算并返回它的值。
 * 表达式可能包含 '+', '-', '(', ')' 和空格。
 *
 * 示例:
 *   输入: s = "1 + 1"
 *   输出: 2
 *
 *   输入: s = " 2-1 + 2 "
 *   输出: 3
 *
 *   输入: s = "(1+(4+5+2)-3)+(6+8)"
 *   输出: 23
 *
 * 思路: 栈处理括号和符号
 *
 * @param s 表达式字符串
 * @return 计算结果
 */
int calculate(std::string s) {
    // TODO: 实现你的解法
    return 0;
}

/**
 * 题目6: 逆波兰表达式求值
 *
 * 根据逆波兰表示法，求表达式的值。
 *
 * 有效的算符包括 +、-、*、/。每个运算对象可以是整数，也可以是另一个逆波兰表达式。
 * 整数除法只保留整数部分。
 *
 * 逆波兰表达式是一种后缀表达式，运算符写在操作数之后。
 *
 * 示例:
 *   输入: tokens = ["2","1","+","3","*"]
 *   输出: 9
 *   解释: ((2 + 1) * 3) = 9
 *
 *   输入: tokens = ["4","13","5","/","+"]
 *   输出: 6
 *   解释: (4 + (13 / 5)) = 6
 *
 *   输入: tokens = ["10","6","9","3","+","-11","*","/","*","17","+","5","+"]
 *   输出: 22
 *
 * @param tokens 逆波兰表达式
 * @return 计算结果
 */
int evalRPN(std::vector<std::string>& tokens) {
    // TODO: 实现你的解法
    return 0;
}

/**
 * 题目7: 简化路径
 *
 * 给你一个字符串 path，表示指向某一文件或目录的 Unix 风格绝对路径，请你将其转化为更加简洁的规范路径。
 *
 * 规则：
 *   - 单个点 '.' 表示当前目录
 *   - 双点 '..' 表示上一级目录
 *   - 多个连续斜杠 '//' 视为单个斜杠 '/'
 *   - 末尾不应有斜杠
 *
 * 示例:
 *   输入: path = "/home/"
 *   输出: "/home"
 *
 *   输入: path = "/../"
 *   输出: "/"
 *
 *   输入: path = "/home//foo/"
 *   输出: "/home/foo"
 *
 *   输入: path = "/a/./b/../../c/"
 *   输出: "/c"
 *
 * @param path Unix 路径
 * @return 简化后的路径
 */
std::string simplifyPath(std::string path) {
    // TODO: 实现你的解法
    return "";
}

/**
 * 题目8: 字符串解码
 *
 * 给定一个经过编码的字符串，返回它解码后的字符串。
 *
 * 编码规则为: k[encoded_string]，表示其中方括号内部的 encoded_string 正好重复 k 次。
 * 注意 k 保证为正整数。
 *
 * 示例:
 *   输入: s = "3[a]2[bc]"
 *   输出: "aaabcbc"
 *
 *   输入: s = "3[a2[c]]"
 *   输出: "accaccacc"
 *
 *   输入: s = "2[abc]3[cd]ef"
 *   输出: "abcabccdcdcdef"
 *
 * 思路: 栈处理嵌套结构
 *
 * @param s 编码字符串
 * @return 解码后的字符串
 */
std::string decodeString(std::string s) {
    // TODO: 实现你的解法
    return "";
}

/**
 * 题目9: 每日温度
 *
 * 给定一个整数数组 temperatures，表示每天的温度，返回一个数组 answer，
 * 其中 answer[i] 是指对于第 i 天，下一个更高温度出现在几天后。
 * 如果气温在这之后都不会升高，请在该位置用 0 来代替。
 *
 * 示例:
 *   输入: temperatures = [73,74,75,71,69,72,76,73]
 *   输出: [1,1,4,2,1,1,0,0]
 *
 *   输入: temperatures = [30,40,50,60]
 *   输出: [1,1,1,0]
 *
 * 思路: 单调栈
 *
 * @param temperatures 温度数组
 * @return 等待天数数组
 */
std::vector<int> dailyTemperatures(std::vector<int>& temperatures) {
    // TODO: 实现你的解法
    return {};
}

/**
 * 题目10: 验证栈序列
 *
 * 给定 pushed 和 popped 两个序列，每个序列中的值都不重复，
 * 只有当它们可能是在最初空栈上进行的推入 push 和弹出 pop 操作序列的结果时，返回 true；
 * 否则，返回 false。
 *
 * 示例:
 *   输入: pushed = [1,2,3,4,5], popped = [4,5,3,2,1]
 *   输出: true
 *   解释: 我们可以按以下顺序执行：
 *         push(1), push(2), push(3), push(4), pop() -> 4,
 *         push(5), pop() -> 5, pop() -> 3, pop() -> 2, pop() -> 1
 *
 *   输入: pushed = [1,2,3,4,5], popped = [4,3,5,1,2]
 *   输出: false
 *   解释: 1 不能在 2 之前弹出
 *
 * @param pushed 入栈序列
 * @param popped 出栈序列
 * @return 是否合法
 */
bool validateStackSequences(std::vector<int>& pushed, std::vector<int>& popped) {
    // TODO: 实现你的解法
    return false;
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
    std::cout << "=== Stack Implementation Tests ===\n";

    // 取消注释以运行测试（实现解法后）
    // testMyQueue();
    // testMyStack();
    // testMinStack();
    // testIsValid();
    // testCalculate();
    // testEvalRPN();
    // testSimplifyPath();
    // testDecodeString();
    // testDailyTemperatures();
    // testValidateStackSequences();

    std::cout << "All tests passed!\n";
    return 0;
}
