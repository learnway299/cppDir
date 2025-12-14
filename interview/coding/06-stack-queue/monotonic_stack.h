/**
 * @file monotonic_stack.h
 * @brief 单调栈问题 - 函数声明
 */

#ifndef MONOTONIC_STACK_H
#define MONOTONIC_STACK_H

#include <vector>
#include <string>
#include "../../util/Util.h"

namespace MonotonicStackProblems {

// 题目1: 下一个更大元素 I
std::vector<int> nextGreaterElement(std::vector<int>& nums1, std::vector<int>& nums2);
std::vector<int> nextGreaterElementSolution(std::vector<int>& nums1, std::vector<int>& nums2);

// 题目2: 下一个更大元素 II（循环数组）
std::vector<int> nextGreaterElements(std::vector<int>& nums);
std::vector<int> nextGreaterElementsSolution(std::vector<int>& nums);

// 题目3: 每日温度
std::vector<int> dailyTemperatures(std::vector<int>& temperatures);
std::vector<int> dailyTemperaturesSolution(std::vector<int>& temperatures);

// 题目4: 柱状图中最大的矩形
int largestRectangleArea(std::vector<int>& heights);
int largestRectangleAreaSolution(std::vector<int>& heights);

// 题目5: 最大矩形
int maximalRectangle(std::vector<std::vector<char>>& matrix);
int maximalRectangleSolution(std::vector<std::vector<char>>& matrix);

// 题目6: 接雨水
int trap(std::vector<int>& height);
int trapSolution(std::vector<int>& height);

// 题目7: 去除重复字母
std::string removeDuplicateLetters(std::string s);
std::string removeDuplicateLettersSolution(std::string s);

// 题目8: 股票价格跨度
class StockSpanner {
public:
    StockSpanner();
    int next(int price);
};

class StockSpannerSolution {
public:
    StockSpannerSolution();
    int next(int price);
};

// 题目9: 132 模式
bool find132pattern(std::vector<int>& nums);
bool find132patternSolution(std::vector<int>& nums);

// 题目10: 移掉 K 位数字
std::string removeKdigits(std::string num, int k);
std::string removeKdigitsSolution(std::string num, int k);

// ==================== 测试函数声明 ====================

void testMonotonicStack();          // 测试面试者实现
void testMonotonicStackSolution();  // 测试解答实现

// runTests 在头文件 inline 定义，调用两个测试函数
inline void runTests() {
    testMonotonicStack();
    testMonotonicStackSolution();
}

} // namespace MonotonicStackProblems

#endif // MONOTONIC_STACK_H
