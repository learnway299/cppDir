/**
 * @file sliding_window.cpp
 * @brief 滑动窗口问题 - 题目文件
 *
 * 滑动窗口是处理字符串/数组连续子序列问题的重要技巧。
 * 核心思想：维护一个窗口，通过左右指针的移动来满足条件。
 */

#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <climits>

/**
 * 题目1: 无重复字符的最长子串
 *
 * 给定一个字符串 s，请你找出其中不含有重复字符的最长子串的长度。
 *
 * 示例:
 *   输入: s = "abcabcbb"
 *   输出: 3
 *   解释: 因为无重复字符的最长子串是 "abc"，所以其长度为 3
 *
 *   输入: s = "bbbbb"
 *   输出: 1
 *
 *   输入: s = "pwwkew"
 *   输出: 3
 *   解释: 答案是 "wke"，长度为 3
 *
 * 思路: 滑动窗口 + 哈希集合/哈希表
 *
 * @param s 输入字符串
 * @return 最长无重复子串长度
 */
int lengthOfLongestSubstring(std::string s) {
    // TODO: 实现你的解法
    return 0;
}

/**
 * 题目2: 最小覆盖子串
 *
 * 给你一个字符串 s、一个字符串 t。返回 s 中涵盖 t 所有字符的最小子串。
 * 如果 s 中不存在涵盖 t 所有字符的子串，则返回空字符串 ""。
 *
 * 注意：
 * - 对于 t 中重复字符，我们寻找的子字符串中该字符数量必须不少于 t 中该字符数量。
 * - 如果 s 中存在这样的子串，我们保证它是唯一的答案。
 *
 * 示例:
 *   输入: s = "ADOBECODEBANC", t = "ABC"
 *   输出: "BANC"
 *
 *   输入: s = "a", t = "a"
 *   输出: "a"
 *
 *   输入: s = "a", t = "aa"
 *   输出: ""
 *
 * 思路: 滑动窗口 + 双指针 + 哈希表计数
 *
 * @param s 源字符串
 * @param t 目标字符串
 * @return 最小覆盖子串
 */
std::string minWindow(std::string s, std::string t) {
    // TODO: 实现你的解法
    return "";
}

/**
 * 题目3: 字符串的排列
 *
 * 给你两个字符串 s1 和 s2，写一个函数来判断 s2 是否包含 s1 的排列。
 * 如果是，返回 true；否则，返回 false。
 *
 * 换句话说，s1 的排列之一是 s2 的子串。
 *
 * 示例:
 *   输入: s1 = "ab", s2 = "eidbaooo"
 *   输出: true
 *   解释: s2 包含 s1 的排列之一 ("ba")
 *
 *   输入: s1 = "ab", s2 = "eidboaoo"
 *   输出: false
 *
 * 思路: 固定大小的滑动窗口
 *
 * @param s1 模式串
 * @param s2 文本串
 * @return 是否包含排列
 */
bool checkInclusion(std::string s1, std::string s2) {
    // TODO: 实现你的解法
    return false;
}

/**
 * 题目4: 找到字符串中所有字母异位词
 *
 * 给定两个字符串 s 和 p，找到 s 中所有 p 的异位词的子串，返回这些子串的起始索引。
 * 不考虑答案输出的顺序。
 *
 * 异位词指由相同字母重排列形成的字符串（包括相同的字符串）。
 *
 * 示例:
 *   输入: s = "cbaebabacd", p = "abc"
 *   输出: [0, 6]
 *   解释:
 *     起始索引等于 0 的子串是 "cba", 它是 "abc" 的异位词
 *     起始索引等于 6 的子串是 "bac", 它是 "abc" 的异位词
 *
 *   输入: s = "abab", p = "ab"
 *   输出: [0, 1, 2]
 *
 * 思路: 固定大小滑动窗口 + 字符计数
 *
 * @param s 文本串
 * @param p 模式串
 * @return 所有异位词起始索引
 */
std::vector<int> findAnagrams(std::string s, std::string p) {
    // TODO: 实现你的解法
    return {};
}

/**
 * 题目5: 长度最小的子数组
 *
 * 给定一个含有 n 个正整数的数组和一个正整数 target。
 * 找出该数组中满足其和 ≥ target 的长度最小的连续子数组，
 * 并返回其长度。如果不存在符合条件的子数组，返回 0。
 *
 * 示例:
 *   输入: target = 7, nums = [2,3,1,2,4,3]
 *   输出: 2
 *   解释: 子数组 [4,3] 是该条件下的长度最小的子数组
 *
 *   输入: target = 4, nums = [1,4,4]
 *   输出: 1
 *
 *   输入: target = 11, nums = [1,1,1,1,1,1,1,1]
 *   输出: 0
 *
 * 思路: 可变大小滑动窗口
 *
 * @param target 目标和
 * @param nums 正整数数组
 * @return 最小长度
 */
int minSubArrayLen(int target, std::vector<int>& nums) {
    // TODO: 实现你的解法
    return 0;
}

/**
 * 题目6: 滑动窗口最大值
 *
 * 给你一个整数数组 nums，有一个大小为 k 的滑动窗口从数组的最左侧移动到数组的最右侧。
 * 你只可以看到在滑动窗口内的 k 个数字。滑动窗口每次只向右移动一位。
 * 返回滑动窗口中的最大值。
 *
 * 示例:
 *   输入: nums = [1,3,-1,-3,5,3,6,7], k = 3
 *   输出: [3,3,5,5,6,7]
 *   解释:
 *     滑动窗口的位置                最大值
 *     ---------------               -----
 *     [1  3  -1] -3  5  3  6  7       3
 *      1 [3  -1  -3] 5  3  6  7       3
 *      1  3 [-1  -3  5] 3  6  7       5
 *      1  3  -1 [-3  5  3] 6  7       5
 *      1  3  -1  -3 [5  3  6] 7       6
 *      1  3  -1  -3  5 [3  6  7]      7
 *
 * 思路: 单调递减队列（双端队列）
 *
 * @param nums 整数数组
 * @param k 窗口大小
 * @return 每个窗口的最大值
 */
std::vector<int> maxSlidingWindow(std::vector<int>& nums, int k) {
    // TODO: 实现你的解法
    return {};
}

/**
 * 题目7: 至多包含两个不同字符的最长子串
 *
 * 给定一个字符串 s，找出至多包含两个不同字符的最长子串，返回其长度。
 *
 * 示例:
 *   输入: s = "eceba"
 *   输出: 3
 *   解释: 子串 "ece" 的长度为 3
 *
 *   输入: s = "ccaabbb"
 *   输出: 5
 *   解释: 子串 "aabbb" 的长度为 5
 *
 * 思路: 滑动窗口 + 哈希表记录字符种类
 *
 * @param s 输入字符串
 * @return 最长子串长度
 */
int lengthOfLongestSubstringTwoDistinct(std::string s) {
    // TODO: 实现你的解法
    return 0;
}

/**
 * 题目8: 至多包含 K 个不同字符的最长子串
 *
 * 给定一个字符串 s 和一个整数 k，找出至多包含 k 个不同字符的最长子串，返回其长度。
 *
 * 示例:
 *   输入: s = "eceba", k = 2
 *   输出: 3
 *   解释: 子串 "ece" 的长度为 3
 *
 *   输入: s = "aa", k = 1
 *   输出: 2
 *
 * @param s 输入字符串
 * @param k 最多不同字符数
 * @return 最长子串长度
 */
int lengthOfLongestSubstringKDistinct(std::string s, int k) {
    // TODO: 实现你的解法
    return 0;
}

/**
 * 题目9: 替换后的最长重复字符
 *
 * 给你一个字符串 s 和一个整数 k。你可以选择字符串中的任一字符，
 * 并将其更改为任何其他大写英文字符。该操作最多可执行 k 次。
 *
 * 在执行上述操作后，返回包含相同字母的最长子串的长度。
 *
 * 示例:
 *   输入: s = "ABAB", k = 2
 *   输出: 4
 *   解释: 用两个'A'替换为两个'B'，反之亦然
 *
 *   输入: s = "AABABBA", k = 1
 *   输出: 4
 *   解释: 将中间的一个'A'替换为'B'，得到 "AABBBBA" 子串 "BBBB" 长度为 4
 *
 * 思路: 滑动窗口，窗口长度 - 窗口内最多字符数 <= k
 *
 * @param s 输入字符串
 * @param k 最多替换次数
 * @return 最长重复子串长度
 */
int characterReplacement(std::string s, int k) {
    // TODO: 实现你的解法
    return 0;
}

/**
 * 题目10: 子数组最大平均数 I
 *
 * 给你一个由 n 个元素组成的整数数组 nums 和一个整数 k。
 * 请你找出平均数最大且长度为 k 的连续子数组，并输出该最大平均数。
 *
 * 示例:
 *   输入: nums = [1,12,-5,-6,50,3], k = 4
 *   输出: 12.75
 *   解释: 最大平均数 (12-5-6+50)/4 = 51/4 = 12.75
 *
 *   输入: nums = [5], k = 1
 *   输出: 5.0
 *
 * 思路: 固定大小滑动窗口
 *
 * @param nums 整数数组
 * @param k 窗口大小
 * @return 最大平均数
 */
double findMaxAverage(std::vector<int>& nums, int k) {
    // TODO: 实现你的解法
    return 0.0;
}


// ==================== 测试代码 ====================
#include <cassert>
#include <cmath>

void testLengthOfLongestSubstring() {
    assert(lengthOfLongestSubstring("abcabcbb") == 3);
    assert(lengthOfLongestSubstring("bbbbb") == 1);
    assert(lengthOfLongestSubstring("pwwkew") == 3);
    assert(lengthOfLongestSubstring("") == 0);

    std::cout << "lengthOfLongestSubstring: PASSED\n";
}

void testMinWindow() {
    assert(minWindow("ADOBECODEBANC", "ABC") == "BANC");
    assert(minWindow("a", "a") == "a");
    assert(minWindow("a", "aa") == "");

    std::cout << "minWindow: PASSED\n";
}

void testCheckInclusion() {
    assert(checkInclusion("ab", "eidbaooo") == true);
    assert(checkInclusion("ab", "eidboaoo") == false);

    std::cout << "checkInclusion: PASSED\n";
}

void testFindAnagrams() {
    std::vector<int> expected1 = {0, 6};
    assert(findAnagrams("cbaebabacd", "abc") == expected1);

    std::vector<int> expected2 = {0, 1, 2};
    assert(findAnagrams("abab", "ab") == expected2);

    std::cout << "findAnagrams: PASSED\n";
}

void testMinSubArrayLen() {
    std::vector<int> nums1 = {2, 3, 1, 2, 4, 3};
    assert(minSubArrayLen(7, nums1) == 2);

    std::vector<int> nums2 = {1, 4, 4};
    assert(minSubArrayLen(4, nums2) == 1);

    std::vector<int> nums3 = {1, 1, 1, 1, 1, 1, 1, 1};
    assert(minSubArrayLen(11, nums3) == 0);

    std::cout << "minSubArrayLen: PASSED\n";
}

void testMaxSlidingWindow() {
    std::vector<int> nums = {1, 3, -1, -3, 5, 3, 6, 7};
    std::vector<int> expected = {3, 3, 5, 5, 6, 7};
    assert(maxSlidingWindow(nums, 3) == expected);

    std::cout << "maxSlidingWindow: PASSED\n";
}

void testLengthOfLongestSubstringTwoDistinct() {
    assert(lengthOfLongestSubstringTwoDistinct("eceba") == 3);
    assert(lengthOfLongestSubstringTwoDistinct("ccaabbb") == 5);

    std::cout << "lengthOfLongestSubstringTwoDistinct: PASSED\n";
}

void testLengthOfLongestSubstringKDistinct() {
    assert(lengthOfLongestSubstringKDistinct("eceba", 2) == 3);
    assert(lengthOfLongestSubstringKDistinct("aa", 1) == 2);

    std::cout << "lengthOfLongestSubstringKDistinct: PASSED\n";
}

void testCharacterReplacement() {
    assert(characterReplacement("ABAB", 2) == 4);
    assert(characterReplacement("AABABBA", 1) == 4);

    std::cout << "characterReplacement: PASSED\n";
}

void testFindMaxAverage() {
    std::vector<int> nums = {1, 12, -5, -6, 50, 3};
    assert(std::abs(findMaxAverage(nums, 4) - 12.75) < 1e-5);

    std::cout << "findMaxAverage: PASSED\n";
}

int main() {
    std::cout << "=== Sliding Window Tests ===\n";

    // 取消注释以运行测试（实现解法后）
    // testLengthOfLongestSubstring();
    // testMinWindow();
    // testCheckInclusion();
    // testFindAnagrams();
    // testMinSubArrayLen();
    // testMaxSlidingWindow();
    // testLengthOfLongestSubstringTwoDistinct();
    // testLengthOfLongestSubstringKDistinct();
    // testCharacterReplacement();
    // testFindMaxAverage();

    std::cout << "All tests passed!\n";
    return 0;
}
