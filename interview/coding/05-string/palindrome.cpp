/**
 * @file palindrome.cpp
 * @brief 回文字符串问题 - 题目文件
 *
 * 回文是正读和反读都相同的字符串。
 * 常见解法：中心扩展、动态规划、双指针、Manacher 算法
 */

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

/**
 * 题目1: 验证回文串
 *
 * 给定一个字符串，验证它是否是回文串，只考虑字母和数字字符，可以忽略字母的大小写。
 *
 * 示例:
 *   输入: "A man, a plan, a canal: Panama"
 *   输出: true
 *   解释: "amanaplanacanalpanama" 是回文串
 *
 *   输入: "race a car"
 *   输出: false
 *
 * 思路: 双指针，跳过非字母数字字符
 *
 * @param s 输入字符串
 * @return 是否为回文串
 */
bool isPalindrome(std::string s) {
    // TODO: 实现你的解法
    return false;
}

/**
 * 题目2: 验证回文串 II（最多删除一个字符）
 *
 * 给定一个非空字符串 s，最多删除一个字符。判断是否能成为回文字符串。
 *
 * 示例:
 *   输入: s = "aba"
 *   输出: true
 *
 *   输入: s = "abca"
 *   输出: true
 *   解释: 你可以删除 c 字符
 *
 *   输入: s = "abc"
 *   输出: false
 *
 * 思路: 双指针 + 贪心
 *
 * @param s 输入字符串
 * @return 是否可以成为回文串
 */
bool validPalindrome(std::string s) {
    // TODO: 实现你的解法
    return false;
}

/**
 * 题目3: 最长回文子串
 *
 * 给你一个字符串 s，找到 s 中最长的回文子串。
 *
 * 示例:
 *   输入: s = "babad"
 *   输出: "bab" 或 "aba"
 *
 *   输入: s = "cbbd"
 *   输出: "bb"
 *
 * 思路:
 *   1. 中心扩展法 O(n²)
 *   2. 动态规划 O(n²)
 *   3. Manacher 算法 O(n)
 *
 * @param s 输入字符串
 * @return 最长回文子串
 */
std::string longestPalindrome(std::string s) {
    // TODO: 实现你的解法
    return "";
}

/**
 * 题目4: 回文子串数目
 *
 * 给你一个字符串 s，请你统计并返回这个字符串中回文子串的数目。
 *
 * 回文字符串是正着读和倒过来读一样的字符串。
 * 子字符串是字符串中的由连续字符组成的一个序列。
 *
 * 示例:
 *   输入: s = "abc"
 *   输出: 3
 *   解释: 三个回文子串: "a", "b", "c"
 *
 *   输入: s = "aaa"
 *   输出: 6
 *   解释: 6 个回文子串: "a", "a", "a", "aa", "aa", "aaa"
 *
 * @param s 输入字符串
 * @return 回文子串数目
 */
int countSubstrings(std::string s) {
    // TODO: 实现你的解法
    return 0;
}

/**
 * 题目5: 最长回文子序列
 *
 * 给你一个字符串 s，找出其中最长的回文子序列，并返回该序列的长度。
 *
 * 子序列定义为：不改变剩余字符顺序的情况下，删除某些字符或不删除任何字符形成的一个序列。
 *
 * 示例:
 *   输入: s = "bbbab"
 *   输出: 4
 *   解释: 一个可能的最长回文子序列为 "bbbb"
 *
 *   输入: s = "cbbd"
 *   输出: 2
 *   解释: 一个可能的最长回文子序列为 "bb"
 *
 * 状态定义: dp[i][j] = s[i..j] 的最长回文子序列长度
 *
 * @param s 输入字符串
 * @return 最长回文子序列长度
 */
int longestPalindromeSubseq(std::string s) {
    // TODO: 实现你的解法
    return 0;
}

/**
 * 题目6: 分割回文串
 *
 * 给你一个字符串 s，请你将 s 分割成一些子串，使每个子串都是回文串。
 * 返回 s 所有可能的分割方案。
 *
 * 示例:
 *   输入: s = "aab"
 *   输出: [["a","a","b"],["aa","b"]]
 *
 *   输入: s = "a"
 *   输出: [["a"]]
 *
 * 思路: 回溯 + 预处理回文判断
 *
 * @param s 输入字符串
 * @return 所有分割方案
 */
std::vector<std::vector<std::string>> partition(std::string s) {
    // TODO: 实现你的解法
    return {};
}

/**
 * 题目7: 分割回文串 II（最少分割次数）
 *
 * 给你一个字符串 s，请你将 s 分割成一些子串，使每个子串都是回文串。
 * 返回符合要求的最少分割次数。
 *
 * 示例:
 *   输入: s = "aab"
 *   输出: 1
 *   解释: 只需一次分割就可将 s 分割成 ["aa","b"] 这样两个回文子串
 *
 *   输入: s = "a"
 *   输出: 0
 *
 *   输入: s = "ab"
 *   输出: 1
 *
 * 思路: 动态规划
 * dp[i] = s[0..i] 的最少分割次数
 *
 * @param s 输入字符串
 * @return 最少分割次数
 */
int minCut(std::string s) {
    // TODO: 实现你的解法
    return 0;
}

/**
 * 题目8: 回文数
 *
 * 给你一个整数 x，如果 x 是一个回文整数，返回 true；否则，返回 false。
 * 回文数是指正序（从左向右）和倒序（从右向左）读都是一样的整数。
 *
 * 示例:
 *   输入: x = 121
 *   输出: true
 *
 *   输入: x = -121
 *   输出: false
 *   解释: 从左向右读是 -121，从右向左读是 121-
 *
 *   输入: x = 10
 *   输出: false
 *
 * 要求: 不将整数转为字符串
 *
 * @param x 输入整数
 * @return 是否为回文数
 */
bool isPalindromeNumber(int x) {
    // TODO: 实现你的解法
    return false;
}

/**
 * 题目9: 最短回文串
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
 * 思路: 找从开头开始的最长回文子串，可用 KMP 或 Manacher
 *
 * @param s 输入字符串
 * @return 最短回文串
 */
std::string shortestPalindrome(std::string s) {
    // TODO: 实现你的解法
    return "";
}

/**
 * 题目10: 回文对
 *
 * 给定一组唯一的单词，找出所有不同的索引对 (i, j)，
 * 使得列表中的两个单词 words[i] + words[j]，可拼接成回文串。
 *
 * 示例:
 *   输入: words = ["abcd","dcba","lls","s","sssll"]
 *   输出: [[0,1],[1,0],[3,2],[2,4]]
 *   解释:
 *     words[0] + words[1] = "abcddcba" 是回文
 *     words[1] + words[0] = "dcbaabcd" 是回文
 *     words[3] + words[2] = "slls" 是回文
 *     words[2] + words[4] = "llssssll" 是回文
 *
 * 思路: 哈希表 + 回文判断优化
 *
 * @param words 单词数组
 * @return 所有回文对的索引
 */
std::vector<std::vector<int>> palindromePairs(std::vector<std::string>& words) {
    // TODO: 实现你的解法
    return {};
}


// ==================== 测试代码 ====================
#include <cassert>

void testIsPalindrome() {
    assert(isPalindrome("A man, a plan, a canal: Panama") == true);
    assert(isPalindrome("race a car") == false);
    assert(isPalindrome(" ") == true);

    std::cout << "isPalindrome: PASSED\n";
}

void testValidPalindrome() {
    assert(validPalindrome("aba") == true);
    assert(validPalindrome("abca") == true);
    assert(validPalindrome("abc") == false);

    std::cout << "validPalindrome: PASSED\n";
}

void testLongestPalindrome() {
    std::string result = longestPalindrome("babad");
    assert(result == "bab" || result == "aba");

    assert(longestPalindrome("cbbd") == "bb");

    std::cout << "longestPalindrome: PASSED\n";
}

void testCountSubstrings() {
    assert(countSubstrings("abc") == 3);
    assert(countSubstrings("aaa") == 6);

    std::cout << "countSubstrings: PASSED\n";
}

void testLongestPalindromeSubseq() {
    assert(longestPalindromeSubseq("bbbab") == 4);
    assert(longestPalindromeSubseq("cbbd") == 2);

    std::cout << "longestPalindromeSubseq: PASSED\n";
}

void testPartition() {
    auto result = partition("aab");
    assert(result.size() == 2);

    std::cout << "partition: PASSED\n";
}

void testMinCut() {
    assert(minCut("aab") == 1);
    assert(minCut("a") == 0);
    assert(minCut("ab") == 1);

    std::cout << "minCut: PASSED\n";
}

void testIsPalindromeNumber() {
    assert(isPalindromeNumber(121) == true);
    assert(isPalindromeNumber(-121) == false);
    assert(isPalindromeNumber(10) == false);

    std::cout << "isPalindromeNumber: PASSED\n";
}

void testShortestPalindrome() {
    assert(shortestPalindrome("aacecaaa") == "aaacecaaa");
    assert(shortestPalindrome("abcd") == "dcbabcd");

    std::cout << "shortestPalindrome: PASSED\n";
}

void testPalindromePairs() {
    std::vector<std::string> words = {"abcd", "dcba", "lls", "s", "sssll"};
    auto result = palindromePairs(words);
    assert(result.size() == 4);

    std::cout << "palindromePairs: PASSED\n";
}

int main() {
    std::cout << "=== Palindrome Tests ===\n";

    // 取消注释以运行测试（实现解法后）
    // testIsPalindrome();
    // testValidPalindrome();
    // testLongestPalindrome();
    // testCountSubstrings();
    // testLongestPalindromeSubseq();
    // testPartition();
    // testMinCut();
    // testIsPalindromeNumber();
    // testShortestPalindrome();
    // testPalindromePairs();

    std::cout << "All tests passed!\n";
    return 0;
}
