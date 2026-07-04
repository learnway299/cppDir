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

/**
 * combine - 从n个数中选k个的所有组合
 * 时间复杂度: O(k * C(n,k)) - C(n,k)个结果，每个长度k
 * 空间复杂度: O(k) - 递归栈深度
 * 特点: start参数避免重复，剪枝优化
 */
std::vector<std::vector<int>> combineSolution(int n, int k) {
    std::vector<std::vector<int>> result;
    std::vector<int> path;
    backtrackCombine(result, path, n, k, 1);
    return result;
}

/**
 * combinationSum - 组合总和I（可重复使用）
 * 时间复杂度: O(n^(target/min)) - 最深递归深度
 * 空间复杂度: O(target/min) - 递归栈
 * 特点: 同一元素可重复使用，排序后剪枝
 */
std::vector<std::vector<int>> combinationSumSolution(std::vector<int>& candidates, int target) {
    std::vector<std::vector<int>> result;
    std::vector<int> path;
    std::sort(candidates.begin(), candidates.end());
    backtrackSum(result, path, candidates, target, 0);
    return result;
}

/**
 * combinationSum2 - 组合总和II（每个数只能用一次，有重复）
 * 时间复杂度: O(2^n) - 子集数量
 * 空间复杂度: O(n) - 递归栈
 * 特点: 排序 + 同层去重
 */
std::vector<std::vector<int>> combinationSum2Solution(std::vector<int>& candidates, int target) {
    std::vector<std::vector<int>> result;
    std::vector<int> path;
    std::sort(candidates.begin(), candidates.end());
    backtrackSum2(result, path, candidates, target, 0);
    return result;
}

/**
 * combinationSum3 - 组合总和III（1-9中选k个和为n）
 * 时间复杂度: O(C(9,k)) - 从9个数中选k个
 * 空间复杂度: O(k) - 递归栈
 * 特点: 固定范围1-9，剪枝优化
 */
std::vector<std::vector<int>> combinationSum3Solution(int k, int n) {
    std::vector<std::vector<int>> result;
    std::vector<int> path;
    backtrackSum3(result, path, k, n, 1);
    return result;
}

// ============================================================
// 子集 Subsets - 多种实现方式
// ============================================================

// 方法1: 递归（回溯）
// 时间复杂度: O(n * 2^n), 空间复杂度: O(n)
std::vector<std::vector<int>> subsetsRecursive(std::vector<int>& nums) {
    std::vector<std::vector<int>> result;
    std::vector<int> path;
    backtrackSubsets(result, path, nums, 0);
    return result;
}

// 方法2: 迭代（位运算枚举）
// 原理: n 个元素有 2^n 个子集，用 0 ~ 2^n-1 的二进制表示每个子集
// 二进制第 i 位为 1 表示选择 nums[i]
// 时间复杂度: O(n * 2^n), 空间复杂度: O(1)（不计结果空间）
std::vector<std::vector<int>> subsetsIterative(std::vector<int>& nums) {
    std::vector<std::vector<int>> result;
    int n = static_cast<int>(nums.size());
    int total = 1 << n;  // 2^n

    for (int mask = 0; mask < total; ++mask) {
        std::vector<int> subset;
        for (int i = 0; i < n; ++i) {
            if (mask & (1 << i)) {  // 第 i 位为 1
                subset.push_back(nums[i]);
            }
        }
        result.push_back(subset);
    }
    return result;
}

// 方法3: 迭代（逐个元素扩展）
// 原理: 从空集开始，每遇到一个新元素，在现有所有子集基础上加入该元素
// [], 遇到1 -> [], [1]
// [], [1], 遇到2 -> [], [1], [2], [1,2]
// [], [1], [2], [1,2], 遇到3 -> [], [1], [2], [1,2], [3], [1,3], [2,3], [1,2,3]
std::vector<std::vector<int>> subsetsIterativeExpand(std::vector<int>& nums) {
    std::vector<std::vector<int>> result;
    result.push_back({});  // 空集

    for (int num : nums) {
        int size = static_cast<int>(result.size());
        for (int i = 0; i < size; ++i) {
            std::vector<int> newSubset = result[i];
            newSubset.push_back(num);
            result.push_back(newSubset);
        }
    }
    return result;
}

std::vector<std::vector<int>> subsetsSolution(std::vector<int>& nums) {
    return subsetsRecursive(nums);  // 默认使用递归
}

// ============================================================
// 子集 II SubsetsWithDup - 多种实现方式（含重复元素）
// ============================================================

// 方法1: 递归（回溯 + 去重）
std::vector<std::vector<int>> subsetsWithDupRecursive(std::vector<int>& nums) {
    std::vector<std::vector<int>> result;
    std::vector<int> path;
    std::sort(nums.begin(), nums.end());
    backtrackSubsetsDup(result, path, nums, 0);
    return result;
}

// 方法2: 迭代（逐个元素扩展 + 去重）
// 原理: 遇到重复元素时，只在上一轮新增的子集上扩展，避免重复
// 例如 [1,2,2]:
// [] -> [], [1]
// [], [1] -> [], [1], [2], [1,2]  (在全部基础上扩展)
// [], [1], [2], [1,2] -> [], [1], [2], [1,2], [2,2], [1,2,2]  (只在上轮新增的 [2],[1,2] 上扩展)
std::vector<std::vector<int>> subsetsWithDupIterative(std::vector<int>& nums) {
    std::sort(nums.begin(), nums.end());
    std::vector<std::vector<int>> result;
    result.push_back({});

    int start = 0;  // 上一轮新增子集的起始位置
    for (size_t j = 0; j < nums.size(); ++j) {
        int size = static_cast<int>(result.size());
        // 如果当前元素与上一个相同，只在上轮新增的子集上扩展
        int begin = (j > 0 && nums[j] == nums[j - 1]) ? start : 0;

        for (int i = begin; i < size; ++i) {
            std::vector<int> newSubset = result[i];
            newSubset.push_back(nums[j]);
            result.push_back(newSubset);
        }
        start = size;  // 记录本轮新增子集的起始位置
    }
    return result;
}

std::vector<std::vector<int>> subsetsWithDupSolution(std::vector<int>& nums) {
    return subsetsWithDupRecursive(nums);  // 默认使用递归
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
