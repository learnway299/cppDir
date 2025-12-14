/**
 * @file sliding_window.h
 * @brief 滑动窗口问题 - 头文件
 *
 * 滑动窗口是处理字符串/数组连续子序列问题的重要技巧。
 * 核心思想：维护一个窗口，通过左右指针的移动来满足条件。
 */

#ifndef INTERVIEW_SLIDING_WINDOW_H
#define INTERVIEW_SLIDING_WINDOW_H

#include <string>
#include <vector>

namespace SlidingWindowProblems {

// ==================== 面试者实现 ====================

/**
 * 题目1: 无重复字符的最长子串
 * 给定一个字符串 s，请你找出其中不含有重复字符的最长子串的长度。
 */
int lengthOfLongestSubstring(std::string s);

/**
 * 题目2: 最小覆盖子串
 * 给你一个字符串 s、一个字符串 t。返回 s 中涵盖 t 所有字符的最小子串。
 */
std::string minWindow(std::string s, std::string t);

/**
 * 题目3: 字符串的排列
 * 给你两个字符串 s1 和 s2，写一个函数来判断 s2 是否包含 s1 的排列。
 */
bool checkInclusion(std::string s1, std::string s2);

/**
 * 题目4: 找到字符串中所有字母异位词
 * 给定两个字符串 s 和 p，找到 s 中所有 p 的异位词的子串，返回这些子串的起始索引。
 */
std::vector<int> findAnagrams(std::string s, std::string p);

/**
 * 题目5: 长度最小的子数组
 * 给定一个含有 n 个正整数的数组和一个正整数 target。
 * 找出该数组中满足其和 ≥ target 的长度最小的连续子数组。
 */
int minSubArrayLen(int target, std::vector<int>& nums);

/**
 * 题目6: 滑动窗口最大值
 * 给你一个整数数组 nums，有一个大小为 k 的滑动窗口从数组的最左侧移动到数组的最右侧。
 * 返回滑动窗口中的最大值。
 */
std::vector<int> maxSlidingWindow(std::vector<int>& nums, int k);

/**
 * 题目7: 至多包含两个不同字符的最长子串
 * 给定一个字符串 s，找出至多包含两个不同字符的最长子串，返回其长度。
 */
int lengthOfLongestSubstringTwoDistinct(std::string s);

/**
 * 题目8: 至多包含 K 个不同字符的最长子串
 * 给定一个字符串 s 和一个整数 k，找出至多包含 k 个不同字符的最长子串。
 */
int lengthOfLongestSubstringKDistinct(std::string s, int k);

/**
 * 题目9: 替换后的最长重复字符
 * 给你一个字符串 s 和一个整数 k。你可以选择字符串中的任一字符并将其更改。
 * 该操作最多可执行 k 次。返回包含相同字母的最长子串的长度。
 */
int characterReplacement(std::string s, int k);

/**
 * 题目10: 子数组最大平均数 I
 * 给你一个由 n 个元素组成的整数数组 nums 和一个整数 k。
 * 请你找出平均数最大且长度为 k 的连续子数组，并输出该最大平均数。
 */
double findMaxAverage(std::vector<int>& nums, int k);

// ==================== 参考答案实现 ====================

int lengthOfLongestSubstringSolution(std::string s);
std::string minWindowSolution(std::string s, std::string t);
bool checkInclusionSolution(std::string s1, std::string s2);
std::vector<int> findAnagramsSolution(std::string s, std::string p);
int minSubArrayLenSolution(int target, std::vector<int>& nums);
std::vector<int> maxSlidingWindowSolution(std::vector<int>& nums, int k);
int lengthOfLongestSubstringTwoDistinctSolution(std::string s);
int lengthOfLongestSubstringKDistinctSolution(std::string s, int k);
int characterReplacementSolution(std::string s, int k);
double findMaxAverageSolution(std::vector<int>& nums, int k);

// ==================== 测试函数声明 ====================

void testSlidingWindow();          // 测试面试者实现
void testSlidingWindowSolution();  // 测试解答实现

// runTests 在头文件 inline 定义，调用两个测试函数
inline void runTests() {
    testSlidingWindow();
    testSlidingWindowSolution();
}

} // namespace SlidingWindowProblems

#endif // INTERVIEW_SLIDING_WINDOW_H
