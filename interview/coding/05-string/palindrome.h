/**
 * @file palindrome.h
 * @brief 回文字符串问题 - 头文件
 *
 * 回文是正读和反读都相同的字符串。
 * 常见解法：中心扩展、动态规划、双指针、Manacher 算法
 */

#ifndef INTERVIEW_PALINDROME_H
#define INTERVIEW_PALINDROME_H

#include <string>
#include <vector>

namespace PalindromeProblems {

// ==================== 面试者实现 ====================

/**
 * 题目1: 验证回文串
 * 给定一个字符串，验证它是否是回文串，只考虑字母和数字字符，可以忽略字母的大小写。
 */
bool isPalindrome(std::string s);

/**
 * 题目2: 验证回文串 II（最多删除一个字符）
 * 给定一个非空字符串 s，最多删除一个字符。判断是否能成为回文字符串。
 */
bool validPalindrome(std::string s);

/**
 * 题目3: 最长回文子串
 * 给你一个字符串 s，找到 s 中最长的回文子串。
 */
std::string longestPalindrome(std::string s);

/**
 * 题目4: 回文子串数目
 * 给你一个字符串 s，请你统计并返回这个字符串中回文子串的数目。
 */
int countSubstrings(std::string s);

/**
 * 题目5: 最长回文子序列
 * 给你一个字符串 s，找出其中最长的回文子序列，并返回该序列的长度。
 */
int longestPalindromeSubseq(std::string s);

/**
 * 题目6: 分割回文串
 * 给你一个字符串 s，请你将 s 分割成一些子串，使每个子串都是回文串。
 * 返回 s 所有可能的分割方案。
 */
std::vector<std::vector<std::string>> partition(std::string s);

/**
 * 题目7: 分割回文串 II（最少分割次数）
 * 给你一个字符串 s，请你将 s 分割成一些子串，使每个子串都是回文串。
 * 返回符合要求的最少分割次数。
 */
int minCut(std::string s);

/**
 * 题目8: 回文数
 * 给你一个整数 x，如果 x 是一个回文整数，返回 true；否则，返回 false。
 */
bool isPalindromeNumber(int x);

/**
 * 题目9: 最短回文串
 * 给定一个字符串 s，你可以通过在字符串前面添加字符将其转换为回文串。
 * 找到并返回可以用这种方式转换的最短回文串。
 */
std::string shortestPalindrome(std::string s);

/**
 * 题目10: 回文对
 * 给定一组唯一的单词，找出所有不同的索引对 (i, j)，
 * 使得列表中的两个单词 words[i] + words[j]，可拼接成回文串。
 */
std::vector<std::vector<int>> palindromePairs(std::vector<std::string>& words);

// ==================== 参考答案实现 ====================

bool isPalindromeSolution(std::string s);
bool validPalindromeSolution(std::string s);
std::string longestPalindromeSolution(std::string s);
int countSubstringsSolution(std::string s);
int longestPalindromeSubseqSolution(std::string s);
std::vector<std::vector<std::string>> partitionSolution(std::string s);
int minCutSolution(std::string s);
bool isPalindromeNumberSolution(int x);
std::string shortestPalindromeSolution(std::string s);
std::vector<std::vector<int>> palindromePairsSolution(std::vector<std::string>& words);

// ==================== 测试函数声明 ====================

void testPalindrome();          // 测试面试者实现
void testPalindromeSolution();  // 测试解答实现

// runTests 在头文件 inline 定义，调用两个测试函数
inline void runTests() {
    testPalindrome();
    testPalindromeSolution();
}

} // namespace PalindromeProblems

#endif // INTERVIEW_PALINDROME_H
