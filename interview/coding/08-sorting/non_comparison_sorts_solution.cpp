/**
 * @file non_comparison_sorts_solution.cpp
 * @brief 非比较排序算法实现 - 参考答案
 */

#include "non_comparison_sorts.h"
#include <iostream>
#include <cassert>
#include <algorithm>
#include <unordered_map>
#include <random>
#include <climits>

namespace NonComparisonSorts {

// ==================== 参考答案实现 ====================

void countingSortSolution(std::vector<int>& nums) {
    if (nums.empty()) return;

    int minVal = *std::min_element(nums.begin(), nums.end());
    int maxVal = *std::max_element(nums.begin(), nums.end());
    int range = maxVal - minVal + 1;

    std::vector<int> count(range, 0);

    for (int num : nums) {
        ++count[num - minVal];
    }

    int idx = 0;
    for (int i = 0; i < range; ++i) {
        while (count[i]-- > 0) {
            nums[idx++] = i + minVal;
        }
    }
}

void bucketSortSolution(std::vector<float>& nums) {
    if (nums.empty()) return;

    int n = static_cast<int>(nums.size());
    std::vector<std::vector<float>> buckets(n);

    for (float num : nums) {
        int idx = static_cast<int>(n * num);
        if (idx >= n) idx = n - 1;
        buckets[idx].push_back(num);
    }

    for (auto& bucket : buckets) {
        std::sort(bucket.begin(), bucket.end());
    }

    int idx = 0;
    for (const auto& bucket : buckets) {
        for (float num : bucket) {
            nums[idx++] = num;
        }
    }
}

void radixSortSolution(std::vector<int>& nums) {
    if (nums.empty()) return;

    // 分离正负数
    std::vector<int> negative, nonNegative;
    for (int num : nums) {
        if (num < 0) {
            negative.push_back(-num);
        } else {
            nonNegative.push_back(num);
        }
    }

    auto radixSortPositive = [](std::vector<int>& arr) {
        if (arr.empty()) return;

        int maxVal = *std::max_element(arr.begin(), arr.end());
        int exp = 1;

        std::vector<int> output(arr.size());
        std::vector<int> count(10);

        while (maxVal / exp > 0) {
            std::fill(count.begin(), count.end(), 0);

            for (int num : arr) {
                int digit = (num / exp) % 10;
                ++count[digit];
            }

            for (int i = 1; i < 10; ++i) {
                count[i] += count[i - 1];
            }

            for (int i = static_cast<int>(arr.size()) - 1; i >= 0; --i) {
                int digit = (arr[i] / exp) % 10;
                output[--count[digit]] = arr[i];
            }

            arr = output;
            exp *= 10;
        }
    };

    radixSortPositive(nonNegative);
    radixSortPositive(negative);

    // 合并：负数逆序取反 + 非负数
    int idx = 0;
    for (int i = static_cast<int>(negative.size()) - 1; i >= 0; --i) {
        nums[idx++] = -negative[i];
    }
    for (int num : nonNegative) {
        nums[idx++] = num;
    }
}

int maximumGapSolution(std::vector<int>& nums) {
    int n = static_cast<int>(nums.size());
    if (n < 2) return 0;

    int minVal = *std::min_element(nums.begin(), nums.end());
    int maxVal = *std::max_element(nums.begin(), nums.end());

    if (minVal == maxVal) return 0;

    // 桶大小：保证最大间距在桶之间
    int bucketSize = std::max(1, (maxVal - minVal) / (n - 1));
    int bucketCount = (maxVal - minVal) / bucketSize + 1;

    std::vector<int> bucketMin(bucketCount, INT_MAX);
    std::vector<int> bucketMax(bucketCount, INT_MIN);

    for (int num : nums) {
        int idx = (num - minVal) / bucketSize;
        bucketMin[idx] = std::min(bucketMin[idx], num);
        bucketMax[idx] = std::max(bucketMax[idx], num);
    }

    int maxGap = 0;
    int prevMax = minVal;
    for (int i = 0; i < bucketCount; ++i) {
        if (bucketMin[i] == INT_MAX) continue;
        maxGap = std::max(maxGap, bucketMin[i] - prevMax);
        prevMax = bucketMax[i];
    }

    return maxGap;
}

int findKthLargestSolution(std::vector<int>& nums, int k) {
    static std::mt19937 rng{std::random_device{}()};

    auto partition = [&](int left, int right) -> int {
        std::uniform_int_distribution<int> dist(left, right);
        int pivotIdx = dist(rng);
        std::swap(nums[pivotIdx], nums[right]);

        int pivot = nums[right];
        int i = left;
        for (int j = left; j < right; ++j) {
            if (nums[j] > pivot) {  // 降序排列
                std::swap(nums[i++], nums[j]);
            }
        }
        std::swap(nums[i], nums[right]);
        return i;
    };

    int left = 0, right = static_cast<int>(nums.size()) - 1;
    --k;  // 转换为 0-indexed

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

std::vector<int> sortArrayByParitySolution(std::vector<int>& nums) {
    int left = 0, right = static_cast<int>(nums.size()) - 1;
    while (left < right) {
        while (left < right && nums[left] % 2 == 0) ++left;
        while (left < right && nums[right] % 2 == 1) --right;
        if (left < right) {
            std::swap(nums[left], nums[right]);
        }
    }
    return nums;
}

std::vector<int> sortArrayByParityIISolution(std::vector<int>& nums) {
    int n = static_cast<int>(nums.size());
    int even = 0, odd = 1;

    while (even < n && odd < n) {
        while (even < n && nums[even] % 2 == 0) even += 2;
        while (odd < n && nums[odd] % 2 == 1) odd += 2;
        if (even < n && odd < n) {
            std::swap(nums[even], nums[odd]);
        }
    }
    return nums;
}

int hIndexSolution(std::vector<int>& citations) {
    int n = static_cast<int>(citations.size());
    std::vector<int> count(n + 1, 0);

    for (int c : citations) {
        ++count[std::min(c, n)];
    }

    int total = 0;
    for (int h = n; h >= 0; --h) {
        total += count[h];
        if (total >= h) {
            return h;
        }
    }
    return 0;
}

std::string frequencySortSolution(std::string s) {
    std::unordered_map<char, int> freq;
    for (char c : s) {
        ++freq[c];
    }

    int maxFreq = 0;
    for (const auto& [c, f] : freq) {
        maxFreq = std::max(maxFreq, f);
    }

    // 桶排序：按频率分桶
    std::vector<std::string> buckets(maxFreq + 1);
    for (const auto& [c, f] : freq) {
        buckets[f] += std::string(f, c);
    }

    std::string result;
    for (int i = maxFreq; i >= 1; --i) {
        result += buckets[i];
    }
    return result;
}

void sortColorsSolution(std::vector<int>& nums) {
    // 荷兰国旗问题：三路划分
    int p0 = 0, p2 = static_cast<int>(nums.size()) - 1;
    int i = 0;

    while (i <= p2) {
        if (nums[i] == 0) {
            std::swap(nums[i++], nums[p0++]);
        } else if (nums[i] == 2) {
            std::swap(nums[i], nums[p2--]);
            // 注意：交换后 i 不增加，因为换来的元素还需检查
        } else {
            ++i;
        }
    }
}

std::vector<std::vector<std::string>> groupAnagramsSolution(std::vector<std::string>& strs) {
    std::unordered_map<std::string, std::vector<std::string>> groups;

    for (const std::string& s : strs) {
        // 使用计数排序生成 key
        std::vector<int> count(26, 0);
        for (char c : s) {
            ++count[c - 'a'];
        }

        std::string key;
        for (int i = 0; i < 26; ++i) {
            if (count[i] > 0) {
                key += ('a' + i);
                key += std::to_string(count[i]);
            }
        }

        groups[key].push_back(s);
    }

    std::vector<std::vector<std::string>> result;
    for (auto& [key, group] : groups) {
        result.push_back(std::move(group));
    }
    return result;
}

// ==================== 测试函数 ====================

void testNonComparisonSortsSolution() {
    std::cout << "=== Non-Comparison Sorts Tests (Solution) ===\n";

    // countingSortSolution
    std::vector<int> nums1 = {4, 2, 2, 8, 3, 3, 1};
    countingSortSolution(nums1);
    assert((nums1 == std::vector<int>{1, 2, 2, 3, 3, 4, 8}));
    std::cout << "  countingSortSolution: PASSED\n";

    // bucketSortSolution
    std::vector<float> nums2 = {0.42f, 0.32f, 0.23f, 0.52f, 0.25f, 0.47f, 0.51f};
    bucketSortSolution(nums2);
    for (size_t i = 1; i < nums2.size(); ++i) {
        assert(nums2[i] >= nums2[i-1]);
    }
    std::cout << "  bucketSortSolution: PASSED\n";

    // radixSortSolution
    std::vector<int> nums3 = {170, 45, 75, 90, 802, 24, 2, 66};
    radixSortSolution(nums3);
    assert((nums3 == std::vector<int>{2, 24, 45, 66, 75, 90, 170, 802}));
    std::vector<int> nums3b = {-3, 5, -1, 0, 2, -2};
    radixSortSolution(nums3b);
    assert((nums3b == std::vector<int>{-3, -2, -1, 0, 2, 5}));
    std::cout << "  radixSortSolution: PASSED\n";

    // maximumGapSolution
    std::vector<int> nums4 = {3, 6, 9, 1};
    assert(maximumGapSolution(nums4) == 3);
    std::cout << "  maximumGapSolution: PASSED\n";

    // findKthLargestSolution
    std::vector<int> nums5 = {3, 2, 1, 5, 6, 4};
    assert(findKthLargestSolution(nums5, 2) == 5);
    std::cout << "  findKthLargestSolution: PASSED\n";

    // sortColorsSolution
    std::vector<int> nums6 = {2, 0, 2, 1, 1, 0};
    sortColorsSolution(nums6);
    assert((nums6 == std::vector<int>{0, 0, 1, 1, 2, 2}));
    std::cout << "  sortColorsSolution: PASSED\n";

    // hIndexSolution
    std::vector<int> citations = {3, 0, 6, 1, 5};
    assert(hIndexSolution(citations) == 3);
    std::cout << "  hIndexSolution: PASSED\n";

    // frequencySortSolution
    std::string s = "tree";
    std::string result = frequencySortSolution(s);
    assert(result[0] == 'e' && result[1] == 'e');
    std::cout << "  frequencySortSolution: PASSED\n";

    // sortArrayByParitySolution
    std::vector<int> nums7 = {3, 1, 2, 4};
    auto parityResult = sortArrayByParitySolution(nums7);
    int idx = 0;
    while (idx < static_cast<int>(parityResult.size()) && parityResult[idx] % 2 == 0) ++idx;
    while (idx < static_cast<int>(parityResult.size())) {
        assert(parityResult[idx] % 2 == 1);
        ++idx;
    }
    std::cout << "  sortArrayByParitySolution: PASSED\n";

    // sortArrayByParityIISolution
    std::vector<int> nums8 = {4, 2, 5, 7};
    auto parityResult2 = sortArrayByParityIISolution(nums8);
    for (int i = 0; i < static_cast<int>(parityResult2.size()); ++i) {
        assert(parityResult2[i] % 2 == i % 2);
    }
    std::cout << "  sortArrayByParityIISolution: PASSED\n";

    // groupAnagramsSolution
    std::vector<std::string> strs = {"eat", "tea", "tan", "ate", "nat", "bat"};
    auto groups = groupAnagramsSolution(strs);
    assert(groups.size() == 3);
    std::cout << "  groupAnagramsSolution: PASSED\n";
}

} // namespace NonComparisonSorts
