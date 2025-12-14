/**
 * @file monotonic_queue_solution.cpp
 * @brief 单调队列问题 - 解答文件
 */

#include "monotonic_queue.h"
#include <iostream>
#include <set>
#include <climits>
#include <cmath>
#include <cassert>

namespace MonotonicQueueProblems {

// 题目1: 滑动窗口最大值
std::vector<int> maxSlidingWindowSolution(std::vector<int>& nums, int k) {
    std::deque<int> dq;
    std::vector<int> result;

    for (int i = 0; i < static_cast<int>(nums.size()); ++i) {
        while (!dq.empty() && dq.front() <= i - k) {
            dq.pop_front();
        }

        while (!dq.empty() && nums[dq.back()] < nums[i]) {
            dq.pop_back();
        }

        dq.push_back(i);

        if (i >= k - 1) {
            result.push_back(nums[dq.front()]);
        }
    }

    return result;
}

// 题目2: 滑动窗口最小值
std::vector<int> minSlidingWindowSolution(std::vector<int>& nums, int k) {
    std::deque<int> dq;
    std::vector<int> result;

    for (int i = 0; i < static_cast<int>(nums.size()); ++i) {
        while (!dq.empty() && dq.front() <= i - k) {
            dq.pop_front();
        }

        while (!dq.empty() && nums[dq.back()] > nums[i]) {
            dq.pop_back();
        }

        dq.push_back(i);

        if (i >= k - 1) {
            result.push_back(nums[dq.front()]);
        }
    }

    return result;
}

// 题目3: 跳跃游戏 VI
int maxResultSolution(std::vector<int>& nums, int k) {
    int n = static_cast<int>(nums.size());
    std::deque<int> dq;

    std::vector<int> dp(n);
    dp[0] = nums[0];
    dq.push_back(0);

    for (int i = 1; i < n; ++i) {
        while (!dq.empty() && dq.front() < i - k) {
            dq.pop_front();
        }

        dp[i] = dp[dq.front()] + nums[i];

        while (!dq.empty() && dp[dq.back()] <= dp[i]) {
            dq.pop_back();
        }

        dq.push_back(i);
    }

    return dp[n - 1];
}

// 题目4: 绝对差不超过限制的最长连续子数组
int longestSubarraySolution(std::vector<int>& nums, int limit) {
    std::deque<int> maxDq, minDq;
    int left = 0, result = 0;

    for (int right = 0; right < static_cast<int>(nums.size()); ++right) {
        while (!maxDq.empty() && nums[maxDq.back()] < nums[right]) {
            maxDq.pop_back();
        }
        maxDq.push_back(right);

        while (!minDq.empty() && nums[minDq.back()] > nums[right]) {
            minDq.pop_back();
        }
        minDq.push_back(right);

        while (nums[maxDq.front()] - nums[minDq.front()] > limit) {
            if (maxDq.front() == left) maxDq.pop_front();
            if (minDq.front() == left) minDq.pop_front();
            ++left;
        }

        result = std::max(result, right - left + 1);
    }

    return result;
}

// 题目5: 和至少为 K 的最短子数组
int shortestSubarraySolution(std::vector<int>& nums, int k) {
    int n = static_cast<int>(nums.size());
    std::vector<long long> prefix(n + 1, 0);

    for (int i = 0; i < n; ++i) {
        prefix[i + 1] = prefix[i] + nums[i];
    }

    std::deque<int> dq;
    int result = INT_MAX;

    for (int i = 0; i <= n; ++i) {
        while (!dq.empty() && prefix[i] - prefix[dq.front()] >= k) {
            result = std::min(result, i - dq.front());
            dq.pop_front();
        }

        while (!dq.empty() && prefix[dq.back()] >= prefix[i]) {
            dq.pop_back();
        }

        dq.push_back(i);
    }

    return result == INT_MAX ? -1 : result;
}

// 题目6: 带限制的子序列和
int constrainedSubsetSumSolution(std::vector<int>& nums, int k) {
    int n = static_cast<int>(nums.size());
    std::vector<int> dp(n);
    std::deque<int> dq;

    int result = INT_MIN;

    for (int i = 0; i < n; ++i) {
        while (!dq.empty() && dq.front() < i - k) {
            dq.pop_front();
        }

        dp[i] = nums[i];
        if (!dq.empty() && dp[dq.front()] > 0) {
            dp[i] += dp[dq.front()];
        }

        result = std::max(result, dp[i]);

        while (!dq.empty() && dp[dq.back()] <= dp[i]) {
            dq.pop_back();
        }

        dq.push_back(i);
    }

    return result;
}

// 题目7: 实现单调队列类（解答版本）
class MonotonicQueueSolutionImpl {
private:
    std::deque<int> data;

public:
    MonotonicQueueSolutionImpl() {}

    void push(int x) {
        while (!data.empty() && data.back() < x) {
            data.pop_back();
        }
        data.push_back(x);
    }

    void pop(int x) {
        if (!data.empty() && data.front() == x) {
            data.pop_front();
        }
    }

    int max() {
        return data.front();
    }
};

MonotonicQueueSolution::MonotonicQueueSolution() {}
void MonotonicQueueSolution::push(int x) {}
void MonotonicQueueSolution::pop(int x) {}
int MonotonicQueueSolution::max() { return 0; }

// 题目8: 滑动窗口中位数
std::vector<double> medianSlidingWindowSolution(std::vector<int>& nums, int k) {
    std::multiset<int> small, large;
    std::vector<double> result;

    auto balance = [&]() {
        while (small.size() > large.size() + 1) {
            large.insert(*small.rbegin());
            small.erase(std::prev(small.end()));
        }
        while (large.size() > small.size()) {
            small.insert(*large.begin());
            large.erase(large.begin());
        }
    };

    auto getMedian = [&]() -> double {
        if (k % 2 == 1) {
            return *small.rbegin();
        } else {
            return ((double)*small.rbegin() + *large.begin()) / 2.0;
        }
    };

    for (int i = 0; i < static_cast<int>(nums.size()); ++i) {
        if (small.empty() || nums[i] <= *small.rbegin()) {
            small.insert(nums[i]);
        } else {
            large.insert(nums[i]);
        }
        balance();

        if (i >= k - 1) {
            result.push_back(getMedian());

            int toRemove = nums[i - k + 1];
            if (small.count(toRemove)) {
                small.erase(small.find(toRemove));
            } else {
                large.erase(large.find(toRemove));
            }
            balance();
        }
    }

    return result;
}

// ==================== 测试函数 ====================

void testMonotonicQueueSolution() {
    std::cout << "=== Monotonic Queue Tests (Solution) ===\n";

    // maxSlidingWindow
    std::vector<int> nums1 = {1, 3, -1, -3, 5, 3, 6, 7};
    std::vector<int> expected1 = {3, 3, 5, 5, 6, 7};
    assert(maxSlidingWindowSolution(nums1, 3) == expected1);
    std::cout << "  maxSlidingWindow: PASSED\n";

    // minSlidingWindow
    std::vector<int> nums2 = {1, 3, -1, -3, 5, 3, 6, 7};
    std::vector<int> expected2 = {-1, -3, -3, -3, 3, 3};
    assert(minSlidingWindowSolution(nums2, 3) == expected2);
    std::cout << "  minSlidingWindow: PASSED\n";

    // maxResult
    std::vector<int> nums3 = {1, -1, -2, 4, -7, 3};
    assert(maxResultSolution(nums3, 2) == 7);
    std::vector<int> nums4 = {10, -5, -2, 4, 0, 3};
    assert(maxResultSolution(nums4, 3) == 17);
    std::cout << "  maxResult: PASSED\n";

    // longestSubarray
    std::vector<int> nums5 = {8, 2, 4, 7};
    assert(longestSubarraySolution(nums5, 4) == 2);
    std::vector<int> nums6 = {10, 1, 2, 4, 7, 2};
    assert(longestSubarraySolution(nums6, 5) == 4);
    std::cout << "  longestSubarray: PASSED\n";

    // shortestSubarray
    std::vector<int> nums7 = {1};
    assert(shortestSubarraySolution(nums7, 1) == 1);
    std::vector<int> nums8 = {1, 2};
    assert(shortestSubarraySolution(nums8, 4) == -1);
    std::vector<int> nums9 = {2, -1, 2};
    assert(shortestSubarraySolution(nums9, 3) == 3);
    std::cout << "  shortestSubarray: PASSED\n";

    // constrainedSubsetSum
    std::vector<int> nums10 = {10, 2, -10, 5, 20};
    assert(constrainedSubsetSumSolution(nums10, 2) == 37);
    std::vector<int> nums11 = {-1, -2, -3};
    assert(constrainedSubsetSumSolution(nums11, 1) == -1);
    std::cout << "  constrainedSubsetSum: PASSED\n";

    // MonotonicQueue
    MonotonicQueueSolutionImpl mq;
    mq.push(1);
    mq.push(3);
    mq.push(2);
    assert(mq.max() == 3);
    mq.pop(1);
    assert(mq.max() == 3);
    mq.pop(3);
    assert(mq.max() == 2);
    std::cout << "  MonotonicQueue: PASSED\n";

    // medianSlidingWindow
    std::vector<int> nums12 = {1, 3, -1, -3, 5, 3, 6, 7};
    std::vector<double> expected3 = {1.0, -1.0, -1.0, 3.0, 5.0, 6.0};
    auto medianResult = medianSlidingWindowSolution(nums12, 3);
    for (size_t i = 0; i < expected3.size(); ++i) {
        assert(std::abs(medianResult[i] - expected3[i]) < 1e-5);
    }
    std::cout << "  medianSlidingWindow: PASSED\n";

    std::cout << "All Monotonic Queue tests passed!\n";
}

} // namespace MonotonicQueueProblems
