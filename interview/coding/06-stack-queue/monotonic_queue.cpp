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

#include <iostream>
#include <vector>
#include <deque>
#include <queue>

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
 *   解释:
 *     滑动窗口的位置                最大值
 *     [1  3  -1] -3  5  3  6  7      3
 *      1 [3  -1  -3] 5  3  6  7      3
 *      1  3 [-1  -3  5] 3  6  7      5
 *      1  3  -1 [-3  5  3] 6  7      5
 *      1  3  -1  -3 [5  3  6] 7      6
 *      1  3  -1  -3  5 [3  6  7]     7
 *
 * 思路: 单调递减队列
 *
 * @param nums 整数数组
 * @param k 窗口大小
 * @return 每个窗口的最大值
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
 *
 * @param nums 整数数组
 * @param k 窗口大小
 * @return 每个窗口的最小值
 */
std::vector<int> minSlidingWindow(std::vector<int>& nums, int k) {
    // TODO: 实现你的解法
    return {};
}

/**
 * 题目3: 跳跃游戏 VI
 *
 * 给你一个下标从 0 开始的整数数组 nums 和一个整数 k。
 *
 * 一开始你在下标 0 处。每一步，你最多可以往前跳 k 步，但你不能跳出数组的边界。
 * 你想到达数组最后一个位置（下标为 n - 1），你的得分为经过的所有数字之和。
 *
 * 返回你能得到的最大得分。
 *
 * 示例:
 *   输入: nums = [1,-1,-2,4,-7,3], k = 2
 *   输出: 7
 *   解释: 选择下标 0 -> 3 -> 5，得分为 1 + 4 + 3 = 8，但不能直接从 0 跳到 3
 *         实际路径 0 -> 1 -> 3 -> 5，得分 1 + (-1) + 4 + 3 = 7
 *
 *   输入: nums = [10,-5,-2,4,0,3], k = 3
 *   输出: 17
 *
 *   输入: nums = [1,-5,-20,4,-1,3,-6,-3], k = 2
 *   输出: 0
 *
 * 思路: 单调递减队列优化 DP
 * dp[i] = max(dp[j]) + nums[i]，其中 max(i-k, 0) <= j < i
 *
 * @param nums 整数数组
 * @param k 最大跳跃距离
 * @return 最大得分
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
 * 如果不存在满足条件的子数组，则返回 0。
 *
 * 示例:
 *   输入: nums = [8,2,4,7], limit = 4
 *   输出: 2
 *   解释: 子数组 [2,4] 和 [4,7] 的最大绝对差都是 2，满足 |2-4| <= 4 或 |4-7| <= 4
 *
 *   输入: nums = [10,1,2,4,7,2], limit = 5
 *   输出: 4
 *   解释: 子数组 [2,4,7,2] 满足条件，最大绝对差 |2-7| = 5 <= 5
 *
 *   输入: nums = [4,2,2,2,4,4,2,2], limit = 0
 *   输出: 3
 *
 * 思路: 滑动窗口 + 两个单调队列（最大值和最小值）
 *
 * @param nums 整数数组
 * @param limit 绝对差限制
 * @return 最长子数组长度
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
 * 子数组是数组中连续的一部分。
 *
 * 示例:
 *   输入: nums = [1], k = 1
 *   输出: 1
 *
 *   输入: nums = [1,2], k = 4
 *   输出: -1
 *
 *   输入: nums = [2,-1,2], k = 3
 *   输出: 3
 *
 * 注意: 数组可能包含负数！
 *
 * 思路: 前缀和 + 单调递增队列
 *
 * @param nums 整数数组
 * @param k 目标和
 * @return 最短子数组长度
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
 * 数组的子序列定义为：将数组中的若干个数字删除（可以删除 0 个数字），剩下的数字按照原本的顺序排布。
 *
 * 示例:
 *   输入: nums = [10,2,-10,5,20], k = 2
 *   输出: 37
 *   解释: 子序列为 [10, 2, 5, 20]
 *
 *   输入: nums = [-1,-2,-3], k = 1
 *   输出: -1
 *
 *   输入: nums = [10,-2,-10,-5,20], k = 2
 *   输出: 23
 *
 * 思路: 与跳跃游戏 VI 类似，单调队列优化 DP
 *
 * @param nums 整数数组
 * @param k 相邻元素的最大距离
 * @return 最大子序列和
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
 *   - pop(): 移除队首元素
 *   - max(): 返回队列中的最大值
 *
 * 所有操作均摊时间复杂度 O(1)
 */
class MonotonicQueue {
public:
    MonotonicQueue() {
        // TODO: 初始化
    }

    void push(int x) {
        // TODO: 实现
    }

    void pop(int x) {
        // TODO: 如果 x 是队首元素，则移除
    }

    int max() {
        // TODO: 返回最大值
        return 0;
    }
};

/**
 * 题目8: 滑动窗口中位数
 *
 * 中位数是有序序列最中间的那个数。如果序列的长度是偶数，则没有最中间的数；
 * 此时中位数是最中间的两个数的平均数。
 *
 * 给你一个数组 nums，有一个长度为 k 的窗口从最左端滑动到最右端。
 * 窗口中有 k 个数，每次窗口向右移动 1 位。
 * 你的任务是找出每次窗口移动后得到的新窗口中元素的中位数。
 *
 * 示例:
 *   输入: nums = [1,3,-1,-3,5,3,6,7], k = 3
 *   输出: [1.0,-1.0,-1.0,3.0,5.0,6.0]
 *   解释:
 *     窗口位置                      中位数
 *     [1  3  -1] -3  5  3  6  7       1
 *      1 [3  -1  -3] 5  3  6  7      -1
 *      1  3 [-1  -3  5] 3  6  7      -1
 *      1  3  -1 [-3  5  3] 6  7       3
 *      1  3  -1  -3 [5  3  6] 7       5
 *      1  3  -1  -3  5 [3  6  7]      6
 *
 * 思路: 使用两个堆（大顶堆 + 小顶堆）或平衡树
 *
 * @param nums 整数数组
 * @param k 窗口大小
 * @return 每个窗口的中位数
 */
std::vector<double> medianSlidingWindow(std::vector<int>& nums, int k) {
    // TODO: 实现你的解法
    return {};
}


// ==================== 测试代码 ====================
#include <cassert>
#include <cmath>

void testMaxSlidingWindow() {
    std::vector<int> nums = {1, 3, -1, -3, 5, 3, 6, 7};
    std::vector<int> expected = {3, 3, 5, 5, 6, 7};
    assert(maxSlidingWindow(nums, 3) == expected);

    std::cout << "maxSlidingWindow: PASSED\n";
}

void testMinSlidingWindow() {
    std::vector<int> nums = {1, 3, -1, -3, 5, 3, 6, 7};
    std::vector<int> expected = {-1, -3, -3, -3, 3, 3};
    assert(minSlidingWindow(nums, 3) == expected);

    std::cout << "minSlidingWindow: PASSED\n";
}

void testMaxResult() {
    std::vector<int> nums1 = {1, -1, -2, 4, -7, 3};
    assert(maxResult(nums1, 2) == 7);

    std::vector<int> nums2 = {10, -5, -2, 4, 0, 3};
    assert(maxResult(nums2, 3) == 17);

    std::cout << "maxResult: PASSED\n";
}

void testLongestSubarray() {
    std::vector<int> nums1 = {8, 2, 4, 7};
    assert(longestSubarray(nums1, 4) == 2);

    std::vector<int> nums2 = {10, 1, 2, 4, 7, 2};
    assert(longestSubarray(nums2, 5) == 4);

    std::cout << "longestSubarray: PASSED\n";
}

void testShortestSubarray() {
    std::vector<int> nums1 = {1};
    assert(shortestSubarray(nums1, 1) == 1);

    std::vector<int> nums2 = {1, 2};
    assert(shortestSubarray(nums2, 4) == -1);

    std::vector<int> nums3 = {2, -1, 2};
    assert(shortestSubarray(nums3, 3) == 3);

    std::cout << "shortestSubarray: PASSED\n";
}

void testConstrainedSubsetSum() {
    std::vector<int> nums1 = {10, 2, -10, 5, 20};
    assert(constrainedSubsetSum(nums1, 2) == 37);

    std::vector<int> nums2 = {-1, -2, -3};
    assert(constrainedSubsetSum(nums2, 1) == -1);

    std::cout << "constrainedSubsetSum: PASSED\n";
}

void testMedianSlidingWindow() {
    std::vector<int> nums = {1, 3, -1, -3, 5, 3, 6, 7};
    std::vector<double> expected = {1.0, -1.0, -1.0, 3.0, 5.0, 6.0};
    auto result = medianSlidingWindow(nums, 3);
    for (size_t i = 0; i < expected.size(); ++i) {
        assert(std::abs(result[i] - expected[i]) < 1e-5);
    }

    std::cout << "medianSlidingWindow: PASSED\n";
}

int main() {
    std::cout << "=== Monotonic Queue Tests ===\n";

    // 取消注释以运行测试（实现解法后）
    // testMaxSlidingWindow();
    // testMinSlidingWindow();
    // testMaxResult();
    // testLongestSubarray();
    // testShortestSubarray();
    // testConstrainedSubsetSum();
    // testMedianSlidingWindow();

    std::cout << "All tests passed!\n";
    return 0;
}
