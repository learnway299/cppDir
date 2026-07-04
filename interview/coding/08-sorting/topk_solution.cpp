/**
 * @file topk_solution.cpp
 * @brief Top K 问题集合 - 参考答案
 */

#include "topk.h"
#include <iostream>
#include <cassert>
#include <algorithm>
#include <unordered_map>
#include <unordered_set>
#include <random>
#include <functional>
#include <cmath>
#include <climits>
#include <tuple>

namespace TopK {

// ==================== 参考答案实现 ====================

/**
 * 题目1: 第 K 大元素 - 快速选择
 * 时间复杂度: O(n) 平均，O(n²) 最坏
 * 空间复杂度: O(1) - 原地操作
 */
int findKthLargestSolution(std::vector<int>& nums, int k) {
    static std::mt19937 rng{std::random_device{}()};

    auto partition = [&](int left, int right) -> int {
        std::uniform_int_distribution<int> dist(left, right);
        int pivotIdx = dist(rng);
        std::swap(nums[pivotIdx], nums[right]);

        int pivot = nums[right];
        int i = left;
        for (int j = left; j < right; ++j) {
            if (nums[j] > pivot) {
                std::swap(nums[i++], nums[j]);
            }
        }
        std::swap(nums[i], nums[right]);
        return i;
    };

    int left = 0, right = static_cast<int>(nums.size()) - 1;
    --k;

    while (left <= right) {
        int pivotIdx = partition(left, right);
        if (pivotIdx == k) {
            return nums[pivotIdx];
        } else if (pivotIdx < k) {
            left = pivotIdx + 1;
        } else {
            right = pivotIdx - 1;
        }
    }
    return -1;
}

/**
 * 题目2: 前 K 个高频元素 - 桶排序
 * 时间复杂度: O(n)
 * 空间复杂度: O(n) - 桶和哈希表
 */
std::vector<int> topKFrequentSolution(std::vector<int>& nums, int k) {
    std::unordered_map<int, int> freq;
    for (int num : nums) {
        ++freq[num];
    }

    // 桶排序
    int n = static_cast<int>(nums.size());
    std::vector<std::vector<int>> buckets(n + 1);
    for (const auto& [num, count] : freq) {
        buckets[count].push_back(num);
    }

    std::vector<int> result;
    for (int i = n; i >= 0 && static_cast<int>(result.size()) < k; --i) {
        for (int num : buckets[i]) {
            result.push_back(num);
            if (static_cast<int>(result.size()) == k) break;
        }
    }
    return result;
}

/**
 * 题目3: 前 K 个高频单词 - 最小堆
 * 时间复杂度: O(n log k)
 * 空间复杂度: O(n) - 哈希表 + 堆
 */
std::vector<std::string> topKFrequentWordsSolution(std::vector<std::string>& words, int k) {
    std::unordered_map<std::string, int> freq;
    for (const auto& word : words) {
        ++freq[word];
    }

    auto cmp = [](const std::pair<std::string, int>& a, const std::pair<std::string, int>& b) {
        if (a.second != b.second) {
            return a.second > b.second;
        }
        return a.first < b.first;
    };
    std::priority_queue<std::pair<std::string, int>,
                        std::vector<std::pair<std::string, int>>,
                        decltype(cmp)> minHeap(cmp);

    for (const auto& [word, count] : freq) {
        minHeap.push({word, count});
        if (static_cast<int>(minHeap.size()) > k) {
            minHeap.pop();
        }
    }

    std::vector<std::string> result(k);
    for (int i = k - 1; i >= 0; --i) {
        result[i] = minHeap.top().first;
        minHeap.pop();
    }
    return result;
}

/**
 * 题目4: 最接近原点的 K 个点 - 最大堆
 * 时间复杂度: O(n log k)
 * 空间复杂度: O(k) - 堆大小
 */
std::vector<std::vector<int>> kClosestSolution(std::vector<std::vector<int>>& points, int k) {
    auto cmp = [](const std::vector<int>& a, const std::vector<int>& b) {
        return a[0] * a[0] + a[1] * a[1] < b[0] * b[0] + b[1] * b[1];
    };
    std::priority_queue<std::vector<int>, std::vector<std::vector<int>>, decltype(cmp)> maxHeap(cmp);

    for (const auto& point : points) {
        maxHeap.push(point);
        if (static_cast<int>(maxHeap.size()) > k) {
            maxHeap.pop();
        }
    }

    std::vector<std::vector<int>> result;
    while (!maxHeap.empty()) {
        result.push_back(maxHeap.top());
        maxHeap.pop();
    }
    return result;
}

/**
 * 题目5: 第 K 小的素数分数 - 二分查找
 * 时间复杂度: O(n log(max_value))
 * 空间复杂度: O(1)
 */
std::vector<int> kthSmallestPrimeFractionSolution(std::vector<int>& arr, int k) {
    int n = static_cast<int>(arr.size());
    double left = 0, right = 1;

    while (left < right) {
        double mid = (left + right) / 2;

        int count = 0;
        int maxI = 0, maxJ = 1;

        for (int i = 0, j = 1; i < n - 1; ++i) {
            while (j < n && arr[i] > mid * arr[j]) ++j;
            count += n - j;
            if (j < n && arr[i] * arr[maxJ] > arr[maxI] * arr[j]) {
                maxI = i;
                maxJ = j;
            }
        }

        if (count == k) {
            return {arr[maxI], arr[maxJ]};
        } else if (count < k) {
            left = mid;
        } else {
            right = mid;
        }
    }
    return {};
}

/**
 * 题目6: 数据流中的第 K 大元素 - 最小堆
 * 构造: 时间 O(n log k)
 * add: 时间 O(log k)，空间 O(k)
 */
KthLargestStreamSolution::KthLargestStreamSolution(int k, std::vector<int>& nums) : k_(k) {
    for (int num : nums) {
        add(num);
    }
}

int KthLargestStreamSolution::add(int val) {
    minHeap_.push(val);
    if (static_cast<int>(minHeap_.size()) > k_) {
        minHeap_.pop();
    }
    return minHeap_.top();
}

/**
 * 题目7: 查找和最小的 K 对数字 - 最小堆
 * 时间复杂度: O(k log k)
 * 空间复杂度: O(k) - 堆大小
 */
std::vector<std::vector<int>> kSmallestPairsSolution(std::vector<int>& nums1,
                                                      std::vector<int>& nums2, int k) {
    if (nums1.empty() || nums2.empty()) return {};

    auto cmp = [&](const std::tuple<int, int, int>& a, const std::tuple<int, int, int>& b) {
        return std::get<0>(a) > std::get<0>(b);
    };
    std::priority_queue<std::tuple<int, int, int>,
                        std::vector<std::tuple<int, int, int>>,
                        decltype(cmp)> minHeap(cmp);

    for (int i = 0; i < std::min(static_cast<int>(nums1.size()), k); ++i) {
        minHeap.push({nums1[i] + nums2[0], i, 0});
    }

    std::vector<std::vector<int>> result;
    while (!minHeap.empty() && static_cast<int>(result.size()) < k) {
        auto [sum, i, j] = minHeap.top();
        minHeap.pop();
        result.push_back({nums1[i], nums2[j]});

        if (j + 1 < static_cast<int>(nums2.size())) {
            minHeap.push({nums1[i] + nums2[j + 1], i, j + 1});
        }
    }

    return result;
}

/**
 * 题目8: 有序矩阵中第 K 小的元素 - 二分查找
 * 时间复杂度: O(n log(max - min))
 * 空间复杂度: O(1)
 */
int kthSmallestSolution(std::vector<std::vector<int>>& matrix, int k) {
    int n = static_cast<int>(matrix.size());
    int left = matrix[0][0], right = matrix[n - 1][n - 1];

    while (left < right) {
        int mid = left + (right - left) / 2;

        int count = 0;
        int j = n - 1;
        for (int i = 0; i < n; ++i) {
            while (j >= 0 && matrix[i][j] > mid) --j;
            count += j + 1;
        }

        if (count < k) {
            left = mid + 1;
        } else {
            right = mid;
        }
    }

    return left;
}

/**
 * 题目9: 丑数 II - 三指针 DP
 * 时间复杂度: O(n)
 * 空间复杂度: O(n) - DP 数组
 */
int nthUglyNumberSolution(int n) {
    std::vector<int> dp(n);
    dp[0] = 1;

    int p2 = 0, p3 = 0, p5 = 0;

    for (int i = 1; i < n; ++i) {
        int next2 = dp[p2] * 2;
        int next3 = dp[p3] * 3;
        int next5 = dp[p5] * 5;

        dp[i] = std::min({next2, next3, next5});

        if (dp[i] == next2) ++p2;
        if (dp[i] == next3) ++p3;
        if (dp[i] == next5) ++p5;
    }

    return dp[n - 1];
}

/**
 * 题目10: 超级丑数 - 多指针 DP
 * 时间复杂度: O(n * m) - m 为质数个数
 * 空间复杂度: O(n + m) - DP 数组 + 指针数组
 */
int nthSuperUglyNumberSolution(int n, std::vector<int>& primes) {
    int m = static_cast<int>(primes.size());
    std::vector<int> pointers(m, 0);
    std::vector<long> dp(n);
    dp[0] = 1;

    for (int i = 1; i < n; ++i) {
        long minVal = LONG_MAX;
        for (int j = 0; j < m; ++j) {
            minVal = std::min(minVal, dp[pointers[j]] * primes[j]);
        }
        dp[i] = minVal;

        for (int j = 0; j < m; ++j) {
            if (dp[pointers[j]] * primes[j] == minVal) {
                ++pointers[j];
            }
        }
    }

    return static_cast<int>(dp[n - 1]);
}

/**
 * 题目11: 数据流的中位数 - 对顶堆
 * addNum: 时间 O(log n)
 * findMedian: 时间 O(1)
 * 空间复杂度: O(n) - 存储所有元素
 */
void MedianFinderSolution::addNum(int num) {
    maxHeap_.push(num);
    minHeap_.push(maxHeap_.top());
    maxHeap_.pop();

    if (maxHeap_.size() < minHeap_.size()) {
        maxHeap_.push(minHeap_.top());
        minHeap_.pop();
    }
}

double MedianFinderSolution::findMedian() {
    if (maxHeap_.size() > minHeap_.size()) {
        return maxHeap_.top();
    }
    return (maxHeap_.top() + minHeap_.top()) / 2.0;
}

// ==================== 测试函数 ====================

void testTopKSolution() {
    std::cout << "=== Top K Tests (Solution) ===\n";

    // findKthLargestSolution
    std::vector<int> nums1 = {3, 2, 1, 5, 6, 4};
    assert(findKthLargestSolution(nums1, 2) == 5);
    std::cout << "  findKthLargestSolution: PASSED\n";

    // topKFrequentSolution
    std::vector<int> nums2 = {1, 1, 1, 2, 2, 3};
    auto result2 = topKFrequentSolution(nums2, 2);
    std::sort(result2.begin(), result2.end());
    assert((result2 == std::vector<int>{1, 2}));
    std::cout << "  topKFrequentSolution: PASSED\n";

    // topKFrequentWordsSolution
    std::vector<std::string> words = {"i", "love", "leetcode", "i", "love", "coding"};
    auto result3 = topKFrequentWordsSolution(words, 2);
    assert((result3 == std::vector<std::string>{"i", "love"}));
    std::cout << "  topKFrequentWordsSolution: PASSED\n";

    // kClosestSolution
    std::vector<std::vector<int>> points = {{1, 3}, {-2, 2}};
    auto result4 = kClosestSolution(points, 1);
    assert(result4.size() == 1 && result4[0][0] == -2 && result4[0][1] == 2);
    std::cout << "  kClosestSolution: PASSED\n";

    // KthLargestStreamSolution
    std::vector<int> nums5 = {4, 5, 8, 2};
    KthLargestStreamSolution kthLargest(3, nums5);
    assert(kthLargest.add(3) == 4);
    assert(kthLargest.add(5) == 5);
    assert(kthLargest.add(10) == 5);
    assert(kthLargest.add(9) == 8);
    assert(kthLargest.add(4) == 8);
    std::cout << "  KthLargestStreamSolution: PASSED\n";

    // kSmallestPairsSolution
    std::vector<int> nums6a = {1, 7, 11};
    std::vector<int> nums6b = {2, 4, 6};
    auto result6 = kSmallestPairsSolution(nums6a, nums6b, 3);
    assert(result6.size() == 3);
    assert((result6[0] == std::vector<int>{1, 2}));
    std::cout << "  kSmallestPairsSolution: PASSED\n";

    // kthSmallestSolution
    std::vector<std::vector<int>> matrix = {{1, 5, 9}, {10, 11, 13}, {12, 13, 15}};
    assert(kthSmallestSolution(matrix, 8) == 13);
    std::cout << "  kthSmallestSolution: PASSED\n";

    // nthUglyNumberSolution
    assert(nthUglyNumberSolution(10) == 12);
    assert(nthUglyNumberSolution(1) == 1);
    std::cout << "  nthUglyNumberSolution: PASSED\n";

    // nthSuperUglyNumberSolution
    std::vector<int> primes = {2, 7, 13, 19};
    assert(nthSuperUglyNumberSolution(12, primes) == 32);
    std::cout << "  nthSuperUglyNumberSolution: PASSED\n";

    // MedianFinderSolution
    MedianFinderSolution mf;
    mf.addNum(1);
    mf.addNum(2);
    assert(std::abs(mf.findMedian() - 1.5) < 0.001);
    mf.addNum(3);
    assert(std::abs(mf.findMedian() - 2.0) < 0.001);
    std::cout << "  MedianFinderSolution: PASSED\n";
}

} // namespace TopK
