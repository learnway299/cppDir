/**
 * @file single_number_solution.cpp
 * @brief 只出现一次的数字 - 参考解答
 */

#include "single_number.h"
#include <iostream>
#include <cassert>

namespace SingleNumberImpl {
namespace Solution {

// ==================== 题目1: 只出现一次的数字 I ====================
/**
 * singleNumber - 只出现一次的数字I（其他出现两次）
 * 时间复杂度: O(n) - 遍历数组一次
 * 空间复杂度: O(1) - 常数空间
 * 特点: 异或性质 a^a=0, a^0=a
 */
int singleNumber(std::vector<int>& nums) {
    int result = 0;
    for (int n : nums) result ^= n;
    return result;
}

// ==================== 题目2: 只出现一次的数字 II ====================
/**
 * singleNumber2 - 只出现一次的数字II（其他出现三次）
 * 时间复杂度: O(32n) = O(n) - 32位各遍历一次
 * 空间复杂度: O(1) - 常数空间
 * 特点: 位统计法，每位1的个数模3
 */
int singleNumber2(std::vector<int>& nums) {
    int result = 0;
    for (int i = 0; i < 32; ++i) {
        int sum = 0;
        for (int n : nums) {
            sum += (n >> i) & 1;
        }
        if (sum % 3) result |= (1 << i);
    }
    return result;
}

// ==================== 题目3: 只出现一次的数字 III ====================
/**
 * singleNumber3 - 只出现一次的数字III（两个只出现一次）
 * 时间复杂度: O(n) - 遍历数组两次
 * 空间复杂度: O(1) - 常数空间
 * 特点: 分组异或，按lowBit分成两组
 */
std::vector<int> singleNumber3(std::vector<int>& nums) {
    unsigned int xorAll = 0;
    for (int n : nums) xorAll ^= n;

    unsigned int lowBit = xorAll & (-xorAll);
    int a = 0, b = 0;
    for (int n : nums) {
        if (n & lowBit) a ^= n;
        else b ^= n;
    }
    return {a, b};
}

} // namespace Solution

// ==================== 测试函数 ====================
void runTests() {
    std::cout << "=== Single Number Solution ===" << std::endl;

    // 测试只出现一次的数字 I
    {
        std::vector<int> nums = {2, 2, 1};
        assert(Solution::singleNumber(nums) == 1);
    }
    std::cout << "  Single Number I: PASSED" << std::endl;

    // 测试只出现一次的数字 II
    {
        std::vector<int> nums = {2, 2, 3, 2};
        assert(Solution::singleNumber2(nums) == 3);
    }
    std::cout << "  Single Number II: PASSED" << std::endl;

    // 测试只出现一次的数字 III
    {
        std::vector<int> nums = {1, 2, 1, 3, 2, 5};
        auto result = Solution::singleNumber3(nums);
        assert(result.size() == 2);
        // 3 和 5 是答案（顺序可能不同）
        assert((result[0] == 3 && result[1] == 5) || (result[0] == 5 && result[1] == 3));
    }
    std::cout << "  Single Number III: PASSED" << std::endl;
}

} // namespace SingleNumberImpl

/**
 * 关键要点：
 *
 * 1. 只出现一次 I（异或）：
 *    - a ^ a = 0
 *    - a ^ 0 = a
 *    - 成对的数异或抵消
 *
 * 2. 只出现一次 II（位统计）：
 *    - 统计每位上 1 的个数
 *    - 模 3 得到答案的该位
 *
 * 3. 只出现一次 III（分组异或）：
 *    - 异或得到 a ^ b
 *    - 找到 a 和 b 不同的一位
 *    - 按该位分组后分别异或
 *
 * 4. lowBit = x & (-x)：
 *    - 获取最低位的 1
 *    - 用于分组
 *
 * 5. 时间复杂度：O(n)
 * 6. 空间复杂度：O(1)
 */
