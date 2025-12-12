/**
 * @file monotonic_queue_solution.cpp
 * @brief 单调队列问题 - 解答文件
 *
 * 单调队列核心操作：
 * 1. 队尾入队：维护单调性，移除违反单调性的元素
 * 2. 队首出队：移除过期元素
 * 3. 获取最值：直接返回队首
 */

#include <iostream>
#include <vector>
#include <deque>
#include <queue>
#include <set>
#include <climits>
#include <cmath>

/**
 * 题目1: 滑动窗口最大值
 *
 * 解法: 单调递减队列（双端队列）
 * 时间复杂度: O(n)
 * 空间复杂度: O(k)
 */
std::vector<int> maxSlidingWindow(std::vector<int>& nums, int k) {
    std::deque<int> dq;  // 存储索引，维护单调递减
    std::vector<int> result;

    for (int i = 0; i < static_cast<int>(nums.size()); ++i) {
        // 移除队首过期元素
        while (!dq.empty() && dq.front() <= i - k) {
            dq.pop_front();
        }

        // 维护单调递减：移除队尾所有小于当前元素的
        while (!dq.empty() && nums[dq.back()] < nums[i]) {
            dq.pop_back();
        }

        dq.push_back(i);

        // 窗口形成后记录最大值
        if (i >= k - 1) {
            result.push_back(nums[dq.front()]);
        }
    }

    return result;
}

/**
 * 题目2: 滑动窗口最小值
 *
 * 解法: 单调递增队列
 * 时间复杂度: O(n)
 * 空间复杂度: O(k)
 */
std::vector<int> minSlidingWindow(std::vector<int>& nums, int k) {
    std::deque<int> dq;  // 存储索引，维护单调递增
    std::vector<int> result;

    for (int i = 0; i < static_cast<int>(nums.size()); ++i) {
        // 移除队首过期元素
        while (!dq.empty() && dq.front() <= i - k) {
            dq.pop_front();
        }

        // 维护单调递增：移除队尾所有大于当前元素的
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

/**
 * 题目3: 跳跃游戏 VI
 *
 * 解法: 单调递减队列优化 DP
 * dp[i] = max(dp[j]) + nums[i]，j in [i-k, i-1]
 *
 * 时间复杂度: O(n)
 * 空间复杂度: O(k)
 */
int maxResult(std::vector<int>& nums, int k) {
    int n = nums.size();
    std::deque<int> dq;  // 存储索引，维护 dp 值单调递减

    std::vector<int> dp(n);
    dp[0] = nums[0];
    dq.push_back(0);

    for (int i = 1; i < n; ++i) {
        // 移除过期元素
        while (!dq.empty() && dq.front() < i - k) {
            dq.pop_front();
        }

        // 转移
        dp[i] = dp[dq.front()] + nums[i];

        // 维护单调递减
        while (!dq.empty() && dp[dq.back()] <= dp[i]) {
            dq.pop_back();
        }

        dq.push_back(i);
    }

    return dp[n - 1];
}

/**
 * 题目4: 绝对差不超过限制的最长连续子数组
 *
 * 解法: 滑动窗口 + 两个单调队列
 * 时间复杂度: O(n)
 * 空间复杂度: O(n)
 */
int longestSubarray(std::vector<int>& nums, int limit) {
    std::deque<int> maxDq, minDq;  // 最大值和最小值的单调队列
    int left = 0, result = 0;

    for (int right = 0; right < static_cast<int>(nums.size()); ++right) {
        // 维护最大值单调队列（递减）
        while (!maxDq.empty() && nums[maxDq.back()] < nums[right]) {
            maxDq.pop_back();
        }
        maxDq.push_back(right);

        // 维护最小值单调队列（递增）
        while (!minDq.empty() && nums[minDq.back()] > nums[right]) {
            minDq.pop_back();
        }
        minDq.push_back(right);

        // 收缩窗口直到满足条件
        while (nums[maxDq.front()] - nums[minDq.front()] > limit) {
            if (maxDq.front() == left) maxDq.pop_front();
            if (minDq.front() == left) minDq.pop_front();
            ++left;
        }

        result = std::max(result, right - left + 1);
    }

    return result;
}

/**
 * 使用 multiset 的解法
 */
int longestSubarrayMultiset(std::vector<int>& nums, int limit) {
    std::multiset<int> window;
    int left = 0, result = 0;

    for (int right = 0; right < static_cast<int>(nums.size()); ++right) {
        window.insert(nums[right]);

        while (*window.rbegin() - *window.begin() > limit) {
            window.erase(window.find(nums[left]));
            ++left;
        }

        result = std::max(result, right - left + 1);
    }

    return result;
}

/**
 * 题目5: 和至少为 K 的最短子数组
 *
 * 解法: 前缀和 + 单调递增队列
 * 时间复杂度: O(n)
 * 空间复杂度: O(n)
 */
int shortestSubarray(std::vector<int>& nums, int k) {
    int n = nums.size();
    std::vector<long long> prefix(n + 1, 0);

    for (int i = 0; i < n; ++i) {
        prefix[i + 1] = prefix[i] + nums[i];
    }

    std::deque<int> dq;  // 单调递增队列
    int result = INT_MAX;

    for (int i = 0; i <= n; ++i) {
        // 检查是否满足条件
        while (!dq.empty() && prefix[i] - prefix[dq.front()] >= k) {
            result = std::min(result, i - dq.front());
            dq.pop_front();
        }

        // 维护单调递增
        while (!dq.empty() && prefix[dq.back()] >= prefix[i]) {
            dq.pop_back();
        }

        dq.push_back(i);
    }

    return result == INT_MAX ? -1 : result;
}

/**
 * 题目6: 带限制的子序列和
 *
 * 解法: 单调递减队列优化 DP
 * dp[i] = max(0, max(dp[j])) + nums[i]，j in [i-k, i-1]
 *
 * 时间复杂度: O(n)
 * 空间复杂度: O(n)
 */
int constrainedSubsetSum(std::vector<int>& nums, int k) {
    int n = nums.size();
    std::vector<int> dp(n);
    std::deque<int> dq;

    int result = INT_MIN;

    for (int i = 0; i < n; ++i) {
        // 移除过期元素
        while (!dq.empty() && dq.front() < i - k) {
            dq.pop_front();
        }

        // 转移：可以选择不接任何前面的元素（取 max(0, dp[dq.front()])）
        dp[i] = nums[i];
        if (!dq.empty() && dp[dq.front()] > 0) {
            dp[i] += dp[dq.front()];
        }

        result = std::max(result, dp[i]);

        // 维护单调递减
        while (!dq.empty() && dp[dq.back()] <= dp[i]) {
            dq.pop_back();
        }

        dq.push_back(i);
    }

    return result;
}

/**
 * 题目7: 实现单调队列类
 */
class MonotonicQueue {
private:
    std::deque<int> data;  // 单调递减队列

public:
    MonotonicQueue() {}

    void push(int x) {
        while (!data.empty() && data.back() < x) {
            data.pop_back();
        }
        data.push_back(x);
    }

    void pop(int x) {
        // 只有当队首元素等于要弹出的元素时才弹出
        if (!data.empty() && data.front() == x) {
            data.pop_front();
        }
    }

    int max() {
        return data.front();
    }
};

/**
 * 题目8: 滑动窗口中位数
 *
 * 解法: 使用两个 multiset 或对顶堆
 * 时间复杂度: O(n log k)
 * 空间复杂度: O(k)
 */
std::vector<double> medianSlidingWindow(std::vector<int>& nums, int k) {
    std::multiset<int> small, large;  // small 是大顶堆（较小的一半），large 是小顶堆（较大的一半）
    std::vector<double> result;

    auto balance = [&]() {
        // small 要么等于 large，要么比 large 多一个
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
        // 插入新元素
        if (small.empty() || nums[i] <= *small.rbegin()) {
            small.insert(nums[i]);
        } else {
            large.insert(nums[i]);
        }
        balance();

        // 窗口形成后
        if (i >= k - 1) {
            result.push_back(getMedian());

            // 移除窗口左边界元素
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


// ==================== 单调队列模板总结 ====================
/**
 * 模板：滑动窗口最大值
 *
 * deque<int> dq;  // 存储索引，单调递减
 *
 * for (int i = 0; i < n; ++i) {
 *     // 1. 移除队首过期元素
 *     while (!dq.empty() && dq.front() <= i - k) {
 *         dq.pop_front();
 *     }
 *
 *     // 2. 维护单调性
 *     while (!dq.empty() && nums[dq.back()] < nums[i]) {
 *         dq.pop_back();
 *     }
 *
 *     // 3. 入队
 *     dq.push_back(i);
 *
 *     // 4. 获取结果
 *     if (i >= k - 1) {
 *         result.push_back(nums[dq.front()]);
 *     }
 * }
 *
 * 应用场景：
 * 1. 滑动窗口最值
 * 2. DP 优化：dp[i] = max/min(dp[j]) + cost[i]，j in [i-k, i-1]
 * 3. 满足某种区间条件的最长/最短子数组
 */


// ==================== 测试代码 ====================
#include <cassert>

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

    assert(longestSubarrayMultiset(nums1, 4) == 2);

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

void testMonotonicQueue() {
    MonotonicQueue mq;
    mq.push(1);
    mq.push(3);
    mq.push(2);
    assert(mq.max() == 3);

    mq.pop(1);
    assert(mq.max() == 3);

    mq.pop(3);
    assert(mq.max() == 2);

    std::cout << "MonotonicQueue: PASSED\n";
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
    std::cout << "=== Monotonic Queue Solution Tests ===\n";

    testMaxSlidingWindow();
    testMinSlidingWindow();
    testMaxResult();
    testLongestSubarray();
    testShortestSubarray();
    testConstrainedSubsetSum();
    testMonotonicQueue();
    testMedianSlidingWindow();

    std::cout << "\nAll tests passed!\n";
    return 0;
}
