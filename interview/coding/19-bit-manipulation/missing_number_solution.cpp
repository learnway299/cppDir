/**
 * @file missing_number_solution.cpp
 * @brief 丢失的数字 - 参考解答
 */

#include "missing_number.h"
#include <iostream>
#include <cassert>

namespace MissingNumberImpl {
namespace Solution {

// ==================== 方法1: 异或法 ====================
// a ^ a = 0, 0 ^ a = a
// 将 [0,n] 与数组所有元素异或，成对的抵消
int missingNumber(std::vector<int>& nums) {
    int n = nums.size();
    int xorAll = n;  // 从n开始
    for (int i = 0; i < n; ++i) {
        xorAll ^= i ^ nums[i];
    }
    return xorAll;
}

// ==================== 方法2: 数学公式 ====================
int missingNumberSum(std::vector<int>& nums) {
    int n = nums.size();
    int expectedSum = n * (n + 1) / 2;
    int actualSum = 0;
    for (int num : nums) actualSum += num;
    return expectedSum - actualSum;
}

} // namespace Solution

// ==================== 测试函数 ====================
void runTests() {
    std::cout << "=== Missing Number Solution ===" << std::endl;

    // 测试异或法
    {
        std::vector<int> nums = {3, 0, 1};
        assert(Solution::missingNumber(nums) == 2);
    }
    std::cout << "  Missing Number (XOR): PASSED" << std::endl;

    // 测试较长数组
    {
        std::vector<int> nums = {9, 6, 4, 2, 3, 5, 7, 0, 1};
        assert(Solution::missingNumber(nums) == 8);
    }
    std::cout << "  Missing Number (long array): PASSED" << std::endl;

    // 测试数学公式法
    {
        std::vector<int> nums = {3, 0, 1};
        assert(Solution::missingNumberSum(nums) == 2);
    }
    std::cout << "  Missing Number (sum): PASSED" << std::endl;

    // 验证两种方法一致
    {
        std::vector<int> nums = {0, 1, 3, 4, 5};
        assert(Solution::missingNumber(nums) == Solution::missingNumberSum(nums));
    }
    std::cout << "  Both methods consistent: PASSED" << std::endl;
}

} // namespace MissingNumberImpl

/**
 * 关键要点：
 *
 * 1. 异或法：
 *    - a ^ a = 0, a ^ 0 = a
 *    - 将 0~n 与数组元素全部异或
 *    - 出现两次的数抵消，剩下缺失的
 *    - 时间 O(n)，空间 O(1)
 *
 * 2. 数学公式法：
 *    - 计算 0~n 的期望和：n*(n+1)/2
 *    - 减去实际和得到缺失的数
 *    - 注意大数溢出（可用 long）
 *
 * 3. 其他方法：
 *    - 排序后二分查找
 *    - 哈希集合
 *
 * 4. 时间复杂度：O(n)
 * 5. 空间复杂度：O(1)
 */
