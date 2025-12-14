/**
 * @file three_sum_solution.cpp
 * @brief 三数之和 - 参考答案
 */

#include "three_sum.h"
#include <algorithm>
#include <iostream>
#include <cstdlib>
#include <cassert>

namespace ThreeSumImpl {

namespace Solution {

std::vector<std::vector<int>> threeSum(std::vector<int>& nums) {
    std::vector<std::vector<int>> result;
    std::sort(nums.begin(), nums.end());
    int n = static_cast<int>(nums.size());

    for (int i = 0; i < n - 2; ++i) {
        if (i > 0 && nums[i] == nums[i - 1]) continue;  // 跳过重复
        if (nums[i] > 0) break;  // 最小值大于 0，不可能有解

        int left = i + 1, right = n - 1;
        while (left < right) {
            int sum = nums[i] + nums[left] + nums[right];
            if (sum == 0) {
                result.push_back({nums[i], nums[left], nums[right]});
                while (left < right && nums[left] == nums[left + 1]) ++left;
                while (left < right && nums[right] == nums[right - 1]) --right;
                ++left;
                --right;
            } else if (sum < 0) {
                ++left;
            } else {
                --right;
            }
        }
    }
    return result;
}

int threeSumClosest(std::vector<int>& nums, int target) {
    std::sort(nums.begin(), nums.end());
    int n = static_cast<int>(nums.size());
    int closest = nums[0] + nums[1] + nums[2];

    for (int i = 0; i < n - 2; ++i) {
        int left = i + 1, right = n - 1;
        while (left < right) {
            int sum = nums[i] + nums[left] + nums[right];
            if (std::abs(sum - target) < std::abs(closest - target)) {
                closest = sum;
            }
            if (sum < target) ++left;
            else if (sum > target) --right;
            else return target;
        }
    }
    return closest;
}

} // namespace Solution

// ==================== 测试函数 ====================

void runTests() {
    std::cout << "=== Three Sum Tests ===" << std::endl;

    // 测试三数之和
    {
        std::vector<int> nums = {-1, 0, 1, 2, -1, -4};
        auto result = Solution::threeSum(nums);
        assert(result.size() == 2);  // [-1,-1,2], [-1,0,1]
    }
    std::cout << "  Three Sum: PASSED" << std::endl;

    // 测试最接近的三数之和
    {
        std::vector<int> nums = {-1, 2, 1, -4};
        assert(Solution::threeSumClosest(nums, 1) == 2);  // -1+2+1=2
    }
    std::cout << "  Three Sum Closest: PASSED" << std::endl;

    // 测试空数组
    {
        std::vector<int> nums = {0, 0, 0};
        auto result = Solution::threeSum(nums);
        assert(result.size() == 1);
    }
    std::cout << "  Three Sum (all zeros): PASSED" << std::endl;
}

} // namespace ThreeSumImpl

/**
 * 关键要点：
 *
 * 1. 三数之和转化为两数之和：
 *    - 先排序
 *    - 固定第一个数 nums[i]
 *    - 对剩余数组用双指针找两数之和
 *
 * 2. 去重技巧：
 *    - 外层循环: i > 0 && nums[i] == nums[i-1] 跳过
 *    - 内层循环: 找到解后跳过相同的 left 和 right
 *
 * 3. 剪枝优化：
 *    - nums[i] > 0 时直接 break（有序后不可能和为 0）
 *
 * 4. 时间复杂度：O(n^2)
 * 5. 空间复杂度：O(1)（不计排序空间）
 */
