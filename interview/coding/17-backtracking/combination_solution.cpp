/**
 * @file combination_solution.cpp
 * @brief 组合问题 - 参考答案
 */
#include "combination.h"
#include <algorithm>
#include <iostream>
#include <cassert>

namespace Combination {

// ==================== 辅助函数 ====================
static void backtrackCombine(std::vector<std::vector<int>>& result,
                              std::vector<int>& path, int n, int k, int start) {
    if (static_cast<int>(path.size()) == k) {
        result.push_back(path);
        return;
    }
    // 剪枝：剩余元素不够选
    for (int i = start; i <= n - (k - static_cast<int>(path.size())) + 1; ++i) {
        path.push_back(i);
        backtrackCombine(result, path, n, k, i + 1);
        path.pop_back();
    }
}

static void backtrackSum(std::vector<std::vector<int>>& result, std::vector<int>& path,
                          std::vector<int>& candidates, int remain, int start) {
    if (remain == 0) {
        result.push_back(path);
        return;
    }
    for (size_t i = start; i < candidates.size(); ++i) {
        if (candidates[i] > remain) break;  // 剪枝
        path.push_back(candidates[i]);
        backtrackSum(result, path, candidates, remain - candidates[i], static_cast<int>(i));
        path.pop_back();
    }
}

static void backtrackSum2(std::vector<std::vector<int>>& result, std::vector<int>& path,
                           std::vector<int>& candidates, int remain, int start) {
    if (remain == 0) {
        result.push_back(path);
        return;
    }
    for (size_t i = start; i < candidates.size(); ++i) {
        if (candidates[i] > remain) break;
        // 跳过同层重复元素
        if (i > static_cast<size_t>(start) && candidates[i] == candidates[i - 1]) continue;
        path.push_back(candidates[i]);
        backtrackSum2(result, path, candidates, remain - candidates[i], static_cast<int>(i) + 1);
        path.pop_back();
    }
}

static void backtrackSum3(std::vector<std::vector<int>>& result, std::vector<int>& path,
                           int k, int remain, int start) {
    if (static_cast<int>(path.size()) == k) {
        if (remain == 0) {
            result.push_back(path);
        }
        return;
    }
    for (int i = start; i <= 9; ++i) {
        if (i > remain) break;  // 剪枝
        path.push_back(i);
        backtrackSum3(result, path, k, remain - i, i + 1);
        path.pop_back();
    }
}

static void backtrackSubsets(std::vector<std::vector<int>>& result, std::vector<int>& path,
                              std::vector<int>& nums, int start) {
    result.push_back(path);  // 每个节点都是一个子集
    for (size_t i = start; i < nums.size(); ++i) {
        path.push_back(nums[i]);
        backtrackSubsets(result, path, nums, static_cast<int>(i) + 1);
        path.pop_back();
    }
}

static void backtrackSubsetsDup(std::vector<std::vector<int>>& result, std::vector<int>& path,
                                 std::vector<int>& nums, int start) {
    result.push_back(path);
    for (size_t i = start; i < nums.size(); ++i) {
        // 跳过同层重复
        if (i > static_cast<size_t>(start) && nums[i] == nums[i - 1]) continue;
        path.push_back(nums[i]);
        backtrackSubsetsDup(result, path, nums, static_cast<int>(i) + 1);
        path.pop_back();
    }
}

// ==================== 参考答案实现 ====================

std::vector<std::vector<int>> combineSolution(int n, int k) {
    std::vector<std::vector<int>> result;
    std::vector<int> path;
    backtrackCombine(result, path, n, k, 1);
    return result;
}

std::vector<std::vector<int>> combinationSumSolution(std::vector<int>& candidates, int target) {
    std::vector<std::vector<int>> result;
    std::vector<int> path;
    std::sort(candidates.begin(), candidates.end());
    backtrackSum(result, path, candidates, target, 0);
    return result;
}

std::vector<std::vector<int>> combinationSum2Solution(std::vector<int>& candidates, int target) {
    std::vector<std::vector<int>> result;
    std::vector<int> path;
    std::sort(candidates.begin(), candidates.end());
    backtrackSum2(result, path, candidates, target, 0);
    return result;
}

std::vector<std::vector<int>> combinationSum3Solution(int k, int n) {
    std::vector<std::vector<int>> result;
    std::vector<int> path;
    backtrackSum3(result, path, k, n, 1);
    return result;
}

std::vector<std::vector<int>> subsetsSolution(std::vector<int>& nums) {
    std::vector<std::vector<int>> result;
    std::vector<int> path;
    backtrackSubsets(result, path, nums, 0);
    return result;
}

std::vector<std::vector<int>> subsetsWithDupSolution(std::vector<int>& nums) {
    std::vector<std::vector<int>> result;
    std::vector<int> path;
    std::sort(nums.begin(), nums.end());
    backtrackSubsetsDup(result, path, nums, 0);
    return result;
}

// ==================== 测试函数 ====================

void runTests() {
    std::cout << "\n=== Combination Tests ===" << std::endl;

    // 测试组合
    {
        auto result = combineSolution(4, 2);
        assert(result.size() == 6);  // C(4,2) = 6
        std::cout << "  [PASS] Combine (n=4, k=2)" << std::endl;
    }

    // 测试组合总和
    {
        std::vector<int> candidates = {2, 3, 6, 7};
        auto result = combinationSumSolution(candidates, 7);
        assert(result.size() == 2);  // [7], [2,2,3]
        std::cout << "  [PASS] Combination Sum" << std::endl;
    }

    // 测试组合总和 II
    {
        std::vector<int> candidates = {10, 1, 2, 7, 6, 1, 5};
        auto result = combinationSum2Solution(candidates, 8);
        assert(result.size() == 4);  // [1,1,6], [1,2,5], [1,7], [2,6]
        std::cout << "  [PASS] Combination Sum II" << std::endl;
    }

    // 测试组合总和 III
    {
        auto result = combinationSum3Solution(3, 9);
        assert(result.size() == 3);  // [1,2,6], [1,3,5], [2,3,4]
        std::cout << "  [PASS] Combination Sum III" << std::endl;
    }

    // 测试子集
    {
        std::vector<int> nums = {1, 2, 3};
        auto result = subsetsSolution(nums);
        assert(result.size() == 8);  // 2^3 = 8
        std::cout << "  [PASS] Subsets" << std::endl;
    }

    // 测试子集 II
    {
        std::vector<int> nums = {1, 2, 2};
        auto result = subsetsWithDupSolution(nums);
        assert(result.size() == 6);  // [], [1], [1,2], [1,2,2], [2], [2,2]
        std::cout << "  [PASS] Subsets With Dup" << std::endl;
    }

    std::cout << "=== All Combination Tests Passed! ===" << std::endl;
}

} // namespace Combination

/**
 * 关键要点：
 *
 * 1. 组合 vs 排列：
 *    - 组合：不考虑顺序，[1,2] == [2,1]
 *    - 排列：考虑顺序，[1,2] != [2,1]
 *    - 组合通过 start 参数避免重复
 *
 * 2. 回溯模板：
 *    - 结束条件：检查是否满足要求
 *    - 选择：将当前元素加入路径
 *    - 递归：处理下一个位置
 *    - 撤销：回溯到上一状态
 *
 * 3. 去重技巧：
 *    - 先排序
 *    - 跳过同层重复元素
 *    - 条件：i > start && nums[i] == nums[i-1]
 *
 * 4. 剪枝：
 *    - 排序后，当前元素 > remain 时直接 break
 *    - 剩余元素不够时提前结束
 *
 * 5. 可重复 vs 不可重复：
 *    - 可重复：递归时传 i
 *    - 不可重复：递归时传 i + 1
 */
