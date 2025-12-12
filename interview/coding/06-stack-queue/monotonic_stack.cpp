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

#include <iostream>
#include <vector>
#include <stack>
#include <string>

/**
 * 题目1: 下一个更大元素 I
 *
 * nums1 中数字 x 的下一个更大元素是指 x 在 nums2 中对应位置右侧的第一个比 x 大的元素。
 * 给你两个没有重复元素的数组 nums1 和 nums2，其中 nums1 是 nums2 的子集。
 * 请你找出 nums1 中每个元素在 nums2 中的下一个更大元素。
 * 如果不存在，对应位置输出 -1。
 *
 * 示例:
 *   输入: nums1 = [4,1,2], nums2 = [1,3,4,2]
 *   输出: [-1,3,-1]
 *   解释:
 *     对于 nums1 中的 4，在 nums2 中找不到下一个更大元素，所以是 -1
 *     对于 nums1 中的 1，在 nums2 中下一个更大元素是 3
 *     对于 nums1 中的 2，在 nums2 中找不到下一个更大元素，所以是 -1
 *
 * @param nums1 查询数组
 * @param nums2 完整数组
 * @return 下一个更大元素数组
 */
std::vector<int> nextGreaterElement(std::vector<int>& nums1, std::vector<int>& nums2) {
    // TODO: 实现你的解法
    return {};
}

/**
 * 题目2: 下一个更大元素 II（循环数组）
 *
 * 给定一个循环数组 nums（nums[nums.length - 1] 的下一个元素是 nums[0]），
 * 返回 nums 中每个元素的下一个更大元素。
 *
 * 数字 x 的下一个更大的元素是按数组遍历顺序，这个数字之后的第一个比它更大的数，
 * 这意味着你应该循环地搜索它的下一个更大的数。如果不存在，则输出 -1。
 *
 * 示例:
 *   输入: nums = [1,2,1]
 *   输出: [2,-1,2]
 *   解释: 第一个 1 的下一个更大的数是 2；
 *         数字 2 找不到下���个更大的数；
 *         第二个 1 的下一个更大的数需要循环搜索，结果是 2。
 *
 *   输入: nums = [1,2,3,4,3]
 *   输出: [2,3,4,-1,4]
 *
 * 思路: 遍历两遍数组或使用取模
 *
 * @param nums 循环数组
 * @return 下一个更大元素数组
 */
std::vector<int> nextGreaterElements(std::vector<int>& nums) {
    // TODO: 实现你的解法
    return {};
}

/**
 * 题目3: 每日温度
 *
 * 给定一个整数数组 temperatures，表示每天的温度，返回一个数组 answer，
 * 其中 answer[i] 是指对于第 i 天，下一个更高温度出现在几天后。
 * 如果气温在这之后都不会升高，请用 0 代替。
 *
 * 示例:
 *   输入: temperatures = [73,74,75,71,69,72,76,73]
 *   输出: [1,1,4,2,1,1,0,0]
 *
 * @param temperatures 温度数组
 * @return 等待天数数组
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
 *   解释: 最大的矩形为图中红色区域，面积为 10
 *
 *   输入: heights = [2,4]
 *   输出: 4
 *
 * 思路: 单调递增栈，找每个柱子左右第一个更小的位置
 *
 * @param heights 柱子高度数组
 * @return 最大矩形面积
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
 *
 * 思路: 将每一行作为柱状图的底部，转化为柱状图最大矩形问题
 *
 * @param matrix 二维矩阵
 * @return 最大矩形面积
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
 *
 *   输入: height = [4,2,0,3,2,5]
 *   输出: 9
 *
 * 思路:
 *   1. 双指针
 *   2. 单调栈（按行计算）
 *   3. 动态规划（预处理左右最大高度）
 *
 * @param height 高度数组
 * @return 能接的雨水量
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
 *
 *   输入: s = "cbacdcbc"
 *   输出: "acdb"
 *
 * 思路: 单调栈 + 贪心
 *
 * @param s 输入字符串
 * @return 去重后的字符串
 */
std::string removeDuplicateLetters(std::string s) {
    // TODO: 实现你的解法
    return "";
}

/**
 * 题目8: 股票价格跨度
 *
 * 编写一个 StockSpanner 类，它收集某些股票的每日报价，并返回该股票当日价格的跨度。
 *
 * 今天股票价格的跨度被定义为股票价格小于或等于今天价格的最大连续日数（从今天开始往回数，包括今天）。
 *
 * 示例:
 *   StockSpanner stockSpanner = new StockSpanner();
 *   stockSpanner.next(100); // 返回 1
 *   stockSpanner.next(80);  // 返回 1
 *   stockSpanner.next(60);  // 返回 1
 *   stockSpanner.next(70);  // 返回 2
 *   stockSpanner.next(60);  // 返回 1
 *   stockSpanner.next(75);  // 返回 4
 *   stockSpanner.next(85);  // 返回 6
 */
class StockSpanner {
public:
    StockSpanner() {
        // TODO: 初始化
    }

    int next(int price) {
        // TODO: 实现
        return 0;
    }
};

/**
 * 题目9: 132 模式
 *
 * 给你一个整数数组 nums，数组中共有 n 个整数。
 * 132 模式的子序列由三个整数 nums[i]、nums[j] 和 nums[k] 组成，
 * 并同时满足：i < j < k 且 nums[i] < nums[k] < nums[j]。
 *
 * 如果 nums 中存在 132 模式的子序列，返回 true；否则返回 false。
 *
 * 示例:
 *   输入: nums = [1,2,3,4]
 *   输出: false
 *
 *   输入: nums = [3,1,4,2]
 *   输出: true
 *   解释: 子序列 [1, 4, 2] 满足 1 < 2 < 4
 *
 *   输入: nums = [-1,3,2,0]
 *   输出: true
 *   解释: 子序列 [-1, 3, 2] 或 [-1, 3, 0] 或 [-1, 2, 0]
 *
 * 思路: 单调栈从右向左遍历
 *
 * @param nums 整数数组
 * @return 是否存在 132 模式
 */
bool find132pattern(std::vector<int>& nums) {
    // TODO: 实现你的解法
    return false;
}

/**
 * 题目10: 移掉 K 位数字
 *
 * 给你一个以字符串表示的非负整数 num 和一个整数 k，
 * 移除这个数中的 k 位数字，使得剩下的数字最小。请你以字符串形式返回这个最小的数字。
 *
 * 示例:
 *   输入: num = "1432219", k = 3
 *   输出: "1219"
 *   解释: 移除 4, 3, 2，得到最小数字 1219
 *
 *   输入: num = "10200", k = 1
 *   输出: "200"
 *   解释: 移掉首位的 1，剩下的数字为 200
 *
 *   输入: num = "10", k = 2
 *   输出: "0"
 *
 * 思路: 单调递增栈 + 贪心
 *
 * @param num 数字字符串
 * @param k 要移除的位数
 * @return 最小数字字符串
 */
std::string removeKdigits(std::string num, int k) {
    // TODO: 实现你的解法
    return "";
}


// ==================== 测试代码 ====================
#include <cassert>

void testNextGreaterElement() {
    std::vector<int> nums1 = {4, 1, 2};
    std::vector<int> nums2 = {1, 3, 4, 2};
    std::vector<int> expected = {-1, 3, -1};
    assert(nextGreaterElement(nums1, nums2) == expected);

    std::cout << "nextGreaterElement: PASSED\n";
}

void testNextGreaterElements() {
    std::vector<int> nums = {1, 2, 1};
    std::vector<int> expected = {2, -1, 2};
    assert(nextGreaterElements(nums) == expected);

    std::cout << "nextGreaterElements: PASSED\n";
}

void testDailyTemperatures() {
    std::vector<int> temps = {73, 74, 75, 71, 69, 72, 76, 73};
    std::vector<int> expected = {1, 1, 4, 2, 1, 1, 0, 0};
    assert(dailyTemperatures(temps) == expected);

    std::cout << "dailyTemperatures: PASSED\n";
}

void testLargestRectangleArea() {
    std::vector<int> heights = {2, 1, 5, 6, 2, 3};
    assert(largestRectangleArea(heights) == 10);

    std::cout << "largestRectangleArea: PASSED\n";
}

void testMaximalRectangle() {
    std::vector<std::vector<char>> matrix = {
        {'1','0','1','0','0'},
        {'1','0','1','1','1'},
        {'1','1','1','1','1'},
        {'1','0','0','1','0'}
    };
    assert(maximalRectangle(matrix) == 6);

    std::cout << "maximalRectangle: PASSED\n";
}

void testTrap() {
    std::vector<int> height = {0, 1, 0, 2, 1, 0, 1, 3, 2, 1, 2, 1};
    assert(trap(height) == 6);

    std::cout << "trap: PASSED\n";
}

void testRemoveDuplicateLetters() {
    assert(removeDuplicateLetters("bcabc") == "abc");
    assert(removeDuplicateLetters("cbacdcbc") == "acdb");

    std::cout << "removeDuplicateLetters: PASSED\n";
}

void testStockSpanner() {
    StockSpanner stockSpanner;
    assert(stockSpanner.next(100) == 1);
    assert(stockSpanner.next(80) == 1);
    assert(stockSpanner.next(60) == 1);
    assert(stockSpanner.next(70) == 2);
    assert(stockSpanner.next(60) == 1);
    assert(stockSpanner.next(75) == 4);
    assert(stockSpanner.next(85) == 6);

    std::cout << "StockSpanner: PASSED\n";
}

void testFind132pattern() {
    std::vector<int> nums1 = {1, 2, 3, 4};
    assert(find132pattern(nums1) == false);

    std::vector<int> nums2 = {3, 1, 4, 2};
    assert(find132pattern(nums2) == true);

    std::cout << "find132pattern: PASSED\n";
}

void testRemoveKdigits() {
    assert(removeKdigits("1432219", 3) == "1219");
    assert(removeKdigits("10200", 1) == "200");
    assert(removeKdigits("10", 2) == "0");

    std::cout << "removeKdigits: PASSED\n";
}

int main() {
    std::cout << "=== Monotonic Stack Tests ===\n";

    // 取消注释以运行测试（实现解法后）
    // testNextGreaterElement();
    // testNextGreaterElements();
    // testDailyTemperatures();
    // testLargestRectangleArea();
    // testMaximalRectangle();
    // testTrap();
    // testRemoveDuplicateLetters();
    // testStockSpanner();
    // testFind132pattern();
    // testRemoveKdigits();

    std::cout << "All tests passed!\n";
    return 0;
}
