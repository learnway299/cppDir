/**
 * @file knapsack.cpp
 * @brief 背包问题 - 题目文件
 *
 * 背包问题是 DP 的经典模型，包括 0-1 背包、完全背包、多重背包等变体。
 */

#include <iostream>
#include <vector>
#include <algorithm>
#include <numeric>

/**
 * 题目1: 0-1 背包问题
 *
 * 有 n 件物品和一个最多能背重量为 W 的背包。
 * 第 i 件物品的重量是 weight[i]，价值是 value[i]。
 * 每件物品只能用一次，求解将哪些物品装入背包里物品价值总和最大。
 *
 * 示例:
 *   输入: W = 4, weight = [1,3,4], value = [15,20,30]
 *   输出: 35
 *   解释: 选择物品 0 和物品 1，重量 1+3=4，价值 15+20=35
 *
 * 状态定义: dp[i][j] = 前 i 个物品，背包容量为 j 时的最大价值
 * 状态转移: dp[i][j] = max(dp[i-1][j], dp[i-1][j-weight[i]] + value[i])
 *
 * @param W 背包容量
 * @param weight 物品重量
 * @param value 物品价值
 * @return 最大价值
 */
int knapsack01(int W, std::vector<int>& weight, std::vector<int>& value) {
    // TODO: 实现你的解法
    return 0;
}

/**
 * 题目2: 分割等和子集
 *
 * 给你一个只包含正整数的非空数组 nums。
 * 请你判断是否可以将这个数组分割成两个子集，使得两个子集的元素和相等。
 *
 * 示例:
 *   输入: nums = [1,5,11,5]
 *   输出: true
 *   解释: 数组可以分割成 [1, 5, 5] 和 [11]
 *
 *   输入: nums = [1,2,3,5]
 *   输出: false
 *   解释: 数组不能分割成两个元素和相等的子集
 *
 * 思路: 转化为 0-1 背包问题
 *       能否选出若干元素使其和等于 sum/2
 *
 * @param nums 整数数组
 * @return 是否可以等分
 */
bool canPartition(std::vector<int>& nums) {
    // TODO: 实现你的解法
    return false;
}

/**
 * 题目3: 目标和
 *
 * 给你一个整数数组 nums 和一个整数 target。
 * 向数组中的每个整数前添加 '+' 或 '-'，然后串联起所有整数，可以构造一个表达式。
 *
 * 例如，nums = [2, 1]，可以在 2 之前添加 '+' ，在 1 之前添加 '-' ，
 * 然后串联起来得到表达式 "+2-1"。
 *
 * 返回可以通过上述方法构造的、运算结果等于 target 的不同表达式的数目。
 *
 * 示例:
 *   输入: nums = [1,1,1,1,1], target = 3
 *   输出: 5
 *   解释:
 *     -1 + 1 + 1 + 1 + 1 = 3
 *     +1 - 1 + 1 + 1 + 1 = 3
 *     +1 + 1 - 1 + 1 + 1 = 3
 *     +1 + 1 + 1 - 1 + 1 = 3
 *     +1 + 1 + 1 + 1 - 1 = 3
 *
 * 思路: 设正数和为 P，负数和的绝对值为 N
 *       P - N = target
 *       P + N = sum
 *       所以 P = (sum + target) / 2
 *       问题转化为：从 nums 中选出若干元素使其和为 P
 *
 * @param nums 整数数组
 * @param target 目标值
 * @return 表达式数目
 */
int findTargetSumWays(std::vector<int>& nums, int target) {
    // TODO: 实现你的解法
    return 0;
}

/**
 * 题目4: 最后一块石头的重量 II
 *
 * 有一堆石头，用整数数组 stones 表示。其中 stones[i] 表示第 i 块石头的重量。
 *
 * 每一回合，从中选出任意两块石头，然后将它们一起粉碎。假设石头的重量分别为 x 和 y，且 x <= y。
 * 那么粉碎的可能结果如下：
 *   - 如果 x == y，那么两块石头都会被完全粉碎；
 *   - 如果 x != y，那么重量为 x 的石头将会完全粉碎，而重量为 y 的石头新重量为 y-x。
 *
 * 最后，最多只会剩下一块石头。返回此石头最小的可能重量。如果没有石头剩下，就返回 0。
 *
 * 示例:
 *   输入: stones = [2,7,4,1,8,1]
 *   输出: 1
 *   解释:
 *     组合 2 和 4，得到 2，所以数组转化为 [2,7,1,8,1]
 *     组合 7 和 8，得到 1，所以数组转化为 [2,1,1,1]
 *     组合 2 和 1，得到 1，所以数组转化为 [1,1,1]
 *     组合 1 和 1，得到 0，所以数组转化为 [1]
 *
 * 思路: 将石头分成两堆，使两堆重量差最小
 *       问题转化为：选出若干石头使其重量和尽量接近 sum/2
 *
 * @param stones 石头重量数组
 * @return 最小可能重量
 */
int lastStoneWeightII(std::vector<int>& stones) {
    // TODO: 实现你的解法
    return 0;
}

/**
 * 题目5: 完全背包 - 零钱兑换 II（组合数）
 *
 * 给你一个整数数组 coins 表示不同面额的硬币，另给一个整数 amount 表示总金额。
 * 请你计算并返回可以凑成总金额的硬币组合数。如果任何硬币组合都无法凑出总金额，返回 0。
 *
 * 假设每一种面额的硬币有无限个。
 *
 * 示例:
 *   输入: amount = 5, coins = [1,2,5]
 *   输出: 4
 *   解释: 有四种方式可以凑成总金额：
 *         5=5
 *         5=2+2+1
 *         5=2+1+1+1
 *         5=1+1+1+1+1
 *
 * 注意：这是组合问题（无序），不是排列问题
 *
 * @param amount 目标金额
 * @param coins 硬币面额
 * @return 组合数
 */
int change(int amount, std::vector<int>& coins) {
    // TODO: 实现你的解法
    return 0;
}

/**
 * 题目6: 完全背包 - 组合总和 IV（排列数）
 *
 * 给你一个由不同整数组成的数组 nums，和一个目标整数 target。
 * 请你从 nums 中找出并返回总和为 target 的元素组合的个数。
 *
 * 请注意，顺序不同的序列被视作不同的组合。
 *
 * 示例:
 *   输入: nums = [1,2,3], target = 4
 *   输出: 7
 *   解释: 所有可能的组合为：
 *         (1,1,1,1)
 *         (1,1,2)
 *         (1,2,1)
 *         (2,1,1)
 *         (2,2)
 *         (1,3)
 *         (3,1)
 *
 * 注意：这是排列问题（有序），遍历顺序与组合问题相反
 *
 * @param nums 整数数组
 * @param target 目标和
 * @return 排列数
 */
int combinationSum4(std::vector<int>& nums, int target) {
    // TODO: 实现你的解法
    return 0;
}

/**
 * 题目7: 完全平方数
 *
 * 给你一个整数 n，返回和为 n 的完全平方数的最少数量。
 *
 * 完全平方数是一个整数，其值等于另一个整数的平方；
 * 换句话说，其值等于一个整数自乘的积。例如，1、4、9 和 16 都是完全平方数，而 3 和 11 不是。
 *
 * 示例:
 *   输入: n = 12
 *   输出: 3
 *   解释: 12 = 4 + 4 + 4
 *
 *   输入: n = 13
 *   输出: 2
 *   解释: 13 = 4 + 9
 *
 * 思路: 完全背包，物品是 1, 4, 9, 16, ...，求装满背包的最少物品数
 *
 * @param n 目标值
 * @return 最少完全平方数数量
 */
int numSquares(int n) {
    // TODO: 实现你的解法
    return 0;
}

/**
 * 题目8: 单词拆分
 *
 * 给你一个字符串 s 和一个字符串列表 wordDict 作为字典。
 * 请你判断是否可以利用字典中出现的单词拼接出 s。
 *
 * 注意：不要求字典中出现的单词全部都使用，并且字典中的单词可以重复使用。
 *
 * 示例:
 *   输入: s = "leetcode", wordDict = ["leet","code"]
 *   输出: true
 *   解释: 返回 true 因为 "leetcode" 可以由 "leet" 和 "code" 拼接成。
 *
 *   输入: s = "applepenapple", wordDict = ["apple","pen"]
 *   输出: true
 *
 *   输入: s = "catsandog", wordDict = ["cats","dog","sand","and","cat"]
 *   输出: false
 *
 * @param s 字符串
 * @param wordDict 字典
 * @return 是否可以拆分
 */
bool wordBreak(std::string s, std::vector<std::string>& wordDict) {
    // TODO: 实现你的解法
    return false;
}

/**
 * 题目9: 一和零
 *
 * 给你一个二进制字符串数组 strs 和两个整数 m 和 n。
 * 请你找出并返回 strs 的最大子集的长度，该子集中最多有 m 个 0 和 n 个 1。
 *
 * 示例:
 *   输入: strs = ["10","0001","111001","1","0"], m = 5, n = 3
 *   输出: 4
 *   解释: 最多有 5 个 0 和 3 个 1 的最大子集是 {"10","0001","1","0"} ，因此答案是 4。
 *
 * 思路: 二维费用的 0-1 背包
 *       两个约束条件：0 的数量和 1 的数量
 *
 * @param strs 二进制字符串数组
 * @param m 最多 0 的数量
 * @param n 最多 1 的数量
 * @return 最大子集长度
 */
int findMaxForm(std::vector<std::string>& strs, int m, int n) {
    // TODO: 实现你的解法
    return 0;
}


// ==================== 测试代码 ====================
#include <cassert>

void testKnapsack01() {
    std::vector<int> weight = {1, 3, 4};
    std::vector<int> value = {15, 20, 30};
    assert(knapsack01(4, weight, value) == 35);

    weight = {1, 2, 3};
    value = {6, 10, 12};
    assert(knapsack01(5, weight, value) == 22);

    std::cout << "knapsack01: PASSED\n";
}

void testCanPartition() {
    std::vector<int> nums = {1, 5, 11, 5};
    assert(canPartition(nums) == true);

    nums = {1, 2, 3, 5};
    assert(canPartition(nums) == false);

    std::cout << "canPartition: PASSED\n";
}

void testFindTargetSumWays() {
    std::vector<int> nums = {1, 1, 1, 1, 1};
    assert(findTargetSumWays(nums, 3) == 5);

    nums = {1};
    assert(findTargetSumWays(nums, 1) == 1);

    std::cout << "findTargetSumWays: PASSED\n";
}

void testLastStoneWeightII() {
    std::vector<int> stones = {2, 7, 4, 1, 8, 1};
    assert(lastStoneWeightII(stones) == 1);

    stones = {31, 26, 33, 21, 40};
    assert(lastStoneWeightII(stones) == 5);

    std::cout << "lastStoneWeightII: PASSED\n";
}

void testChange() {
    std::vector<int> coins = {1, 2, 5};
    assert(change(5, coins) == 4);

    coins = {2};
    assert(change(3, coins) == 0);

    std::cout << "change: PASSED\n";
}

void testCombinationSum4() {
    std::vector<int> nums = {1, 2, 3};
    assert(combinationSum4(nums, 4) == 7);

    nums = {9};
    assert(combinationSum4(nums, 3) == 0);

    std::cout << "combinationSum4: PASSED\n";
}

void testNumSquares() {
    assert(numSquares(12) == 3);
    assert(numSquares(13) == 2);
    assert(numSquares(1) == 1);

    std::cout << "numSquares: PASSED\n";
}

void testWordBreak() {
    std::vector<std::string> dict = {"leet", "code"};
    assert(wordBreak("leetcode", dict) == true);

    dict = {"apple", "pen"};
    assert(wordBreak("applepenapple", dict) == true);

    dict = {"cats", "dog", "sand", "and", "cat"};
    assert(wordBreak("catsandog", dict) == false);

    std::cout << "wordBreak: PASSED\n";
}

void testFindMaxForm() {
    std::vector<std::string> strs = {"10", "0001", "111001", "1", "0"};
    assert(findMaxForm(strs, 5, 3) == 4);

    strs = {"10", "0", "1"};
    assert(findMaxForm(strs, 1, 1) == 2);

    std::cout << "findMaxForm: PASSED\n";
}

int main() {
    std::cout << "=== Knapsack Tests ===\n";

    // 取消注释以运行测试（实现解法后）
    // testKnapsack01();
    // testCanPartition();
    // testFindTargetSumWays();
    // testLastStoneWeightII();
    // testChange();
    // testCombinationSum4();
    // testNumSquares();
    // testWordBreak();
    // testFindMaxForm();

    std::cout << "All tests passed!\n";
    return 0;
}
