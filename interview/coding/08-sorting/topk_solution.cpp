/**
 * @file topk_solution.cpp
 * @brief Top K 问题集合 - 解答文件
 *
 * Top K 问题解法总结：
 * | 方法        | 时间       | 空间   | 适用场景               |
 * |-------------|------------|--------|------------------------|
 * | 排序        | O(nlogn)   | O(1)   | 数据量小               |
 * | 最小/大堆   | O(nlogk)   | O(k)   | 数据流、k 较小         |
 * | 快速选择    | O(n) 平均  | O(1)   | 一次性查询             |
 * | 计数/桶排序 | O(n)       | O(n)   | 数据范围有限           |
 */

#include <iostream>
#include <vector>
#include <string>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <random>
#include <functional>

/**
 * 题目1: 数组中的第K个最大元素
 *
 * 方法1：最小堆 O(n log k)
 */
int findKthLargestHeap(std::vector<int>& nums, int k) {
    std::priority_queue<int, std::vector<int>, std::greater<int>> minHeap;

    for (int num : nums) {
        minHeap.push(num);
        if (static_cast<int>(minHeap.size()) > k) {
            minHeap.pop();
        }
    }

    return minHeap.top();
}

/**
 * 方法2：快速选择 O(n) 平均
 */
int findKthLargest(std::vector<int>& nums, int k) {
    std::mt19937 rng(std::random_device{}());

    auto partition = [&](int left, int right) {
        std::uniform_int_distribution<int> dist(left, right);
        int pivotIdx = dist(rng);
        std::swap(nums[pivotIdx], nums[right]);

        int pivot = nums[right];
        int i = left;
        for (int j = left; j < right; ++j) {
            if (nums[j] > pivot) {  // 降序
                std::swap(nums[i++], nums[j]);
            }
        }
        std::swap(nums[i], nums[right]);
        return i;
    };

    int left = 0, right = nums.size() - 1;
    --k;  // 转为 0-indexed

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
 * 题目2: 前 K 个高频元素
 *
 * 方法1：最小堆 O(n log k)
 */
std::vector<int> topKFrequentHeap(std::vector<int>& nums, int k) {
    std::unordered_map<int, int> freq;
    for (int num : nums) {
        ++freq[num];
    }

    auto cmp = [](const std::pair<int, int>& a, const std::pair<int, int>& b) {
        return a.second > b.second;  // 最小堆
    };
    std::priority_queue<std::pair<int, int>, std::vector<std::pair<int, int>>, decltype(cmp)> minHeap(cmp);

    for (const auto& [num, count] : freq) {
        minHeap.push({num, count});
        if (static_cast<int>(minHeap.size()) > k) {
            minHeap.pop();
        }
    }

    std::vector<int> result;
    while (!minHeap.empty()) {
        result.push_back(minHeap.top().first);
        minHeap.pop();
    }
    return result;
}

/**
 * 方法2：桶排序 O(n)
 */
std::vector<int> topKFrequent(std::vector<int>& nums, int k) {
    std::unordered_map<int, int> freq;
    for (int num : nums) {
        ++freq[num];
    }

    // 桶排序：按频率分桶
    int n = nums.size();
    std::vector<std::vector<int>> buckets(n + 1);
    for (const auto& [num, count] : freq) {
        buckets[count].push_back(num);
    }

    // 从高频到低频取 k 个
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
 * 题目3: 前 K 个高频单词
 */
std::vector<std::string> topKFrequentWords(std::vector<std::string>& words, int k) {
    std::unordered_map<std::string, int> freq;
    for (const auto& word : words) {
        ++freq[word];
    }

    // 自定义比较器：频率低的优先，频率相同字典序大的优先
    auto cmp = [](const std::pair<std::string, int>& a, const std::pair<std::string, int>& b) {
        if (a.second != b.second) {
            return a.second > b.second;  // 频率高的在堆底
        }
        return a.first < b.first;  // 字典序小的在堆底
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
 * 题目4: 最接近原点的 K 个点
 */
std::vector<std::vector<int>> kClosest(std::vector<std::vector<int>>& points, int k) {
    // 最大堆，按距离排序
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
 * 题目5: 第K个最小的素数分数
 *
 * 方法1：最小堆
 */
std::vector<int> kthSmallestPrimeFractionHeap(std::vector<int>& arr, int k) {
    int n = arr.size();
    // {分数值, i, j}，arr[i]/arr[j]
    auto cmp = [&](const std::tuple<double, int, int>& a, const std::tuple<double, int, int>& b) {
        return std::get<0>(a) > std::get<0>(b);
    };
    std::priority_queue<std::tuple<double, int, int>,
                        std::vector<std::tuple<double, int, int>>,
                        decltype(cmp)> minHeap(cmp);

    // 初始化：每个 arr[i] 与 arr[n-1] 的分数
    for (int i = 0; i < n - 1; ++i) {
        minHeap.push({static_cast<double>(arr[i]) / arr[n - 1], i, n - 1});
    }

    for (int i = 0; i < k - 1; ++i) {
        auto [frac, pi, pj] = minHeap.top();
        minHeap.pop();
        if (pj > pi + 1) {
            minHeap.push({static_cast<double>(arr[pi]) / arr[pj - 1], pi, pj - 1});
        }
    }

    auto [frac, pi, pj] = minHeap.top();
    return {arr[pi], arr[pj]};
}

/**
 * 方法2：二分查找
 */
std::vector<int> kthSmallestPrimeFraction(std::vector<int>& arr, int k) {
    int n = arr.size();
    double left = 0, right = 1;

    while (left < right) {
        double mid = (left + right) / 2;

        // 统计小于 mid 的分数个数
        int count = 0;
        int maxI = 0, maxJ = 1;  // 记录最大的小于 mid 的分数

        for (int i = 0, j = 1; i < n - 1; ++i) {
            while (j < n && arr[i] > mid * arr[j]) ++j;
            count += n - j;
            if (j < n && arr[i] * maxJ > arr[maxI] * arr[j]) {
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
 * 题目6: 数据流中的第 K 大元素
 */
class KthLargest {
private:
    int k;
    std::priority_queue<int, std::vector<int>, std::greater<int>> minHeap;

public:
    KthLargest(int k, std::vector<int>& nums) : k(k) {
        for (int num : nums) {
            add(num);
        }
    }

    int add(int val) {
        minHeap.push(val);
        if (static_cast<int>(minHeap.size()) > k) {
            minHeap.pop();
        }
        return minHeap.top();
    }
};

/**
 * 题目7: 查找和最小的 K 对数字
 *
 * 多路归并思想
 */
std::vector<std::vector<int>> kSmallestPairs(std::vector<int>& nums1,
                                              std::vector<int>& nums2, int k) {
    if (nums1.empty() || nums2.empty()) return {};

    // {和, nums1索引, nums2索引}
    auto cmp = [&](const std::tuple<int, int, int>& a, const std::tuple<int, int, int>& b) {
        return std::get<0>(a) > std::get<0>(b);
    };
    std::priority_queue<std::tuple<int, int, int>,
                        std::vector<std::tuple<int, int, int>>,
                        decltype(cmp)> minHeap(cmp);

    // 初始化：nums1 的每个元素与 nums2[0] 配对
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
 * 题目8: 有序矩阵中第 K 小的元素
 *
 * 方法1：最小堆，多路归并
 */
int kthSmallestHeap(std::vector<std::vector<int>>& matrix, int k) {
    int n = matrix.size();
    // {值, 行, 列}
    auto cmp = [](const std::tuple<int, int, int>& a, const std::tuple<int, int, int>& b) {
        return std::get<0>(a) > std::get<0>(b);
    };
    std::priority_queue<std::tuple<int, int, int>,
                        std::vector<std::tuple<int, int, int>>,
                        decltype(cmp)> minHeap(cmp);

    // 初始化：每行第一个元素
    for (int i = 0; i < n; ++i) {
        minHeap.push({matrix[i][0], i, 0});
    }

    for (int i = 0; i < k - 1; ++i) {
        auto [val, row, col] = minHeap.top();
        minHeap.pop();
        if (col + 1 < n) {
            minHeap.push({matrix[row][col + 1], row, col + 1});
        }
    }

    return std::get<0>(minHeap.top());
}

/**
 * 方法2：二分查找
 */
int kthSmallest(std::vector<std::vector<int>>& matrix, int k) {
    int n = matrix.size();
    int left = matrix[0][0], right = matrix[n - 1][n - 1];

    while (left < right) {
        int mid = left + (right - left) / 2;

        // 统计 <= mid 的元素个数
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
 * 题目9: 丑数 II
 *
 * 方法1：最小堆
 */
int nthUglyNumberHeap(int n) {
    std::priority_queue<long, std::vector<long>, std::greater<long>> minHeap;
    std::unordered_set<long> seen;

    minHeap.push(1);
    seen.insert(1);

    long ugly = 1;
    std::vector<int> factors = {2, 3, 5};

    for (int i = 0; i < n; ++i) {
        ugly = minHeap.top();
        minHeap.pop();

        for (int f : factors) {
            long next = ugly * f;
            if (!seen.count(next)) {
                seen.insert(next);
                minHeap.push(next);
            }
        }
    }

    return static_cast<int>(ugly);
}

/**
 * 方法2：三指针动态规划
 */
int nthUglyNumber(int n) {
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
 * 题目10: 超级丑数
 */
int nthSuperUglyNumber(int n, std::vector<int>& primes) {
    int m = primes.size();
    std::vector<int> pointers(m, 0);  // 每个质数的指针
    std::vector<long> dp(n);
    dp[0] = 1;

    for (int i = 1; i < n; ++i) {
        long minVal = LONG_MAX;
        for (int j = 0; j < m; ++j) {
            minVal = std::min(minVal, dp[pointers[j]] * primes[j]);
        }
        dp[i] = minVal;

        // 移动所有产生最小值的指针
        for (int j = 0; j < m; ++j) {
            if (dp[pointers[j]] * primes[j] == minVal) {
                ++pointers[j];
            }
        }
    }

    return static_cast<int>(dp[n - 1]);
}

/**
 * 附加: 数据流的中位数
 * LeetCode 295
 */
class MedianFinder {
private:
    std::priority_queue<int> maxHeap;  // 存较小的一半
    std::priority_queue<int, std::vector<int>, std::greater<int>> minHeap;  // 存较大的一半

public:
    void addNum(int num) {
        maxHeap.push(num);
        minHeap.push(maxHeap.top());
        maxHeap.pop();

        if (maxHeap.size() < minHeap.size()) {
            maxHeap.push(minHeap.top());
            minHeap.pop();
        }
    }

    double findMedian() {
        if (maxHeap.size() > minHeap.size()) {
            return maxHeap.top();
        }
        return (maxHeap.top() + minHeap.top()) / 2.0;
    }
};


// ==================== 测试代码 ====================
#include <cassert>
#include <algorithm>
#include <cmath>

void testFindKthLargest() {
    std::vector<int> nums = {3, 2, 1, 5, 6, 4};
    assert(findKthLargest(nums, 2) == 5);

    std::vector<int> nums2 = {3, 2, 3, 1, 2, 4, 5, 5, 6};
    assert(findKthLargest(nums2, 4) == 4);

    std::cout << "findKthLargest: PASSED\n";
}

void testTopKFrequent() {
    std::vector<int> nums = {1, 1, 1, 2, 2, 3};
    auto result = topKFrequent(nums, 2);
    std::sort(result.begin(), result.end());
    assert((result == std::vector<int>{1, 2}));
    std::cout << "topKFrequent: PASSED\n";
}

void testTopKFrequentWords() {
    std::vector<std::string> words = {"i", "love", "leetcode", "i", "love", "coding"};
    auto result = topKFrequentWords(words, 2);
    assert((result == std::vector<std::string>{"i", "love"}));
    std::cout << "topKFrequentWords: PASSED\n";
}

void testKClosest() {
    std::vector<std::vector<int>> points = {{1, 3}, {-2, 2}};
    auto result = kClosest(points, 1);
    assert(result.size() == 1 && result[0][0] == -2 && result[0][1] == 2);
    std::cout << "kClosest: PASSED\n";
}

void testKthLargestClass() {
    std::vector<int> nums = {4, 5, 8, 2};
    KthLargest kthLargest(3, nums);
    assert(kthLargest.add(3) == 4);
    assert(kthLargest.add(5) == 5);
    assert(kthLargest.add(10) == 5);
    assert(kthLargest.add(9) == 8);
    assert(kthLargest.add(4) == 8);
    std::cout << "KthLargest: PASSED\n";
}

void testKSmallestPairs() {
    std::vector<int> nums1 = {1, 7, 11};
    std::vector<int> nums2 = {2, 4, 6};
    auto result = kSmallestPairs(nums1, nums2, 3);
    assert(result.size() == 3);
    assert((result[0] == std::vector<int>{1, 2}));
    std::cout << "kSmallestPairs: PASSED\n";
}

void testKthSmallest() {
    std::vector<std::vector<int>> matrix = {{1, 5, 9}, {10, 11, 13}, {12, 13, 15}};
    assert(kthSmallest(matrix, 8) == 13);
    std::cout << "kthSmallest: PASSED\n";
}

void testNthUglyNumber() {
    assert(nthUglyNumber(10) == 12);
    assert(nthUglyNumber(1) == 1);
    std::cout << "nthUglyNumber: PASSED\n";
}

void testNthSuperUglyNumber() {
    std::vector<int> primes = {2, 7, 13, 19};
    assert(nthSuperUglyNumber(12, primes) == 32);
    std::cout << "nthSuperUglyNumber: PASSED\n";
}

void testMedianFinder() {
    MedianFinder mf;
    mf.addNum(1);
    mf.addNum(2);
    assert(std::abs(mf.findMedian() - 1.5) < 0.001);
    mf.addNum(3);
    assert(std::abs(mf.findMedian() - 2.0) < 0.001);
    std::cout << "MedianFinder: PASSED\n";
}

int main() {
    std::cout << "=== Top K Problems Solution Tests ===\n";

    testFindKthLargest();
    testTopKFrequent();
    testTopKFrequentWords();
    testKClosest();
    testKthLargestClass();
    testKSmallestPairs();
    testKthSmallest();
    testNthUglyNumber();
    testNthSuperUglyNumber();
    testMedianFinder();

    std::cout << "\nAll tests passed!\n";
    return 0;
}
