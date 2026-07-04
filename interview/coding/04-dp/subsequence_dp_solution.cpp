/**
 * @file subsequence_dp_solution.cpp
 * @brief 子序列动态规划问题 - 参考答案
 */

#include "subsequence_dp.h"
#include <iostream>
#include <cassert>
#include <algorithm>

namespace SubsequenceDP {

// ==================== 参考答案实现 ====================

/**
 * 题目1: 最长公共子序列 (LCS) - 二维 DP
 * 时间复杂度: O(m * n) - 遍历两个字符串
 * 空间复杂度: O(m * n) - 二维 DP 数组
 */
int longestCommonSubsequenceSolution(const std::string& text1, const std::string& text2) {
    int m = static_cast<int>(text1.size()), n = static_cast<int>(text2.size());
    std::vector<std::vector<int>> dp(m + 1, std::vector<int>(n + 1, 0));

    for (int i = 1; i <= m; ++i) {
        for (int j = 1; j <= n; ++j) {
            if (text1[i - 1] == text2[j - 1]) {
                dp[i][j] = dp[i - 1][j - 1] + 1;
            } else {
                dp[i][j] = std::max(dp[i - 1][j], dp[i][j - 1]);
            }
        }
    }

    return dp[m][n];
}

/**
 * 题目2: 最长公共子串 - 二维 DP
 * 时间复杂度: O(m * n) - 遍历两个字符串
 * 空间复杂度: O(m * n) - 二维 DP 数组
 */
int longestCommonSubstringSolution(const std::string& str1, const std::string& str2) {
    int m = static_cast<int>(str1.size()), n = static_cast<int>(str2.size());
    std::vector<std::vector<int>> dp(m + 1, std::vector<int>(n + 1, 0));
    int maxLen = 0;

    for (int i = 1; i <= m; ++i) {
        for (int j = 1; j <= n; ++j) {
            if (str1[i - 1] == str2[j - 1]) {
                dp[i][j] = dp[i - 1][j - 1] + 1;
                maxLen = std::max(maxLen, dp[i][j]);
            }
        }
    }

    return maxLen;
}

/**
 * 题目3: 编辑距离 - 二维 DP
 * 时间复杂度: O(m * n) - 遍历两个字符串
 * 空间复杂度: O(m * n) - 二维 DP 数组
 */
int minDistanceSolution(const std::string& word1, const std::string& word2) {
    int m = static_cast<int>(word1.size()), n = static_cast<int>(word2.size());
    std::vector<std::vector<int>> dp(m + 1, std::vector<int>(n + 1));

    for (int i = 0; i <= m; ++i) dp[i][0] = i;
    for (int j = 0; j <= n; ++j) dp[0][j] = j;

    for (int i = 1; i <= m; ++i) {
        for (int j = 1; j <= n; ++j) {
            if (word1[i - 1] == word2[j - 1]) {
                dp[i][j] = dp[i - 1][j - 1];
            } else {
                dp[i][j] = std::min({dp[i - 1][j - 1], dp[i - 1][j], dp[i][j - 1]}) + 1;
            }
        }
    }

    return dp[m][n];
}

/**
 * 题目4: 两个字符串的删除操作 - 基于 LCS
 * 时间复杂度: O(m * n) - LCS 复杂度
 * 空间复杂度: O(m * n) - LCS 数组
 */
int minDeleteDistanceSolution(const std::string& word1, const std::string& word2) {
    int lcs = longestCommonSubsequenceSolution(word1, word2);
    return static_cast<int>(word1.size() + word2.size()) - 2 * lcs;
}

/**
 * 题目5: 不同的子序列 - 二维 DP
 * 时间复杂度: O(m * n) - m, n 分别为 s, t 长度
 * 空间复杂度: O(m * n) - 二维 DP 数组
 */
int numDistinctSolution(const std::string& s, const std::string& t) {
    int m = static_cast<int>(s.size()), n = static_cast<int>(t.size());
    if (m < n) return 0;

    std::vector<std::vector<unsigned long long>> dp(m + 1, std::vector<unsigned long long>(n + 1, 0));

    for (int i = 0; i <= m; ++i) dp[i][0] = 1;

    for (int i = 1; i <= m; ++i) {
        for (int j = 1; j <= n; ++j) {
            dp[i][j] = dp[i - 1][j];
            if (s[i - 1] == t[j - 1]) {
                dp[i][j] += dp[i - 1][j - 1];
            }
        }
    }

    return static_cast<int>(dp[m][n]);
}

/**
 * 题目6: 判断子序列 - 双指针
 * 时间复杂度: O(n) - n 为 t 的长度
 * 空间复杂度: O(1) - 仅用两个指针
 */
bool isSubsequenceSolution(const std::string& s, const std::string& t) {
    int i = 0, j = 0;
    while (i < static_cast<int>(s.size()) && j < static_cast<int>(t.size())) {
        if (s[i] == t[j]) {
            ++i;
        }
        ++j;
    }
    return i == static_cast<int>(s.size());
}

/**
 * 题目7: 最长回文子序列 - 区间 DP
 * 时间复杂度: O(n²) - 枚举所有区间
 * 空间复杂度: O(n²) - 二维 DP 数组
 */
int longestPalindromeSubseqSolution(const std::string& s) {
    int n = static_cast<int>(s.size());
    std::vector<std::vector<int>> dp(n, std::vector<int>(n, 0));

    for (int i = 0; i < n; ++i) dp[i][i] = 1;

    for (int len = 2; len <= n; ++len) {
        for (int i = 0; i + len - 1 < n; ++i) {
            int j = i + len - 1;
            if (s[i] == s[j]) {
                dp[i][j] = dp[i + 1][j - 1] + 2;
            } else {
                dp[i][j] = std::max(dp[i + 1][j], dp[i][j - 1]);
            }
        }
    }

    return dp[0][n - 1];
}

/**
 * 题目8: 最长递增子序列的个数 - DP + 计数
 * 时间复杂度: O(n²) - 双重循环
 * 空间复杂度: O(n) - dp 和 count 数组
 */
int findNumberOfLISSolution(std::vector<int>& nums) {
    int n = static_cast<int>(nums.size());
    if (n == 0) return 0;

    std::vector<int> dp(n, 1);
    std::vector<int> count(n, 1);

    int maxLen = 1;

    for (int i = 1; i < n; ++i) {
        for (int j = 0; j < i; ++j) {
            if (nums[j] < nums[i]) {
                if (dp[j] + 1 > dp[i]) {
                    dp[i] = dp[j] + 1;
                    count[i] = count[j];
                } else if (dp[j] + 1 == dp[i]) {
                    count[i] += count[j];
                }
            }
        }
        maxLen = std::max(maxLen, dp[i]);
    }

    int result = 0;
    for (int i = 0; i < n; ++i) {
        if (dp[i] == maxLen) {
            result += count[i];
        }
    }

    return result;
}

/**
 * 题目9: 俄罗斯套娃信封问题 - 排序 + LIS（二分优化）
 * 时间复杂度: O(n log n) - 排序 + 二分查找
 * 空间复杂度: O(n) - tails 数组
 */
int maxEnvelopesSolution(std::vector<std::vector<int>>& envelopes) {
    if (envelopes.empty()) return 0;

    std::sort(envelopes.begin(), envelopes.end(), [](const auto& a, const auto& b) {
        return a[0] < b[0] || (a[0] == b[0] && a[1] > b[1]);
    });

    std::vector<int> tails;
    for (const auto& env : envelopes) {
        int h = env[1];
        auto it = std::lower_bound(tails.begin(), tails.end(), h);
        if (it == tails.end()) {
            tails.push_back(h);
        } else {
            *it = h;
        }
    }

    return static_cast<int>(tails.size());
}

// ==================== 测试函数 ====================

void testSubsequenceDPSolution() {
    std::cout << "=== Subsequence DP Tests (Solution) ===\n";

    // longestCommonSubsequence
    assert(longestCommonSubsequenceSolution("abcde", "ace") == 3);
    assert(longestCommonSubsequenceSolution("abc", "abc") == 3);
    assert(longestCommonSubsequenceSolution("abc", "def") == 0);
    std::cout << "  longestCommonSubsequenceSolution: PASSED\n";

    // longestCommonSubstring
    assert(longestCommonSubstringSolution("abcde", "abfce") == 2);
    assert(longestCommonSubstringSolution("abc", "abc") == 3);
    assert(longestCommonSubstringSolution("abc", "def") == 0);
    std::cout << "  longestCommonSubstringSolution: PASSED\n";

    // minDistance
    assert(minDistanceSolution("horse", "ros") == 3);
    assert(minDistanceSolution("intention", "execution") == 5);
    assert(minDistanceSolution("", "a") == 1);
    std::cout << "  minDistanceSolution: PASSED\n";

    // minDeleteDistance
    assert(minDeleteDistanceSolution("sea", "eat") == 2);
    assert(minDeleteDistanceSolution("leetcode", "etco") == 4);
    std::cout << "  minDeleteDistanceSolution: PASSED\n";

    // numDistinct
    assert(numDistinctSolution("rabbbit", "rabbit") == 3);
    assert(numDistinctSolution("babgbag", "bag") == 5);
    std::cout << "  numDistinctSolution: PASSED\n";

    // isSubsequence
    assert(isSubsequenceSolution("abc", "ahbgdc") == true);
    assert(isSubsequenceSolution("axc", "ahbgdc") == false);
    std::cout << "  isSubsequenceSolution: PASSED\n";

    // longestPalindromeSubseq
    assert(longestPalindromeSubseqSolution("bbbab") == 4);
    assert(longestPalindromeSubseqSolution("cbbd") == 2);
    std::cout << "  longestPalindromeSubseqSolution: PASSED\n";

    // findNumberOfLIS
    std::vector<int> nums = {1, 3, 5, 4, 7};
    assert(findNumberOfLISSolution(nums) == 2);
    nums = {2, 2, 2, 2, 2};
    assert(findNumberOfLISSolution(nums) == 5);
    std::cout << "  findNumberOfLISSolution: PASSED\n";

    // maxEnvelopes
    std::vector<std::vector<int>> env = {{5, 4}, {6, 4}, {6, 7}, {2, 3}};
    assert(maxEnvelopesSolution(env) == 3);
    env = {{1, 1}, {1, 1}, {1, 1}};
    assert(maxEnvelopesSolution(env) == 1);
    std::cout << "  maxEnvelopesSolution: PASSED\n";
}

} // namespace SubsequenceDP
