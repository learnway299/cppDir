/**
 * @file linear_dp_solution.cpp
 * @brief 线性动态规划问题 - 解答文件
 *
 * 包含 linear_dp.cpp 中所有题目的解答和详细解析。
 */

#include <iostream>
#include <vector>
#include <algorithm>
#include <climits>
#include <string>

/**
 * 题目1: 爬楼梯
 *
 * 解法1: 基础 DP
 * 时间复杂度: O(n)
 * 空间复杂度: O(n)
 */
int climbStairsDP(int n) {
    if (n <= 2) return n;

    std::vector<int> dp(n + 1);
    dp[1] = 1;
    dp[2] = 2;

    for (int i = 3; i <= n; ++i) {
        dp[i] = dp[i - 1] + dp[i - 2];
    }

    return dp[n];
}

/**
 * 解法2: 空间优化
 * 时间复杂度: O(n)
 * 空间复杂度: O(1)
 */
int climbStairs(int n) {
    if (n <= 2) return n;

    int prev2 = 1, prev1 = 2;
    for (int i = 3; i <= n; ++i) {
        int curr = prev1 + prev2;
        prev2 = prev1;
        prev1 = curr;
    }

    return prev1;
}

/**
 * 题目2: 最大子数组和
 *
 * 解法1: Kadane 算法（DP 思想）
 * 状态定义: dp[i] = 以 nums[i] 结尾的最大子数组和
 * 状态转移: dp[i] = max(dp[i-1] + nums[i], nums[i])
 *
 * 时间复杂度: O(n)
 * 空间复杂度: O(1)
 */
int maxSubArray(std::vector<int>& nums) {
    int maxSum = nums[0];
    int currentSum = nums[0];

    for (size_t i = 1; i < nums.size(); ++i) {
        // 要么从当前元素重新开始，要么接上之前的和
        currentSum = std::max(nums[i], currentSum + nums[i]);
        maxSum = std::max(maxSum, currentSum);
    }

    return maxSum;
}

/**
 * 解法2: 分治法（O(n log n)，面试时作为备选方案）
 */
int maxCrossingSum(std::vector<int>& nums, int left, int mid, int right) {
    int leftSum = INT_MIN;
    int sum = 0;
    for (int i = mid; i >= left; --i) {
        sum += nums[i];
        leftSum = std::max(leftSum, sum);
    }

    int rightSum = INT_MIN;
    sum = 0;
    for (int i = mid + 1; i <= right; ++i) {
        sum += nums[i];
        rightSum = std::max(rightSum, sum);
    }

    return leftSum + rightSum;
}

int maxSubArrayDivide(std::vector<int>& nums, int left, int right) {
    if (left == right) return nums[left];

    int mid = left + (right - left) / 2;
    int leftMax = maxSubArrayDivide(nums, left, mid);
    int rightMax = maxSubArrayDivide(nums, mid + 1, right);
    int crossMax = maxCrossingSum(nums, left, mid, right);

    return std::max({leftMax, rightMax, crossMax});
}

/**
 * 题目3: 打家劫舍
 *
 * 解法1: 基础 DP
 * 状态定义: dp[i] = 前 i 个房屋能偷到的最大金额
 * 状态转移: dp[i] = max(dp[i-1], dp[i-2] + nums[i])
 *
 * 时间复杂度: O(n)
 * 空间复杂度: O(n)
 */
int robDP(std::vector<int>& nums) {
    int n = nums.size();
    if (n == 0) return 0;
    if (n == 1) return nums[0];

    std::vector<int> dp(n);
    dp[0] = nums[0];
    dp[1] = std::max(nums[0], nums[1]);

    for (int i = 2; i < n; ++i) {
        dp[i] = std::max(dp[i - 1], dp[i - 2] + nums[i]);
    }

    return dp[n - 1];
}

/**
 * 解法2: 空间优化
 */
int rob(std::vector<int>& nums) {
    int n = nums.size();
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

/**
 * 题目4: 打家劫舍 II（环形）
 *
 * 解法: 分两种情况
 *   1. 偷第一个，不偷最后一个: rob(nums[0..n-2])
 *   2. 不偷第一个，偷最后一个: rob(nums[1..n-1])
 *   取两者最大值
 *
 * 时间复杂度: O(n)
 * 空间复杂度: O(1)
 */
int robRange(std::vector<int>& nums, int start, int end) {
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

int robII(std::vector<int>& nums) {
    int n = nums.size();
    if (n == 0) return 0;
    if (n == 1) return nums[0];
    if (n == 2) return std::max(nums[0], nums[1]);

    // 情况1: 偷 [0, n-2]
    // 情况2: 偷 [1, n-1]
    return std::max(robRange(nums, 0, n - 2), robRange(nums, 1, n - 1));
}

/**
 * 题目5: 最小路径和
 *
 * 解法1: 二维 DP
 * 时间复杂度: O(m*n)
 * 空间复杂度: O(m*n)
 */
int minPathSumDP(std::vector<std::vector<int>>& grid) {
    int m = grid.size(), n = grid[0].size();
    std::vector<std::vector<int>> dp(m, std::vector<int>(n));

    dp[0][0] = grid[0][0];

    // 初始化第一列
    for (int i = 1; i < m; ++i) {
        dp[i][0] = dp[i - 1][0] + grid[i][0];
    }

    // 初始化第一行
    for (int j = 1; j < n; ++j) {
        dp[0][j] = dp[0][j - 1] + grid[0][j];
    }

    // 填充其他位置
    for (int i = 1; i < m; ++i) {
        for (int j = 1; j < n; ++j) {
            dp[i][j] = grid[i][j] + std::min(dp[i - 1][j], dp[i][j - 1]);
        }
    }

    return dp[m - 1][n - 1];
}

/**
 * 解法2: 空间优化（一维数组）
 */
int minPathSum(std::vector<std::vector<int>>& grid) {
    int m = grid.size(), n = grid[0].size();
    std::vector<int> dp(n);

    dp[0] = grid[0][0];
    for (int j = 1; j < n; ++j) {
        dp[j] = dp[j - 1] + grid[0][j];
    }

    for (int i = 1; i < m; ++i) {
        dp[0] += grid[i][0];
        for (int j = 1; j < n; ++j) {
            dp[j] = grid[i][j] + std::min(dp[j], dp[j - 1]);
        }
    }

    return dp[n - 1];
}

/**
 * 题目6: 不同路径
 *
 * 解法1: DP
 * 时间复杂度: O(m*n)
 * 空间复杂度: O(n)
 */
int uniquePaths(int m, int n) {
    std::vector<int> dp(n, 1);

    for (int i = 1; i < m; ++i) {
        for (int j = 1; j < n; ++j) {
            dp[j] += dp[j - 1];
        }
    }

    return dp[n - 1];
}

/**
 * 解法2: 组合数学
 * 总共需要走 m-1 步向下 + n-1 步向右
 * 答案 = C(m+n-2, m-1) = (m+n-2)! / ((m-1)! * (n-1)!)
 */
int uniquePathsCombination(int m, int n) {
    long long result = 1;
    for (int i = 0; i < m - 1; ++i) {
        result = result * (n - 1 + m - 1 - i) / (i + 1);
    }
    return static_cast<int>(result);
}

/**
 * 题目7: 不同路径 II（有障碍物）
 *
 * 解法: DP，遇到障碍物设为 0
 * 时间复杂度: O(m*n)
 * 空间复杂度: O(n)
 */
int uniquePathsWithObstacles(std::vector<std::vector<int>>& obstacleGrid) {
    int m = obstacleGrid.size(), n = obstacleGrid[0].size();
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

/**
 * 题目8: 解码方法
 *
 * 解法: DP
 * 状态定义: dp[i] = 前 i 个字符的解码方法数
 * 状态转移:
 *   - 如果 s[i-1] != '0'，可以单独解码: dp[i] += dp[i-1]
 *   - 如果 s[i-2..i-1] 在 [10,26] 范围内，可以两位一起解码: dp[i] += dp[i-2]
 *
 * 时间复杂度: O(n)
 * 空间复杂度: O(1)
 */
int numDecodings(std::string s) {
    if (s.empty() || s[0] == '0') return 0;

    int n = s.size();
    int prev2 = 1;  // dp[i-2]
    int prev1 = 1;  // dp[i-1]

    for (int i = 1; i < n; ++i) {
        int curr = 0;

        // 单个字符解码
        if (s[i] != '0') {
            curr += prev1;
        }

        // 两个字符一起解码
        int twoDigit = (s[i - 1] - '0') * 10 + (s[i] - '0');
        if (twoDigit >= 10 && twoDigit <= 26) {
            curr += prev2;
        }

        prev2 = prev1;
        prev1 = curr;
    }

    return prev1;
}

/**
 * 题目9: 最长递增子序列
 *
 * 解法1: 基础 DP
 * 状态定义: dp[i] = 以 nums[i] 结尾的 LIS 长度
 * 状态转移: dp[i] = max(dp[j] + 1) for all j < i where nums[j] < nums[i]
 *
 * 时间复杂度: O(n^2)
 * 空间复杂度: O(n)
 */
int lengthOfLISDP(std::vector<int>& nums) {
    int n = nums.size();
    if (n == 0) return 0;

    std::vector<int> dp(n, 1);
    int maxLen = 1;

    for (int i = 1; i < n; ++i) {
        for (int j = 0; j < i; ++j) {
            if (nums[j] < nums[i]) {
                dp[i] = std::max(dp[i], dp[j] + 1);
            }
        }
        maxLen = std::max(maxLen, dp[i]);
    }

    return maxLen;
}

/**
 * 解法2: 二分查找优化
 * 维护一个数组 tails，tails[i] 表示长度为 i+1 的 LIS 的最小结尾元素
 *
 * 时间复杂度: O(n log n)
 * 空间复杂度: O(n)
 */
int lengthOfLIS(std::vector<int>& nums) {
    std::vector<int> tails;

    for (int num : nums) {
        // 找到第一个 >= num 的位置
        auto it = std::lower_bound(tails.begin(), tails.end(), num);

        if (it == tails.end()) {
            tails.push_back(num);
        } else {
            *it = num;
        }
    }

    return tails.size();
}

/**
 * 题目10: 乘积最大子数组
 *
 * 解法: 同时维护最大和最小乘积
 * 原因: 负数乘以最小负数可能变成最大正数
 *
 * 时间复杂度: O(n)
 * 空间复杂度: O(1)
 */
int maxProduct(std::vector<int>& nums) {
    if (nums.empty()) return 0;

    int maxProd = nums[0];
    int minProd = nums[0];
    int result = nums[0];

    for (size_t i = 1; i < nums.size(); ++i) {
        // 遇到负数时，最大最小互换
        if (nums[i] < 0) {
            std::swap(maxProd, minProd);
        }

        // 更新最大和最小乘积
        maxProd = std::max(nums[i], maxProd * nums[i]);
        minProd = std::min(nums[i], minProd * nums[i]);

        result = std::max(result, maxProd);
    }

    return result;
}


// ==================== 测试代码 ====================
#include <cassert>

void testClimbStairs() {
    assert(climbStairs(2) == 2);
    assert(climbStairs(3) == 3);
    assert(climbStairs(4) == 5);
    assert(climbStairsDP(5) == 8);
    std::cout << "climbStairs: PASSED\n";
}

void testMaxSubArray() {
    std::vector<int> nums = {-2, 1, -3, 4, -1, 2, 1, -5, 4};
    assert(maxSubArray(nums) == 6);

    nums = {1};
    assert(maxSubArray(nums) == 1);

    nums = {5, 4, -1, 7, 8};
    assert(maxSubArray(nums) == 23);

    // 测试分治解法
    nums = {-2, 1, -3, 4, -1, 2, 1, -5, 4};
    assert(maxSubArrayDivide(nums, 0, nums.size() - 1) == 6);

    std::cout << "maxSubArray: PASSED\n";
}

void testRob() {
    std::vector<int> nums = {1, 2, 3, 1};
    assert(rob(nums) == 4);
    assert(robDP(nums) == 4);

    nums = {2, 7, 9, 3, 1};
    assert(rob(nums) == 12);

    std::cout << "rob: PASSED\n";
}

void testRobII() {
    std::vector<int> nums = {2, 3, 2};
    assert(robII(nums) == 3);

    nums = {1, 2, 3, 1};
    assert(robII(nums) == 4);

    nums = {1, 2, 3};
    assert(robII(nums) == 3);

    std::cout << "robII: PASSED\n";
}

void testMinPathSum() {
    std::vector<std::vector<int>> grid = {{1, 3, 1}, {1, 5, 1}, {4, 2, 1}};
    assert(minPathSum(grid) == 7);
    assert(minPathSumDP(grid) == 7);

    grid = {{1, 2, 3}, {4, 5, 6}};
    assert(minPathSum(grid) == 12);

    std::cout << "minPathSum: PASSED\n";
}

void testUniquePaths() {
    assert(uniquePaths(3, 7) == 28);
    assert(uniquePaths(3, 2) == 3);
    assert(uniquePaths(1, 1) == 1);
    assert(uniquePathsCombination(3, 7) == 28);

    std::cout << "uniquePaths: PASSED\n";
}

void testUniquePathsWithObstacles() {
    std::vector<std::vector<int>> grid = {{0, 0, 0}, {0, 1, 0}, {0, 0, 0}};
    assert(uniquePathsWithObstacles(grid) == 2);

    grid = {{0, 1}, {0, 0}};
    assert(uniquePathsWithObstacles(grid) == 1);

    grid = {{1, 0}};
    assert(uniquePathsWithObstacles(grid) == 0);

    std::cout << "uniquePathsWithObstacles: PASSED\n";
}

void testNumDecodings() {
    assert(numDecodings("12") == 2);
    assert(numDecodings("226") == 3);
    assert(numDecodings("06") == 0);
    assert(numDecodings("10") == 1);
    assert(numDecodings("27") == 1);

    std::cout << "numDecodings: PASSED\n";
}

void testLengthOfLIS() {
    std::vector<int> nums = {10, 9, 2, 5, 3, 7, 101, 18};
    assert(lengthOfLIS(nums) == 4);
    assert(lengthOfLISDP(nums) == 4);

    nums = {0, 1, 0, 3, 2, 3};
    assert(lengthOfLIS(nums) == 4);

    nums = {7, 7, 7, 7, 7, 7, 7};
    assert(lengthOfLIS(nums) == 1);

    std::cout << "lengthOfLIS: PASSED\n";
}

void testMaxProduct() {
    std::vector<int> nums = {2, 3, -2, 4};
    assert(maxProduct(nums) == 6);

    nums = {-2, 0, -1};
    assert(maxProduct(nums) == 0);

    nums = {-2, 3, -4};
    assert(maxProduct(nums) == 24);

    nums = {-2};
    assert(maxProduct(nums) == -2);

    std::cout << "maxProduct: PASSED\n";
}

int main() {
    std::cout << "=== Linear DP Solution Tests ===\n";

    testClimbStairs();
    testMaxSubArray();
    testRob();
    testRobII();
    testMinPathSum();
    testUniquePaths();
    testUniquePathsWithObstacles();
    testNumDecodings();
    testLengthOfLIS();
    testMaxProduct();

    std::cout << "\nAll tests passed!\n";
    return 0;
}
