/**
 * @file permutation_solution.cpp
 * @brief 排列问题 - 参考答案
 */
#include "permutation.h"
#include <algorithm>
#include <iostream>
#include <cassert>

namespace Permutation {

// ==================== 辅助函数 ====================
static void backtrackPermute(std::vector<int>& nums, int start,
                              std::vector<std::vector<int>>& result) {
    if (start == static_cast<int>(nums.size())) {
        result.push_back(nums);
        return;
    }
    for (int i = start; i < static_cast<int>(nums.size()); ++i) {
        std::swap(nums[start], nums[i]);
        backtrackPermute(nums, start + 1, result);
        std::swap(nums[start], nums[i]);
    }
}

static void backtrackPermuteUnique(std::vector<int>& nums, std::vector<bool>& used,
                                    std::vector<int>& path,
                                    std::vector<std::vector<int>>& result) {
    if (path.size() == nums.size()) {
        result.push_back(path);
        return;
    }
    for (size_t i = 0; i < nums.size(); ++i) {
        if (used[i]) continue;
        if (i > 0 && nums[i] == nums[i - 1] && !used[i - 1]) continue;
        used[i] = true;
        path.push_back(nums[i]);
        backtrackPermuteUnique(nums, used, path, result);
        path.pop_back();
        used[i] = false;
    }
}

// ==================== 参考答案实现 ====================

std::vector<std::vector<int>> permuteSolution(std::vector<int>& nums) {
    std::vector<std::vector<int>> result;
    backtrackPermute(nums, 0, result);
    return result;
}

std::vector<std::vector<int>> permuteUniqueSolution(std::vector<int>& nums) {
    std::vector<std::vector<int>> result;
    std::sort(nums.begin(), nums.end());
    std::vector<bool> used(nums.size(), false);
    std::vector<int> path;
    backtrackPermuteUnique(nums, used, path, result);
    return result;
}

void nextPermutationSolution(std::vector<int>& nums) {
    int n = static_cast<int>(nums.size()), i = n - 2;
    while (i >= 0 && nums[i] >= nums[i + 1]) --i;
    if (i >= 0) {
        int j = n - 1;
        while (nums[j] <= nums[i]) --j;
        std::swap(nums[i], nums[j]);
    }
    std::reverse(nums.begin() + i + 1, nums.end());
}

// ==================== 测试函数 ====================

void runTests() {
    std::cout << "\n=== Permutation Tests ===" << std::endl;

    // 测试全排列
    {
        std::vector<int> nums = {1, 2, 3};
        auto result = permuteSolution(nums);
        assert(result.size() == 6);  // 3! = 6
        std::cout << "  [PASS] Permute" << std::endl;
    }

    // 测试含重复元素的全排列
    {
        std::vector<int> nums = {1, 1, 2};
        auto result = permuteUniqueSolution(nums);
        assert(result.size() == 3);  // {1,1,2}, {1,2,1}, {2,1,1}
        std::cout << "  [PASS] Permute Unique" << std::endl;
    }

    // 测试下一个排列
    {
        std::vector<int> nums = {1, 2, 3};
        nextPermutationSolution(nums);
        assert((nums == std::vector<int>{1, 3, 2}));

        nums = {3, 2, 1};
        nextPermutationSolution(nums);
        assert((nums == std::vector<int>{1, 2, 3}));
        std::cout << "  [PASS] Next Permutation" << std::endl;
    }

    std::cout << "=== All Permutation Tests Passed! ===" << std::endl;
}

} // namespace Permutation

/**
 * 关键要点：
 *
 * 1. 全排列基本思路：
 *    - 交换法：每个位置与后面位置交换
 *    - 回溯撤销交换
 *
 * 2. 处理重复元素：
 *    - 先排序
 *    - 跳过相同元素（剪枝条件）
 *    - used 数组记录使用状态
 *
 * 3. 下一个排列：
 *    - 从右向左找第一个递减位置 i
 *    - 从右向左找第一个大于 nums[i] 的位置 j
 *    - 交换 i 和 j，然后反转 i+1 到末尾
 */
