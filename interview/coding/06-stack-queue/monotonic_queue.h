/**
 * @file monotonic_queue.h
 * @brief 单调队列问题 - 函数声明
 */

#ifndef MONOTONIC_QUEUE_H
#define MONOTONIC_QUEUE_H

#include <vector>
#include <deque>
#include "../../util/Util.h"

namespace MonotonicQueueProblems {

// 题目1: 滑动窗口最大值
std::vector<int> maxSlidingWindow(std::vector<int>& nums, int k);
std::vector<int> maxSlidingWindowSolution(std::vector<int>& nums, int k);

// 题目2: 滑动窗口最小值
std::vector<int> minSlidingWindow(std::vector<int>& nums, int k);
std::vector<int> minSlidingWindowSolution(std::vector<int>& nums, int k);

// 题目3: 跳跃游戏 VI
int maxResult(std::vector<int>& nums, int k);
int maxResultSolution(std::vector<int>& nums, int k);

// 题目4: 绝对差不超过限制的最长连续子数组
int longestSubarray(std::vector<int>& nums, int limit);
int longestSubarraySolution(std::vector<int>& nums, int limit);

// 题目5: 和至少为 K 的最短子数组
int shortestSubarray(std::vector<int>& nums, int k);
int shortestSubarraySolution(std::vector<int>& nums, int k);

// 题目6: 带限制的子序列和
int constrainedSubsetSum(std::vector<int>& nums, int k);
int constrainedSubsetSumSolution(std::vector<int>& nums, int k);

// 题目7: 实现单调队列类
class MonotonicQueue {
public:
    MonotonicQueue();
    void push(int x);
    void pop(int x);
    int max();
};

class MonotonicQueueSolution {
public:
    MonotonicQueueSolution();
    void push(int x);
    void pop(int x);
    int max();
};

// 题目8: 滑动窗口中位数
std::vector<double> medianSlidingWindow(std::vector<int>& nums, int k);
std::vector<double> medianSlidingWindowSolution(std::vector<int>& nums, int k);

// ==================== 测试函数声明 ====================

void testMonotonicQueue();          // 测试面试者实现
void testMonotonicQueueSolution();  // 测试解答实现

// runTests 在头文件 inline 定义，调用两个测试函数
inline void runTests() {
    testMonotonicQueue();
    testMonotonicQueueSolution();
}

} // namespace MonotonicQueueProblems

#endif // MONOTONIC_QUEUE_H
