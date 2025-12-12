/**
 * @file interval_dp_solution.cpp
 * @brief 区间动态规划问题 - 解答文件
 *
 * 包含 interval_dp.cpp 中所有题目的解答和详细解析。
 */

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <climits>
#include <numeric>

/**
 * 题目1: 最长回文子串
 *
 * 解法1: 区间 DP
 * dp[i][j] = s[i..j] 是否为回文串
 *
 * 时间复杂度: O(n^2)
 * 空间复杂度: O(n^2)
 */
std::string longestPalindromeDP(std::string s) {
    int n = s.size();
    if (n < 2) return s;

    std::vector<std::vector<bool>> dp(n, std::vector<bool>(n, false));
    int start = 0, maxLen = 1;

    // 单个字符是回文
    for (int i = 0; i < n; ++i) {
        dp[i][i] = true;
    }

    // 按区间长度遍历
    for (int len = 2; len <= n; ++len) {
        for (int i = 0; i + len - 1 < n; ++i) {
            int j = i + len - 1;
            if (s[i] == s[j]) {
                if (len == 2) {
                    dp[i][j] = true;
                } else {
                    dp[i][j] = dp[i + 1][j - 1];
                }
            }

            if (dp[i][j] && len > maxLen) {
                maxLen = len;
                start = i;
            }
        }
    }

    return s.substr(start, maxLen);
}

/**
 * 解法2: 中心扩展法
 * 时间复杂度: O(n^2)
 * 空间复杂度: O(1)
 */
std::string longestPalindrome(std::string s) {
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
        expandAroundCenter(i, i);      // 奇数长度
        expandAroundCenter(i, i + 1);  // 偶数长度
    }

    return s.substr(start, maxLen);
}

/**
 * 解法3: Manacher 算法
 * 时间复杂度: O(n)
 * 空间复杂度: O(n)
 */
std::string longestPalindromeManacher(std::string s) {
    // 预处理：在每个字符间插入 #
    std::string t = "#";
    for (char c : s) {
        t += c;
        t += '#';
    }

    int n = t.size();
    std::vector<int> p(n, 0);  // p[i] = 以 i 为中心的最长回文半径
    int center = 0, rightBound = 0;

    for (int i = 0; i < n; ++i) {
        if (i < rightBound) {
            int mirror = 2 * center - i;
            p[i] = std::min(rightBound - i, p[mirror]);
        }

        // 尝试扩展
        while (i - p[i] - 1 >= 0 && i + p[i] + 1 < n && t[i - p[i] - 1] == t[i + p[i] + 1]) {
            ++p[i];
        }

        // 更新中心和右边界
        if (i + p[i] > rightBound) {
            center = i;
            rightBound = i + p[i];
        }
    }

    // 找最长回文
    int maxLen = 0, centerIdx = 0;
    for (int i = 0; i < n; ++i) {
        if (p[i] > maxLen) {
            maxLen = p[i];
            centerIdx = i;
        }
    }

    // 转换回原字符串的索引
    int start = (centerIdx - maxLen) / 2;
    return s.substr(start, maxLen);
}

/**
 * 题目2: 回文子串数目
 *
 * 解法1: 区间 DP
 */
int countSubstringsDP(std::string s) {
    int n = s.size();
    std::vector<std::vector<bool>> dp(n, std::vector<bool>(n, false));
    int count = 0;

    for (int i = 0; i < n; ++i) {
        dp[i][i] = true;
        ++count;
    }

    for (int len = 2; len <= n; ++len) {
        for (int i = 0; i + len - 1 < n; ++i) {
            int j = i + len - 1;
            if (s[i] == s[j]) {
                if (len == 2 || dp[i + 1][j - 1]) {
                    dp[i][j] = true;
                    ++count;
                }
            }
        }
    }

    return count;
}

/**
 * 解法2: 中心扩展法
 */
int countSubstrings(std::string s) {
    int count = 0;

    auto expandAroundCenter = [&](int left, int right) {
        while (left >= 0 && right < static_cast<int>(s.size()) && s[left] == s[right]) {
            ++count;
            --left;
            ++right;
        }
    };

    for (int i = 0; i < static_cast<int>(s.size()); ++i) {
        expandAroundCenter(i, i);      // 奇数长度
        expandAroundCenter(i, i + 1);  // 偶数长度
    }

    return count;
}

/**
 * 题目3: 戳气球
 *
 * 解法: 区间 DP
 * 关键思路：反向考虑，设 dp[i][j] 为戳破开区间 (i,j) 内所有气球能获得的最大硬币
 * k 是区间内最后一个被戳破的气球
 *
 * 时间复杂度: O(n^3)
 * 空间复杂度: O(n^2)
 */
int maxCoins(std::vector<int>& nums) {
    int n = nums.size();

    // 在两端添加虚拟气球 1
    std::vector<int> balloons(n + 2, 1);
    for (int i = 0; i < n; ++i) {
        balloons[i + 1] = nums[i];
    }

    // dp[i][j] = 戳破开区间 (i,j) 内所有气球的最大硬币
    std::vector<std::vector<int>> dp(n + 2, std::vector<int>(n + 2, 0));

    // 区间长度从 3 开始（至少包含一个真实气球）
    for (int len = 3; len <= n + 2; ++len) {
        for (int i = 0; i + len - 1 <= n + 1; ++i) {
            int j = i + len - 1;
            // k 是开区间 (i,j) 内最后一个被戳破的气球
            for (int k = i + 1; k < j; ++k) {
                int coins = balloons[i] * balloons[k] * balloons[j];
                dp[i][j] = std::max(dp[i][j], dp[i][k] + coins + dp[k][j]);
            }
        }
    }

    return dp[0][n + 1];
}

/**
 * 题目4: 最低票价
 *
 * 解法: DP
 * dp[i] = 从第 i 天开始到年底所需的最小花费
 *
 * 时间复杂度: O(max(days))
 * 空间复杂度: O(max(days))
 */
int mincostTickets(std::vector<int>& days, std::vector<int>& costs) {
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

/**
 * 题目5: 石子合并
 *
 * 解法: 区间 DP
 * dp[i][j] = 合并 [i,j] 范围内石子的最小代价
 *
 * 时间复杂度: O(n^3)
 * 空间复杂度: O(n^2)
 */
int mergeStones(std::vector<int>& stones) {
    int n = stones.size();

    // 前缀和
    std::vector<int> prefix(n + 1, 0);
    for (int i = 0; i < n; ++i) {
        prefix[i + 1] = prefix[i] + stones[i];
    }

    // dp[i][j] = 合并 [i,j] 的最小代价
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

/**
 * 题目6: 合并石头的最低成本（每次合并 K 堆）
 *
 * 解法: 区间 DP
 * 能合并成一堆的条件: (n - 1) % (K - 1) == 0
 *
 * 时间复杂度: O(n^3 / K)
 * 空间复杂度: O(n^2)
 */
int mergeKStones(std::vector<int>& stones, int K) {
    int n = stones.size();
    if ((n - 1) % (K - 1) != 0) return -1;

    std::vector<int> prefix(n + 1, 0);
    for (int i = 0; i < n; ++i) {
        prefix[i + 1] = prefix[i] + stones[i];
    }

    // dp[i][j] = 将 [i,j] 合并成尽可能少的堆的最小代价
    std::vector<std::vector<int>> dp(n, std::vector<int>(n, 0));

    for (int len = K; len <= n; ++len) {
        for (int i = 0; i + len - 1 < n; ++i) {
            int j = i + len - 1;
            dp[i][j] = INT_MAX;

            // 分割点，步长为 K-1
            for (int k = i; k < j; k += K - 1) {
                dp[i][j] = std::min(dp[i][j], dp[i][k] + dp[k + 1][j]);
            }

            // 如果 [i,j] 可以合并成一堆
            if ((j - i) % (K - 1) == 0) {
                dp[i][j] += prefix[j + 1] - prefix[i];
            }
        }
    }

    return dp[0][n - 1];
}

/**
 * 题目7: 多边形三角剖分的最低得分
 *
 * 解法: 区间 DP
 * dp[i][j] = 顶点 i 到 j 形成的多边形的最低剖分得分
 *
 * 时间复杂度: O(n^3)
 * 空间复杂度: O(n^2)
 */
int minScoreTriangulation(std::vector<int>& values) {
    int n = values.size();
    std::vector<std::vector<int>> dp(n, std::vector<int>(n, 0));

    for (int len = 3; len <= n; ++len) {
        for (int i = 0; i + len - 1 < n; ++i) {
            int j = i + len - 1;
            dp[i][j] = INT_MAX;

            // k 是三角形的第三个顶点
            for (int k = i + 1; k < j; ++k) {
                int score = values[i] * values[k] * values[j];
                dp[i][j] = std::min(dp[i][j], dp[i][k] + score + dp[k][j]);
            }
        }
    }

    return dp[0][n - 1];
}

/**
 * 题目8: 矩阵链乘法
 *
 * 解法: 区间 DP
 * dp[i][j] = 计算矩阵 i 到 j 的乘积所需的最少乘法次数
 *
 * 时间复杂度: O(n^3)
 * 空间复杂度: O(n^2)
 */
int matrixChainMultiplication(std::vector<int>& p) {
    int n = p.size() - 1;  // 矩阵数量
    std::vector<std::vector<int>> dp(n, std::vector<int>(n, 0));

    for (int len = 2; len <= n; ++len) {
        for (int i = 0; i + len - 1 < n; ++i) {
            int j = i + len - 1;
            dp[i][j] = INT_MAX;

            // k 是分割点
            for (int k = i; k < j; ++k) {
                int cost = dp[i][k] + dp[k + 1][j] + p[i] * p[k + 1] * p[j + 1];
                dp[i][j] = std::min(dp[i][j], cost);
            }
        }
    }

    return dp[0][n - 1];
}


// ==================== 区间 DP 模板总结 ====================
/**
 * 区间 DP 的一般形式：
 *
 * for (int len = 2; len <= n; ++len) {        // 枚举区间长度
 *     for (int i = 0; i + len - 1 < n; ++i) { // 枚举起点
 *         int j = i + len - 1;                 // 计算终点
 *         for (int k = i; k < j; ++k) {       // 枚举分割点
 *             dp[i][j] = opt(dp[i][j], dp[i][k] + dp[k+1][j] + cost(i,k,j));
 *         }
 *     }
 * }
 *
 * 关键点：
 * 1. 外层循环是区间长度（从小到大）
 * 2. 内层循环枚举起点和分割点
 * 3. 状态转移通常是将大区间分割成小区间
 */


// ==================== 测试代码 ====================
#include <cassert>

void testLongestPalindrome() {
    std::string result = longestPalindrome("babad");
    assert(result == "bab" || result == "aba");

    result = longestPalindromeDP("babad");
    assert(result == "bab" || result == "aba");

    result = longestPalindromeManacher("babad");
    assert(result == "bab" || result == "aba");

    result = longestPalindrome("cbbd");
    assert(result == "bb");

    std::cout << "longestPalindrome: PASSED\n";
}

void testCountSubstrings() {
    assert(countSubstrings("abc") == 3);
    assert(countSubstringsDP("abc") == 3);
    assert(countSubstrings("aaa") == 6);
    assert(countSubstrings("abba") == 6);

    std::cout << "countSubstrings: PASSED\n";
}

void testMaxCoins() {
    std::vector<int> nums = {3, 1, 5, 8};
    assert(maxCoins(nums) == 167);

    nums = {1, 5};
    assert(maxCoins(nums) == 10);

    std::cout << "maxCoins: PASSED\n";
}

void testMincostTickets() {
    std::vector<int> days = {1, 4, 6, 7, 8, 20};
    std::vector<int> costs = {2, 7, 15};
    assert(mincostTickets(days, costs) == 11);

    days = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 30, 31};
    costs = {2, 7, 15};
    assert(mincostTickets(days, costs) == 17);

    std::cout << "mincostTickets: PASSED\n";
}

void testMergeStones() {
    std::vector<int> stones = {3, 2, 4, 1};
    assert(mergeStones(stones) == 20);

    stones = {1, 2, 3, 4};
    assert(mergeStones(stones) == 19);

    std::cout << "mergeStones: PASSED\n";
}

void testMergeKStones() {
    std::vector<int> stones = {3, 2, 4, 1};
    assert(mergeKStones(stones, 2) == 20);

    stones = {3, 2, 4, 1};
    assert(mergeKStones(stones, 3) == -1);

    stones = {3, 5, 1, 2, 6};
    assert(mergeKStones(stones, 3) == 25);

    std::cout << "mergeKStones: PASSED\n";
}

void testMinScoreTriangulation() {
    std::vector<int> values = {1, 2, 3};
    assert(minScoreTriangulation(values) == 6);

    values = {3, 7, 4, 5};
    assert(minScoreTriangulation(values) == 144);

    values = {1, 3, 1, 4, 1, 5};
    assert(minScoreTriangulation(values) == 13);

    std::cout << "minScoreTriangulation: PASSED\n";
}

void testMatrixChainMultiplication() {
    std::vector<int> p = {10, 30, 5, 60};
    assert(matrixChainMultiplication(p) == 4500);

    p = {40, 20, 30, 10, 30};
    assert(matrixChainMultiplication(p) == 26000);

    std::cout << "matrixChainMultiplication: PASSED\n";
}

int main() {
    std::cout << "=== Interval DP Solution Tests ===\n";

    testLongestPalindrome();
    testCountSubstrings();
    testMaxCoins();
    testMincostTickets();
    testMergeStones();
    testMergeKStones();
    testMinScoreTriangulation();
    testMatrixChainMultiplication();

    std::cout << "\nAll tests passed!\n";
    return 0;
}
