/**
 * @file non_comparison_sorts_solution.cpp
 * @brief 非比较排序算法实现 - 解答文件
 *
 * 非比较排序对比：
 * | 算法   | 时间       | 空间    | 稳定 | 适用场景            |
 * |--------|------------|---------|------|---------------------|
 * | 计数   | O(n + k)   | O(k)    | ✓    | 范围有限的整数      |
 * | 桶     | O(n + k)   | O(n+k)  | ✓    | 均匀分布的数据      |
 * | 基数   | O(d * n)   | O(n+k)  | ✓    | 位数有限的整数/字符串|
 */

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <unordered_map>
#include <random>

/**
 * 题目1: 计数排序
 */
void countingSort(std::vector<int>& nums) {
    if (nums.empty()) return;

    int minVal = *std::min_element(nums.begin(), nums.end());
    int maxVal = *std::max_element(nums.begin(), nums.end());
    int range = maxVal - minVal + 1;

    std::vector<int> count(range, 0);

    // 统计每个元素的出现次数
    for (int num : nums) {
        ++count[num - minVal];
    }

    // 重建数组
    int idx = 0;
    for (int i = 0; i < range; ++i) {
        while (count[i]-- > 0) {
            nums[idx++] = i + minVal;
        }
    }
}

/**
 * 稳定版计数排序（用于基数排序）
 */
void countingSortStable(std::vector<int>& nums) {
    if (nums.empty()) return;

    int minVal = *std::min_element(nums.begin(), nums.end());
    int maxVal = *std::max_element(nums.begin(), nums.end());
    int range = maxVal - minVal + 1;

    std::vector<int> count(range, 0);

    // 统计
    for (int num : nums) {
        ++count[num - minVal];
    }

    // 累加（计算每个元素的最终位置）
    for (int i = 1; i < range; ++i) {
        count[i] += count[i - 1];
    }

    // 从后往前遍历，保证稳定性
    std::vector<int> output(nums.size());
    for (int i = nums.size() - 1; i >= 0; --i) {
        int idx = --count[nums[i] - minVal];
        output[idx] = nums[i];
    }

    nums = std::move(output);
}

/**
 * 题目2: 桶排序
 */
void bucketSort(std::vector<float>& nums) {
    if (nums.empty()) return;

    int n = nums.size();
    std::vector<std::vector<float>> buckets(n);

    // 假设数据在 [0, 1) 范围内
    for (float num : nums) {
        int idx = static_cast<int>(n * num);
        if (idx >= n) idx = n - 1;  // 处理边界
        buckets[idx].push_back(num);
    }

    // 桶内排序
    for (auto& bucket : buckets) {
        std::sort(bucket.begin(), bucket.end());
    }

    // 合并
    int idx = 0;
    for (const auto& bucket : buckets) {
        for (float num : bucket) {
            nums[idx++] = num;
        }
    }
}

/**
 * 整数桶排序
 */
void bucketSortInt(std::vector<int>& nums, int bucketSize = 10) {
    if (nums.empty()) return;

    int minVal = *std::min_element(nums.begin(), nums.end());
    int maxVal = *std::max_element(nums.begin(), nums.end());

    int bucketCount = (maxVal - minVal) / bucketSize + 1;
    std::vector<std::vector<int>> buckets(bucketCount);

    for (int num : nums) {
        int idx = (num - minVal) / bucketSize;
        buckets[idx].push_back(num);
    }

    int idx = 0;
    for (auto& bucket : buckets) {
        std::sort(bucket.begin(), bucket.end());
        for (int num : bucket) {
            nums[idx++] = num;
        }
    }
}

/**
 * 题目3: 基数排序
 */
void radixSort(std::vector<int>& nums) {
    if (nums.empty()) return;

    // 处理负数：分开排序
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

            // 统计
            for (int num : arr) {
                int digit = (num / exp) % 10;
                ++count[digit];
            }

            // 累加
            for (int i = 1; i < 10; ++i) {
                count[i] += count[i - 1];
            }

            // 从后往前放置
            for (int i = arr.size() - 1; i >= 0; --i) {
                int digit = (arr[i] / exp) % 10;
                output[--count[digit]] = arr[i];
            }

            arr = output;
            exp *= 10;
        }
    };

    radixSortPositive(nonNegative);
    radixSortPositive(negative);

    // 合并结果
    int idx = 0;
    // 负数部分（倒序）
    for (int i = negative.size() - 1; i >= 0; --i) {
        nums[idx++] = -negative[i];
    }
    // 非负数部分
    for (int num : nonNegative) {
        nums[idx++] = num;
    }
}

/**
 * 题目4: 最大间距
 *
 * 桶排序思想：
 * - 将 n 个数放入 n 个桶中
 * - 桶的大小 = (max - min) / (n - 1)
 * - 最大间距一定在相邻桶之间（桶内间距 < 平均间距）
 */
int maximumGap(std::vector<int>& nums) {
    int n = nums.size();
    if (n < 2) return 0;

    int minVal = *std::min_element(nums.begin(), nums.end());
    int maxVal = *std::max_element(nums.begin(), nums.end());

    if (minVal == maxVal) return 0;

    // 桶的大小（向上取整保证最大间距在桶间）
    int bucketSize = std::max(1, (maxVal - minVal) / (n - 1));
    int bucketCount = (maxVal - minVal) / bucketSize + 1;

    // 每个桶只记录最大值和最小值
    std::vector<int> bucketMin(bucketCount, INT_MAX);
    std::vector<int> bucketMax(bucketCount, INT_MIN);

    for (int num : nums) {
        int idx = (num - minVal) / bucketSize;
        bucketMin[idx] = std::min(bucketMin[idx], num);
        bucketMax[idx] = std::max(bucketMax[idx], num);
    }

    // 最大间距 = 当前桶的最小值 - 前一个非空桶的最大值
    int maxGap = 0;
    int prevMax = minVal;
    for (int i = 0; i < bucketCount; ++i) {
        if (bucketMin[i] == INT_MAX) continue;  // 空桶
        maxGap = std::max(maxGap, bucketMin[i] - prevMax);
        prevMax = bucketMax[i];
    }

    return maxGap;
}

/**
 * 题目5: 快速选择算法（QuickSelect）
 *
 * 平均 O(n)，最坏 O(n²)
 */
class QuickSelect {
private:
    std::mt19937 rng{std::random_device{}()};

    int partition(std::vector<int>& nums, int left, int right) {
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
    }

public:
    int findKthLargest(std::vector<int>& nums, int k) {
        int left = 0, right = nums.size() - 1;
        --k;  // 转为 0-indexed

        while (left <= right) {
            int pivotIdx = partition(nums, left, right);
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
};

int findKthLargest(std::vector<int>& nums, int k) {
    QuickSelect qs;
    return qs.findKthLargest(nums, k);
}

/**
 * BFPRT 算法（中位数的中位数）
 *
 * 最坏情况 O(n)
 */
class BFPRT {
private:
    int partition(std::vector<int>& nums, int left, int right, int pivotVal) {
        int pivotIdx = left;
        for (int i = left; i <= right; ++i) {
            if (nums[i] == pivotVal) {
                pivotIdx = i;
                break;
            }
        }
        std::swap(nums[pivotIdx], nums[right]);

        int i = left;
        for (int j = left; j < right; ++j) {
            if (nums[j] > pivotVal) {
                std::swap(nums[i++], nums[j]);
            }
        }
        std::swap(nums[i], nums[right]);
        return i;
    }

    int medianOfMedians(std::vector<int>& nums, int left, int right) {
        int n = right - left + 1;
        if (n <= 5) {
            std::sort(nums.begin() + left, nums.begin() + right + 1, std::greater<int>());
            return nums[left + n / 2];
        }

        // 将数组分成每组 5 个，找每组的中位数
        std::vector<int> medians;
        for (int i = left; i <= right; i += 5) {
            int subRight = std::min(i + 4, right);
            std::sort(nums.begin() + i, nums.begin() + subRight + 1, std::greater<int>());
            medians.push_back(nums[i + (subRight - i) / 2]);
        }

        // 递归找中位数的中位数
        return select(medians, 0, medians.size() - 1, medians.size() / 2);
    }

    int select(std::vector<int>& nums, int left, int right, int k) {
        if (left == right) return nums[left];

        int pivot = medianOfMedians(nums, left, right);
        int pivotIdx = partition(nums, left, right, pivot);

        if (pivotIdx == k) {
            return nums[pivotIdx];
        } else if (pivotIdx < k) {
            return select(nums, pivotIdx + 1, right, k);
        } else {
            return select(nums, left, pivotIdx - 1, k);
        }
    }

public:
    int findKthLargest(std::vector<int>& nums, int k) {
        return select(nums, 0, nums.size() - 1, k - 1);
    }
};

/**
 * 题目6: 按奇偶排序数组
 */
std::vector<int> sortArrayByParity(std::vector<int>& nums) {
    int left = 0, right = nums.size() - 1;
    while (left < right) {
        while (left < right && nums[left] % 2 == 0) ++left;
        while (left < right && nums[right] % 2 == 1) --right;
        if (left < right) {
            std::swap(nums[left], nums[right]);
        }
    }
    return nums;
}

/**
 * 题目7: 按奇偶排序数组 II
 */
std::vector<int> sortArrayByParityII(std::vector<int>& nums) {
    int n = nums.size();
    int even = 0, odd = 1;

    while (even < n && odd < n) {
        // 找到偶数位上的奇数
        while (even < n && nums[even] % 2 == 0) even += 2;
        // 找到奇数位上的偶数
        while (odd < n && nums[odd] % 2 == 1) odd += 2;
        // 交换
        if (even < n && odd < n) {
            std::swap(nums[even], nums[odd]);
        }
    }
    return nums;
}

/**
 * 题目8: H 指数
 *
 * 计数排序：O(n) 时间
 */
int hIndex(std::vector<int>& citations) {
    int n = citations.size();
    std::vector<int> count(n + 1, 0);

    // 统计（大于 n 的统一计入 n）
    for (int c : citations) {
        ++count[std::min(c, n)];
    }

    // 从高到低累计
    int total = 0;
    for (int h = n; h >= 0; --h) {
        total += count[h];
        if (total >= h) {
            return h;
        }
    }
    return 0;
}

/**
 * 题目9: 根据字符出现频率排序
 *
 * 计数 + 桶排序
 */
std::string frequencySort(std::string s) {
    // 统计频率
    std::unordered_map<char, int> freq;
    for (char c : s) {
        ++freq[c];
    }

    // 桶排序：按频率分桶
    int maxFreq = 0;
    for (const auto& [c, f] : freq) {
        maxFreq = std::max(maxFreq, f);
    }

    std::vector<std::string> buckets(maxFreq + 1);
    for (const auto& [c, f] : freq) {
        buckets[f] += std::string(f, c);
    }

    // 从高频到低频合并
    std::string result;
    for (int i = maxFreq; i >= 1; --i) {
        result += buckets[i];
    }
    return result;
}

/**
 * 题目10: 排序颜色（荷兰国旗问题）
 *
 * 三路划分：维护三个指针
 * - [0, p0): 全是 0
 * - [p0, i): 全是 1
 * - (p2, n-1]: 全是 2
 */
void sortColors(std::vector<int>& nums) {
    int p0 = 0, p2 = nums.size() - 1;
    int i = 0;

    while (i <= p2) {
        if (nums[i] == 0) {
            std::swap(nums[i++], nums[p0++]);
        } else if (nums[i] == 2) {
            std::swap(nums[i], nums[p2--]);
            // 不移动 i，因为交换来的数还没检查
        } else {
            ++i;
        }
    }
}

/**
 * 题目11: 字母异位词分组
 *
 * 使用计数排序生成字符串的标准化表示
 */
std::vector<std::vector<std::string>> groupAnagrams(std::vector<std::string>& strs) {
    std::unordered_map<std::string, std::vector<std::string>> groups;

    for (const std::string& s : strs) {
        // 计数排序生成 key
        std::vector<int> count(26, 0);
        for (char c : s) {
            ++count[c - 'a'];
        }

        // 将计数转为字符串 key
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

/**
 * 附加: 字符串基数排序
 */
void radixSortStrings(std::vector<std::string>& strs) {
    if (strs.empty()) return;

    // 找到最长字符串长度
    size_t maxLen = 0;
    for (const auto& s : strs) {
        maxLen = std::max(maxLen, s.length());
    }

    // 从最低位开始排序
    for (int d = maxLen - 1; d >= 0; --d) {
        // 计数排序（256 个 ASCII 字符 + 1 个空字符）
        std::vector<std::vector<std::string>> buckets(257);

        for (const auto& s : strs) {
            int charVal = (d < static_cast<int>(s.length())) ? static_cast<unsigned char>(s[d]) + 1 : 0;
            buckets[charVal].push_back(s);
        }

        // 合并
        int idx = 0;
        for (const auto& bucket : buckets) {
            for (const auto& s : bucket) {
                strs[idx++] = s;
            }
        }
    }
}


// ==================== 测试代码 ====================
#include <cassert>

void testCountingSort() {
    std::vector<int> nums = {4, 2, 2, 8, 3, 3, 1};
    countingSort(nums);
    assert((nums == std::vector<int>{1, 2, 2, 3, 3, 4, 8}));
    std::cout << "countingSort: PASSED\n";
}

void testBucketSort() {
    std::vector<float> nums = {0.42f, 0.32f, 0.23f, 0.52f, 0.25f, 0.47f, 0.51f};
    bucketSort(nums);
    for (size_t i = 1; i < nums.size(); ++i) {
        assert(nums[i] >= nums[i-1]);
    }
    std::cout << "bucketSort: PASSED\n";
}

void testRadixSort() {
    std::vector<int> nums = {170, 45, 75, 90, 802, 24, 2, 66};
    radixSort(nums);
    assert((nums == std::vector<int>{2, 24, 45, 66, 75, 90, 170, 802}));

    // 测试负数
    std::vector<int> nums2 = {-3, 5, -1, 0, 2, -2};
    radixSort(nums2);
    assert((nums2 == std::vector<int>{-3, -2, -1, 0, 2, 5}));

    std::cout << "radixSort: PASSED\n";
}

void testMaximumGap() {
    std::vector<int> nums = {3, 6, 9, 1};
    assert(maximumGap(nums) == 3);

    std::vector<int> nums2 = {10};
    assert(maximumGap(nums2) == 0);

    std::cout << "maximumGap: PASSED\n";
}

void testFindKthLargest() {
    std::vector<int> nums = {3, 2, 1, 5, 6, 4};
    assert(findKthLargest(nums, 2) == 5);

    std::vector<int> nums2 = {3, 2, 3, 1, 2, 4, 5, 5, 6};
    assert(findKthLargest(nums2, 4) == 4);

    std::cout << "findKthLargest: PASSED\n";
}

void testSortColors() {
    std::vector<int> nums = {2, 0, 2, 1, 1, 0};
    sortColors(nums);
    assert((nums == std::vector<int>{0, 0, 1, 1, 2, 2}));
    std::cout << "sortColors: PASSED\n";
}

void testHIndex() {
    std::vector<int> citations = {3, 0, 6, 1, 5};
    assert(hIndex(citations) == 3);
    std::cout << "hIndex: PASSED\n";
}

void testFrequencySort() {
    std::string s = "tree";
    std::string result = frequencySort(s);
    assert(result[0] == 'e' && result[1] == 'e');
    std::cout << "frequencySort: PASSED\n";
}

void testSortArrayByParity() {
    std::vector<int> nums = {3, 1, 2, 4};
    auto result = sortArrayByParity(nums);
    // 验证偶数在前
    int i = 0;
    while (i < static_cast<int>(result.size()) && result[i] % 2 == 0) ++i;
    while (i < static_cast<int>(result.size())) {
        assert(result[i] % 2 == 1);
        ++i;
    }
    std::cout << "sortArrayByParity: PASSED\n";
}

void testGroupAnagrams() {
    std::vector<std::string> strs = {"eat", "tea", "tan", "ate", "nat", "bat"};
    auto result = groupAnagrams(strs);
    assert(result.size() == 3);
    std::cout << "groupAnagrams: PASSED\n";
}

int main() {
    std::cout << "=== Non-Comparison Sorts Solution Tests ===\n";

    testCountingSort();
    testBucketSort();
    testRadixSort();
    testMaximumGap();
    testFindKthLargest();
    testSortColors();
    testHIndex();
    testFrequencySort();
    testSortArrayByParity();
    testGroupAnagrams();

    std::cout << "\nAll tests passed!\n";
    return 0;
}
