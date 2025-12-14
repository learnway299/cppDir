/**
 * @file interval_dp_solution.cpp
 * @brief 区间动态规划问题 - 参考答案
 */

#include "interval_dp.h"
#include <iostream>
#include <cassert>
#include <algorithm>
#include <climits>

namespace IntervalDP {

// ==================== 参考答案实现 ====================

// 题目1: 最长回文子串 - 中心扩展法
std::string longestPalindromeSolution(const std::string& s) {
    if (s.empty()) return "";

    int start = 0, maxLen = 1;

    auto expandAroundCenter = [&](int left, int right) {
        while (left >= 0 && right < static_cast<int>(s.size()) && s[left] == s[right]) {
            if (right - left + 1 > maxLen) {
                maxLen = right - left + 1;
                start = left;
            }
            --left;
            ++right;
        }
    };

    for (int i = 0; i < static_cast<int>(s.size()); ++i) {
        expandAroundCenter(i, i);
        expandAroundCenter(i, i + 1);
    }

    return s.substr(start, maxLen);
}

// 题目2: 回文子串数目 - 中心扩展法
int countSubstringsSolution(const std::string& s) {
    int count = 0;

    auto expandAroundCenter = [&](int left, int right) {
        while (left >= 0 && right < static_cast<int>(s.size()) && s[left] == s[right]) {
            ++count;
            --left;
            ++right;
        }
    };

    for (int i = 0; i < static_cast<int>(s.size()); ++i) {
        expandAroundCenter(i, i);
        expandAroundCenter(i, i + 1);
    }

    return count;
}

// 题目3: 戳气球
int maxCoinsSolution(std::vector<int>& nums) {
    int n = static_cast<int>(nums.size());

    std::vector<int> balloons(n + 2, 1);
    for (int i = 0; i < n; ++i) {
        balloons[i + 1] = nums[i];
    }

    std::vector<std::vector<int>> dp(n + 2, std::vector<int>(n + 2, 0));

    for (int len = 3; len <= n + 2; ++len) {
        for (int i = 0; i + len - 1 <= n + 1; ++i) {
            int j = i + len - 1;
            for (int k = i + 1; k < j; ++k) {
                int coins = balloons[i] * balloons[k] * balloons[j];
                dp[i][j] = std::max(dp[i][j], dp[i][k] + coins + dp[k][j]);
            }
        }
    }

    return dp[0][n + 1];
}

// 题目4: 最低票价
int mincostTicketsSolution(std::vector<int>& days, std::vector<int>& costs) {
    int lastDay = days.back();
    std::vector<bool> isTravel(lastDay + 1, false);
    for (int day : days) {
        isTravel[day] = true;
    }

    std::vector<int> dp(lastDay + 1, 0);

    for (int i = 1; i <= lastDay; ++i) {
        if (!isTravel[i]) {
            dp[i] = dp[i - 1];
        } else {
            dp[i] = dp[i - 1] + costs[0];
            dp[i] = std::min(dp[i], dp[std::max(0, i - 7)] + costs[1]);
            dp[i] = std::min(dp[i], dp[std::max(0, i - 30)] + costs[2]);
        }
    }

    return dp[lastDay];
}

// 题目5: 石子合并
int mergeStonesSolution(std::vector<int>& stones) {
    int n = static_cast<int>(stones.size());

    std::vector<int> prefix(n + 1, 0);
    for (int i = 0; i < n; ++i) {
        prefix[i + 1] = prefix[i] + stones[i];
    }

    std::vector<std::vector<int>> dp(n, std::vector<int>(n, 0));

    for (int len = 2; len <= n; ++len) {
        for (int i = 0; i + len - 1 < n; ++i) {
            int j = i + len - 1;
            dp[i][j] = INT_MAX;
            for (int k = i; k < j; ++k) {
                dp[i][j] = std::min(dp[i][j], dp[i][k] + dp[k + 1][j]);
            }
            dp[i][j] += prefix[j + 1] - prefix[i];
        }
    }

    return dp[0][n - 1];
}

// 题目6: 合并石头的最低成本（每次合并 K 堆）
int mergeKStonesSolution(std::vector<int>& stones, int k) {
    int n = static_cast<int>(stones.size());
    if ((n - 1) % (k - 1) != 0) return -1;

    std::vector<int> prefix(n + 1, 0);
    for (int i = 0; i < n; ++i) {
        prefix[i + 1] = prefix[i] + stones[i];
    }

    std::vector<std::vector<int>> dp(n, std::vector<int>(n, 0));

    for (int len = k; len <= n; ++len) {
        for (int i = 0; i + len - 1 < n; ++i) {
            int j = i + len - 1;
            dp[i][j] = INT_MAX;

            for (int m = i; m < j; m += k - 1) {
                dp[i][j] = std::min(dp[i][j], dp[i][m] + dp[m + 1][j]);
            }

            if ((j - i) % (k - 1) == 0) {
                dp[i][j] += prefix[j + 1] - prefix[i];
            }
        }
    }

    return dp[0][n - 1];
}

// 题目7: 多边形三角剖分的最低得分
int minScoreTriangulationSolution(std::vector<int>& values) {
    int n = static_cast<int>(values.size());
    std::vector<std::vector<int>> dp(n, std::vector<int>(n, 0));

    for (int len = 3; len <= n; ++len) {
        for (int i = 0; i + len - 1 < n; ++i) {
            int j = i + len - 1;
            dp[i][j] = INT_MAX;

            for (int k = i + 1; k < j; ++k) {
                int score = values[i] * values[k] * values[j];
                dp[i][j] = std::min(dp[i][j], dp[i][k] + score + dp[k][j]);
            }
        }
    }

    return dp[0][n - 1];
}

// 题目8: 矩阵链乘法
int matrixChainMultiplicationSolution(std::vector<int>& p) {
    int n = static_cast<int>(p.size()) - 1;
    std::vector<std::vector<int>> dp(n, std::vector<int>(n, 0));

    for (int len = 2; len <= n; ++len) {
        for (int i = 0; i + len - 1 < n; ++i) {
            int j = i + len - 1;
            dp[i][j] = INT_MAX;

            for (int k = i; k < j; ++k) {
                int cost = dp[i][k] + dp[k + 1][j] + p[i] * p[k + 1] * p[j + 1];
                dp[i][j] = std::min(dp[i][j], cost);
            }
        }
    }

    return dp[0][n - 1];
}

// ==================== 测试函数 ====================

void testIntervalDPSolution() {
    std::cout << "=== Interval DP Tests (Solution) ===\n";

    // longestPalindrome
    std::string result = longestPalindromeSolution("babad");
    assert(result == "bab" || result == "aba");
    result = longestPalindromeSolution("cbbd");
    assert(result == "bb");
    result = longestPalindromeSolution("a");
    assert(result == "a");
    std::cout << "  longestPalindromeSolution: PASSED\n";

    // countSubstrings
    assert(countSubstringsSolution("abc") == 3);
    assert(countSubstringsSolution("aaa") == 6);
    assert(countSubstringsSolution("abba") == 6);
    std::cout << "  countSubstringsSolution: PASSED\n";

    // maxCoins
    std::vector<int> nums = {3, 1, 5, 8};
    assert(maxCoinsSolution(nums) == 167);
    nums = {1, 5};
    assert(maxCoinsSolution(nums) == 10);
    std::cout << "  maxCoinsSolution: PASSED\n";

    // mincostTickets
    std::vector<int> days = {1, 4, 6, 7, 8, 20};
    std::vector<int> costs = {2, 7, 15};
    assert(mincostTicketsSolution(days, costs) == 11);
    days = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 30, 31};
    costs = {2, 7, 15};
    assert(mincostTicketsSolution(days, costs) == 17);
    std::cout << "  mincostTicketsSolution: PASSED\n";

    // mergeStones
    std::vector<int> stones = {3, 2, 4, 1};
    assert(mergeStonesSolution(stones) == 20);
    std::cout << "  mergeStonesSolution: PASSED\n";

    // mergeKStones
    stones = {3, 2, 4, 1};
    assert(mergeKStonesSolution(stones, 2) == 20);
    stones = {3, 2, 4, 1};
    assert(mergeKStonesSolution(stones, 3) == -1);
    std::cout << "  mergeKStonesSolution: PASSED\n";

    // minScoreTriangulation
    std::vector<int> values = {1, 2, 3};
    assert(minScoreTriangulationSolution(values) == 6);
    values = {3, 7, 4, 5};
    assert(minScoreTriangulationSolution(values) == 144);
    std::cout << "  minScoreTriangulationSolution: PASSED\n";

    // matrixChainMultiplication
    std::vector<int> p = {10, 30, 5, 60};
    assert(matrixChainMultiplicationSolution(p) == 4500);
    p = {40, 20, 30, 10, 30};
    assert(matrixChainMultiplicationSolution(p) == 26000);
    std::cout << "  matrixChainMultiplicationSolution: PASSED\n";
}

} // namespace IntervalDP
