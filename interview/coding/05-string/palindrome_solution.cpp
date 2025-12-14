/**
 * @file palindrome_solution.cpp
 * @brief 回文字符串问题 - 参考答案
 */

#include "palindrome.h"
#include <iostream>
#include <cassert>
#include <algorithm>
#include <functional>
#include <unordered_map>

namespace PalindromeProblems {

// ==================== 参考答案实现 ====================

// 题目1: 验证回文串
bool isPalindromeSolution(std::string s) {
    int left = 0, right = static_cast<int>(s.size()) - 1;

    while (left < right) {
        while (left < right && !std::isalnum(s[left])) {
            ++left;
        }
        while (left < right && !std::isalnum(s[right])) {
            --right;
        }

        if (std::tolower(s[left]) != std::tolower(s[right])) {
            return false;
        }

        ++left;
        --right;
    }

    return true;
}

// 题目2: 验证回文串 II
bool validPalindromeSolution(std::string s) {
    auto isPalin = [&](int left, int right) -> bool {
        while (left < right) {
            if (s[left] != s[right]) return false;
            ++left;
            --right;
        }
        return true;
    };

    int left = 0, right = static_cast<int>(s.size()) - 1;

    while (left < right) {
        if (s[left] != s[right]) {
            return isPalin(left + 1, right) || isPalin(left, right - 1);
        }
        ++left;
        --right;
    }

    return true;
}

// 题目3: 最长回文子串 - 中心扩展
std::string longestPalindromeSolution(std::string s) {
    if (s.empty()) return "";

    int start = 0, maxLen = 1;

    auto expandAroundCenter = [&](int left, int right) {
        while (left >= 0 && right < static_cast<int>(s.size()) && s[left] == s[right]) {
            if (right - left + 1 > maxLen) {
                start = left;
                maxLen = right - left + 1;
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

// 题目4: 回文子串数目 - 中心扩展
int countSubstringsSolution(std::string s) {
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

// 题目5: 最长回文子序列
int longestPalindromeSubseqSolution(std::string s) {
    int n = static_cast<int>(s.size());
    std::vector<std::vector<int>> dp(n, std::vector<int>(n, 0));

    for (int i = 0; i < n; ++i) {
        dp[i][i] = 1;
    }

    for (int len = 2; len <= n; ++len) {
        for (int i = 0; i + len - 1 < n; ++i) {
            int j = i + len - 1;

            if (s[i] == s[j]) {
                dp[i][j] = dp[i + 1][j - 1] + 2;
            } else {
                dp[i][j] = std::max(dp[i + 1][j], dp[i][j - 1]);
            }
        }
    }

    return dp[0][n - 1];
}

// 题目6: 分割回文串
std::vector<std::vector<std::string>> partitionSolution(std::string s) {
    int n = static_cast<int>(s.size());

    std::vector<std::vector<bool>> isPalin(n, std::vector<bool>(n, false));
    for (int i = n - 1; i >= 0; --i) {
        for (int j = i; j < n; ++j) {
            if (s[i] == s[j] && (j - i <= 2 || isPalin[i + 1][j - 1])) {
                isPalin[i][j] = true;
            }
        }
    }

    std::vector<std::vector<std::string>> result;
    std::vector<std::string> path;

    std::function<void(int)> backtrack = [&](int start) {
        if (start == n) {
            result.push_back(path);
            return;
        }

        for (int end = start; end < n; ++end) {
            if (isPalin[start][end]) {
                path.push_back(s.substr(start, end - start + 1));
                backtrack(end + 1);
                path.pop_back();
            }
        }
    };

    backtrack(0);
    return result;
}

// 题目7: 分割回文串 II
int minCutSolution(std::string s) {
    int n = static_cast<int>(s.size());
    if (n <= 1) return 0;

    std::vector<std::vector<bool>> isPalin(n, std::vector<bool>(n, false));
    for (int i = n - 1; i >= 0; --i) {
        for (int j = i; j < n; ++j) {
            if (s[i] == s[j] && (j - i <= 2 || isPalin[i + 1][j - 1])) {
                isPalin[i][j] = true;
            }
        }
    }

    std::vector<int> dp(n, 0);
    for (int i = 0; i < n; ++i) {
        if (isPalin[0][i]) {
            dp[i] = 0;
        } else {
            dp[i] = i;
            for (int j = 1; j <= i; ++j) {
                if (isPalin[j][i]) {
                    dp[i] = std::min(dp[i], dp[j - 1] + 1);
                }
            }
        }
    }

    return dp[n - 1];
}

// 题目8: 回文数
bool isPalindromeNumberSolution(int x) {
    if (x < 0 || (x % 10 == 0 && x != 0)) {
        return false;
    }

    int reversed = 0;
    while (x > reversed) {
        reversed = reversed * 10 + x % 10;
        x /= 10;
    }

    return x == reversed || x == reversed / 10;
}

// 题目9: 最短回文串
std::string shortestPalindromeSolution(std::string s) {
    std::string rev = s;
    std::reverse(rev.begin(), rev.end());

    std::string combined = s + "#" + rev;
    int n = static_cast<int>(combined.size());

    std::vector<int> lps(n, 0);
    for (int i = 1; i < n; ++i) {
        int j = lps[i - 1];
        while (j > 0 && combined[i] != combined[j]) {
            j = lps[j - 1];
        }
        if (combined[i] == combined[j]) {
            ++j;
        }
        lps[i] = j;
    }

    int longestPalinPrefix = lps[n - 1];

    std::string toAdd = s.substr(longestPalinPrefix);
    std::reverse(toAdd.begin(), toAdd.end());

    return toAdd + s;
}

// 题目10: 回文对
std::vector<std::vector<int>> palindromePairsSolution(std::vector<std::string>& words) {
    std::unordered_map<std::string, int> wordIndex;
    for (int i = 0; i < static_cast<int>(words.size()); ++i) {
        std::string rev = words[i];
        std::reverse(rev.begin(), rev.end());
        wordIndex[rev] = i;
    }

    auto isPalin = [](const std::string& s, int left, int right) -> bool {
        while (left < right) {
            if (s[left] != s[right]) return false;
            ++left;
            --right;
        }
        return true;
    };

    std::vector<std::vector<int>> result;

    for (int i = 0; i < static_cast<int>(words.size()); ++i) {
        const std::string& word = words[i];
        int m = static_cast<int>(word.size());

        for (int j = 0; j <= m; ++j) {
            std::string left = word.substr(0, j);
            std::string right = word.substr(j);

            if (isPalin(left, 0, static_cast<int>(left.size()) - 1)) {
                if (wordIndex.count(right) && wordIndex[right] != i) {
                    result.push_back({wordIndex[right], i});
                }
            }

            if (j > 0 && isPalin(right, 0, static_cast<int>(right.size()) - 1)) {
                if (wordIndex.count(left) && wordIndex[left] != i) {
                    result.push_back({i, wordIndex[left]});
                }
            }
        }
    }

    return result;
}

// ==================== 测试函数 ====================

void testPalindromeSolution() {
    std::cout << "=== Palindrome Tests (Solution) ===\n";

    // 测试1: 验证回文串
    assert(isPalindromeSolution("A man, a plan, a canal: Panama") == true);
    assert(isPalindromeSolution("race a car") == false);
    assert(isPalindromeSolution(" ") == true);
    std::cout << "  isPalindrome: PASSED\n";

    // 测试2: 验证回文串 II
    assert(validPalindromeSolution("aba") == true);
    assert(validPalindromeSolution("abca") == true);
    assert(validPalindromeSolution("abc") == false);
    std::cout << "  validPalindrome: PASSED\n";

    // 测试3: 最长回文子串
    std::string result = longestPalindromeSolution("babad");
    assert(result == "bab" || result == "aba");
    assert(longestPalindromeSolution("cbbd") == "bb");
    std::cout << "  longestPalindrome: PASSED\n";

    // 测试4: 回文子串数目
    assert(countSubstringsSolution("abc") == 3);
    assert(countSubstringsSolution("aaa") == 6);
    std::cout << "  countSubstrings: PASSED\n";

    // 测试5: 最长回文子序列
    assert(longestPalindromeSubseqSolution("bbbab") == 4);
    assert(longestPalindromeSubseqSolution("cbbd") == 2);
    std::cout << "  longestPalindromeSubseq: PASSED\n";

    // 测试6: 分割回文串
    auto partResult = partitionSolution("aab");
    assert(partResult.size() == 2);
    std::cout << "  partition: PASSED\n";

    // 测试7: 分割回文串 II
    assert(minCutSolution("aab") == 1);
    assert(minCutSolution("a") == 0);
    assert(minCutSolution("ab") == 1);
    std::cout << "  minCut: PASSED\n";

    // 测试8: 回文数
    assert(isPalindromeNumberSolution(121) == true);
    assert(isPalindromeNumberSolution(-121) == false);
    assert(isPalindromeNumberSolution(10) == false);
    std::cout << "  isPalindromeNumber: PASSED\n";

    // 测试9: 最短回文串
    assert(shortestPalindromeSolution("aacecaaa") == "aaacecaaa");
    assert(shortestPalindromeSolution("abcd") == "dcbabcd");
    std::cout << "  shortestPalindrome: PASSED\n";

    // 测试10: 回文对
    std::vector<std::string> words = {"abcd", "dcba", "lls", "s", "sssll"};
    auto pairsResult = palindromePairsSolution(words);
    assert(pairsResult.size() == 4);
    std::cout << "  palindromePairs: PASSED\n";

    std::cout << "All Palindrome tests passed!\n";
}

} // namespace PalindromeProblems
