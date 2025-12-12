/**
 * @file sliding_window_solution.cpp
 * @brief 滑动窗口问题 - 解答文件
 *
 * 滑动窗口模板：
 * 1. 初始化左右指针 left = right = 0
 * 2. 扩展右边界：right++，更新窗口状态
 * 3. 收缩左边界：当窗口不满足条件时，left++，更新窗口状态
 * 4. 在合适的时机更新答案
 */

#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <deque>
#include <algorithm>
#include <climits>

/**
 * 题目1: 无重复字符的最长子串
 *
 * 解法: 滑动窗口 + 哈希集合
 * 时间复杂度: O(n)
 * 空间复杂度: O(min(n, m))，m 为字符集大小
 */
int lengthOfLongestSubstring(std::string s) {
    std::unordered_set<char> window;
    int left = 0, maxLen = 0;

    for (int right = 0; right < static_cast<int>(s.size()); ++right) {
        // 收缩窗口直到没有重复
        while (window.count(s[right])) {
            window.erase(s[left]);
            ++left;
        }
        window.insert(s[right]);
        maxLen = std::max(maxLen, right - left + 1);
    }

    return maxLen;
}

/**
 * 优化解法: 使用哈希表记录字符最后出现位置
 * 可以直接跳转 left 到重复字符的下一位
 */
int lengthOfLongestSubstringOptimized(std::string s) {
    std::unordered_map<char, int> lastPos;
    int left = 0, maxLen = 0;

    for (int right = 0; right < static_cast<int>(s.size()); ++right) {
        if (lastPos.count(s[right]) && lastPos[s[right]] >= left) {
            left = lastPos[s[right]] + 1;
        }
        lastPos[s[right]] = right;
        maxLen = std::max(maxLen, right - left + 1);
    }

    return maxLen;
}

/**
 * 题目2: 最小覆盖子串
 *
 * 解法: 滑动窗口 + 计数
 * 时间复杂度: O(n + m)
 * 空间复杂度: O(字符集大小)
 */
std::string minWindow(std::string s, std::string t) {
    if (s.empty() || t.empty() || s.size() < t.size()) return "";

    std::unordered_map<char, int> need, window;
    for (char c : t) {
        ++need[c];
    }

    int left = 0, right = 0;
    int valid = 0;  // 满足条件的字符种类数
    int start = 0, minLen = INT_MAX;

    while (right < static_cast<int>(s.size())) {
        char c = s[right];
        ++right;

        // 更新窗口
        if (need.count(c)) {
            ++window[c];
            if (window[c] == need[c]) {
                ++valid;
            }
        }

        // 收缩窗口
        while (valid == static_cast<int>(need.size())) {
            // 更新最小覆盖子串
            if (right - left < minLen) {
                start = left;
                minLen = right - left;
            }

            char d = s[left];
            ++left;

            if (need.count(d)) {
                if (window[d] == need[d]) {
                    --valid;
                }
                --window[d];
            }
        }
    }

    return minLen == INT_MAX ? "" : s.substr(start, minLen);
}

/**
 * 题目3: 字符串的排列
 *
 * 解法: 固定大小滑动窗口
 * 时间复杂度: O(n)
 * 空间复杂度: O(1)，字符集固定为26
 */
bool checkInclusion(std::string s1, std::string s2) {
    if (s1.size() > s2.size()) return false;

    std::vector<int> need(26, 0), window(26, 0);
    for (char c : s1) {
        ++need[c - 'a'];
    }

    int k = s1.size();

    for (int i = 0; i < static_cast<int>(s2.size()); ++i) {
        // 添加右边元素
        ++window[s2[i] - 'a'];

        // 移除左边元素（窗口大小超过 k）
        if (i >= k) {
            --window[s2[i - k] - 'a'];
        }

        // 检查是否匹配
        if (window == need) {
            return true;
        }
    }

    return false;
}

/**
 * 使用计数差值的解法
 */
bool checkInclusionDiff(std::string s1, std::string s2) {
    if (s1.size() > s2.size()) return false;

    std::vector<int> diff(26, 0);
    int k = s1.size();

    // 初始化差值
    for (int i = 0; i < k; ++i) {
        --diff[s1[i] - 'a'];
        ++diff[s2[i] - 'a'];
    }

    auto isZero = [&]() {
        for (int d : diff) {
            if (d != 0) return false;
        }
        return true;
    };

    if (isZero()) return true;

    for (int i = k; i < static_cast<int>(s2.size()); ++i) {
        ++diff[s2[i] - 'a'];
        --diff[s2[i - k] - 'a'];
        if (isZero()) return true;
    }

    return false;
}

/**
 * 题目4: 找到字符串中所有字母异位词
 *
 * 解法: 固定大小滑动窗口
 * 时间复杂度: O(n)
 * 空间复杂度: O(1)
 */
std::vector<int> findAnagrams(std::string s, std::string p) {
    std::vector<int> result;
    if (s.size() < p.size()) return result;

    std::vector<int> need(26, 0), window(26, 0);
    for (char c : p) {
        ++need[c - 'a'];
    }

    int k = p.size();

    for (int i = 0; i < static_cast<int>(s.size()); ++i) {
        ++window[s[i] - 'a'];

        if (i >= k) {
            --window[s[i - k] - 'a'];
        }

        if (i >= k - 1 && window == need) {
            result.push_back(i - k + 1);
        }
    }

    return result;
}

/**
 * 题目5: 长度最小的子数组
 *
 * 解法: 可变大小滑动窗口
 * 时间复杂度: O(n)
 * 空间复杂度: O(1)
 */
int minSubArrayLen(int target, std::vector<int>& nums) {
    int left = 0, sum = 0;
    int minLen = INT_MAX;

    for (int right = 0; right < static_cast<int>(nums.size()); ++right) {
        sum += nums[right];

        // 收缩窗口
        while (sum >= target) {
            minLen = std::min(minLen, right - left + 1);
            sum -= nums[left];
            ++left;
        }
    }

    return minLen == INT_MAX ? 0 : minLen;
}

/**
 * 二分查找解法（前缀和 + 二分）
 * 时间复杂度: O(n log n)
 */
int minSubArrayLenBinarySearch(int target, std::vector<int>& nums) {
    int n = nums.size();
    std::vector<int> prefix(n + 1, 0);

    for (int i = 0; i < n; ++i) {
        prefix[i + 1] = prefix[i] + nums[i];
    }

    int minLen = INT_MAX;

    for (int i = 0; i < n; ++i) {
        // 找最小的 j 使得 prefix[j] - prefix[i] >= target
        int need = prefix[i] + target;
        auto it = std::lower_bound(prefix.begin(), prefix.end(), need);
        if (it != prefix.end()) {
            int j = it - prefix.begin();
            minLen = std::min(minLen, j - i);
        }
    }

    return minLen == INT_MAX ? 0 : minLen;
}

/**
 * 题目6: 滑动窗口最大值
 *
 * 解法: 单调递减队列
 * 时间复杂度: O(n)
 * 空间复杂度: O(k)
 */
std::vector<int> maxSlidingWindow(std::vector<int>& nums, int k) {
    std::vector<int> result;
    std::deque<int> dq;  // 存储索引，维护单调递减

    for (int i = 0; i < static_cast<int>(nums.size()); ++i) {
        // 移除队首超出窗口范围的元素
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
 * 题目7: 至多包含两个不同字符的最长子串
 *
 * 解法: 滑动窗口 + 哈希表
 * 时间复杂度: O(n)
 * 空间复杂度: O(1)
 */
int lengthOfLongestSubstringTwoDistinct(std::string s) {
    std::unordered_map<char, int> window;
    int left = 0, maxLen = 0;

    for (int right = 0; right < static_cast<int>(s.size()); ++right) {
        ++window[s[right]];

        // 收缩窗口直到最多两种字符
        while (window.size() > 2) {
            if (--window[s[left]] == 0) {
                window.erase(s[left]);
            }
            ++left;
        }

        maxLen = std::max(maxLen, right - left + 1);
    }

    return maxLen;
}

/**
 * 题目8: 至多包含 K 个不同字符的最长子串
 *
 * 解法: 滑动窗口 + 哈希表
 * 时间复杂度: O(n)
 * 空间复杂度: O(k)
 */
int lengthOfLongestSubstringKDistinct(std::string s, int k) {
    if (k == 0) return 0;

    std::unordered_map<char, int> window;
    int left = 0, maxLen = 0;

    for (int right = 0; right < static_cast<int>(s.size()); ++right) {
        ++window[s[right]];

        while (static_cast<int>(window.size()) > k) {
            if (--window[s[left]] == 0) {
                window.erase(s[left]);
            }
            ++left;
        }

        maxLen = std::max(maxLen, right - left + 1);
    }

    return maxLen;
}

/**
 * 题目9: 替换后的最长重复字符
 *
 * 解法: 滑动窗口
 * 关键：窗口长度 - 窗口内最多字符数 <= k
 *
 * 时间复杂度: O(n)
 * 空间复杂度: O(1)
 */
int characterReplacement(std::string s, int k) {
    std::vector<int> count(26, 0);
    int left = 0, maxCount = 0, maxLen = 0;

    for (int right = 0; right < static_cast<int>(s.size()); ++right) {
        ++count[s[right] - 'A'];
        maxCount = std::max(maxCount, count[s[right] - 'A']);

        // 如果需要替换的字符数 > k，收缩窗口
        // 窗口长度 - 最多字符数 = 需要替换的字符数
        while (right - left + 1 - maxCount > k) {
            --count[s[left] - 'A'];
            ++left;
        }

        maxLen = std::max(maxLen, right - left + 1);
    }

    return maxLen;
}

/**
 * 优化：maxCount 不需要更新减小
 * 因为我们只关心更大的窗口
 */
int characterReplacementOptimized(std::string s, int k) {
    std::vector<int> count(26, 0);
    int left = 0, maxCount = 0;

    for (int right = 0; right < static_cast<int>(s.size()); ++right) {
        ++count[s[right] - 'A'];
        maxCount = std::max(maxCount, count[s[right] - 'A']);

        // 注意：这里用 if 而不是 while
        // maxCount 不减小，所以窗口只会增大或保持不变
        if (right - left + 1 - maxCount > k) {
            --count[s[left] - 'A'];
            ++left;
        }
    }

    return s.size() - left;
}

/**
 * 题目10: 子数组最大平均数 I
 *
 * 解法: 固定大小滑动窗口
 * 时间复杂度: O(n)
 * 空间复杂度: O(1)
 */
double findMaxAverage(std::vector<int>& nums, int k) {
    double sum = 0;

    // 计算第一个窗口的和
    for (int i = 0; i < k; ++i) {
        sum += nums[i];
    }

    double maxSum = sum;

    // 滑动窗口
    for (int i = k; i < static_cast<int>(nums.size()); ++i) {
        sum += nums[i] - nums[i - k];
        maxSum = std::max(maxSum, sum);
    }

    return maxSum / k;
}


// ==================== 滑动窗口模板总结 ====================
/**
 * 模板一：可变窗口（找最长）
 *
 * int left = 0;
 * for (int right = 0; right < n; ++right) {
 *     // 更新窗口状态
 *     window.add(arr[right]);
 *
 *     // 当不满足条件时，收缩窗口
 *     while (!valid(window)) {
 *         window.remove(arr[left]);
 *         ++left;
 *     }
 *
 *     // 更新答案（窗口满足条件）
 *     ans = max(ans, right - left + 1);
 * }
 *
 * 模板二：可变窗口（找最短）
 *
 * int left = 0;
 * for (int right = 0; right < n; ++right) {
 *     // 更新窗口状态
 *     window.add(arr[right]);
 *
 *     // 当满足条件时，更新答案并收缩窗口
 *     while (valid(window)) {
 *         ans = min(ans, right - left + 1);
 *         window.remove(arr[left]);
 *         ++left;
 *     }
 * }
 *
 * 模板三：固定大小窗口
 *
 * for (int i = 0; i < n; ++i) {
 *     // 添加右边元素
 *     window.add(arr[i]);
 *
 *     // 移除左边元素（超出窗口）
 *     if (i >= k) {
 *         window.remove(arr[i - k]);
 *     }
 *
 *     // 窗口形成后处理
 *     if (i >= k - 1) {
 *         // 处理窗口
 *     }
 * }
 */


// ==================== 测试代码 ====================
#include <cassert>
#include <cmath>

void testLengthOfLongestSubstring() {
    assert(lengthOfLongestSubstring("abcabcbb") == 3);
    assert(lengthOfLongestSubstring("bbbbb") == 1);
    assert(lengthOfLongestSubstring("pwwkew") == 3);
    assert(lengthOfLongestSubstring("") == 0);

    assert(lengthOfLongestSubstringOptimized("abcabcbb") == 3);

    std::cout << "lengthOfLongestSubstring: PASSED\n";
}

void testMinWindow() {
    assert(minWindow("ADOBECODEBANC", "ABC") == "BANC");
    assert(minWindow("a", "a") == "a");
    assert(minWindow("a", "aa") == "");

    std::cout << "minWindow: PASSED\n";
}

void testCheckInclusion() {
    assert(checkInclusion("ab", "eidbaooo") == true);
    assert(checkInclusion("ab", "eidboaoo") == false);

    assert(checkInclusionDiff("ab", "eidbaooo") == true);

    std::cout << "checkInclusion: PASSED\n";
}

void testFindAnagrams() {
    std::vector<int> expected1 = {0, 6};
    assert(findAnagrams("cbaebabacd", "abc") == expected1);

    std::vector<int> expected2 = {0, 1, 2};
    assert(findAnagrams("abab", "ab") == expected2);

    std::cout << "findAnagrams: PASSED\n";
}

void testMinSubArrayLen() {
    std::vector<int> nums1 = {2, 3, 1, 2, 4, 3};
    assert(minSubArrayLen(7, nums1) == 2);

    std::vector<int> nums2 = {1, 4, 4};
    assert(minSubArrayLen(4, nums2) == 1);

    std::vector<int> nums3 = {1, 1, 1, 1, 1, 1, 1, 1};
    assert(minSubArrayLen(11, nums3) == 0);

    std::cout << "minSubArrayLen: PASSED\n";
}

void testMaxSlidingWindow() {
    std::vector<int> nums = {1, 3, -1, -3, 5, 3, 6, 7};
    std::vector<int> expected = {3, 3, 5, 5, 6, 7};
    assert(maxSlidingWindow(nums, 3) == expected);

    std::cout << "maxSlidingWindow: PASSED\n";
}

void testLengthOfLongestSubstringTwoDistinct() {
    assert(lengthOfLongestSubstringTwoDistinct("eceba") == 3);
    assert(lengthOfLongestSubstringTwoDistinct("ccaabbb") == 5);

    std::cout << "lengthOfLongestSubstringTwoDistinct: PASSED\n";
}

void testLengthOfLongestSubstringKDistinct() {
    assert(lengthOfLongestSubstringKDistinct("eceba", 2) == 3);
    assert(lengthOfLongestSubstringKDistinct("aa", 1) == 2);

    std::cout << "lengthOfLongestSubstringKDistinct: PASSED\n";
}

void testCharacterReplacement() {
    assert(characterReplacement("ABAB", 2) == 4);
    assert(characterReplacement("AABABBA", 1) == 4);

    assert(characterReplacementOptimized("ABAB", 2) == 4);

    std::cout << "characterReplacement: PASSED\n";
}

void testFindMaxAverage() {
    std::vector<int> nums = {1, 12, -5, -6, 50, 3};
    assert(std::abs(findMaxAverage(nums, 4) - 12.75) < 1e-5);

    std::cout << "findMaxAverage: PASSED\n";
}

int main() {
    std::cout << "=== Sliding Window Solution Tests ===\n";

    testLengthOfLongestSubstring();
    testMinWindow();
    testCheckInclusion();
    testFindAnagrams();
    testMinSubArrayLen();
    testMaxSlidingWindow();
    testLengthOfLongestSubstringTwoDistinct();
    testLengthOfLongestSubstringKDistinct();
    testCharacterReplacement();
    testFindMaxAverage();

    std::cout << "\nAll tests passed!\n";
    return 0;
}
