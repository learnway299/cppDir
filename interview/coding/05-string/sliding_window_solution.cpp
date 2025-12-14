/**
 * @file sliding_window_solution.cpp
 * @brief 滑动窗口问题 - 参考答案
 */

#include "sliding_window.h"
#include <iostream>
#include <cassert>
#include <unordered_map>
#include <unordered_set>
#include <deque>
#include <algorithm>
#include <climits>
#include <cmath>

namespace SlidingWindowProblems {

// ==================== 参考答案实现 ====================

// 题目1: 无重复字符的最长子串
int lengthOfLongestSubstringSolution(std::string s) {
    std::unordered_set<char> window;
    int left = 0, maxLen = 0;

    for (int right = 0; right < static_cast<int>(s.size()); ++right) {
        while (window.count(s[right])) {
            window.erase(s[left]);
            ++left;
        }
        window.insert(s[right]);
        maxLen = std::max(maxLen, right - left + 1);
    }

    return maxLen;
}

// 题目2: 最小覆盖子串
std::string minWindowSolution(std::string s, std::string t) {
    if (s.empty() || t.empty() || s.size() < t.size()) return "";

    std::unordered_map<char, int> need, window;
    for (char c : t) {
        ++need[c];
    }

    int left = 0, right = 0;
    int valid = 0;
    int start = 0, minLen = INT_MAX;

    while (right < static_cast<int>(s.size())) {
        char c = s[right];
        ++right;

        if (need.count(c)) {
            ++window[c];
            if (window[c] == need[c]) {
                ++valid;
            }
        }

        while (valid == static_cast<int>(need.size())) {
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

// 题目3: 字符串的排列
bool checkInclusionSolution(std::string s1, std::string s2) {
    if (s1.size() > s2.size()) return false;

    std::vector<int> need(26, 0), window(26, 0);
    for (char c : s1) {
        ++need[c - 'a'];
    }

    int k = static_cast<int>(s1.size());

    for (int i = 0; i < static_cast<int>(s2.size()); ++i) {
        ++window[s2[i] - 'a'];

        if (i >= k) {
            --window[s2[i - k] - 'a'];
        }

        if (window == need) {
            return true;
        }
    }

    return false;
}

// 题目4: 找到字符串中所有字母异位词
std::vector<int> findAnagramsSolution(std::string s, std::string p) {
    std::vector<int> result;
    if (s.size() < p.size()) return result;

    std::vector<int> need(26, 0), window(26, 0);
    for (char c : p) {
        ++need[c - 'a'];
    }

    int k = static_cast<int>(p.size());

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

// 题目5: 长度最小的子数组
int minSubArrayLenSolution(int target, std::vector<int>& nums) {
    int left = 0, sum = 0;
    int minLen = INT_MAX;

    for (int right = 0; right < static_cast<int>(nums.size()); ++right) {
        sum += nums[right];

        while (sum >= target) {
            minLen = std::min(minLen, right - left + 1);
            sum -= nums[left];
            ++left;
        }
    }

    return minLen == INT_MAX ? 0 : minLen;
}

// 题目6: 滑动窗口最大值
std::vector<int> maxSlidingWindowSolution(std::vector<int>& nums, int k) {
    std::vector<int> result;
    std::deque<int> dq;

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

// 题目7: 至多包含两个不同字符的最长子串
int lengthOfLongestSubstringTwoDistinctSolution(std::string s) {
    std::unordered_map<char, int> window;
    int left = 0, maxLen = 0;

    for (int right = 0; right < static_cast<int>(s.size()); ++right) {
        ++window[s[right]];

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

// 题目8: 至多包含 K 个不同字符的最长子串
int lengthOfLongestSubstringKDistinctSolution(std::string s, int k) {
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

// 题目9: 替换后的最长重复字符
int characterReplacementSolution(std::string s, int k) {
    std::vector<int> count(26, 0);
    int left = 0, maxCount = 0, maxLen = 0;

    for (int right = 0; right < static_cast<int>(s.size()); ++right) {
        ++count[s[right] - 'A'];
        maxCount = std::max(maxCount, count[s[right] - 'A']);

        while (right - left + 1 - maxCount > k) {
            --count[s[left] - 'A'];
            ++left;
        }

        maxLen = std::max(maxLen, right - left + 1);
    }

    return maxLen;
}

// 题目10: 子数组最大平均数 I
double findMaxAverageSolution(std::vector<int>& nums, int k) {
    double sum = 0;

    for (int i = 0; i < k; ++i) {
        sum += nums[i];
    }

    double maxSum = sum;

    for (int i = k; i < static_cast<int>(nums.size()); ++i) {
        sum += nums[i] - nums[i - k];
        maxSum = std::max(maxSum, sum);
    }

    return maxSum / k;
}

// ==================== 测试函数 ====================

void testSlidingWindowSolution() {
    std::cout << "=== Sliding Window Tests (Solution) ===\n";

    // lengthOfLongestSubstring
    assert(lengthOfLongestSubstringSolution("abcabcbb") == 3);
    assert(lengthOfLongestSubstringSolution("bbbbb") == 1);
    assert(lengthOfLongestSubstringSolution("pwwkew") == 3);
    assert(lengthOfLongestSubstringSolution("") == 0);
    std::cout << "  lengthOfLongestSubstringSolution: PASSED\n";

    // minWindow
    assert(minWindowSolution("ADOBECODEBANC", "ABC") == "BANC");
    assert(minWindowSolution("a", "a") == "a");
    assert(minWindowSolution("a", "aa") == "");
    std::cout << "  minWindowSolution: PASSED\n";

    // checkInclusion
    assert(checkInclusionSolution("ab", "eidbaooo") == true);
    assert(checkInclusionSolution("ab", "eidboaoo") == false);
    std::cout << "  checkInclusionSolution: PASSED\n";

    // findAnagrams
    std::vector<int> expected1 = {0, 6};
    assert(findAnagramsSolution("cbaebabacd", "abc") == expected1);
    std::vector<int> expected2 = {0, 1, 2};
    assert(findAnagramsSolution("abab", "ab") == expected2);
    std::cout << "  findAnagramsSolution: PASSED\n";

    // minSubArrayLen
    std::vector<int> nums1 = {2, 3, 1, 2, 4, 3};
    assert(minSubArrayLenSolution(7, nums1) == 2);
    std::vector<int> nums2 = {1, 4, 4};
    assert(minSubArrayLenSolution(4, nums2) == 1);
    std::vector<int> nums3 = {1, 1, 1, 1, 1, 1, 1, 1};
    assert(minSubArrayLenSolution(11, nums3) == 0);
    std::cout << "  minSubArrayLenSolution: PASSED\n";

    // maxSlidingWindow
    std::vector<int> nums = {1, 3, -1, -3, 5, 3, 6, 7};
    std::vector<int> expected = {3, 3, 5, 5, 6, 7};
    assert(maxSlidingWindowSolution(nums, 3) == expected);
    std::cout << "  maxSlidingWindowSolution: PASSED\n";

    // lengthOfLongestSubstringTwoDistinct
    assert(lengthOfLongestSubstringTwoDistinctSolution("eceba") == 3);
    assert(lengthOfLongestSubstringTwoDistinctSolution("ccaabbb") == 5);
    std::cout << "  lengthOfLongestSubstringTwoDistinctSolution: PASSED\n";

    // lengthOfLongestSubstringKDistinct
    assert(lengthOfLongestSubstringKDistinctSolution("eceba", 2) == 3);
    assert(lengthOfLongestSubstringKDistinctSolution("aa", 1) == 2);
    std::cout << "  lengthOfLongestSubstringKDistinctSolution: PASSED\n";

    // characterReplacement
    assert(characterReplacementSolution("ABAB", 2) == 4);
    assert(characterReplacementSolution("AABABBA", 1) == 4);
    std::cout << "  characterReplacementSolution: PASSED\n";

    // findMaxAverage
    std::vector<int> avgNums = {1, 12, -5, -6, 50, 3};
    assert(std::abs(findMaxAverageSolution(avgNums, 4) - 12.75) < 1e-5);
    std::cout << "  findMaxAverageSolution: PASSED\n";
}

} // namespace SlidingWindowProblems
