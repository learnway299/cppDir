/**
 * @file four_sum_solution.cpp
 * @brief 四数之和 - 参考答案
 */

#include "four_sum.h"
#include <algorithm>
#include <iostream>
#include <cassert>

namespace FourSumImpl {

namespace Solution {

std::vector<std::vector<int>> fourSum(std::vector<int>& nums, int target) {
    std::vector<std::vector<int>> result;
    int n = static_cast<int>(nums.size());
    if (n < 4) return result;

    std::sort(nums.begin(), nums.end());

    for (int i = 0; i < n - 3; ++i) {
        if (i > 0 && nums[i] == nums[i - 1]) continue;  // 去重

        for (int j = i + 1; j < n - 2; ++j) {
            if (j > i + 1 && nums[j] == nums[j - 1]) continue;  // 去重

            int left = j + 1, right = n - 1;
            while (left < right) {
                long long sum = static_cast<long long>(nums[i]) + nums[j] + nums[left] + nums[right];

                if (sum == target) {
                    result.push_back({nums[i], nums[j], nums[left], nums[right]});
                    // 去重
                    while (left < right && nums[left] == nums[left + 1]) ++left;
                    while (left < right && nums[right] == nums[right - 1]) --right;
                    ++left;
                    --right;
                } else if (sum < target) {
                    ++left;
                } else {
                    --right;
                }
            }
        }
    }
    return result;
}

} // namespace Solution

// ==================== 测试函数 ====================

void runTests() {
    std::cout << "=== Four Sum Tests ===" << std::endl;

    {
        std::vector<int> nums = {1, 0, -1, 0, -2, 2};
        auto result = Solution::fourSum(nums, 0);
        assert(result.size() == 3);  // [-2,-1,1,2], [-2,0,0,2], [-1,0,0,1]
    }
    std::cout << "  Four Sum (basic): PASSED" << std::endl;

    {
        std::vector<int> nums = {2, 2, 2, 2, 2};
        auto result = Solution::fourSum(nums, 8);
        assert(result.size() == 1);  // [2,2,2,2]
    }
    std::cout << "  Four Sum (duplicates): PASSED" << std::endl;

    {
        std::vector<int> nums = {-3, -1, 0, 2, 4, 5};
        auto result = Solution::fourSum(nums, 2);
        assert(result.size() == 2);  // [[-3,-1,2,4], [-3,0,1,4]]
    }
    std::cout << "  Four Sum (various): PASSED" << std::endl;
}

} // namespace FourSumImpl

/**
 * 关键要点：
 *
 * 1. 扩展三数之和：
 *    - 外层两重循环固定两个数 i, j
 *    - 内层用双指针找剩余两数 left, right
 *    - 四数之和 = nums[i] + nums[j] + nums[left] + nums[right]
 *
 * 2. 去重逻辑（三个位置）：
 *    - 第一层: i > 0 && nums[i] == nums[i-1] -> continue
 *    - 第二层: j > i+1 && nums[j] == nums[j-1] -> continue
 *    - 双指针: 找到解后跳过所有相同元素
 *
 * 3. 溢出处理：
 *    - 四数之和可能溢出 int 范围
 *    - 使用 long long 进行计算
 *
 * 4. 时间复杂度：O(n^3)
 *    - 两层循环 O(n^2)，内层双指针 O(n)
 *
 * 5. 空间复杂度：O(1)
 *    - 不算结果数组，只使用常数空间
 *
 * 6. K 数之和通用模式：
 *    - K-2 层循环 + 双指针
 *    - 时间复杂度 O(n^(K-1))
 */
