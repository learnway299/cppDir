/**
 * @file easy_problems.h
 * @brief 基础编程题集合 - 题目说明与声明
 *
 * 包含常见的基础面试编程题，适合热身和基础考察。
 */

#ifndef EASY_PROBLEMS_H
#define EASY_PROBLEMS_H

#include <vector>
#include <string>

namespace EasyProblems {

// ==================== 题目声明 ====================

/**
 * 题目1: 两数之和
 *
 * 给定一个整数数组 nums 和一个目标值 target，
 * 找出数组中和为目标值的两个数的下标。
 *
 * 示例:
 *   输入: nums = [2, 7, 11, 15], target = 9
 *   输出: [0, 1]  (因为 nums[0] + nums[1] = 2 + 7 = 9)
 *
 * 要求:
 *   - 每个输入只有一个有效答案
 *   - 同一个元素不能使用两次
 *   - 时间复杂度 O(n)
 */
std::vector<int> twoSum(std::vector<int>& nums, int target);
std::vector<int> twoSumSolution(std::vector<int>& nums, int target);

/**
 * 题目2: 反转字符串
 *
 * 编写一个函数，将输入的字符串反转过来。
 * 要求原地修改，使用 O(1) 的额外空间。
 *
 * 示例:
 *   输入: s = ['h','e','l','l','o']
 *   输出: ['o','l','l','e','h']
 */
void reverseString(std::vector<char>& s);
void reverseStringSolution(std::vector<char>& s);

/**
 * 题目3: 回文数判断
 *
 * 判断一个整数是否是回文数。
 * 回文数是指正序和倒序读都一样的整数。
 *
 * 示例:
 *   输入: 121  输出: true
 *   输入: -121 输出: false (从左读是 -121，从右读是 121-)
 *   输入: 10   输出: false
 *
 * 进阶: 不将整数转为字符串来解决
 */
bool isPalindrome(int x);
bool isPalindromeSolution(int x);

/**
 * 题目4: 有效的括号
 *
 * 给定一个只包括 '('，')'，'{'，'}'，'['，']' 的字符串 s，
 * 判断字符串是否有效。
 *
 * 有效字符串需满足：
 *   1. 左括号必须用相同类型的右括号闭合
 *   2. 左括号必须以正确的顺序闭合
 *   3. 每个右括号都有一个对应的相同类型的左括号
 *
 * 示例:
 *   输入: "()"     输出: true
 *   输入: "()[]{}" 输出: true
 *   输入: "(]"     输出: false
 */
bool isValid(const std::string& s);
bool isValidSolution(const std::string& s);

/**
 * 题目5: 合并两个有序数组
 *
 * 给你两个有序整数数组 nums1 和 nums2，将 nums2 合并到 nums1 中，
 * 使 nums1 成为一个有序数组。
 *
 * 说明:
 *   - nums1 的初始长度为 m + n，其中前 m 个元素是有效元素，
 *     后 n 个元素为 0，用于存放 nums2 的元素
 *   - nums2 的长度为 n
 *
 * 示例:
 *   输入: nums1 = [1,2,3,0,0,0], m = 3, nums2 = [2,5,6], n = 3
 *   输出: [1,2,2,3,5,6]
 */
void merge(std::vector<int>& nums1, int m, std::vector<int>& nums2, int n);
void mergeSolution(std::vector<int>& nums1, int m, std::vector<int>& nums2, int n);

/**
 * 题目6: 移除元素
 *
 * 给你一个数组 nums 和一个值 val，你需要原地移除所有数值等于 val 的元素，
 * 并返回移除后数组的新长度。
 *
 * 要求:
 *   - 不要使用额外的数组空间，必须仅使用 O(1) 额外空间
 *   - 元素的顺序可以改变
 *
 * 示例:
 *   输入: nums = [3,2,2,3], val = 3
 *   输出: 2, nums = [2,2,_,_]
 */
int removeElement(std::vector<int>& nums, int val);
int removeElementSolution(std::vector<int>& nums, int val);

/**
 * 题目7: 加一
 *
 * 给定一个由整数组成的非空数组所表示的非负整数，在该数的基础上加一。
 * 最高位数字存放在数组的首位，数组中每个元素只存储单个数字。
 *
 * 示例:
 *   输入: digits = [1,2,3]
 *   输出: [1,2,4]  (123 + 1 = 124)
 *
 *   输入: digits = [9,9,9]
 *   输出: [1,0,0,0]  (999 + 1 = 1000)
 */
std::vector<int> plusOne(std::vector<int>& digits);
std::vector<int> plusOneSolution(std::vector<int>& digits);

/**
 * 题目8: 最大子数组和
 *
 * 给你一个整数数组 nums，请你找出一个具有最大和的连续子数组（至少包含一个元素），
 * 返回其最大和。
 *
 * 示例:
 *   输入: nums = [-2,1,-3,4,-1,2,1,-5,4]
 *   输出: 6  (连续子数组 [4,-1,2,1] 的和最大为 6)
 *
 * 提示: 可以使用动态规划或 Kadane 算法
 */
int maxSubArray(std::vector<int>& nums);
int maxSubArraySolution(std::vector<int>& nums);

// ==================== 测试函数声明 ====================

void testEasyProblems();          // 测试面试者实现
void testEasyProblemsSolution();  // 测试解答实现

// runTests 在头文件 inline 定义，调用两个测试函数
inline void runTests() {
    testEasyProblems();
    testEasyProblemsSolution();
}

} // namespace EasyProblems

#endif // EASY_PROBLEMS_H
