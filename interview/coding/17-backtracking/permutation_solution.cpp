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

// ============================================================
// 下一个排列 - 核心算法（其他迭代方法依赖此实现）
// ============================================================

/**
 * nextPermutation - 下一个排列
 * 时间复杂度: O(n) - 最多两次遍历
 * 空间复杂度: O(1) - 原地操作
 * 特点: 找降序点→找交换点→反转后缀
 */
void nextPermutationSolution(std::vector<int>& nums) {
    int n = static_cast<int>(nums.size()), i = n - 2;
    // 1. 从右向左找第一个递减位置 i（nums[i] < nums[i+1]）
    while (i >= 0 && nums[i] >= nums[i + 1]) --i;
    if (i >= 0) {
        // 2. 从右向左找第一个大于 nums[i] 的位置 j
        int j = n - 1;
        while (nums[j] <= nums[i]) --j;
        // 3. 交换 i 和 j
        std::swap(nums[i], nums[j]);
    }
    // 4. 反转 i+1 到末尾（使其变为最小排列）
    std::reverse(nums.begin() + i + 1, nums.end());
}

// ============================================================
// 全排列 Permute - 多种实现方式（无重复元素）
// ============================================================

// 方法1: 递归（交换法）
// 原理: 每个位置依次与后面位置交换，生成所有排列
// 时间复杂度: O(n * n!), 空间复杂度: O(n)
std::vector<std::vector<int>> permuteRecursive(std::vector<int>& nums) {
    std::vector<std::vector<int>> result;
    backtrackPermute(nums, 0, result);
    return result;
}

// 方法2: 迭代（基于 nextPermutation）
// 原理: 从最小排列开始，不断求下一个排列，直到回到最小排列
// 时间复杂度: O(n * n!), 空间复杂度: O(1)（不计结果空间）
std::vector<std::vector<int>> permuteIterative(std::vector<int>& nums) {
    std::vector<std::vector<int>> result;
    std::sort(nums.begin(), nums.end());  // 先排序得到最小排列

    do {
        result.push_back(nums);
        nextPermutationSolution(nums);
    } while (!std::is_sorted(nums.begin(), nums.end()));
    // 当 nums 回到最小排列（有序）时结束

    return result;
}

// 方法3: 迭代（逐个元素插入）
// 原理: 从空排列开始，每次将新元素插入到现有排列的所有可能位置
// [1] -> [2,1], [1,2]
// [2,1], [1,2] -> [3,2,1], [2,3,1], [2,1,3], [3,1,2], [1,3,2], [1,2,3]
std::vector<std::vector<int>> permuteIterativeInsert(std::vector<int>& nums) {
    std::vector<std::vector<int>> result;
    result.push_back({});

    for (int num : nums) {
        std::vector<std::vector<int>> newResult;
        for (auto& perm : result) {
            // 将 num 插入到 perm 的每个位置
            for (size_t i = 0; i <= perm.size(); ++i) {
                std::vector<int> newPerm = perm;
                newPerm.insert(newPerm.begin() + i, num);
                newResult.push_back(newPerm);
            }
        }
        result = std::move(newResult);
    }
    return result;
}

std::vector<std::vector<int>> permuteSolution(std::vector<int>& nums) {
    return permuteRecursive(nums);  // 默认使用递归
}

// ============================================================
// 全排列 II PermuteUnique - 多种实现方式（含重复元素）
// ============================================================

// 方法1: 递归（回溯 + 去重）
// 原理: 排序后，跳过同层重复元素
std::vector<std::vector<int>> permuteUniqueRecursive(std::vector<int>& nums) {
    std::vector<std::vector<int>> result;
    std::sort(nums.begin(), nums.end());
    std::vector<bool> used(nums.size(), false);
    std::vector<int> path;
    backtrackPermuteUnique(nums, used, path, result);
    return result;
}

// 方法2: 迭代（基于 nextPermutation）
// 原理: 从最小排列开始，不断求下一个排列
// 优势: nextPermutation 天然处理重复元素，不会产生重复排列
std::vector<std::vector<int>> permuteUniqueIterative(std::vector<int>& nums) {
    std::vector<std::vector<int>> result;
    std::sort(nums.begin(), nums.end());

    do {
        result.push_back(nums);
        nextPermutationSolution(nums);
    } while (!std::is_sorted(nums.begin(), nums.end()));

    return result;
}

std::vector<std::vector<int>> permuteUniqueSolution(std::vector<int>& nums) {
    return permuteUniqueRecursive(nums);  // 默认使用递归
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
 * 1. 递归 vs 迭代对比：
 *    ┌────────────┬─────────────────┬─────────────────┐
 *    │            │ 递归（回溯）      │ 迭代             │
 *    ├────────────┼─────────────────┼─────────────────┤
 *    │ 思路       │ DFS 遍历决策树   │ 数学规律/模拟     │
 *    │ 空间       │ O(n) 栈空间      │ O(1) 额外空间    │
 *    │ 代码       │ 模板化，易理解   │ 需要理解原理      │
 *    │ 去重       │ 需要额外处理     │ 某些方法天然去重  │
 *    └────────────┴─────────────────┴─────────────────┘
 *
 * 2. 全排列实现方式：
 *    - 递归（交换法）：每个位置与后面位置交换，回溯撤销
 *    - 迭代（nextPermutation）：不断求下一个排列，天然去重
 *    - 迭代（插入法）：逐个元素插入所有可能位置
 *
 * 3. 子集实现方式：
 *    - 递归（回溯）：选/不选 每个元素
 *    - 迭代（位运算）：0~2^n-1 枚举所有状态
 *    - 迭代（扩展法）：遍历元素，在现有子集上扩展
 *
 * 4. 下一个排列算法：
 *    - 从右向左找第一个递减位置 i（nums[i] < nums[i+1]）
 *    - 从右向左找第一个大于 nums[i] 的位置 j
 *    - 交换 i 和 j，然后反转 i+1 到末尾
 *    - 此算法天然跳过重复排列
 *
 * 5. 处理重复元素（递归）：
 *    - 先排序
 *    - 跳过同层相同元素：i > start && nums[i] == nums[i-1]
 *    - used 数组记录使用状态
 */
