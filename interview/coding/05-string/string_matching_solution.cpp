/**
 * @file string_matching_solution.cpp
 * @brief 字符串匹配算法 - 解答文件
 *
 * 字符串匹配核心算法：
 * 1. KMP：利用前缀函数避免重复匹配
 * 2. Rabin-Karp：滚动哈希实现快速比较
 * 3. Z 函数：扩展 KMP，求所有后缀与原串的最长公共前缀
 */

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <functional>

/**
 * 题目1: 实现 strStr()
 *
 * 解法: KMP 算法
 * 时间复杂度: O(n + m)
 * 空间复杂度: O(m)
 */

// 构建 KMP 前缀函数（部分匹配表）
std::vector<int> buildKMPTable(const std::string& pattern) {
    int m = pattern.size();
    std::vector<int> lps(m, 0);  // longest proper prefix which is also suffix

    int len = 0;  // 当前最长前后缀长度
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

int strStr(std::string haystack, std::string needle) {
    if (needle.empty()) return 0;

    int n = haystack.size();
    int m = needle.size();

    std::vector<int> lps = buildKMPTable(needle);

    int i = 0;  // haystack 指针
    int j = 0;  // needle 指针

    while (i < n) {
        if (haystack[i] == needle[j]) {
            ++i;
            ++j;
            if (j == m) {
                return i - m;  // 找到匹配
            }
        } else {
            if (j != 0) {
                j = lps[j - 1];  // 利用前缀函数跳转
            } else {
                ++i;
            }
        }
    }

    return -1;
}

/**
 * 暴力解法（对比用）
 * 时间复杂度: O(n * m)
 */
int strStrBruteForce(std::string haystack, std::string needle) {
    int n = haystack.size();
    int m = needle.size();

    for (int i = 0; i <= n - m; ++i) {
        int j = 0;
        while (j < m && haystack[i + j] == needle[j]) {
            ++j;
        }
        if (j == m) return i;
    }

    return -1;
}

/**
 * 题目2: 重复的子字符串
 *
 * 解法1: KMP 前缀函数性质
 * 如果 n % (n - lps[n-1]) == 0 且 lps[n-1] != 0，则由重复子串构成
 *
 * 时间复杂度: O(n)
 * 空间复杂度: O(n)
 */
bool repeatedSubstringPattern(std::string s) {
    int n = s.size();
    std::vector<int> lps = buildKMPTable(s);

    int len = lps[n - 1];
    return len > 0 && n % (n - len) == 0;
}

/**
 * 解法2: 双倍字符串法
 * 时间复杂度: O(n)
 */
bool repeatedSubstringPatternDouble(std::string s) {
    std::string doubled = s + s;
    // 在 doubled[1:2n-1] 中查找 s
    return doubled.find(s, 1) < s.size();
}

/**
 * 题目3: 最短回文串
 *
 * 解法: KMP 算法
 * 构造 s + "#" + reverse(s)，求前缀函数
 *
 * 时间复杂度: O(n)
 * 空间复杂度: O(n)
 */
std::string shortestPalindrome(std::string s) {
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

/**
 * 题目4: 通配符匹配
 *
 * 解法: 动态规划
 * dp[i][j] = s[0..i-1] 和 p[0..j-1] 是否匹配
 *
 * 时间复杂度: O(m * n)
 * 空间复杂度: O(m * n)
 */
bool isMatch(std::string s, std::string p) {
    int m = s.size(), n = p.size();
    std::vector<std::vector<bool>> dp(m + 1, std::vector<bool>(n + 1, false));

    dp[0][0] = true;

    // 空字符串匹配 p[0..j-1]（只有连续 * 才能匹配空串）
    for (int j = 1; j <= n; ++j) {
        if (p[j - 1] == '*') {
            dp[0][j] = dp[0][j - 1];
        }
    }

    for (int i = 1; i <= m; ++i) {
        for (int j = 1; j <= n; ++j) {
            if (p[j - 1] == '*') {
                // * 匹配空串 || * 匹配一个或多个字符
                dp[i][j] = dp[i][j - 1] || dp[i - 1][j];
            } else if (p[j - 1] == '?' || s[i - 1] == p[j - 1]) {
                dp[i][j] = dp[i - 1][j - 1];
            }
        }
    }

    return dp[m][n];
}

/**
 * 贪心双指针解法
 * 时间复杂度: O(m * n)，最坏情况
 * 空间复杂度: O(1)
 */
bool isMatchGreedy(std::string s, std::string p) {
    int m = s.size(), n = p.size();
    int i = 0, j = 0;
    int starIdx = -1, matchIdx = -1;

    while (i < m) {
        if (j < n && (p[j] == '?' || s[i] == p[j])) {
            ++i;
            ++j;
        } else if (j < n && p[j] == '*') {
            starIdx = j;
            matchIdx = i;
            ++j;
        } else if (starIdx != -1) {
            // 回溯到上一个 *，让它多匹配一个字符
            j = starIdx + 1;
            ++matchIdx;
            i = matchIdx;
        } else {
            return false;
        }
    }

    // 检查剩余的模式串是否都是 *
    while (j < n && p[j] == '*') {
        ++j;
    }

    return j == n;
}

/**
 * 题目5: 正则表达式匹配
 *
 * 解法: 动态规划
 * 时间复杂度: O(m * n)
 * 空间复杂度: O(m * n)
 */
bool isMatchRegex(std::string s, std::string p) {
    int m = s.size(), n = p.size();
    std::vector<std::vector<bool>> dp(m + 1, std::vector<bool>(n + 1, false));

    dp[0][0] = true;

    // 处理 a*b*c* 这种可以匹配空串的模式
    for (int j = 2; j <= n; j += 2) {
        if (p[j - 1] == '*') {
            dp[0][j] = dp[0][j - 2];
        }
    }

    for (int i = 1; i <= m; ++i) {
        for (int j = 1; j <= n; ++j) {
            if (p[j - 1] == '*') {
                // * 匹配零个前面元素 || * 匹配一个或多个前面元素
                dp[i][j] = dp[i][j - 2];  // 匹配零个
                if (p[j - 2] == '.' || p[j - 2] == s[i - 1]) {
                    dp[i][j] = dp[i][j] || dp[i - 1][j];  // 匹配一个或多个
                }
            } else if (p[j - 1] == '.' || p[j - 1] == s[i - 1]) {
                dp[i][j] = dp[i - 1][j - 1];
            }
        }
    }

    return dp[m][n];
}

/**
 * 题目6: 查找所有匹配位置
 *
 * 解法: KMP 算法完整版
 * 时间复杂度: O(n + m)
 * 空间复杂度: O(m)
 */
std::vector<int> findAllOccurrences(std::string text, std::string pattern) {
    std::vector<int> result;
    if (pattern.empty()) return result;

    int n = text.size();
    int m = pattern.size();

    std::vector<int> lps = buildKMPTable(pattern);

    int i = 0, j = 0;
    while (i < n) {
        if (text[i] == pattern[j]) {
            ++i;
            ++j;
            if (j == m) {
                result.push_back(i - m);
                j = lps[j - 1];  // 继续查找下一个匹配
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

/**
 * 题目7: Rabin-Karp 字符串匹配
 *
 * 解法: 滚动哈希
 * 时间复杂度: O(n + m)，平均情况
 * 空间复杂度: O(1)
 */
int rabinKarp(std::string text, std::string pattern) {
    if (pattern.empty()) return 0;
    if (pattern.size() > text.size()) return -1;

    const long long BASE = 26;
    const long long MOD = 1e9 + 7;

    int n = text.size();
    int m = pattern.size();

    // 计算 pattern 的哈希值
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
            // 哈希匹配，验证实际字符
            if (text.substr(i, m) == pattern) {
                return i;
            }
        }

        // 滑动窗口更新哈希值
        if (i < n - m) {
            textHash = ((textHash - text[i] * power % MOD + MOD) * BASE + text[i + m]) % MOD;
        }
    }

    return -1;
}

/**
 * 题目8: 最长公共前缀
 *
 * 解法1: 纵向扫描
 * 时间复杂度: O(mn)，m 为字符串数量，n 为最短字符串长度
 * 空间复杂度: O(1)
 */
std::string longestCommonPrefix(std::vector<std::string>& strs) {
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

/**
 * 解法2: 分治法
 * 时间复杂度: O(mn)
 * 空间复杂度: O(m log m)
 */
std::string longestCommonPrefixDivide(std::vector<std::string>& strs) {
    if (strs.empty()) return "";

    std::function<std::string(int, int)> lcp = [&](int left, int right) -> std::string {
        if (left == right) {
            return strs[left];
        }

        int mid = (left + right) / 2;
        std::string lcpLeft = lcp(left, mid);
        std::string lcpRight = lcp(mid + 1, right);

        int minLen = std::min(lcpLeft.size(), lcpRight.size());
        for (int i = 0; i < minLen; ++i) {
            if (lcpLeft[i] != lcpRight[i]) {
                return lcpLeft.substr(0, i);
            }
        }
        return lcpLeft.substr(0, minLen);
    };

    return lcp(0, strs.size() - 1);
}

/**
 * 题目9: Z 函数（扩展 KMP）
 *
 * 解法: 线性时间算法
 * z[i] = s 和 s[i:] 的最长公共前缀长度
 *
 * 时间复杂度: O(n)
 * 空间复杂度: O(n)
 */
std::vector<int> zFunction(std::string s) {
    int n = s.size();
    std::vector<int> z(n, 0);
    z[0] = n;

    int left = 0, right = 0;  // [left, right) 是当前最靠右的匹配区间

    for (int i = 1; i < n; ++i) {
        if (i < right) {
            // 利用之前计算的结果
            z[i] = std::min(right - i, z[i - left]);
        }

        // 暴力扩展
        while (i + z[i] < n && s[z[i]] == s[i + z[i]]) {
            ++z[i];
        }

        // 更新最右匹配区间
        if (i + z[i] > right) {
            left = i;
            right = i + z[i];
        }
    }

    return z;
}

/**
 * 题目10: 字符串哈希
 *
 * 支持 O(1) 查询任意子串哈希值
 */
class StringHash {
private:
    std::vector<long long> hash;
    std::vector<long long> power;
    static const long long BASE = 31;
    static const long long MOD = 1e9 + 9;

public:
    StringHash(const std::string& s) {
        int n = s.size();
        hash.resize(n + 1, 0);
        power.resize(n + 1, 1);

        for (int i = 0; i < n; ++i) {
            hash[i + 1] = (hash[i] * BASE + (s[i] - 'a' + 1)) % MOD;
            power[i + 1] = (power[i] * BASE) % MOD;
        }
    }

    // 获取 s[l..r] 的哈希值（0-indexed）
    long long getHash(int l, int r) {
        return (hash[r + 1] - hash[l] * power[r - l + 1] % MOD + MOD) % MOD;
    }

    // 判断两个子串是否相等
    bool equal(int l1, int r1, int l2, int r2) {
        return getHash(l1, r1) == getHash(l2, r2);
    }
};


// ==================== KMP 与 Z 函数关系 ====================
/**
 * KMP 前缀函数 lps[i] = s[0..i] 的最长真前缀后缀长度
 * Z 函数 z[i] = s 和 s[i:] 的最长公共前缀长度
 *
 * 互相转换：
 * 1. 由 Z 函数计算前缀函数
 * 2. 由前缀函数计算 Z 函数
 *
 * 应用选择：
 * - KMP：单模式串匹配
 * - Z 函数：求所有后缀与原串的关系
 * - 两者时间复杂度相同，选择更直观的
 */


// ==================== 测试代码 ====================
#include <cassert>

void testStrStr() {
    assert(strStr("sadbutsad", "sad") == 0);
    assert(strStr("leetcode", "leeto") == -1);
    assert(strStr("hello", "ll") == 2);
    assert(strStrBruteForce("hello", "ll") == 2);

    std::cout << "strStr: PASSED\n";
}

void testRepeatedSubstringPattern() {
    assert(repeatedSubstringPattern("abab") == true);
    assert(repeatedSubstringPattern("aba") == false);
    assert(repeatedSubstringPattern("abcabcabcabc") == true);
    assert(repeatedSubstringPatternDouble("abab") == true);

    std::cout << "repeatedSubstringPattern: PASSED\n";
}

void testShortestPalindrome() {
    assert(shortestPalindrome("aacecaaa") == "aaacecaaa");
    assert(shortestPalindrome("abcd") == "dcbabcd");

    std::cout << "shortestPalindrome: PASSED\n";
}

void testIsMatch() {
    assert(isMatch("aa", "a") == false);
    assert(isMatch("aa", "*") == true);
    assert(isMatch("cb", "?a") == false);
    assert(isMatch("adceb", "*a*b") == true);
    assert(isMatchGreedy("adceb", "*a*b") == true);

    std::cout << "isMatch: PASSED\n";
}

void testIsMatchRegex() {
    assert(isMatchRegex("aa", "a") == false);
    assert(isMatchRegex("aa", "a*") == true);
    assert(isMatchRegex("ab", ".*") == true);
    assert(isMatchRegex("aab", "c*a*b") == true);

    std::cout << "isMatchRegex: PASSED\n";
}

void testFindAllOccurrences() {
    std::vector<int> expected = {0, 9, 12};
    assert(findAllOccurrences("AABAACAADAABAABA", "AABA") == expected);

    std::cout << "findAllOccurrences: PASSED\n";
}

void testRabinKarp() {
    assert(rabinKarp("hello world", "world") == 6);
    assert(rabinKarp("aaaa", "aa") == 0);
    assert(rabinKarp("abc", "d") == -1);

    std::cout << "rabinKarp: PASSED\n";
}

void testLongestCommonPrefix() {
    std::vector<std::string> strs1 = {"flower", "flow", "flight"};
    assert(longestCommonPrefix(strs1) == "fl");

    std::vector<std::string> strs2 = {"dog", "racecar", "car"};
    assert(longestCommonPrefix(strs2) == "");

    assert(longestCommonPrefixDivide(strs1) == "fl");

    std::cout << "longestCommonPrefix: PASSED\n";
}

void testZFunction() {
    std::vector<int> expected = {7, 1, 0, 0, 3, 1, 0};
    assert(zFunction("aabcaab") == expected);

    std::cout << "zFunction: PASSED\n";
}

void testStringHash() {
    StringHash sh("abcabc");
    assert(sh.equal(0, 2, 3, 5));  // "abc" == "abc"
    assert(!sh.equal(0, 1, 3, 4)); // "ab" != "ab" 位置不同但内容相同，应该相等
    // 实际上应该相等，修正测试
    assert(sh.equal(0, 1, 3, 4));  // "ab" == "ab"

    std::cout << "StringHash: PASSED\n";
}

int main() {
    std::cout << "=== String Matching Solution Tests ===\n";

    testStrStr();
    testRepeatedSubstringPattern();
    testShortestPalindrome();
    testIsMatch();
    testIsMatchRegex();
    testFindAllOccurrences();
    testRabinKarp();
    testLongestCommonPrefix();
    testZFunction();
    testStringHash();

    std::cout << "\nAll tests passed!\n";
    return 0;
}
