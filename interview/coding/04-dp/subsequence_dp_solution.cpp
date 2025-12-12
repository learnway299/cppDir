/**
 * @file subsequence_dp_solution.cpp
 * @brief 子序列动态规划问题 - 解答文件
 *
 * 包含 subsequence_dp.cpp 中所有题目的解答和详细解析。
 */

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

/**
 * 题目1: 最长公共子序列 (LCS)
 *
 * 解法: 二维 DP
 * 时间复杂度: O(m*n)
 * 空间复杂度: O(m*n)，可优化到 O(min(m,n))
 */
int longestCommonSubsequence(std::string text1, std::string text2) {
    int m = text1.size(), n = text2.size();
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
 * 空间优化版本
 */
int longestCommonSubsequenceOptimized(std::string text1, std::string text2) {
    int m = text1.size(), n = text2.size();
    if (m < n) {
        std::swap(text1, text2);
        std::swap(m, n);
    }

    std::vector<int> dp(n + 1, 0);

    for (int i = 1; i <= m; ++i) {
        int prev = 0;
        for (int j = 1; j <= n; ++j) {
            int temp = dp[j];
            if (text1[i - 1] == text2[j - 1]) {
                dp[j] = prev + 1;
            } else {
                dp[j] = std::max(dp[j], dp[j - 1]);
            }
            prev = temp;
        }
    }

    return dp[n];
}

/**
 * 题目2: 最长公共子串
 *
 * 解法: DP
 * 时间复杂度: O(m*n)
 * 空间复杂度: O(m*n)
 */
int longestCommonSubstring(std::string str1, std::string str2) {
    int m = str1.size(), n = str2.size();
    std::vector<std::vector<int>> dp(m + 1, std::vector<int>(n + 1, 0));
    int maxLen = 0;

    for (int i = 1; i <= m; ++i) {
        for (int j = 1; j <= n; ++j) {
            if (str1[i - 1] == str2[j - 1]) {
                dp[i][j] = dp[i - 1][j - 1] + 1;
                maxLen = std::max(maxLen, dp[i][j]);
            }
            // 不相等时 dp[i][j] = 0，因为子串必须连续
        }
    }

    return maxLen;
}

/**
 * 题目3: 编辑距离
 *
 * 解法: DP
 * dp[i][j] = word1[0..i-1] 转换为 word2[0..j-1] 的最小操作数
 *
 * 状态转移:
 *   if word1[i-1] == word2[j-1]:
 *       dp[i][j] = dp[i-1][j-1]  // 不需要操作
 *   else:
 *       dp[i][j] = min(
 *           dp[i-1][j-1] + 1,  // 替换
 *           dp[i-1][j] + 1,    // 删除 word1[i-1]
 *           dp[i][j-1] + 1     // 插入 word2[j-1]
 *       )
 *
 * 时间复杂度: O(m*n)
 * 空间复杂度: O(m*n)
 */
int minDistance(std::string word1, std::string word2) {
    int m = word1.size(), n = word2.size();
    std::vector<std::vector<int>> dp(m + 1, std::vector<int>(n + 1));

    // 初始化：空串转换需要的操作数
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
 * 空间优化版本
 */
int minDistanceOptimized(std::string word1, std::string word2) {
    int m = word1.size(), n = word2.size();
    std::vector<int> dp(n + 1);

    for (int j = 0; j <= n; ++j) dp[j] = j;

    for (int i = 1; i <= m; ++i) {
        int prev = dp[0];
        dp[0] = i;
        for (int j = 1; j <= n; ++j) {
            int temp = dp[j];
            if (word1[i - 1] == word2[j - 1]) {
                dp[j] = prev;
            } else {
                dp[j] = std::min({prev, dp[j], dp[j - 1]}) + 1;
            }
            prev = temp;
        }
    }

    return dp[n];
}

/**
 * 题目4: 两个字符串的删除操作
 *
 * 解法: 基于 LCS
 * 答案 = len(word1) + len(word2) - 2 * LCS
 */
int minDeleteDistance(std::string word1, std::string word2) {
    int lcs = longestCommonSubsequence(word1, word2);
    return word1.size() + word2.size() - 2 * lcs;
}

/**
 * 直接 DP 解法
 */
int minDeleteDistanceDP(std::string word1, std::string word2) {
    int m = word1.size(), n = word2.size();
    std::vector<std::vector<int>> dp(m + 1, std::vector<int>(n + 1));

    for (int i = 0; i <= m; ++i) dp[i][0] = i;
    for (int j = 0; j <= n; ++j) dp[0][j] = j;

    for (int i = 1; i <= m; ++i) {
        for (int j = 1; j <= n; ++j) {
            if (word1[i - 1] == word2[j - 1]) {
                dp[i][j] = dp[i - 1][j - 1];
            } else {
                dp[i][j] = std::min(dp[i - 1][j], dp[i][j - 1]) + 1;
            }
        }
    }

    return dp[m][n];
}

/**
 * 题目5: 不同的子序列
 *
 * 解法: DP
 * dp[i][j] = s[0..i-1] 的子序列中 t[0..j-1] 出现的个数
 *
 * 时间复杂度: O(m*n)
 * 空间复杂度: O(m*n)
 */
int numDistinct(std::string s, std::string t) {
    int m = s.size(), n = t.size();
    if (m < n) return 0;

    std::vector<std::vector<unsigned long long>> dp(m + 1, std::vector<unsigned long long>(n + 1, 0));

    // 空串 t 可以在任何 s 中匹配一次
    for (int i = 0; i <= m; ++i) dp[i][0] = 1;

    for (int i = 1; i <= m; ++i) {
        for (int j = 1; j <= n; ++j) {
            // 不使用 s[i-1]
            dp[i][j] = dp[i - 1][j];

            // 使用 s[i-1]（如果匹配）
            if (s[i - 1] == t[j - 1]) {
                dp[i][j] += dp[i - 1][j - 1];
            }
        }
    }

    return static_cast<int>(dp[m][n]);
}

/**
 * 题目6: 判断子序列
 *
 * 解法1: 双指针
 * 时间复杂度: O(n)
 * 空间复杂度: O(1)
 */
bool isSubsequence(std::string s, std::string t) {
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
 * 解法2: DP（进阶：大量查询时预处理 t）
 * 预处理 next[i][c] = t[i..] 中第一个字符 c 出现的位置
 */
bool isSubsequenceDP(std::string s, std::string t) {
    int m = s.size(), n = t.size();

    // 预处理：next[i][c] = t[i..] 中第一个字符 c 的位置
    std::vector<std::vector<int>> next(n + 1, std::vector<int>(26, n));
    for (int i = n - 1; i >= 0; --i) {
        for (int c = 0; c < 26; ++c) {
            next[i][c] = next[i + 1][c];
        }
        next[i][t[i] - 'a'] = i;
    }

    // 匹配
    int j = 0;
    for (char c : s) {
        if (next[j][c - 'a'] == n) return false;
        j = next[j][c - 'a'] + 1;
    }

    return true;
}

/**
 * 题目7: 最长回文子序列
 *
 * 解法1: 转化为 LCS
 * 最长回文子序列 = LCS(s, reverse(s))
 */
int longestPalindromeSubseqLCS(std::string s) {
    std::string t = s;
    std::reverse(t.begin(), t.end());
    return longestCommonSubsequence(s, t);
}

/**
 * 解法2: 区间 DP
 * dp[i][j] = s[i..j] 的最长回文子序列长度
 *
 * 时间复杂度: O(n^2)
 * 空间复杂度: O(n^2)
 */
int longestPalindromeSubseq(std::string s) {
    int n = s.size();
    std::vector<std::vector<int>> dp(n, std::vector<int>(n, 0));

    // 单个字符是长度为 1 的回文
    for (int i = 0; i < n; ++i) dp[i][i] = 1;

    // 区间 DP：从小区间到大区间
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
 * 题目8: 最长递增子序列的个数
 *
 * 解法: 同时维护长度和个数
 * dp[i] = 以 nums[i] 结尾的 LIS 长度
 * count[i] = 以 nums[i] 结尾的 LIS 个数
 *
 * 时间复杂度: O(n^2)
 * 空间复杂度: O(n)
 */
int findNumberOfLIS(std::vector<int>& nums) {
    int n = nums.size();
    if (n == 0) return 0;

    std::vector<int> dp(n, 1);      // 长度
    std::vector<int> count(n, 1);   // 个数

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

    // 统计所有长度为 maxLen 的 LIS 个数
    int result = 0;
    for (int i = 0; i < n; ++i) {
        if (dp[i] == maxLen) {
            result += count[i];
        }
    }

    return result;
}

/**
 * 题目9: 俄罗斯套娃信封问题
 *
 * 解法: 排序 + LIS
 * 按宽度升序排序，宽度相同时按高度降序排序
 * 然后对高度求 LIS
 *
 * 为什么高度降序？
 * 相同宽度的信封不能嵌套，降序保证相同宽度最多只选一个
 *
 * 时间复杂度: O(n log n)
 * 空间复杂度: O(n)
 */
int maxEnvelopes(std::vector<std::vector<int>>& envelopes) {
    if (envelopes.empty()) return 0;

    // 按宽度升序，高度降序排序
    std::sort(envelopes.begin(), envelopes.end(), [](const auto& a, const auto& b) {
        return a[0] < b[0] || (a[0] == b[0] && a[1] > b[1]);
    });

    // 对高度求 LIS（二分优化）
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

    return tails.size();
}


// ==================== 测试代码 ====================
#include <cassert>

void testLongestCommonSubsequence() {
    assert(longestCommonSubsequence("abcde", "ace") == 3);
    assert(longestCommonSubsequence("abc", "abc") == 3);
    assert(longestCommonSubsequence("abc", "def") == 0);
    assert(longestCommonSubsequenceOptimized("abcde", "ace") == 3);

    std::cout << "longestCommonSubsequence: PASSED\n";
}

void testLongestCommonSubstring() {
    assert(longestCommonSubstring("abcde", "abfce") == 2);
    assert(longestCommonSubstring("abc", "abc") == 3);
    assert(longestCommonSubstring("abc", "def") == 0);

    std::cout << "longestCommonSubstring: PASSED\n";
}

void testMinDistance() {
    assert(minDistance("horse", "ros") == 3);
    assert(minDistance("intention", "execution") == 5);
    assert(minDistance("", "a") == 1);
    assert(minDistanceOptimized("horse", "ros") == 3);

    std::cout << "minDistance: PASSED\n";
}

void testMinDeleteDistance() {
    assert(minDeleteDistance("sea", "eat") == 2);
    assert(minDeleteDistanceDP("sea", "eat") == 2);
    assert(minDeleteDistance("leetcode", "etco") == 4);

    std::cout << "minDeleteDistance: PASSED\n";
}

void testNumDistinct() {
    assert(numDistinct("rabbbit", "rabbit") == 3);
    assert(numDistinct("babgbag", "bag") == 5);
    assert(numDistinct("a", "b") == 0);

    std::cout << "numDistinct: PASSED\n";
}

void testIsSubsequence() {
    assert(isSubsequence("abc", "ahbgdc") == true);
    assert(isSubsequence("axc", "ahbgdc") == false);
    assert(isSubsequenceDP("abc", "ahbgdc") == true);

    std::cout << "isSubsequence: PASSED\n";
}

void testLongestPalindromeSubseq() {
    assert(longestPalindromeSubseq("bbbab") == 4);
    assert(longestPalindromeSubseq("cbbd") == 2);
    assert(longestPalindromeSubseqLCS("bbbab") == 4);

    std::cout << "longestPalindromeSubseq: PASSED\n";
}

void testFindNumberOfLIS() {
    std::vector<int> nums = {1, 3, 5, 4, 7};
    assert(findNumberOfLIS(nums) == 2);

    nums = {2, 2, 2, 2, 2};
    assert(findNumberOfLIS(nums) == 5);

    nums = {1, 2, 4, 3, 5, 4, 7, 2};
    assert(findNumberOfLIS(nums) == 3);

    std::cout << "findNumberOfLIS: PASSED\n";
}

void testMaxEnvelopes() {
    std::vector<std::vector<int>> env = {{5, 4}, {6, 4}, {6, 7}, {2, 3}};
    assert(maxEnvelopes(env) == 3);

    env = {{1, 1}, {1, 1}, {1, 1}};
    assert(maxEnvelopes(env) == 1);

    env = {{4, 5}, {4, 6}, {6, 7}, {2, 3}, {1, 1}};
    assert(maxEnvelopes(env) == 4);

    std::cout << "maxEnvelopes: PASSED\n";
}

int main() {
    std::cout << "=== Subsequence DP Solution Tests ===\n";

    testLongestCommonSubsequence();
    testLongestCommonSubstring();
    testMinDistance();
    testMinDeleteDistance();
    testNumDistinct();
    testIsSubsequence();
    testLongestPalindromeSubseq();
    testFindNumberOfLIS();
    testMaxEnvelopes();

    std::cout << "\nAll tests passed!\n";
    return 0;
}
