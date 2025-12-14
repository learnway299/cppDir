/**
 * @file monotonic_stack.cpp
 * @brief 单调栈问题 - 题目文件
 *
 * 单调栈是一种特殊的栈结构，栈内元素保持单调递增或单调递减。
 * 常用于解决"下一个更大/更小元素"类问题。
 *
 * 核心思想：
 * - 单调递增栈：找下一个更大元素
 * - 单调递减栈：找下一个更小元素
 */

#include "monotonic_stack.h"
#include <iostream>

namespace MonotonicStackProblems {

/**
 * 题目1: 下一个更大元素 I
 *
 * nums1 中数字 x 的下一个更大元素是指 x 在 nums2 中对应位置右侧的第一个比 x 大的元素。
 *
 * 示例:
 *   输入: nums1 = [4,1,2], nums2 = [1,3,4,2]
 *   输出: [-1,3,-1]
 */
std::vector<int> nextGreaterElement(std::vector<int>& nums1, std::vector<int>& nums2) {
    // TODO: 实现你的解法
    return {};
}

/**
 * 题目2: 下一个更大元素 II（循环数组）
 *
 * 给定一个循环数组 nums，返回 nums 中每个元素的下一个更大元素。
 *
 * 示例:
 *   输入: nums = [1,2,1]
 *   输出: [2,-1,2]
 */
std::vector<int> nextGreaterElements(std::vector<int>& nums) {
    // TODO: 实现你的解法
    return {};
}

/**
 * 题目3: 每日温度
 *
 * 给定一个整数数组 temperatures，返回一个数组 answer，
 * 其中 answer[i] 是指对于第 i 天，下一个更高温度出现在几天后。
 *
 * 示例:
 *   输入: temperatures = [73,74,75,71,69,72,76,73]
 *   输出: [1,1,4,2,1,1,0,0]
 */
std::vector<int> dailyTemperatures(std::vector<int>& temperatures) {
    // TODO: 实现你的解法
    return {};
}

/**
 * 题目4: 柱状图中最大的矩形
 *
 * 给定 n 个非负整数，用来表示柱状图中各个柱子的高度。每个柱子彼此相邻，且宽度为 1。
 * 求在该柱状图中，能够勾勒出来的矩形的最大面积。
 *
 * 示例:
 *   输入: heights = [2,1,5,6,2,3]
 *   输出: 10
 */
int largestRectangleArea(std::vector<int>& heights) {
    // TODO: 实现你的解法
    return 0;
}

/**
 * 题目5: 最大矩形
 *
 * 给定一个仅包含 0 和 1、大小为 rows x cols 的二维二进制矩阵，
 * 找出只包含 1 的最大矩形，并返回其面积。
 *
 * 示例:
 *   输入: matrix = [["1","0","1","0","0"],
 *                   ["1","0","1","1","1"],
 *                   ["1","1","1","1","1"],
 *                   ["1","0","0","1","0"]]
 *   输出: 6
 */
int maximalRectangle(std::vector<std::vector<char>>& matrix) {
    // TODO: 实现你的解法
    return 0;
}

/**
 * 题目6: 接雨水
 *
 * 给定 n 个非负整数表示每个宽度为 1 的柱子的高度图，计算按此排列的柱子，下雨之后能接多少雨水。
 *
 * 示例:
 *   输入: height = [0,1,0,2,1,0,1,3,2,1,2,1]
 *   输出: 6
 */
int trap(std::vector<int>& height) {
    // TODO: 实现你的解法
    return 0;
}

/**
 * 题目7: 去除重复字母
 *
 * 给你一个字符串 s，请你去除字符串中重复的字母，使得每个字母只出现一次。
 * 需保证返回结果的字典序最小（要求不能打乱其他字符的相对位置）。
 *
 * 示例:
 *   输入: s = "bcabc"
 *   输出: "abc"
 */
std::string removeDuplicateLetters(std::string s) {
    // TODO: 实现你的解法
    return "";
}

/**
 * 题目8: 股票价格跨度
 *
 * 编写一个 StockSpanner 类，它收集某些股票的每日报价，并返回该股票当日价格的跨度。
 */
StockSpanner::StockSpanner() {
    // TODO: 初始化
}

int StockSpanner::next(int price) {
    // TODO: 实现
    return 0;
}

/**
 * 题目9: 132 模式
 *
 * 给你一个整数数组 nums，数组中共有 n 个整数。
 * 132 模式的子序列由三个整数 nums[i]、nums[j] 和 nums[k] 组成，
 * 并同时满足：i < j < k 且 nums[i] < nums[k] < nums[j]。
 *
 * 示例:
 *   输入: nums = [3,1,4,2]
 *   输出: true
 */
bool find132pattern(std::vector<int>& nums) {
    // TODO: 实现你的解法
    return false;
}

/**
 * 题目10: 移掉 K 位数字
 *
 * 给你一个以字符串表示的非负整数 num 和一个整数 k，
 * 移除这个数中的 k 位数字，使得剩下的数字最小。
 *
 * 示例:
 *   输入: num = "1432219", k = 3
 *   输出: "1219"
 */
std::string removeKdigits(std::string num, int k) {
    // TODO: 实现你的解法
    return "";
}

// ==================== 测试函数 ====================

void testMonotonicStack() {
    std::cout << "=== Monotonic Stack Tests (User Implementation) ===\n";
    // TODO: 添加面试者实现的测试
    std::cout << "  (No tests yet - implement your solutions first)\n";
}

} // namespace MonotonicStackProblems
