/**
 * @file easy_problems.cpp
 * @brief 基础编程题集合 - 题目文件
 *
 * 包含常见的基础面试编程题，适合热身和基础考察。
 * 每个函数需要你实现具体逻辑。
 */

#include <vector>
#include <string>

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
 *
 * @param nums 整数数组
 * @param target 目标和
 * @return 两个数的下标
 */
std::vector<int> twoSum(std::vector<int>& nums, int target) {
    // TODO: 实现你的解法
    return {};
}

/**
 * 题目2: 反转字符串
 *
 * 编写一个函数，将输入的字符串反转过来。
 * 要求原地修改，使用 O(1) 的额外空间。
 *
 * 示例:
 *   输入: s = ['h','e','l','l','o']
 *   输出: ['o','l','l','e','h']
 *
 * @param s 字符数组
 */
void reverseString(std::vector<char>& s) {
    // TODO: 实现你的解法
}

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
 *
 * @param x 整数
 * @return 是否为回文数
 */
bool isPalindrome(int x) {
    // TODO: 实现你的解法
    return false;
}

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
 *   输入: "([)]"   输出: false
 *   输入: "{[]}"   输出: true
 *
 * @param s 括号字符串
 * @return 是否有效
 */
bool isValid(std::string s) {
    // TODO: 实现你的解法
    return false;
}

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
 *
 * @param nums1 第一个有序数组（有额外空间）
 * @param m nums1 的有效元素个数
 * @param nums2 第二个有序数组
 * @param n nums2 的元素个数
 */
void merge(std::vector<int>& nums1, int m, std::vector<int>& nums2, int n) {
    // TODO: 实现你的解法
}

/**
 * 题目6: 移除元素
 *
 * 给你一个数组 nums 和一个值 val，你需要原地移除所有数值等于 val 的元素，
 * 并返回移除后数组的新长度。
 *
 * 要求:
 *   - 不要使用额外的数组空间，必须仅使用 O(1) 额外空间
 *   - 元素的顺序可以改变
 *   - 不需要考虑数组中超出新长度后面的元素
 *
 * 示例:
 *   输入: nums = [3,2,2,3], val = 3
 *   输出: 2, nums = [2,2,_,_]
 *
 *   输入: nums = [0,1,2,2,3,0,4,2], val = 2
 *   输出: 5, nums = [0,1,3,0,4,_,_,_]
 *
 * @param nums 数组
 * @param val 要移除的值
 * @return 新数组长度
 */
int removeElement(std::vector<int>& nums, int val) {
    // TODO: 实现你的解法
    return 0;
}

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
 *
 * @param digits 数字数组
 * @return 加一后的数组
 */
std::vector<int> plusOne(std::vector<int>& digits) {
    // TODO: 实现你的解法
    return {};
}

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
 *   输入: nums = [1]
 *   输出: 1
 *
 *   输入: nums = [-1]
 *   输出: -1
 *
 * 提示: 可以使用动态规划或 Kadane 算法
 *
 * @param nums 整数数组
 * @return 最大子数组和
 */
int maxSubArray(std::vector<int>& nums) {
    // TODO: 实现你的解法
    return 0;
}


// ==================== 测试代码 ====================
#include <iostream>
#include <cassert>

void testTwoSum() {
    std::vector<int> nums = {2, 7, 11, 15};
    auto result = twoSum(nums, 9);
    assert(result.size() == 2);
    assert(nums[result[0]] + nums[result[1]] == 9);
    std::cout << "twoSum: PASSED\n";
}

void testReverseString() {
    std::vector<char> s = {'h', 'e', 'l', 'l', 'o'};
    reverseString(s);
    assert(s == std::vector<char>({'o', 'l', 'l', 'e', 'h'}));
    std::cout << "reverseString: PASSED\n";
}

void testIsPalindrome() {
    assert(isPalindrome(121) == true);
    assert(isPalindrome(-121) == false);
    assert(isPalindrome(10) == false);
    assert(isPalindrome(0) == true);
    std::cout << "isPalindrome: PASSED\n";
}

void testIsValid() {
    assert(isValid("()") == true);
    assert(isValid("()[]{}") == true);
    assert(isValid("(]") == false);
    assert(isValid("([)]") == false);
    assert(isValid("{[]}") == true);
    std::cout << "isValid: PASSED\n";
}

void testMerge() {
    std::vector<int> nums1 = {1, 2, 3, 0, 0, 0};
    std::vector<int> nums2 = {2, 5, 6};
    merge(nums1, 3, nums2, 3);
    assert(nums1 == std::vector<int>({1, 2, 2, 3, 5, 6}));
    std::cout << "merge: PASSED\n";
}

void testRemoveElement() {
    std::vector<int> nums = {3, 2, 2, 3};
    int len = removeElement(nums, 3);
    assert(len == 2);
    std::cout << "removeElement: PASSED\n";
}

void testPlusOne() {
    std::vector<int> digits1 = {1, 2, 3};
    assert(plusOne(digits1) == std::vector<int>({1, 2, 4}));

    std::vector<int> digits2 = {9, 9, 9};
    assert(plusOne(digits2) == std::vector<int>({1, 0, 0, 0}));
    std::cout << "plusOne: PASSED\n";
}

void testMaxSubArray() {
    std::vector<int> nums = {-2, 1, -3, 4, -1, 2, 1, -5, 4};
    assert(maxSubArray(nums) == 6);
    std::cout << "maxSubArray: PASSED\n";
}

int main() {
    std::cout << "=== Easy Problems Tests ===\n";

    // 取消注释以运行测试（实现解法后）
    // testTwoSum();
    // testReverseString();
    // testIsPalindrome();
    // testIsValid();
    // testMerge();
    // testRemoveElement();
    // testPlusOne();
    // testMaxSubArray();

    std::cout << "All tests passed!\n";
    return 0;
}
