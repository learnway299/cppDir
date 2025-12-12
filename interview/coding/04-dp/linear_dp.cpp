/**
 * @file linear_dp.cpp
 * @brief 线性动态规划问题 - 题目文件
 *
 * 线性 DP 是最基础的 DP 类型，状态转移是线性的。
 * 常见模式：dp[i] 表示以第 i 个元素结尾/前 i 个元素的最优解。
 */

#include <iostream>
#include <vector>
#include <algorithm>
#include <climits>

/**
 * 题目1: 爬楼梯
 *
 * 假设你正在爬楼梯。需要 n 阶你才能到达楼顶。
 * 每次你可以爬 1 或 2 个台阶。你有多少种不同的方法可以爬到楼顶呢？
 *
 * 示例:
 *   输入: n = 2
 *   输出: 2
 *   解释: 有两种方法可以爬到楼顶：1+1 或 2
 *
 *   输入: n = 3
 *   输出: 3
 *   解释: 1+1+1, 1+2, 2+1
 *
 * 状态定义: dp[i] = 到达第 i 阶的方法数
 * 状态转移: dp[i] = dp[i-1] + dp[i-2]
 *
 * @param n 楼梯阶数
 * @return 方法数
 */
int climbStairs(int n) {
    // TODO: 实现你的解法
    return 0;
}

/**
 * 题目2: 最大子数组和
 *
 * 给你一个整数数组 nums，请你找出一个具有最大和的连续子数组（子数组最少包含一个元素），
 * 返回其最大和。
 *
 * 示例:
 *   输入: nums = [-2,1,-3,4,-1,2,1,-5,4]
 *   输出: 6
 *   解释: 连续子数组 [4,-1,2,1] 的和最大，为 6
 *
 * 状态定义: dp[i] = 以 nums[i] 结尾的最大子数组和
 * 状态转移: dp[i] = max(dp[i-1] + nums[i], nums[i])
 *
 * @param nums 整数数组
 * @return 最大子数组和
 */
int maxSubArray(std::vector<int>& nums) {
    // TODO: 实现你的解法
    return 0;
}

/**
 * 题目3: 打家劫舍
 *
 * 你是一个专业的小偷，计划偷窃沿街的房屋。每间房内都藏有一定的现金，
 * 影响你偷窃的唯一制约因素就是相邻的房屋装有相互连通的防盗系统，
 * 如果两间相邻的房屋在同一晚上被小偷闯入，系统会自动报警。
 *
 * 给定一个代表每个房屋存放金额的非负整数数组，
 * 计算你不触动警报装置的情况下，一夜之内能够偷窃到的最高金额。
 *
 * 示例:
 *   输入: nums = [1,2,3,1]
 *   输出: 4
 *   解释: 偷窃 1 号房屋 (金额 = 1) ，然后偷窃 3 号房屋 (金额 = 3)。
 *         偷窃到的最高金额 = 1 + 3 = 4
 *
 *   输入: nums = [2,7,9,3,1]
 *   输出: 12
 *   解释: 偷窃 1 号房屋 (金额 = 2), 偷窃 3 号房屋 (金额 = 9)，接着偷窃 5 号房屋 (金额 = 1)。
 *         偷窃到的最高金额 = 2 + 9 + 1 = 12
 *
 * @param nums 每个房屋的金额
 * @return 最高金额
 */
int rob(std::vector<int>& nums) {
    // TODO: 实现你的解法
    return 0;
}

/**
 * 题目4: 打家劫舍 II
 *
 * 你是一个专业的小偷，计划偷窃沿街的房屋，每间房内都藏有一定的现金。
 * 这个地方所有的房屋都围成一圈，这意味着第一个房屋和最后一个房屋是紧挨着的。
 * 同时，相邻的房屋装有相互连通的防盗系统。
 *
 * 给定一个代表每个房屋存放金额的非负整数数组，
 * 计算你在不触动警报装置的情况下，今晚能够偷窃到的最高金额。
 *
 * 示例:
 *   输入: nums = [2,3,2]
 *   输出: 3
 *   解释: 你不能先偷窃 1 号房屋（金额 = 2），然后偷窃 3 号房屋（金额 = 2），因为他们是相邻的。
 *
 *   输入: nums = [1,2,3,1]
 *   输出: 4
 *   解释: 偷窃 1 号房屋 (金额 = 1)，然后偷窃 3 号房屋 (金额 = 3)。
 *
 * 思路: 分两种情况，偷第一个不偷最后一个，或者不偷第一个偷最后一个
 *
 * @param nums 每个房屋的金额
 * @return 最高金额
 */
int robII(std::vector<int>& nums) {
    // TODO: 实现你的解法
    return 0;
}

/**
 * 题目5: 最小路径和
 *
 * 给定一个包含非负整数的 m x n 网格 grid，
 * 请找出一条从左上角到右下角的路径，使得路径上的数字总和为最小。
 *
 * 说明：每次只能向下或者向右移动一步。
 *
 * 示例:
 *   输入: grid = [[1,3,1],[1,5,1],[4,2,1]]
 *   输出: 7
 *   解释: 路径 1→3→1→1→1 的总和最小
 *
 * 状态定义: dp[i][j] = 到达 (i,j) 的最小路径和
 * 状态转移: dp[i][j] = grid[i][j] + min(dp[i-1][j], dp[i][j-1])
 *
 * @param grid 网格
 * @return 最小路径和
 */
int minPathSum(std::vector<std::vector<int>>& grid) {
    // TODO: 实现你的解法
    return 0;
}

/**
 * 题目6: 不同路径
 *
 * 一个机器人位于一个 m x n 网格的左上角。
 * 机器人每次只能向下或者向右移动一步。
 * 机器人试图达到网格的右下角。
 * 问总共有多少条不同的路径？
 *
 * 示例:
 *   输入: m = 3, n = 7
 *   输出: 28
 *
 *   输入: m = 3, n = 2
 *   输出: 3
 *   解释: 从左上角到右下角一共有 3 条路径：
 *         1. 向右 -> 向下 -> 向下
 *         2. 向下 -> 向下 -> 向右
 *         3. 向下 -> 向右 -> 向下
 *
 * @param m 行数
 * @param n 列数
 * @return 路径数
 */
int uniquePaths(int m, int n) {
    // TODO: 实现你的解法
    return 0;
}

/**
 * 题目7: 不同路径 II（有障碍物）
 *
 * 一个机器人位于一个 m x n 网格的左上角。
 * 机器人每次只能向下或者向右移动一步。
 * 现在考虑网格中有障碍物。那么从左上角到右下角将会有多少条不同的路径？
 *
 * 网格中的障碍物和空位置分别用 1 和 0 来表示。
 *
 * 示例:
 *   输入: obstacleGrid = [[0,0,0],[0,1,0],[0,0,0]]
 *   输出: 2
 *   解释: 中间有一个障碍物，只有两条路径绕过
 *
 * @param obstacleGrid 带障碍的网格
 * @return 路径数
 */
int uniquePathsWithObstacles(std::vector<std::vector<int>>& obstacleGrid) {
    // TODO: 实现你的解法
    return 0;
}

/**
 * 题目8: 解码方法
 *
 * 一条包含字母 A-Z 的消息通过以下映射进行了编码：
 *   'A' -> "1"
 *   'B' -> "2"
 *   ...
 *   'Z' -> "26"
 *
 * 要解码已编码的消息，所有数字必须基于上述映射的方法，反向映射回字母。
 *
 * 给你一个只含数字的非空字符串 s，请计算并返回解码方法的总数。
 *
 * 示例:
 *   输入: s = "12"
 *   输出: 2
 *   解释: "12" 可以解码为 "AB"（1 2）或者 "L"（12）
 *
 *   输入: s = "226"
 *   输出: 3
 *   解释: "226" 可以解码为 "BZ" (2 26), "VF" (22 6), 或者 "BBF" (2 2 6)
 *
 *   输入: s = "06"
 *   输出: 0
 *   解释: "06" 无法映射到 "F"，因为 "6" 和 "06" 不同
 *
 * @param s 编码字符串
 * @return 解码方法数
 */
int numDecodings(std::string s) {
    // TODO: 实现你的解法
    return 0;
}

/**
 * 题目9: 最长递增子序列
 *
 * 给你一个整数数组 nums，找到其中最长严格递增子序列的长度。
 *
 * 子序列是由数组派生而来的序列，删除（或不删除）数组中的元素而不改变其余元素的顺序。
 * 例如，[3,6,2,7] 是数组 [0,3,1,6,2,2,7] 的子序列。
 *
 * 示例:
 *   输入: nums = [10,9,2,5,3,7,101,18]
 *   输出: 4
 *   解释: 最长递增子序列是 [2,3,7,101]，长度为 4
 *
 *   输入: nums = [0,1,0,3,2,3]
 *   输出: 4
 *
 *   输入: nums = [7,7,7,7,7,7,7]
 *   输出: 1
 *
 * @param nums 整数数组
 * @return 最长递增子序列长度
 */
int lengthOfLIS(std::vector<int>& nums) {
    // TODO: 实现你的解法
    return 0;
}

/**
 * 题目10: 乘积最大子数组
 *
 * 给你一个整数数组 nums，请你找出数组中乘积最大的非空连续子数组，
 * 并返回该子数组所对应的乘积。
 *
 * 测试用例的答案是一个 32 位整数。
 *
 * 子数组是数组的连续子序列。
 *
 * 示例:
 *   输入: nums = [2,3,-2,4]
 *   输出: 6
 *   解释: 子数组 [2,3] 有最大乘积 6
 *
 *   输入: nums = [-2,0,-1]
 *   输出: 0
 *   解释: 结果不能为 2，因为 [-2,-1] 不是子数组
 *
 * 注意：因为有负数，需要同时记录最大和最小乘积
 *
 * @param nums 整数数组
 * @return 最大乘积
 */
int maxProduct(std::vector<int>& nums) {
    // TODO: 实现你的解法
    return 0;
}


// ==================== 测试代码 ====================
#include <cassert>

void testClimbStairs() {
    assert(climbStairs(2) == 2);
    assert(climbStairs(3) == 3);
    assert(climbStairs(4) == 5);
    std::cout << "climbStairs: PASSED\n";
}

void testMaxSubArray() {
    std::vector<int> nums = {-2, 1, -3, 4, -1, 2, 1, -5, 4};
    assert(maxSubArray(nums) == 6);

    nums = {1};
    assert(maxSubArray(nums) == 1);

    nums = {5, 4, -1, 7, 8};
    assert(maxSubArray(nums) == 23);

    std::cout << "maxSubArray: PASSED\n";
}

void testRob() {
    std::vector<int> nums = {1, 2, 3, 1};
    assert(rob(nums) == 4);

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

    grid = {{1, 2, 3}, {4, 5, 6}};
    assert(minPathSum(grid) == 12);

    std::cout << "minPathSum: PASSED\n";
}

void testUniquePaths() {
    assert(uniquePaths(3, 7) == 28);
    assert(uniquePaths(3, 2) == 3);
    assert(uniquePaths(1, 1) == 1);

    std::cout << "uniquePaths: PASSED\n";
}

void testUniquePathsWithObstacles() {
    std::vector<std::vector<int>> grid = {{0, 0, 0}, {0, 1, 0}, {0, 0, 0}};
    assert(uniquePathsWithObstacles(grid) == 2);

    grid = {{0, 1}, {0, 0}};
    assert(uniquePathsWithObstacles(grid) == 1);

    std::cout << "uniquePathsWithObstacles: PASSED\n";
}

void testNumDecodings() {
    assert(numDecodings("12") == 2);
    assert(numDecodings("226") == 3);
    assert(numDecodings("06") == 0);

    std::cout << "numDecodings: PASSED\n";
}

void testLengthOfLIS() {
    std::vector<int> nums = {10, 9, 2, 5, 3, 7, 101, 18};
    assert(lengthOfLIS(nums) == 4);

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

    std::cout << "maxProduct: PASSED\n";
}

int main() {
    std::cout << "=== Linear DP Tests ===\n";

    // 取消注释以运行测试（实现解法后）
    // testClimbStairs();
    // testMaxSubArray();
    // testRob();
    // testRobII();
    // testMinPathSum();
    // testUniquePaths();
    // testUniquePathsWithObstacles();
    // testNumDecodings();
    // testLengthOfLIS();
    // testMaxProduct();

    std::cout << "All tests passed!\n";
    return 0;
}
