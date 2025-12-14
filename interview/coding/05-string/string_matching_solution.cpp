/**
 * @file string_matching_solution.cpp
 * @brief 字符串匹配算法 - 参考答案
 */

#include "string_matching.h"
#include <iostream>
#include <cassert>
#include <algorithm>

namespace StringMatching {

// ==================== 辅助函数 ====================

// 构建 KMP 前缀函数
static std::vector<int> buildKMPTable(const std::string& pattern) {
    int m = static_cast<int>(pattern.size());
    std::vector<int> lps(m, 0);

    int len = 0;
    int i = 1;

    while (i < m) {
        if (pattern[i] == pattern[len]) {
            ++len;
            lps[i] = len;
            ++i;
        } else {
            if (len != 0) {
                len = lps[len - 1];
            } else {
                lps[i] = 0;
                ++i;
            }
        }
    }

    return lps;
}

// ==================== 参考答案实现 ====================

// 题目1: 实现 strStr() - KMP 算法
int strStrSolution(const std::string& haystack, const std::string& needle) {
    if (needle.empty()) return 0;

    int n = static_cast<int>(haystack.size());
    int m = static_cast<int>(needle.size());

    std::vector<int> lps = buildKMPTable(needle);

    int i = 0;
    int j = 0;

    while (i < n) {
        if (haystack[i] == needle[j]) {
            ++i;
            ++j;
            if (j == m) {
                return i - m;
            }
        } else {
            if (j != 0) {
                j = lps[j - 1];
            } else {
                ++i;
            }
        }
    }

    return -1;
}

// 题目2: 重复的子字符串 - KMP 解法
bool repeatedSubstringPatternSolution(const std::string& s) {
    int n = static_cast<int>(s.size());
    std::vector<int> lps = buildKMPTable(s);

    int len = lps[n - 1];
    return len > 0 && n % (n - len) == 0;
}

// 题目3: 最短回文串
std::string shortestPalindromeSolution(const std::string& s) {
    if (s.empty()) return s;

    std::string rev = s;
    std::reverse(rev.begin(), rev.end());

    std::string combined = s + "#" + rev;
    std::vector<int> lps = buildKMPTable(combined);

    int longestPalinPrefix = lps.back();
    std::string toAdd = s.substr(longestPalinPrefix);
    std::reverse(toAdd.begin(), toAdd.end());

    return toAdd + s;
}

// 题目4: 通配符匹配 - 动态规划
bool isMatchSolution(const std::string& s, const std::string& p) {
    int m = static_cast<int>(s.size()), n = static_cast<int>(p.size());
    std::vector<std::vector<bool>> dp(m + 1, std::vector<bool>(n + 1, false));

    dp[0][0] = true;

    for (int j = 1; j <= n; ++j) {
        if (p[j - 1] == '*') {
            dp[0][j] = dp[0][j - 1];
        }
    }

    for (int i = 1; i <= m; ++i) {
        for (int j = 1; j <= n; ++j) {
            if (p[j - 1] == '*') {
                dp[i][j] = dp[i][j - 1] || dp[i - 1][j];
            } else if (p[j - 1] == '?' || s[i - 1] == p[j - 1]) {
                dp[i][j] = dp[i - 1][j - 1];
            }
        }
    }

    return dp[m][n];
}

// 题目5: 正则表达式匹配
bool isMatchRegexSolution(const std::string& s, const std::string& p) {
    int m = static_cast<int>(s.size()), n = static_cast<int>(p.size());
    std::vector<std::vector<bool>> dp(m + 1, std::vector<bool>(n + 1, false));

    dp[0][0] = true;

    for (int j = 2; j <= n; j += 2) {
        if (p[j - 1] == '*') {
            dp[0][j] = dp[0][j - 2];
        }
    }

    for (int i = 1; i <= m; ++i) {
        for (int j = 1; j <= n; ++j) {
            if (p[j - 1] == '*') {
                dp[i][j] = dp[i][j - 2];
                if (p[j - 2] == '.' || p[j - 2] == s[i - 1]) {
                    dp[i][j] = dp[i][j] || dp[i - 1][j];
                }
            } else if (p[j - 1] == '.' || p[j - 1] == s[i - 1]) {
                dp[i][j] = dp[i - 1][j - 1];
            }
        }
    }

    return dp[m][n];
}

// 题目6: 查找所有匹配位置
std::vector<int> findAllOccurrencesSolution(const std::string& text, const std::string& pattern) {
    std::vector<int> result;
    if (pattern.empty()) return result;

    int n = static_cast<int>(text.size());
    int m = static_cast<int>(pattern.size());

    std::vector<int> lps = buildKMPTable(pattern);

    int i = 0, j = 0;
    while (i < n) {
        if (text[i] == pattern[j]) {
            ++i;
            ++j;
            if (j == m) {
                result.push_back(i - m);
                j = lps[j - 1];
            }
        } else {
            if (j != 0) {
                j = lps[j - 1];
            } else {
                ++i;
            }
        }
    }

    return result;
}

// 题目7: Rabin-Karp 字符串匹配
int rabinKarpSolution(const std::string& text, const std::string& pattern) {
    if (pattern.empty()) return 0;
    if (pattern.size() > text.size()) return -1;

    const long long BASE = 26;
    const long long MOD = 1000000007;

    int n = static_cast<int>(text.size());
    int m = static_cast<int>(pattern.size());

    long long patternHash = 0;
    long long textHash = 0;
    long long power = 1;

    for (int i = 0; i < m; ++i) {
        patternHash = (patternHash * BASE + pattern[i]) % MOD;
        textHash = (textHash * BASE + text[i]) % MOD;
        if (i < m - 1) {
            power = (power * BASE) % MOD;
        }
    }

    for (int i = 0; i <= n - m; ++i) {
        if (patternHash == textHash) {
            if (text.substr(i, m) == pattern) {
                return i;
            }
        }

        if (i < n - m) {
            textHash = ((textHash - text[i] * power % MOD + MOD) * BASE + text[i + m]) % MOD;
        }
    }

    return -1;
}

// 题目8: 最长公共前缀
std::string longestCommonPrefixSolution(std::vector<std::string>& strs) {
    if (strs.empty()) return "";

    for (int i = 0; i < static_cast<int>(strs[0].size()); ++i) {
        char c = strs[0][i];
        for (int j = 1; j < static_cast<int>(strs.size()); ++j) {
            if (i >= static_cast<int>(strs[j].size()) || strs[j][i] != c) {
                return strs[0].substr(0, i);
            }
        }
    }

    return strs[0];
}

// 题目9: Z 函数
std::vector<int> zFunctionSolution(const std::string& s) {
    int n = static_cast<int>(s.size());
    std::vector<int> z(n, 0);
    z[0] = n;

    int left = 0, right = 0;

    for (int i = 1; i < n; ++i) {
        if (i < right) {
            z[i] = std::min(right - i, z[i - left]);
        }

        while (i + z[i] < n && s[z[i]] == s[i + z[i]]) {
            ++z[i];
        }

        if (i + z[i] > right) {
            left = i;
            right = i + z[i];
        }
    }

    return z;
}

// ==================== 测试函数 ====================

void testStringMatchingSolution() {
    std::cout << "=== String Matching Tests (Solution) ===\n";

    // strStr
    assert(strStrSolution("sadbutsad", "sad") == 0);
    assert(strStrSolution("leetcode", "leeto") == -1);
    assert(strStrSolution("hello", "ll") == 2);
    std::cout << "  strStrSolution: PASSED\n";

    // repeatedSubstringPattern
    assert(repeatedSubstringPatternSolution("abab") == true);
    assert(repeatedSubstringPatternSolution("aba") == false);
    assert(repeatedSubstringPatternSolution("abcabcabcabc") == true);
    std::cout << "  repeatedSubstringPatternSolution: PASSED\n";

    // shortestPalindrome
    assert(shortestPalindromeSolution("aacecaaa") == "aaacecaaa");
    assert(shortestPalindromeSolution("abcd") == "dcbabcd");
    std::cout << "  shortestPalindromeSolution: PASSED\n";

    // isMatch
    assert(isMatchSolution("aa", "a") == false);
    assert(isMatchSolution("aa", "*") == true);
    assert(isMatchSolution("cb", "?a") == false);
    assert(isMatchSolution("adceb", "*a*b") == true);
    std::cout << "  isMatchSolution: PASSED\n";

    // isMatchRegex
    assert(isMatchRegexSolution("aa", "a") == false);
    assert(isMatchRegexSolution("aa", "a*") == true);
    assert(isMatchRegexSolution("ab", ".*") == true);
    assert(isMatchRegexSolution("aab", "c*a*b") == true);
    std::cout << "  isMatchRegexSolution: PASSED\n";

    // findAllOccurrences
    std::vector<int> expected = {0, 9, 12};
    assert(findAllOccurrencesSolution("AABAACAADAABAABA", "AABA") == expected);
    std::cout << "  findAllOccurrencesSolution: PASSED\n";

    // rabinKarp
    assert(rabinKarpSolution("hello world", "world") == 6);
    assert(rabinKarpSolution("aaaa", "aa") == 0);
    assert(rabinKarpSolution("abc", "d") == -1);
    std::cout << "  rabinKarpSolution: PASSED\n";

    // longestCommonPrefix
    std::vector<std::string> strs1 = {"flower", "flow", "flight"};
    assert(longestCommonPrefixSolution(strs1) == "fl");
    std::vector<std::string> strs2 = {"dog", "racecar", "car"};
    assert(longestCommonPrefixSolution(strs2) == "");
    std::cout << "  longestCommonPrefixSolution: PASSED\n";

    // zFunction
    std::vector<int> zExpected = {7, 1, 0, 0, 3, 1, 0};
    assert(zFunctionSolution("aabcaab") == zExpected);
    std::cout << "  zFunctionSolution: PASSED\n";
}

} // namespace StringMatching
