/**
 * @file combination_solution.cpp
 * @brief 组合问题 - 解答
 */
#include <vector>
#include <algorithm>
#include <iostream>

using namespace std;

// ==================== 题目1: 组合 ====================
class Solution77 {
public:
    vector<vector<int>> combine(int n, int k) {
        vector<vector<int>> result;
        vector<int> path;
        backtrack(result, path, n, k, 1);
        return result;
    }

private:
    void backtrack(vector<vector<int>>& result, vector<int>& path,
                   int n, int k, int start) {
        if (path.size() == k) {
            result.push_back(path);
            return;
        }

        // 剪枝：剩余元素不够选
        for (int i = start; i <= n - (k - path.size()) + 1; ++i) {
            path.push_back(i);
            backtrack(result, path, n, k, i + 1);
            path.pop_back();
        }
    }
};

// ==================== 题目2: 组合总和 ====================
class Solution39 {
public:
    vector<vector<int>> combinationSum(vector<int>& candidates, int target) {
        vector<vector<int>> result;
        vector<int> path;
        sort(candidates.begin(), candidates.end());  // 排序用于剪枝
        backtrack(result, path, candidates, target, 0);
        return result;
    }

private:
    void backtrack(vector<vector<int>>& result, vector<int>& path,
                   vector<int>& candidates, int remain, int start) {
        if (remain == 0) {
            result.push_back(path);
            return;
        }

        for (int i = start; i < candidates.size(); ++i) {
            if (candidates[i] > remain) break;  // 剪枝

            path.push_back(candidates[i]);
            backtrack(result, path, candidates, remain - candidates[i], i);  // 可重复使用
            path.pop_back();
        }
    }
};

// ==================== 题目3: 组合总和 II ====================
class Solution40 {
public:
    vector<vector<int>> combinationSum2(vector<int>& candidates, int target) {
        vector<vector<int>> result;
        vector<int> path;
        sort(candidates.begin(), candidates.end());
        backtrack(result, path, candidates, target, 0);
        return result;
    }

private:
    void backtrack(vector<vector<int>>& result, vector<int>& path,
                   vector<int>& candidates, int remain, int start) {
        if (remain == 0) {
            result.push_back(path);
            return;
        }

        for (int i = start; i < candidates.size(); ++i) {
            if (candidates[i] > remain) break;

            // 跳过同层重复元素
            if (i > start && candidates[i] == candidates[i - 1]) continue;

            path.push_back(candidates[i]);
            backtrack(result, path, candidates, remain - candidates[i], i + 1);
            path.pop_back();
        }
    }
};

// ==================== 题目4: 组合总和 III ====================
class Solution216 {
public:
    vector<vector<int>> combinationSum3(int k, int n) {
        vector<vector<int>> result;
        vector<int> path;
        backtrack(result, path, k, n, 1);
        return result;
    }

private:
    void backtrack(vector<vector<int>>& result, vector<int>& path,
                   int k, int remain, int start) {
        if (path.size() == k) {
            if (remain == 0) {
                result.push_back(path);
            }
            return;
        }

        for (int i = start; i <= 9; ++i) {
            if (i > remain) break;  // 剪枝

            path.push_back(i);
            backtrack(result, path, k, remain - i, i + 1);
            path.pop_back();
        }
    }
};

// ==================== 题目5: 子集 ====================
class Solution78 {
public:
    // 方法1: 回溯
    vector<vector<int>> subsets(vector<int>& nums) {
        vector<vector<int>> result;
        vector<int> path;
        backtrack(result, path, nums, 0);
        return result;
    }

    // 方法2: 迭代（位运算）
    vector<vector<int>> subsetsIterative(vector<int>& nums) {
        int n = nums.size();
        vector<vector<int>> result;

        for (int mask = 0; mask < (1 << n); ++mask) {
            vector<int> subset;
            for (int i = 0; i < n; ++i) {
                if (mask & (1 << i)) {
                    subset.push_back(nums[i]);
                }
            }
            result.push_back(subset);
        }
        return result;
    }

private:
    void backtrack(vector<vector<int>>& result, vector<int>& path,
                   vector<int>& nums, int start) {
        result.push_back(path);  // 每个节点都是一个子集

        for (int i = start; i < nums.size(); ++i) {
            path.push_back(nums[i]);
            backtrack(result, path, nums, i + 1);
            path.pop_back();
        }
    }
};

// ==================== 题目6: 子集 II ====================
class Solution90 {
public:
    vector<vector<int>> subsetsWithDup(vector<int>& nums) {
        vector<vector<int>> result;
        vector<int> path;
        sort(nums.begin(), nums.end());
        backtrack(result, path, nums, 0);
        return result;
    }

private:
    void backtrack(vector<vector<int>>& result, vector<int>& path,
                   vector<int>& nums, int start) {
        result.push_back(path);

        for (int i = start; i < nums.size(); ++i) {
            // 跳过同层重复
            if (i > start && nums[i] == nums[i - 1]) continue;

            path.push_back(nums[i]);
            backtrack(result, path, nums, i + 1);
            path.pop_back();
        }
    }
};

// ==================== 测试代码 ====================
void printResult(const vector<vector<int>>& result) {
    cout << "[";
    for (int i = 0; i < result.size(); ++i) {
        cout << "[";
        for (int j = 0; j < result[i].size(); ++j) {
            cout << result[i][j];
            if (j < result[i].size() - 1) cout << ",";
        }
        cout << "]";
        if (i < result.size() - 1) cout << ",";
    }
    cout << "]\n";
}

int main() {
    // 组合
    cout << "Combine(4, 2): ";
    printResult(Solution77().combine(4, 2));

    // 组合总和
    vector<int> candidates1 = {2, 3, 6, 7};
    cout << "CombinationSum([2,3,6,7], 7): ";
    printResult(Solution39().combinationSum(candidates1, 7));

    // 组合总和 II
    vector<int> candidates2 = {10, 1, 2, 7, 6, 1, 5};
    cout << "CombinationSum2([10,1,2,7,6,1,5], 8): ";
    printResult(Solution40().combinationSum2(candidates2, 8));

    // 组合总和 III
    cout << "CombinationSum3(3, 9): ";
    printResult(Solution216().combinationSum3(3, 9));

    // 子集
    vector<int> nums1 = {1, 2, 3};
    cout << "Subsets([1,2,3]): ";
    printResult(Solution78().subsets(nums1));

    // 子集 II
    vector<int> nums2 = {1, 2, 2};
    cout << "SubsetsWithDup([1,2,2]): ";
    printResult(Solution90().subsetsWithDup(nums2));

    return 0;
}

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
 *
 * 6. 位运算枚举子集：
 *    - 用 0 到 2^n-1 表示所有子集
 *    - 第 i 位为 1 表示选择第 i 个元素
 */
