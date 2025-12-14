/**
 * @file subsequence_dp.h
 * @brief 子序列动态规划问题 - 题目说明与声明
 *
 * 子序列 DP 是经典的 DP 类型，常见问题包括 LCS、编辑距离等。
 * 注意区分子序列（不连续）和子串（连续）。
 */

#ifndef SUBSEQUENCE_DP_H
#define SUBSEQUENCE_DP_H

#include <vector>
#include <string>

namespace SubsequenceDP {

// ==================== 题目声明 ====================

/**
 * 题目1: 最长公共子序列 (LCS)
 *
 * 给定两个字符串 text1 和 text2，返回这两个字符串的最长公共子序列的长度。
 * 如果不存在公共子序列，返回 0。
 *
 * 子序列是指这样一个新的字符串：它是由原字符串在不改变字符的相对顺序的情况下删除某些字符后组成的新字符串。
 *
 * 示例:
 *   输入: text1 = "abcde", text2 = "ace"
 *   输出: 3
 *   解释: 最长公共子序列是 "ace"，它的长度为 3。
 *
 *   输入: text1 = "abc", text2 = "abc"
 *   输出: 3
 *
 *   输入: text1 = "abc", text2 = "def"
 *   输出: 0
 *
 * 状态定义: dp[i][j] = text1[0..i-1] 和 text2[0..j-1] 的 LCS 长度
 * 状态转移:
 *   if text1[i-1] == text2[j-1]: dp[i][j] = dp[i-1][j-1] + 1
 *   else: dp[i][j] = max(dp[i-1][j], dp[i][j-1])
 */
int longestCommonSubsequence(const std::string& text1, const std::string& text2);
int longestCommonSubsequenceSolution(const std::string& text1, const std::string& text2);

/**
 * 题目2: 最长公共子串
 *
 * 给定两个字符串 str1 和 str2，返回这两个字符串的最长公共子串的长度。
 *
 * 子串必须是连续的。
 *
 * 示例:
 *   输入: str1 = "abcde", str2 = "abfce"
 *   输出: 2
 *   解释: 最长公共子串是 "ab"
 *
 * 状态定义: dp[i][j] = 以 str1[i-1] 和 str2[j-1] 结尾的最长公共子串长度
 * 状态转移:
 *   if str1[i-1] == str2[j-1]: dp[i][j] = dp[i-1][j-1] + 1
 *   else: dp[i][j] = 0
 */
int longestCommonSubstring(const std::string& str1, const std::string& str2);
int longestCommonSubstringSolution(const std::string& str1, const std::string& str2);

/**
 * 题目3: 编辑距离
 *
 * 给你两个单词 word1 和 word2，请返回将 word1 转换成 word2 所使用的最少操作数。
 *
 * 你可以对一个单词进行如下三种操作：
 *   - 插入一个字符
 *   - 删除一个字符
 *   - 替换一个字符
 *
 * 示例:
 *   输入: word1 = "horse", word2 = "ros"
 *   输出: 3
 *   解释:
 *     horse -> rorse (将 'h' 替换为 'r')
 *     rorse -> rose (删除 'r')
 *     rose -> ros (删除 'e')
 *
 *   输入: word1 = "intention", word2 = "execution"
 *   输出: 5
 */
int minDistance(const std::string& word1, const std::string& word2);
int minDistanceSolution(const std::string& word1, const std::string& word2);

/**
 * 题目4: 两个字符串的删除操作
 *
 * 给定两个单词 word1 和 word2，返回使得 word1 和 word2 相同所需的最小步数。
 * 每步可以删除任意一个字符串中的一个字符。
 *
 * 示例:
 *   输入: word1 = "sea", word2 = "eat"
 *   输出: 2
 *   解释: 第一步将 "sea" 变为 "ea"，第二步将 "eat" 变为 "ea"
 *
 * 思路: 找到 LCS，然后计算需要删除的字符数
 *       答案 = len(word1) + len(word2) - 2 * LCS
 */
int minDeleteDistance(const std::string& word1, const std::string& word2);
int minDeleteDistanceSolution(const std::string& word1, const std::string& word2);

/**
 * 题目5: 不同的子序列
 *
 * 给定一个字符串 s 和一个字符串 t，计算在 s 的子序列中 t 出现的个数。
 *
 * 示例:
 *   输入: s = "rabbbit", t = "rabbit"
 *   输出: 3
 *   解释: 有 3 种方式可以从 s 中得到 "rabbit"
 *         rabbbit (选 1,2,3,4,6,7)
 *         rabbbit (选 1,2,3,5,6,7)
 *         rabbbit (选 1,2,4,5,6,7)
 *
 *   输入: s = "babgbag", t = "bag"
 *   输出: 5
 *
 * 状态定义: dp[i][j] = s[0..i-1] 的子序列中 t[0..j-1] 出现的个数
 * 状态转移:
 *   if s[i-1] == t[j-1]: dp[i][j] = dp[i-1][j-1] + dp[i-1][j]
 *   else: dp[i][j] = dp[i-1][j]
 */
int numDistinct(const std::string& s, const std::string& t);
int numDistinctSolution(const std::string& s, const std::string& t);

/**
 * 题目6: 判断子序列
 *
 * 给定字符串 s 和 t，判断 s 是否为 t 的子序列。
 *
 * 示例:
 *   输入: s = "abc", t = "ahbgdc"
 *   输出: true
 *
 *   输入: s = "axc", t = "ahbgdc"
 *   输出: false
 *
 * 进阶: 如果有大量输入的 s，如何优化？
 */
bool isSubsequence(const std::string& s, const std::string& t);
bool isSubsequenceSolution(const std::string& s, const std::string& t);

/**
 * 题目7: 最长回文子序列
 *
 * 给你一个字符串 s，找出其中最长的回文子序列，并返回该序列的长度。
 *
 * 子序列定义为：不改变剩余字符顺序的情况下，删除某些字符或者不删除任何字符形成的一个序列。
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
 * 思路: LCS(s, reverse(s)) 或者区间 DP
 */
int longestPalindromeSubseq(const std::string& s);
int longestPalindromeSubseqSolution(const std::string& s);

/**
 * 题目8: 最长递增子序列的个数
 *
 * 给定一个未排序的整数数组 nums，返回最长递增子序列的个数。
 *
 * 示例:
 *   输入: nums = [1,3,5,4,7]
 *   输出: 2
 *   解释: 有两个最长递增子序列，分别是 [1,3,4,7] 和 [1,3,5,7]
 *
 *   输入: nums = [2,2,2,2,2]
 *   输出: 5
 *   解释: 最长递增子序列的长度是 1，并且存在 5 个子序列的长度为 1
 */
int findNumberOfLIS(std::vector<int>& nums);
int findNumberOfLISSolution(std::vector<int>& nums);

/**
 * 题目9: 俄罗斯套娃信封问题
 *
 * 给你一个二维整数数组 envelopes，其中 envelopes[i] = [wi, hi]，
 * 表示第 i 个信封的宽度和高度。
 *
 * 当另一个信封的宽度和高度都比这个信封大的时候，这个信封就可以放进另一个信封里。
 *
 * 计算最多能有多少个信封能组成一组"俄罗斯套娃"信封（即可以把一个信封放到另一个信封里面）。
 *
 * 注意：不允许旋转信封。
 *
 * 示例:
 *   输入: envelopes = [[5,4],[6,4],[6,7],[2,3]]
 *   输出: 3
 *   解释: 最多信封的个数为 3, 组合为: [2,3] => [5,4] => [6,7]
 *
 * 思路: 按宽度升序、高度降序排序，然后对高度求 LIS
 */
int maxEnvelopes(std::vector<std::vector<int>>& envelopes);
int maxEnvelopesSolution(std::vector<std::vector<int>>& envelopes);

// ==================== 测试函数声明 ====================

void testSubsequenceDP();          // 测试面试者实现
void testSubsequenceDPSolution();  // 测试解答实现

// runTests 在头文件 inline 定义，调用两个测试函数
inline void runTests() {
    testSubsequenceDP();
    testSubsequenceDPSolution();
}

} // namespace SubsequenceDP

#endif // SUBSEQUENCE_DP_H
