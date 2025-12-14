/**
 * @file linear_dp_solution.cpp
 * @brief 线性动态规划问题 - 参考答案
 */

#include "linear_dp.h"
#include <iostream>
#include <cassert>
#include <algorithm>

namespace LinearDP {

// ==================== 辅助函数 ====================

static int robRangeSolution(std::vector<int>& nums, int start, int end) {
    if (start == end) return nums[start];
    int prev2 = nums[start];
    int prev1 = std::max(nums[start], nums[start + 1]);
    for (int i = start + 2; i <= end; ++i) {
        int curr = std::max(prev1, prev2 + nums[i]);
        prev2 = prev1;
        prev1 = curr;
    }
    return prev1;
}

// ==================== 参考答案实现 ====================

// 题目1: 爬楼梯
int climbStairsSolution(int n) {
    if (n <= 2) return n;
    int prev2 = 1, prev1 = 2;
    for (int i = 3; i <= n; ++i) {
        int curr = prev1 + prev2;
        prev2 = prev1;
        prev1 = curr;
    }
    return prev1;
}

// 题目2: 最大子数组和
int maxSubArraySolution(std::vector<int>& nums) {
    int maxSum = nums[0];
    int currentSum = nums[0];
    for (size_t i = 1; i < nums.size(); ++i) {
        currentSum = std::max(nums[i], currentSum + nums[i]);
        maxSum = std::max(maxSum, currentSum);
    }
    return maxSum;
}

// 题目3: 打家劫舍
int robSolution(std::vector<int>& nums) {
    int n = static_cast<int>(nums.size());
    if (n == 0) return 0;
    if (n == 1) return nums[0];
    int prev2 = nums[0];
    int prev1 = std::max(nums[0], nums[1]);
    for (int i = 2; i < n; ++i) {
        int curr = std::max(prev1, prev2 + nums[i]);
        prev2 = prev1;
        prev1 = curr;
    }
    return prev1;
}

// 题目4: 打家劫舍 II（环形）
int robIISolution(std::vector<int>& nums) {
    int n = static_cast<int>(nums.size());
    if (n == 0) return 0;
    if (n == 1) return nums[0];
    if (n == 2) return std::max(nums[0], nums[1]);
    return std::max(robRangeSolution(nums, 0, n - 2), robRangeSolution(nums, 1, n - 1));
}

// 题目5: 最小路径和
int minPathSumSolution(std::vector<std::vector<int>>& grid) {
    int m = static_cast<int>(grid.size()), n = static_cast<int>(grid[0].size());
    std::vector<int> dp(n);
    dp[0] = grid[0][0];
    for (int j = 1; j < n; ++j) dp[j] = dp[j - 1] + grid[0][j];
    for (int i = 1; i < m; ++i) {
        dp[0] += grid[i][0];
        for (int j = 1; j < n; ++j) {
            dp[j] = grid[i][j] + std::min(dp[j], dp[j - 1]);
        }
    }
    return dp[n - 1];
}

// 题目6: 不同路径
int uniquePathsSolution(int m, int n) {
    std::vector<int> dp(n, 1);
    for (int i = 1; i < m; ++i) {
        for (int j = 1; j < n; ++j) {
            dp[j] += dp[j - 1];
        }
    }
    return dp[n - 1];
}

// 题目7: 不同路径 II（有障碍物）
int uniquePathsWithObstaclesSolution(std::vector<std::vector<int>>& obstacleGrid) {
    int m = static_cast<int>(obstacleGrid.size()), n = static_cast<int>(obstacleGrid[0].size());
    if (obstacleGrid[0][0] == 1) return 0;
    std::vector<long long> dp(n, 0);
    dp[0] = 1;
    for (int i = 0; i < m; ++i) {
        for (int j = 0; j < n; ++j) {
            if (obstacleGrid[i][j] == 1) {
                dp[j] = 0;
            } else if (j > 0) {
                dp[j] += dp[j - 1];
            }
        }
    }
    return static_cast<int>(dp[n - 1]);
}

// 题目8: 解码方法
int numDecodingsSolution(const std::string& s) {
    if (s.empty() || s[0] == '0') return 0;
    int n = static_cast<int>(s.size());
    int prev2 = 1, prev1 = 1;
    for (int i = 1; i < n; ++i) {
        int curr = 0;
        if (s[i] != '0') curr += prev1;
        int twoDigit = (s[i - 1] - '0') * 10 + (s[i] - '0');
        if (twoDigit >= 10 && twoDigit <= 26) curr += prev2;
        prev2 = prev1;
        prev1 = curr;
    }
    return prev1;
}

// 题目9: 最长递增子序列（二分查找优化）
int lengthOfLISSolution(std::vector<int>& nums) {
    std::vector<int> tails;
    for (int num : nums) {
        auto it = std::lower_bound(tails.begin(), tails.end(), num);
        if (it == tails.end()) tails.push_back(num);
        else *it = num;
    }
    return static_cast<int>(tails.size());
}

// 题目10: 乘积最大子数组
int maxProductSolution(std::vector<int>& nums) {
    if (nums.empty()) return 0;
    int maxProd = nums[0];
    int minProd = nums[0];
    int result = nums[0];
    for (size_t i = 1; i < nums.size(); ++i) {
        if (nums[i] < 0) std::swap(maxProd, minProd);
        maxProd = std::max(nums[i], maxProd * nums[i]);
        minProd = std::min(nums[i], minProd * nums[i]);
        result = std::max(result, maxProd);
    }
    return result;
}

// ==================== 测试函数 ====================

void testLinearDPSolution() {
    std::cout << "=== Linear DP Tests (Solution) ===\n";

    // climbStairs
    assert(climbStairsSolution(2) == 2);
    assert(climbStairsSolution(3) == 3);
    assert(climbStairsSolution(4) == 5);
    std::cout << "  climbStairsSolution: PASSED\n";

    // maxSubArray
    std::vector<int> nums = {-2, 1, -3, 4, -1, 2, 1, -5, 4};
    assert(maxSubArraySolution(nums) == 6);
    nums = {1};
    assert(maxSubArraySolution(nums) == 1);
    std::cout << "  maxSubArraySolution: PASSED\n";

    // rob
    nums = {1, 2, 3, 1};
    assert(robSolution(nums) == 4);
    nums = {2, 7, 9, 3, 1};
    assert(robSolution(nums) == 12);
    std::cout << "  robSolution: PASSED\n";

    // robII
    nums = {2, 3, 2};
    assert(robIISolution(nums) == 3);
    nums = {1, 2, 3, 1};
    assert(robIISolution(nums) == 4);
    std::cout << "  robIISolution: PASSED\n";

    // minPathSum
    std::vector<std::vector<int>> grid = {{1, 3, 1}, {1, 5, 1}, {4, 2, 1}};
    assert(minPathSumSolution(grid) == 7);
    std::cout << "  minPathSumSolution: PASSED\n";

    // uniquePaths
    assert(uniquePathsSolution(3, 7) == 28);
    assert(uniquePathsSolution(3, 2) == 3);
    std::cout << "  uniquePathsSolution: PASSED\n";

    // uniquePathsWithObstacles
    std::vector<std::vector<int>> obsGrid = {{0, 0, 0}, {0, 1, 0}, {0, 0, 0}};
    assert(uniquePathsWithObstaclesSolution(obsGrid) == 2);
    std::cout << "  uniquePathsWithObstaclesSolution: PASSED\n";

    // numDecodings
    assert(numDecodingsSolution("12") == 2);
    assert(numDecodingsSolution("226") == 3);
    assert(numDecodingsSolution("06") == 0);
    std::cout << "  numDecodingsSolution: PASSED\n";

    // lengthOfLIS
    nums = {10, 9, 2, 5, 3, 7, 101, 18};
    assert(lengthOfLISSolution(nums) == 4);
    std::cout << "  lengthOfLISSolution: PASSED\n";

    // maxProduct
    nums = {2, 3, -2, 4};
    assert(maxProductSolution(nums) == 6);
    nums = {-2, 3, -4};
    assert(maxProductSolution(nums) == 24);
    std::cout << "  maxProductSolution: PASSED\n";
}

} // namespace LinearDP
