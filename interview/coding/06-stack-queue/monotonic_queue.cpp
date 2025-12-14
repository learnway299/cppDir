/**
 * @file monotonic_queue.cpp
 * @brief 单调队列问题 - 题目文件
 *
 * 单调队列是一种特殊的双端队列，队列中的元素保持单调递增或单调递减。
 * 常用于滑动窗口最值问题，支持 O(1) 时间获取窗口最大/最小值。
 *
 * 与单调栈的区别：
 * - 单调栈：只在一端操作
 * - 单调队列：两端都可操作（队首移除过期元素，队尾维护单调性）
 */

#include "monotonic_queue.h"
#include <iostream>

namespace MonotonicQueueProblems {

/**
 * 题目1: 滑动窗口最大值
 *
 * 给你一个整数数组 nums，有一个大小为 k 的滑动窗口从数组的最左侧移动到数组的最右侧。
 * 你只可以看到在滑动窗口内的 k 个数字。滑动窗口每次只向右移动一位。
 * 返回滑动窗口中的最大值。
 *
 * 示例:
 *   输入: nums = [1,3,-1,-3,5,3,6,7], k = 3
 *   输出: [3,3,5,5,6,7]
 *
 * 思路: 单调递减队列
 */
std::vector<int> maxSlidingWindow(std::vector<int>& nums, int k) {
    // TODO: 实现你的解法
    return {};
}

/**
 * 题目2: 滑动窗口最小值
 *
 * 与题目1类似，返回滑动窗口中的最小值。
 *
 * 示例:
 *   输入: nums = [1,3,-1,-3,5,3,6,7], k = 3
 *   输出: [-1,-3,-3,-3,3,3]
 */
std::vector<int> minSlidingWindow(std::vector<int>& nums, int k) {
    // TODO: 实现你的解法
    return {};
}

/**
 * 题目3: 跳跃游戏 VI
 *
 * 给你一个下标从 0 开始的整数数组 nums 和一个整数 k。
 * 一开始你在下标 0 处。每一步，你最多可以往前跳 k 步，但你不能跳出数组的边界。
 * 返回你能得到的最大得分。
 *
 * 示例:
 *   输入: nums = [1,-1,-2,4,-7,3], k = 2
 *   输出: 7
 *
 * 思路: 单调递减队列优化 DP
 */
int maxResult(std::vector<int>& nums, int k) {
    // TODO: 实现你的解法
    return 0;
}

/**
 * 题目4: 绝对差不超过限制的最长连续子数组
 *
 * 给你一个整数数组 nums，和一个表示限制的整数 limit，
 * 请你返回最长连续子数组的长度，该子数组中任意两个元素之间的绝对差必须小于或等于 limit。
 *
 * 示例:
 *   输入: nums = [8,2,4,7], limit = 4
 *   输出: 2
 *
 * 思路: 滑动窗口 + 两个单调队列（最大值和最小值）
 */
int longestSubarray(std::vector<int>& nums, int limit) {
    // TODO: 实现你的解法
    return 0;
}

/**
 * 题目5: 和至少为 K 的最短子数组
 *
 * 给你一个整数数组 nums 和一个整数 k，找出 nums 中和至少为 k 的最短非空子数组，
 * 并返回该子数组的长度。如果不存在这样的子数组，返回 -1。
 *
 * 示例:
 *   输入: nums = [2,-1,2], k = 3
 *   输出: 3
 *
 * 思路: 前缀和 + 单调递增队列
 */
int shortestSubarray(std::vector<int>& nums, int k) {
    // TODO: 实现你的解法
    return -1;
}

/**
 * 题目6: 带限制的子序列和
 *
 * 给你一个整数数组 nums 和一个整数 k，请你返回非空子序列元素和的最大值，
 * 子序列需要满足：子序列中每两个相邻的整数 nums[i] 和 nums[j]，
 * 它们在原数组中的下标 i 和 j 满足 i < j 且 j - i <= k。
 *
 * 示例:
 *   输入: nums = [10,2,-10,5,20], k = 2
 *   输出: 37
 *
 * 思路: 与跳跃游戏 VI 类似，单调队列优化 DP
 */
int constrainedSubsetSum(std::vector<int>& nums, int k) {
    // TODO: 实现你的解法
    return 0;
}

/**
 * 题目7: 实现单调队列类
 *
 * 实现一个单调队列，支持以下操作：
 *   - push(x): 将元素 x 加入队尾
 *   - pop(x): 如果 x 是队首元素，则移除
 *   - max(): 返回队列中的最大值
 *
 * 所有操作均摊时间复杂度 O(1)
 */
MonotonicQueue::MonotonicQueue() {
    // TODO: 初始化
}

void MonotonicQueue::push(int x) {
    // TODO: 实现
}

void MonotonicQueue::pop(int x) {
    // TODO: 实现
}

int MonotonicQueue::max() {
    // TODO: 返回最大值
    return 0;
}

/**
 * 题目8: 滑动窗口中位数
 *
 * 中位数是有序序列最中间的那个数。如果序列的长度是偶数，则没有最中间的数；
 * 此时中位数是最中间的两个数的平均数。
 *
 * 示例:
 *   输入: nums = [1,3,-1,-3,5,3,6,7], k = 3
 *   输出: [1.0,-1.0,-1.0,3.0,5.0,6.0]
 *
 * 思路: 使用两个堆（大顶堆 + 小顶堆）或平衡树
 */
std::vector<double> medianSlidingWindow(std::vector<int>& nums, int k) {
    // TODO: 实现你的解法
    return {};
}

// ==================== 测试函数 ====================

void testMonotonicQueue() {
    std::cout << "=== Monotonic Queue Tests (User Implementation) ===\n";
    // TODO: 添加面试者实现的测试
    std::cout << "  (No tests yet - implement your solutions first)\n";
}

} // namespace MonotonicQueueProblems
