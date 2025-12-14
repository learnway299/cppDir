/**
 * @file generate_parentheses_solution.cpp
 * @brief 生成括号 - 参考答案
 */
#include "generate_parentheses.h"
#include <stack>
#include <iostream>
#include <cassert>

namespace GenerateParentheses {

// ==================== 辅助函数 ====================
static void backtrack(std::vector<std::string>& result, std::string& path,
                       int open, int close, int n) {
    if (static_cast<int>(path.size()) == 2 * n) {
        result.push_back(path);
        return;
    }
    if (open < n) {
        path.push_back('(');
        backtrack(result, path, open + 1, close, n);
        path.pop_back();
    }
    if (close < open) {
        path.push_back(')');
        backtrack(result, path, open, close + 1, n);
        path.pop_back();
    }
}

// ==================== 参考答案实现 ====================

std::vector<std::string> generateParenthesisSolution(int n) {
    std::vector<std::string> result;
    std::string path;
    backtrack(result, path, 0, 0, n);
    return result;
}

bool isValidSolution(const std::string& s) {
    std::stack<char> st;
    for (char c : s) {
        if (c == '(' || c == '[' || c == '{') {
            st.push(c);
        } else {
            if (st.empty()) return false;
            char top = st.top();
            if ((c == ')' && top != '(') ||
                (c == ']' && top != '[') ||
                (c == '}' && top != '{')) return false;
            st.pop();
        }
    }
    return st.empty();
}

// ==================== 测试函数 ====================

void runTests() {
    std::cout << "\n=== Generate Parentheses Tests ===" << std::endl;

    // 测试生成括号
    {
        auto result = generateParenthesisSolution(3);
        assert(result.size() == 5);  // 卡特兰数 C(3) = 5

        // 验证所有生成的括号都是有效的
        for (const auto& s : result) {
            assert(isValidSolution(s));
        }
        std::cout << "  [PASS] Generate Parentheses (n=3)" << std::endl;
    }

    // 测试括号有效性
    {
        assert(isValidSolution("()") == true);
        assert(isValidSolution("()[]{}") == true);
        assert(isValidSolution("(]") == false);
        assert(isValidSolution("([)]") == false);
        assert(isValidSolution("{[]}") == true);
        std::cout << "  [PASS] Valid Parentheses" << std::endl;
    }

    std::cout << "=== All Generate Parentheses Tests Passed! ===" << std::endl;
}

} // namespace GenerateParentheses

/**
 * 关键要点：
 *
 * 1. 生成括号规则：
 *    - 左括号数量 <= n
 *    - 右括号数量 <= 左括号数量
 *    - 保证任意前缀中左括号 >= 右括号
 *
 * 2. 回溯条件：
 *    - open < n 时可以添加左括号
 *    - close < open 时可以添加右括号
 *    - path.size() == 2n 时记录结果
 *
 * 3. 卡特兰数：
 *    - n 对括号的有效组合数 = C(2n,n)/(n+1)
 *    - n=1: 1, n=2: 2, n=3: 5, n=4: 14
 *
 * 4. 有效括号判断：
 *    - 使用栈匹配括号
 *    - 左括号入栈
 *    - 右括号与栈顶匹配
 */
