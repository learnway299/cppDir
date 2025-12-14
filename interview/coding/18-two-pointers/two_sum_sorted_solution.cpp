/**
 * @file two_sum_sorted_solution.cpp
 * @brief 两数之和（有序数组）- 参考答案
 */

#include "two_sum_sorted.h"
#include <iostream>
#include <cassert>

namespace TwoSumSortedImpl {

namespace Solution {

std::vector<int> twoSum(std::vector<int>& numbers, int target) {
    int left = 0, right = static_cast<int>(numbers.size()) - 1;
    while (left < right) {
        int sum = numbers[left] + numbers[right];
        if (sum == target) {
            return {left + 1, right + 1};  // 1-indexed
        } else if (sum < target) {
            ++left;
        } else {
            --right;
        }
    }
    return {};
}

} // namespace Solution

// ==================== 测试函数 ====================

void runTests() {
    std::cout << "=== Two Sum Sorted Tests ===" << std::endl;

    {
        std::vector<int> nums = {2, 7, 11, 15};
        auto result = Solution::twoSum(nums, 9);
        assert(result.size() == 2);
        assert(result[0] == 1 && result[1] == 2);  // nums[0] + nums[1] = 9
    }
    std::cout << "  Two Sum Sorted (basic): PASSED" << std::endl;

    {
        std::vector<int> nums = {2, 3, 4};
        auto result = Solution::twoSum(nums, 6);
        assert(result.size() == 2);
        assert(result[0] == 1 && result[1] == 3);  // nums[0] + nums[2] = 6
    }
    std::cout << "  Two Sum Sorted (another case): PASSED" << std::endl;

    {
        std::vector<int> nums = {-1, 0};
        auto result = Solution::twoSum(nums, -1);
        assert(result.size() == 2);
        assert(result[0] == 1 && result[1] == 2);
    }
    std::cout << "  Two Sum Sorted (negative): PASSED" << std::endl;
}

} // namespace TwoSumSortedImpl

/**
 * 关键要点：
 *
 * 1. 双指针原理：
 *    - 有序数组使用双指针效率高
 *    - left 从最小值开始，right 从最大值开始
 *    - sum < target: 需要更大的数，left++
 *    - sum > target: 需要更小的数，right--
 *
 * 2. 时间复杂度：O(n)
 * 3. 空间复杂度：O(1)
 *
 * 4. 与无序数组的区别：
 *    - 无序数组需要哈希表，O(n) 时间 O(n) 空间
 *    - 有序数组用双指针，O(n) 时间 O(1) 空间
 */
