/**
 * @file subsequence_dp.cpp
 * @brief 子序列动态规划问题 - 题目文件
 *
 * 子序列 DP 是经典的 DP 类型，常见问题包括 LCS、编辑距离等。
 * 注意区分子序列（不连续）和子串（连续）。
 */

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

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
 *
 * @param text1 字符串1
 * @param text2 字符串2
 * @return LCS 长度
 */
int longestCommonSubsequence(std::string text1, std::string text2) {
    // TODO: 实现你的解法
    return 0;
}

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
 *
 * @param str1 字符串1
 * @param str2 字符串2
 * @return 最长公共子串长度
 */
int longestCommonSubstring(std::string str1, std::string str2) {
    // TODO: 实现你的解法
    return 0;
}

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
 *
 * @param word1 单词1
 * @param word2 单词2
 * @return 最小编辑距离
 */
int minDistance(std::string word1, std::string word2) {
    // TODO: 实现你的解法
    return 0;
}

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
 *
 * @param word1 单词1
 * @param word2 单词2
 * @return 最小删除步数
 */
int minDeleteDistance(std::string word1, std::string word2) {
    // TODO: 实现你的解法
    return 0;
}

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
 *
 * @param s 源字符串
 * @param t 目标字符串
 * @return t 在 s 子序列中出现的次数
 */
int numDistinct(std::string s, std::string t) {
    // TODO: 实现你的解法
    return 0;
}

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
 *
 * @param s 子序列候选
 * @param t 主字符串
 * @return s 是否为 t 的子序列
 */
bool isSubsequence(std::string s, std::string t) {
    // TODO: 实现你的解法
    return false;
}

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
 *
 * @param s 字符串
 * @return 最长回文子序列长度
 */
int longestPalindromeSubseq(std::string s) {
    // TODO: 实现你的解法
    return 0;
}

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
 *
 * @param nums 整数数组
 * @return LIS 的个数
 */
int findNumberOfLIS(std::vector<int>& nums) {
    // TODO: 实现你的解法
    return 0;
}

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
 *
 * @param envelopes 信封数组
 * @return 最大套娃数
 */
int maxEnvelopes(std::vector<std::vector<int>>& envelopes) {
    // TODO: 实现你的解法
    return 0;
}


// ==================== 测试代码 ====================
#include <cassert>

void testLongestCommonSubsequence() {
    assert(longestCommonSubsequence("abcde", "ace") == 3);
    assert(longestCommonSubsequence("abc", "abc") == 3);
    assert(longestCommonSubsequence("abc", "def") == 0);

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

    std::cout << "minDistance: PASSED\n";
}

void testMinDeleteDistance() {
    assert(minDeleteDistance("sea", "eat") == 2);
    assert(minDeleteDistance("leetcode", "etco") == 4);

    std::cout << "minDeleteDistance: PASSED\n";
}

void testNumDistinct() {
    assert(numDistinct("rabbbit", "rabbit") == 3);
    assert(numDistinct("babgbag", "bag") == 5);

    std::cout << "numDistinct: PASSED\n";
}

void testIsSubsequence() {
    assert(isSubsequence("abc", "ahbgdc") == true);
    assert(isSubsequence("axc", "ahbgdc") == false);

    std::cout << "isSubsequence: PASSED\n";
}

void testLongestPalindromeSubseq() {
    assert(longestPalindromeSubseq("bbbab") == 4);
    assert(longestPalindromeSubseq("cbbd") == 2);

    std::cout << "longestPalindromeSubseq: PASSED\n";
}

void testFindNumberOfLIS() {
    std::vector<int> nums = {1, 3, 5, 4, 7};
    assert(findNumberOfLIS(nums) == 2);

    nums = {2, 2, 2, 2, 2};
    assert(findNumberOfLIS(nums) == 5);

    std::cout << "findNumberOfLIS: PASSED\n";
}

void testMaxEnvelopes() {
    std::vector<std::vector<int>> env = {{5, 4}, {6, 4}, {6, 7}, {2, 3}};
    assert(maxEnvelopes(env) == 3);

    env = {{1, 1}, {1, 1}, {1, 1}};
    assert(maxEnvelopes(env) == 1);

    std::cout << "maxEnvelopes: PASSED\n";
}

int main() {
    std::cout << "=== Subsequence DP Tests ===\n";

    // 取消注释以运行测试（实现解法后）
    // testLongestCommonSubsequence();
    // testLongestCommonSubstring();
    // testMinDistance();
    // testMinDeleteDistance();
    // testNumDistinct();
    // testIsSubsequence();
    // testLongestPalindromeSubseq();
    // testFindNumberOfLIS();
    // testMaxEnvelopes();

    std::cout << "All tests passed!\n";
    return 0;
}
