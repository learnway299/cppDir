/**
 * @file generate_parentheses.h
 * @brief 生成括号 - 声明
 */
#ifndef GENERATE_PARENTHESES_H
#define GENERATE_PARENTHESES_H

#include <vector>
#include <string>

namespace GenerateParentheses {

// 面试者实现的函数
std::vector<std::string> generateParenthesis(int n);
bool isValid(const std::string& s);

// 参考答案函数
std::vector<std::string> generateParenthesisSolution(int n);
bool isValidSolution(const std::string& s);

// 测试函数
void runTests();

} // namespace GenerateParentheses

#endif // GENERATE_PARENTHESES_H
