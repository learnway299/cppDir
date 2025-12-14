/**
 * @file palindrome_partition_solution.cpp
 * @brief 分割回文串 - 参考答案
 */
#include "palindrome_partition.h"
#include <climits>
#include <iostream>
#include <cassert>

namespace PalindromePartition {

// ==================== 辅助函数 ====================
static void backtrackPartition(const std::string& s, int start,
                                std::vector<std::string>& path,
                                std::vector<std::vector<std::string>>& result,
                                std::vector<std::vector<bool>>& isPalin) {
    if (start == static_cast<int>(s.size())) {
        result.push_back(path);
        return;
    }
    for (int i = start; i < static_cast<int>(s.size()); ++i) {
        if (isPalin[start][i]) {
            path.push_back(s.substr(start, i - start + 1));
            backtrackPartition(s, i + 1, path, result, isPalin);
            path.pop_back();
        }
    }
}

// ==================== 参考答案实现 ====================

std::vector<std::vector<std::string>> partitionSolution(const std::string& s) {
    int n = static_cast<int>(s.size());
    std::vector<std::vector<bool>> isPalin(n, std::vector<bool>(n, false));

    // 预处理回文判断
    for (int i = n - 1; i >= 0; --i) {
        for (int j = i; j < n; ++j) {
            isPalin[i][j] = (s[i] == s[j]) && (j - i < 2 || isPalin[i + 1][j - 1]);
        }
    }

    std::vector<std::vector<std::string>> result;
    std::vector<std::string> path;
    backtrackPartition(s, 0, path, result, isPalin);
    return result;
}

int minCutSolution(const std::string& s) {
    int n = static_cast<int>(s.size());
    std::vector<std::vector<bool>> isPalin(n, std::vector<bool>(n, false));

    // 预处理回文判断
    for (int i = n - 1; i >= 0; --i) {
        for (int j = i; j < n; ++j) {
            isPalin[i][j] = (s[i] == s[j]) && (j - i < 2 || isPalin[i + 1][j - 1]);
        }
    }

    // dp[i] = s[0..i] 的最少分割次数
    std::vector<int> dp(n, INT_MAX);
    for (int i = 0; i < n; ++i) {
        if (isPalin[0][i]) {
            dp[i] = 0;
        } else {
            for (int j = 0; j < i; ++j) {
                if (isPalin[j + 1][i]) {
                    dp[i] = std::min(dp[i], dp[j] + 1);
                }
            }
        }
    }
    return dp[n - 1];
}

// ==================== 测试函数 ====================

void runTests() {
    std::cout << "\n=== Palindrome Partition Tests ===" << std::endl;

    // 测试分割回文串 I
    {
        auto result = partitionSolution("aab");
        assert(result.size() == 2);  // ["a","a","b"], ["aa","b"]
        std::cout << "  [PASS] Partition" << std::endl;
    }

    // 测试分割回文串 II
    {
        assert(minCutSolution("aab") == 1);  // "aa" | "b"
        assert(minCutSolution("a") == 0);    // 本身就是回文
        assert(minCutSolution("ab") == 1);   // "a" | "b"
        std::cout << "  [PASS] Min Cut" << std::endl;
    }

    std::cout << "=== All Palindrome Partition Tests Passed! ===" << std::endl;
}

} // namespace PalindromePartition

/**
 * 关键要点：
 *
 * 1. 回文预处理：
 *    - isPalin[i][j] 表示 s[i..j] 是否回文
 *    - 状态转移: isPalin[i][j] = (s[i] == s[j]) && isPalin[i+1][j-1]
 *    - 边界: j - i < 2 时只需 s[i] == s[j]
 *
 * 2. 分割回文串 I：
 *    - 回溯找所有分割方案
 *    - 每次选择一个回文前缀
 *    - 递归处理剩余部分
 *
 * 3. 分割回文串 II：
 *    - 动态规划
 *    - dp[i] = s[0..i] 的最少分割次数
 *    - 如果 s[0..i] 本身回文，dp[i] = 0
 *    - 否则枚举分割点 j，dp[i] = min(dp[j] + 1)
 */
