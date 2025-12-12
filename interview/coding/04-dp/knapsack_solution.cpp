/**
 * @file knapsack_solution.cpp
 * @brief 背包问题 - 解答文件
 *
 * 包含 knapsack.cpp 中所有题目的解答和详细解析。
 */

#include <iostream>
#include <vector>
#include <algorithm>
#include <numeric>
#include <string>
#include <unordered_set>

/**
 * 题目1: 0-1 背包问题
 *
 * 解法1: 二维 DP
 * 时间复杂度: O(n*W)
 * 空间复杂度: O(n*W)
 */
int knapsack01_2D(int W, std::vector<int>& weight, std::vector<int>& value) {
    int n = weight.size();
    std::vector<std::vector<int>> dp(n + 1, std::vector<int>(W + 1, 0));

    for (int i = 1; i <= n; ++i) {
        for (int j = 0; j <= W; ++j) {
            // 不选第 i 个物品
            dp[i][j] = dp[i - 1][j];
            // 选第 i 个物品（如果容量够）
            if (j >= weight[i - 1]) {
                dp[i][j] = std::max(dp[i][j], dp[i - 1][j - weight[i - 1]] + value[i - 1]);
            }
        }
    }

    return dp[n][W];
}

/**
 * 解法2: 一维 DP（空间优化）
 * 关键：必须逆序遍历容量，保证每个物品只被选一次
 * 时间复杂度: O(n*W)
 * 空间复杂度: O(W)
 */
int knapsack01(int W, std::vector<int>& weight, std::vector<int>& value) {
    int n = weight.size();
    std::vector<int> dp(W + 1, 0);

    for (int i = 0; i < n; ++i) {
        // 逆序遍历容量！
        for (int j = W; j >= weight[i]; --j) {
            dp[j] = std::max(dp[j], dp[j - weight[i]] + value[i]);
        }
    }

    return dp[W];
}

/**
 * 题目2: 分割等和子集
 *
 * 解法: 转化为 0-1 背包
 * dp[j] = 是否可以选出元素使和为 j
 *
 * 时间复杂度: O(n*sum)
 * 空间复杂度: O(sum)
 */
bool canPartition(std::vector<int>& nums) {
    int sum = std::accumulate(nums.begin(), nums.end(), 0);

    // 总和为奇数，无法等分
    if (sum % 2 != 0) return false;

    int target = sum / 2;
    std::vector<bool> dp(target + 1, false);
    dp[0] = true;

    for (int num : nums) {
        // 逆序遍历
        for (int j = target; j >= num; --j) {
            dp[j] = dp[j] || dp[j - num];
        }
    }

    return dp[target];
}

/**
 * 题目3: 目标和
 *
 * 解法: 转化为 0-1 背包计数问题
 * 设正数和为 P，负数和的绝对值为 N
 * P - N = target, P + N = sum
 * => P = (sum + target) / 2
 *
 * 时间复杂度: O(n*P)
 * 空间复杂度: O(P)
 */
int findTargetSumWays(std::vector<int>& nums, int target) {
    int sum = std::accumulate(nums.begin(), nums.end(), 0);

    // (sum + target) 必须是偶数且非负
    if ((sum + target) % 2 != 0 || sum + target < 0) return 0;
    if (std::abs(target) > sum) return 0;

    int P = (sum + target) / 2;
    std::vector<int> dp(P + 1, 0);
    dp[0] = 1;

    for (int num : nums) {
        for (int j = P; j >= num; --j) {
            dp[j] += dp[j - num];
        }
    }

    return dp[P];
}

/**
 * 题目4: 最后一块石头的重量 II
 *
 * 解法: 转化为 0-1 背包
 * 将石头分成两堆，使两堆重量差最小
 * 等价于选出若干石头使其重量和尽量接近 sum/2
 *
 * 时间复杂度: O(n*sum)
 * 空间复杂度: O(sum)
 */
int lastStoneWeightII(std::vector<int>& stones) {
    int sum = std::accumulate(stones.begin(), stones.end(), 0);
    int target = sum / 2;

    std::vector<int> dp(target + 1, 0);

    for (int stone : stones) {
        for (int j = target; j >= stone; --j) {
            dp[j] = std::max(dp[j], dp[j - stone] + stone);
        }
    }

    // 一堆重量为 dp[target]，另一堆为 sum - dp[target]
    // 差为 sum - 2 * dp[target]
    return sum - 2 * dp[target];
}

/**
 * 题目5: 完全背包 - 零钱兑换 II（组合数）
 *
 * 解法: 完全背包
 * 组合问题：先遍历物品，再遍历容量
 *
 * 时间复杂度: O(n*amount)
 * 空间复杂度: O(amount)
 */
int change(int amount, std::vector<int>& coins) {
    std::vector<int> dp(amount + 1, 0);
    dp[0] = 1;

    // 先遍历物品（组合问题）
    for (int coin : coins) {
        // 正序遍历（完全背包）
        for (int j = coin; j <= amount; ++j) {
            dp[j] += dp[j - coin];
        }
    }

    return dp[amount];
}

/**
 * 题目6: 完全背包 - 组合总和 IV（排列数）
 *
 * 解法: 完全背包变体
 * 排列问题：先遍历容量，再遍历物品
 *
 * 时间复杂度: O(n*target)
 * 空间复杂度: O(target)
 */
int combinationSum4(std::vector<int>& nums, int target) {
    std::vector<unsigned long long> dp(target + 1, 0);
    dp[0] = 1;

    // 先遍历容量（排列问题）
    for (int j = 1; j <= target; ++j) {
        for (int num : nums) {
            if (j >= num) {
                dp[j] += dp[j - num];
            }
        }
    }

    return static_cast<int>(dp[target]);
}

/**
 * 题目7: 完全平方数
 *
 * 解法: 完全背包
 * 物品是 1, 4, 9, 16, ...，求装满背包的最少物品数
 *
 * 时间复杂度: O(n*sqrt(n))
 * 空间复杂度: O(n)
 */
int numSquares(int n) {
    std::vector<int> dp(n + 1, INT_MAX);
    dp[0] = 0;

    // 遍历每个完全平方数
    for (int i = 1; i * i <= n; ++i) {
        int square = i * i;
        for (int j = square; j <= n; ++j) {
            if (dp[j - square] != INT_MAX) {
                dp[j] = std::min(dp[j], dp[j - square] + 1);
            }
        }
    }

    return dp[n];
}

/**
 * 数学解法：四平方和定理
 * 任何正整数都可以表示为最多4个完全平方数之和
 */
int numSquaresMath(int n) {
    // 检查是否是完全平方数
    auto isPerfectSquare = [](int x) {
        int s = static_cast<int>(sqrt(x));
        return s * s == x;
    };

    // 检查是否是 4^a * (8b + 7) 的形式
    auto checkAnswer4 = [](int x) {
        while (x % 4 == 0) x /= 4;
        return x % 8 == 7;
    };

    if (isPerfectSquare(n)) return 1;
    if (checkAnswer4(n)) return 4;

    for (int i = 1; i * i <= n; ++i) {
        if (isPerfectSquare(n - i * i)) return 2;
    }

    return 3;
}

/**
 * 题目8: 单词拆分
 *
 * 解法: 完全背包变体
 * dp[i] = s[0..i-1] 是否可以被拆分
 *
 * 时间复杂度: O(n^2)
 * 空间复杂度: O(n)
 */
bool wordBreak(std::string s, std::vector<std::string>& wordDict) {
    std::unordered_set<std::string> wordSet(wordDict.begin(), wordDict.end());
    int n = s.size();

    std::vector<bool> dp(n + 1, false);
    dp[0] = true;

    for (int i = 1; i <= n; ++i) {
        for (int j = 0; j < i; ++j) {
            if (dp[j] && wordSet.count(s.substr(j, i - j))) {
                dp[i] = true;
                break;
            }
        }
    }

    return dp[n];
}

/**
 * 题目9: 一和零
 *
 * 解法: 二维费用的 0-1 背包
 * dp[i][j] = 最多 i 个 0 和 j 个 1 时能选的最大字符串数
 *
 * 时间复杂度: O(l*m*n)，l 为字符串数组长度
 * 空间复杂度: O(m*n)
 */
int findMaxForm(std::vector<std::string>& strs, int m, int n) {
    std::vector<std::vector<int>> dp(m + 1, std::vector<int>(n + 1, 0));

    for (const std::string& str : strs) {
        // 统计 0 和 1 的数量
        int zeros = std::count(str.begin(), str.end(), '0');
        int ones = str.size() - zeros;

        // 逆序遍历（0-1 背包）
        for (int i = m; i >= zeros; --i) {
            for (int j = n; j >= ones; --j) {
                dp[i][j] = std::max(dp[i][j], dp[i - zeros][j - ones] + 1);
            }
        }
    }

    return dp[m][n];
}


// ==================== 背包问题总结 ====================
/**
 * 背包问题分类：
 *
 * 1. 0-1 背包：每个物品只能选一次
 *    - 逆序遍历容量
 *    - dp[j] = max(dp[j], dp[j-weight[i]] + value[i])
 *
 * 2. 完全背包：每个物品可以选无限次
 *    - 正序遍历容量
 *    - dp[j] = max(dp[j], dp[j-weight[i]] + value[i])
 *
 * 3. 组合问题 vs 排列问题：
 *    - 组合（无序）：先遍历物品，再遍历容量
 *    - 排列（有序）：先遍历容量，再遍历物品
 *
 * 4. 求最值 vs 求方案数：
 *    - 最值：dp[j] = max(dp[j], ...)
 *    - 方案数：dp[j] += dp[j - ...]
 */


// ==================== 测试代码 ====================
#include <cassert>

void testKnapsack01() {
    std::vector<int> weight = {1, 3, 4};
    std::vector<int> value = {15, 20, 30};
    assert(knapsack01(4, weight, value) == 35);
    assert(knapsack01_2D(4, weight, value) == 35);

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

    nums = {1, 2, 5};
    assert(canPartition(nums) == false);

    std::cout << "canPartition: PASSED\n";
}

void testFindTargetSumWays() {
    std::vector<int> nums = {1, 1, 1, 1, 1};
    assert(findTargetSumWays(nums, 3) == 5);

    nums = {1};
    assert(findTargetSumWays(nums, 1) == 1);

    nums = {1, 0};
    assert(findTargetSumWays(nums, 1) == 2);

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

    coins = {10};
    assert(change(10, coins) == 1);

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
    assert(numSquaresMath(12) == 3);

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
    std::cout << "=== Knapsack Solution Tests ===\n";

    testKnapsack01();
    testCanPartition();
    testFindTargetSumWays();
    testLastStoneWeightII();
    testChange();
    testCombinationSum4();
    testNumSquares();
    testWordBreak();
    testFindMaxForm();

    std::cout << "\nAll tests passed!\n";
    return 0;
}
