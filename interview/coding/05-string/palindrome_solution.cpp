/**
 * @file palindrome_solution.cpp
 * @brief 回文字符串问题 - 解答文件
 *
 * 回文问题常用技巧：
 * 1. 中心扩展：枚举中心点，向两边扩展
 * 2. 动态规划：dp[i][j] 表示 s[i..j] 是否为回文
 * 3. Manacher 算法：O(n) 求所有回文子串
 */

#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>

/**
 * 题目1: 验证回文串
 *
 * 解法: 双指针
 * 时间复杂度: O(n)
 * 空间复杂度: O(1)
 */
bool isPalindrome(std::string s) {
    int left = 0, right = s.size() - 1;

    while (left < right) {
        // 跳过非字母数字
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

/**
 * 题目2: 验证回文串 II
 *
 * 解法: 双指针 + 贪心
 * 时间复杂度: O(n)
 * 空间复杂度: O(1)
 */
bool validPalindrome(std::string s) {
    auto isPalin = [&](int left, int right) -> bool {
        while (left < right) {
            if (s[left] != s[right]) return false;
            ++left;
            --right;
        }
        return true;
    };

    int left = 0, right = s.size() - 1;

    while (left < right) {
        if (s[left] != s[right]) {
            // 尝试删除左边或右边的字符
            return isPalin(left + 1, right) || isPalin(left, right - 1);
        }
        ++left;
        --right;
    }

    return true;
}

/**
 * 题目3: 最长回文子串
 *
 * 解法1: 中心扩展
 * 时间复杂度: O(n²)
 * 空间复杂度: O(1)
 */
std::string longestPalindrome(std::string s) {
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
        expandAroundCenter(i, i);      // 奇数长度
        expandAroundCenter(i, i + 1);  // 偶数长度
    }

    return s.substr(start, maxLen);
}

/**
 * 解法2: 动态规划
 * 时间复杂度: O(n²)
 * 空间复杂度: O(n²)
 */
std::string longestPalindromeDP(std::string s) {
    int n = s.size();
    if (n < 2) return s;

    std::vector<std::vector<bool>> dp(n, std::vector<bool>(n, false));
    int start = 0, maxLen = 1;

    // 单个字符都是回文
    for (int i = 0; i < n; ++i) {
        dp[i][i] = true;
    }

    // 从小区间到大区间
    for (int len = 2; len <= n; ++len) {
        for (int i = 0; i + len - 1 < n; ++i) {
            int j = i + len - 1;

            if (s[i] == s[j]) {
                if (len == 2) {
                    dp[i][j] = true;
                } else {
                    dp[i][j] = dp[i + 1][j - 1];
                }

                if (dp[i][j] && len > maxLen) {
                    start = i;
                    maxLen = len;
                }
            }
        }
    }

    return s.substr(start, maxLen);
}

/**
 * 解法3: Manacher 算法
 * 时间复杂度: O(n)
 * 空间复杂度: O(n)
 */
std::string longestPalindromeManacher(std::string s) {
    if (s.empty()) return "";

    // 预处理：插入分隔符
    std::string t = "#";
    for (char c : s) {
        t += c;
        t += '#';
    }

    int n = t.size();
    std::vector<int> p(n, 0);  // p[i] = 以 i 为中心的回文半径
    int center = 0, right = 0;
    int maxLen = 0, maxCenter = 0;

    for (int i = 0; i < n; ++i) {
        // 利用对称性
        if (i < right) {
            int mirror = 2 * center - i;
            p[i] = std::min(right - i, p[mirror]);
        }

        // 中心扩展
        while (i - p[i] - 1 >= 0 && i + p[i] + 1 < n &&
               t[i - p[i] - 1] == t[i + p[i] + 1]) {
            ++p[i];
        }

        // 更新最右边界
        if (i + p[i] > right) {
            center = i;
            right = i + p[i];
        }

        // 更新最长回文
        if (p[i] > maxLen) {
            maxLen = p[i];
            maxCenter = i;
        }
    }

    // 从变换后的串定位到原串
    int start = (maxCenter - maxLen) / 2;
    return s.substr(start, maxLen);
}

/**
 * 题目4: 回文子串数目
 *
 * 解法: 中心扩展
 * 时间复杂度: O(n²)
 * 空间复杂度: O(1)
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
        expandAroundCenter(i, i);      // 奇数
        expandAroundCenter(i, i + 1);  // 偶数
    }

    return count;
}

/**
 * Manacher 解法
 * 时间复杂度: O(n)
 */
int countSubstringsManacher(std::string s) {
    std::string t = "#";
    for (char c : s) {
        t += c;
        t += '#';
    }

    int n = t.size();
    std::vector<int> p(n, 0);
    int center = 0, right = 0;

    for (int i = 0; i < n; ++i) {
        if (i < right) {
            p[i] = std::min(right - i, p[2 * center - i]);
        }
        while (i - p[i] - 1 >= 0 && i + p[i] + 1 < n &&
               t[i - p[i] - 1] == t[i + p[i] + 1]) {
            ++p[i];
        }
        if (i + p[i] > right) {
            center = i;
            right = i + p[i];
        }
    }

    int count = 0;
    for (int radius : p) {
        count += (radius + 1) / 2;  // 原串中以该点为中心的回文数
    }

    return count;
}

/**
 * 题目5: 最长回文子序列
 *
 * 解法: 动态规划
 * dp[i][j] = s[i..j] 的最长回文子序列长度
 *
 * 时间复杂度: O(n²)
 * 空间复杂度: O(n²)
 */
int longestPalindromeSubseq(std::string s) {
    int n = s.size();
    std::vector<std::vector<int>> dp(n, std::vector<int>(n, 0));

    // 单个字符的回文长度为 1
    for (int i = 0; i < n; ++i) {
        dp[i][i] = 1;
    }

    // 从小区间到大区间
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

/**
 * 空间优化版本
 * 空间复杂度: O(n)
 */
int longestPalindromeSubseqOptimized(std::string s) {
    int n = s.size();
    std::vector<int> dp(n, 1);

    for (int i = n - 2; i >= 0; --i) {
        int prev = 0;  // dp[i+1][j-1]
        for (int j = i + 1; j < n; ++j) {
            int temp = dp[j];
            if (s[i] == s[j]) {
                dp[j] = prev + 2;
            } else {
                dp[j] = std::max(dp[j], dp[j - 1]);
            }
            prev = temp;
        }
    }

    return dp[n - 1];
}

/**
 * 题目6: 分割回文串
 *
 * 解法: 回溯 + 预处理
 * 时间复杂度: O(n * 2^n)
 * 空间复杂度: O(n²)
 */
std::vector<std::vector<std::string>> partition(std::string s) {
    int n = s.size();

    // 预处理回文判断
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

/**
 * 题目7: 分割回文串 II
 *
 * 解法: 动态规划
 * 时间复杂度: O(n²)
 * 空间复杂度: O(n²)
 */
int minCut(std::string s) {
    int n = s.size();
    if (n <= 1) return 0;

    // 预处理回文判断
    std::vector<std::vector<bool>> isPalin(n, std::vector<bool>(n, false));
    for (int i = n - 1; i >= 0; --i) {
        for (int j = i; j < n; ++j) {
            if (s[i] == s[j] && (j - i <= 2 || isPalin[i + 1][j - 1])) {
                isPalin[i][j] = true;
            }
        }
    }

    // dp[i] = s[0..i] 的最少分割次数
    std::vector<int> dp(n, 0);
    for (int i = 0; i < n; ++i) {
        if (isPalin[0][i]) {
            dp[i] = 0;
        } else {
            dp[i] = i;  // 最多切 i 刀
            for (int j = 1; j <= i; ++j) {
                if (isPalin[j][i]) {
                    dp[i] = std::min(dp[i], dp[j - 1] + 1);
                }
            }
        }
    }

    return dp[n - 1];
}

/**
 * 题目8: 回文数
 *
 * 解法: 反转一半数字
 * 时间复杂度: O(log n)
 * 空间复杂度: O(1)
 */
bool isPalindromeNumber(int x) {
    // 负数和以 0 结尾的非零数都不是回文
    if (x < 0 || (x % 10 == 0 && x != 0)) {
        return false;
    }

    int reversed = 0;
    while (x > reversed) {
        reversed = reversed * 10 + x % 10;
        x /= 10;
    }

    // 偶数位：x == reversed
    // 奇数位：x == reversed / 10
    return x == reversed || x == reversed / 10;
}

/**
 * 题目9: 最短回文串
 *
 * 解法: KMP 算法
 * 构造 s + "#" + reverse(s)，求前缀函数
 *
 * 时间复杂度: O(n)
 * 空间复杂度: O(n)
 */
std::string shortestPalindrome(std::string s) {
    std::string rev = s;
    std::reverse(rev.begin(), rev.end());

    std::string combined = s + "#" + rev;
    int n = combined.size();

    // KMP 前缀函数
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

    // lps[n-1] 是 s 从开头开始的最长回文前缀长度
    int longestPalinPrefix = lps[n - 1];

    // 需要添加的部分
    std::string toAdd = s.substr(longestPalinPrefix);
    std::reverse(toAdd.begin(), toAdd.end());

    return toAdd + s;
}

/**
 * 题目10: 回文对
 *
 * 解法: 哈希表 + 枚举分割点
 * 时间复杂度: O(n * k²)，n 为单词数，k 为平均长度
 * 空间复杂度: O(n * k)
 */
std::vector<std::vector<int>> palindromePairs(std::vector<std::string>& words) {
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
        int m = word.size();

        for (int j = 0; j <= m; ++j) {
            std::string left = word.substr(0, j);
            std::string right = word.substr(j);

            // 情况1: left 是回文，right 的逆序在字典中
            if (isPalin(left, 0, left.size() - 1)) {
                if (wordIndex.count(right) && wordIndex[right] != i) {
                    result.push_back({wordIndex[right], i});
                }
            }

            // 情况2: right 是回文，left 的逆序在字典中
            // 避免重复：j > 0（当 j == 0 时，right 是整个单词，会在情况1处理）
            if (j > 0 && isPalin(right, 0, right.size() - 1)) {
                if (wordIndex.count(left) && wordIndex[left] != i) {
                    result.push_back({i, wordIndex[left]});
                }
            }
        }
    }

    return result;
}


// ==================== 测试代码 ====================
#include <cassert>

void testIsPalindrome() {
    assert(isPalindrome("A man, a plan, a canal: Panama") == true);
    assert(isPalindrome("race a car") == false);
    assert(isPalindrome(" ") == true);

    std::cout << "isPalindrome: PASSED\n";
}

void testValidPalindrome() {
    assert(validPalindrome("aba") == true);
    assert(validPalindrome("abca") == true);
    assert(validPalindrome("abc") == false);

    std::cout << "validPalindrome: PASSED\n";
}

void testLongestPalindrome() {
    std::string result = longestPalindrome("babad");
    assert(result == "bab" || result == "aba");

    assert(longestPalindrome("cbbd") == "bb");
    assert(longestPalindromeDP("cbbd") == "bb");
    assert(longestPalindromeManacher("cbbd") == "bb");

    std::cout << "longestPalindrome: PASSED\n";
}

void testCountSubstrings() {
    assert(countSubstrings("abc") == 3);
    assert(countSubstrings("aaa") == 6);
    assert(countSubstringsManacher("aaa") == 6);

    std::cout << "countSubstrings: PASSED\n";
}

void testLongestPalindromeSubseq() {
    assert(longestPalindromeSubseq("bbbab") == 4);
    assert(longestPalindromeSubseq("cbbd") == 2);
    assert(longestPalindromeSubseqOptimized("bbbab") == 4);

    std::cout << "longestPalindromeSubseq: PASSED\n";
}

void testPartition() {
    auto result = partition("aab");
    assert(result.size() == 2);

    std::cout << "partition: PASSED\n";
}

void testMinCut() {
    assert(minCut("aab") == 1);
    assert(minCut("a") == 0);
    assert(minCut("ab") == 1);

    std::cout << "minCut: PASSED\n";
}

void testIsPalindromeNumber() {
    assert(isPalindromeNumber(121) == true);
    assert(isPalindromeNumber(-121) == false);
    assert(isPalindromeNumber(10) == false);

    std::cout << "isPalindromeNumber: PASSED\n";
}

void testShortestPalindrome() {
    assert(shortestPalindrome("aacecaaa") == "aaacecaaa");
    assert(shortestPalindrome("abcd") == "dcbabcd");

    std::cout << "shortestPalindrome: PASSED\n";
}

void testPalindromePairs() {
    std::vector<std::string> words = {"abcd", "dcba", "lls", "s", "sssll"};
    auto result = palindromePairs(words);
    assert(result.size() == 4);

    std::cout << "palindromePairs: PASSED\n";
}

int main() {
    std::cout << "=== Palindrome Solution Tests ===\n";

    testIsPalindrome();
    testValidPalindrome();
    testLongestPalindrome();
    testCountSubstrings();
    testLongestPalindromeSubseq();
    testPartition();
    testMinCut();
    testIsPalindromeNumber();
    testShortestPalindrome();
    testPalindromePairs();

    std::cout << "\nAll tests passed!\n";
    return 0;
}
