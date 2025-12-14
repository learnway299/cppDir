/**
 * @file string_matching.h
 * @brief 字符串匹配算法 - 题目说明与声明
 *
 * 字符串匹配是经典问题，常见算法：
 * 1. 暴力匹配 O(nm)
 * 2. KMP 算法 O(n+m)
 * 3. Rabin-Karp 算法（滚动哈希）O(n+m)
 * 4. Boyer-Moore 算法
 * 5. Sunday 算法
 */

#ifndef STRING_MATCHING_H
#define STRING_MATCHING_H

#include <string>
#include <vector>

namespace StringMatching {

// ==================== 题目声明 ====================

/**
 * 题目1: 实现 strStr()
 *
 * 给你两个字符串 haystack 和 needle，
 * 请你在 haystack 字符串中找出 needle 字符串的第一个匹配项的下标（下标从 0 开始）。
 * 如果 needle 不是 haystack 的一部分，则返回 -1。
 *
 * 示例:
 *   输入: haystack = "sadbutsad", needle = "sad"
 *   输出: 0
 *
 *   输入: haystack = "leetcode", needle = "leeto"
 *   输出: -1
 *
 * 要求: 尝试实现 KMP 算法
 */
int strStr(const std::string& haystack, const std::string& needle);
int strStrSolution(const std::string& haystack, const std::string& needle);

/**
 * 题目2: 重复的子字符串
 *
 * 给定一个非空的字符串 s，检查是否可以通过由它的一个子串重复多次构成。
 *
 * 示例:
 *   输入: s = "abab"
 *   输出: true
 *   解释: 可由子字符串 "ab" 重复两次构成
 *
 *   输入: s = "aba"
 *   输出: false
 *
 *   输入: s = "abcabcabcabc"
 *   输出: true
 *
 * 思路:
 *   1. KMP 前缀函数的性质
 *   2. (s + s).find(s, 1) < s.size()
 */
bool repeatedSubstringPattern(const std::string& s);
bool repeatedSubstringPatternSolution(const std::string& s);

/**
 * 题目3: 最短回文串（KMP 应用）
 *
 * 给定一个字符串 s，你可以通过在字符串前面添加字符将其转换为回文串。
 * 找到并返回可以用这种方式转换的最短回文串。
 *
 * 示例:
 *   输入: s = "aacecaaa"
 *   输出: "aaacecaaa"
 *
 *   输入: s = "abcd"
 *   输出: "dcbabcd"
 *
 * 思路: 构造 s + "#" + reverse(s)，求 KMP 前缀函数
 */
std::string shortestPalindrome(const std::string& s);
std::string shortestPalindromeSolution(const std::string& s);

/**
 * 题目4: 通配符匹配
 *
 * 给定一个字符串 s 和一个字符模式 p，实现一个支持 '?' 和 '*' 的通配符匹配。
 *   - '?' 可以匹配任何单个字符
 *   - '*' 可以匹配任意字符串（包括空字符串）
 *
 * 判断 s 和 p 是否匹配。必须覆盖整个字符串 s。
 *
 * 示例:
 *   输入: s = "aa", p = "a"
 *   输出: false
 *
 *   输入: s = "aa", p = "*"
 *   输出: true
 *
 *   输入: s = "adceb", p = "*a*b"
 *   输出: true
 *
 * 思路: 动态规划
 */
bool isMatch(const std::string& s, const std::string& p);
bool isMatchSolution(const std::string& s, const std::string& p);

/**
 * 题目5: 正则表达式匹配
 *
 * 给你一个字符串 s 和一个字符规律 p，请你来实现一个支持 '.' 和 '*' 的正则表达式匹配。
 *   - '.' 匹配任意单个字符
 *   - '*' 匹配零个或多个前面的那一个元素
 *
 * 所谓匹配，是要涵盖整个字符串 s 的，而不是部分字符串。
 *
 * 示例:
 *   输入: s = "aa", p = "a"
 *   输出: false
 *
 *   输入: s = "aa", p = "a*"
 *   输出: true
 *
 *   输入: s = "aab", p = "c*a*b"
 *   输出: true
 *   解释: c* 匹配 0 个 c，a* 匹配 2 个 a
 */
bool isMatchRegex(const std::string& s, const std::string& p);
bool isMatchRegexSolution(const std::string& s, const std::string& p);

/**
 * 题目6: 查找所有匹配位置
 *
 * 给定主串 text 和模式串 pattern，找出 pattern 在 text 中所有出现的位置。
 *
 * 示例:
 *   输入: text = "AABAACAADAABAABA", pattern = "AABA"
 *   输出: [0, 9, 12]
 *
 * 思路: KMP 算法的完整应用
 */
std::vector<int> findAllOccurrences(const std::string& text, const std::string& pattern);
std::vector<int> findAllOccurrencesSolution(const std::string& text, const std::string& pattern);

/**
 * 题目7: Rabin-Karp 字符串匹配
 *
 * 使用滚动哈希实现字符串匹配。
 *
 * 滚动哈希思想：
 *   hash("abc") = a * p^2 + b * p + c
 *   hash("bcd") = (hash("abc") - a * p^2) * p + d
 *
 * 示例:
 *   输入: text = "hello world", pattern = "world"
 *   输出: 6
 */
int rabinKarp(const std::string& text, const std::string& pattern);
int rabinKarpSolution(const std::string& text, const std::string& pattern);

/**
 * 题目8: 最长公共前缀
 *
 * 编写一个函数来查找字符串数组中的最长公共前缀。
 * 如果不存在公共前缀，返回空字符串 ""。
 *
 * 示例:
 *   输入: strs = ["flower","flow","flight"]
 *   输出: "fl"
 *
 *   输入: strs = ["dog","racecar","car"]
 *   输出: ""
 */
std::string longestCommonPrefix(std::vector<std::string>& strs);
std::string longestCommonPrefixSolution(std::vector<std::string>& strs);

/**
 * 题目9: Z 函数（扩展 KMP）
 *
 * 对于字符串 s，z[i] 表示 s 和 s[i:] 的最长公共前缀的长度。
 *
 * 示例:
 *   输入: s = "aabcaab"
 *   输出: z = [7, 1, 0, 0, 3, 1, 0]
 *   解释:
 *     z[0] = 7 (整个字符串)
 *     z[1] = 1 (a 和 aabcaab 的公共前缀是 "a")
 *     z[4] = 3 (aab 和 aabcaab 的公共前缀是 "aab")
 */
std::vector<int> zFunction(const std::string& s);
std::vector<int> zFunctionSolution(const std::string& s);

// ==================== 测试函数声明 ====================

void testStringMatching();          // 测试面试者实现
void testStringMatchingSolution();  // 测试解答实现

// runTests 在头文件 inline 定义，调用两个测试函数
inline void runTests() {
    testStringMatching();
    testStringMatchingSolution();
}

} // namespace StringMatching

#endif // STRING_MATCHING_H
