/**
 * @file palindrome_partition_solution.cpp
 * @brief 分割回文串 - 解答
 */
#include <vector>
#include <string>
#include <iostream>
using namespace std;

// 分割回文串 I
class Solution131 {
public:
    vector<vector<string>> partition(string s) {
        int n = s.size();
        isPalin.assign(n, vector<bool>(n, false));
        for (int i = n - 1; i >= 0; --i) {
            for (int j = i; j < n; ++j) {
                isPalin[i][j] = (s[i] == s[j]) && (j - i < 2 || isPalin[i + 1][j - 1]);
            }
        }

        vector<vector<string>> result;
        vector<string> path;
        backtrack(s, 0, path, result);
        return result;
    }

private:
    vector<vector<bool>> isPalin;

    void backtrack(string& s, int start, vector<string>& path, vector<vector<string>>& result) {
        if (start == s.size()) {
            result.push_back(path);
            return;
        }
        for (int i = start; i < s.size(); ++i) {
            if (isPalin[start][i]) {
                path.push_back(s.substr(start, i - start + 1));
                backtrack(s, i + 1, path, result);
                path.pop_back();
            }
        }
    }
};

// 分割回文串 II - 最少分割次数
class Solution132 {
public:
    int minCut(string s) {
        int n = s.size();
        vector<vector<bool>> isPalin(n, vector<bool>(n, false));
        for (int i = n - 1; i >= 0; --i) {
            for (int j = i; j < n; ++j) {
                isPalin[i][j] = (s[i] == s[j]) && (j - i < 2 || isPalin[i + 1][j - 1]);
            }
        }

        vector<int> dp(n, INT_MAX);
        for (int i = 0; i < n; ++i) {
            if (isPalin[0][i]) {
                dp[i] = 0;
            } else {
                for (int j = 0; j < i; ++j) {
                    if (isPalin[j + 1][i]) {
                        dp[i] = min(dp[i], dp[j] + 1);
                    }
                }
            }
        }
        return dp[n - 1];
    }
};

int main() {
    string s = "aab";

    cout << "Partition '" << s << "':\n";
    auto result = Solution131().partition(s);
    for (auto& part : result) {
        cout << "[";
        for (int i = 0; i < part.size(); ++i) {
            cout << "\"" << part[i] << "\"";
            if (i < part.size() - 1) cout << ", ";
        }
        cout << "]\n";
    }

    cout << "\nMin cut for 'aab': " << Solution132().minCut("aab") << "\n";
    cout << "Min cut for 'ab': " << Solution132().minCut("ab") << "\n";

    return 0;
}
