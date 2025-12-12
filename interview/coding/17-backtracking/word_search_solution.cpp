/**
 * @file word_search_solution.cpp
 * @brief 单词搜索 - 解答
 */
#include <vector>
#include <string>
#include <iostream>
#include <unordered_set>
using namespace std;

// 单词搜索 I
class Solution79 {
public:
    bool exist(vector<vector<char>>& board, string word) {
        int m = board.size(), n = board[0].size();
        for (int i = 0; i < m; ++i) {
            for (int j = 0; j < n; ++j) {
                if (dfs(board, word, i, j, 0)) return true;
            }
        }
        return false;
    }

private:
    bool dfs(vector<vector<char>>& board, string& word, int i, int j, int k) {
        if (k == word.size()) return true;
        if (i < 0 || i >= board.size() || j < 0 || j >= board[0].size()) return false;
        if (board[i][j] != word[k]) return false;

        char temp = board[i][j];
        board[i][j] = '#';
        bool found = dfs(board, word, i + 1, j, k + 1) ||
                     dfs(board, word, i - 1, j, k + 1) ||
                     dfs(board, word, i, j + 1, k + 1) ||
                     dfs(board, word, i, j - 1, k + 1);
        board[i][j] = temp;
        return found;
    }
};

// 单词搜索 II (Trie 优化)
class Solution212 {
public:
    struct TrieNode {
        TrieNode* children[26] = {};
        string* word = nullptr;
    };

    vector<string> findWords(vector<vector<char>>& board, vector<string>& words) {
        TrieNode* root = new TrieNode();
        for (auto& w : words) {
            TrieNode* node = root;
            for (char c : w) {
                if (!node->children[c - 'a'])
                    node->children[c - 'a'] = new TrieNode();
                node = node->children[c - 'a'];
            }
            node->word = &w;
        }

        vector<string> result;
        int m = board.size(), n = board[0].size();
        for (int i = 0; i < m; ++i) {
            for (int j = 0; j < n; ++j) {
                dfs(board, i, j, root, result);
            }
        }
        return result;
    }

private:
    void dfs(vector<vector<char>>& board, int i, int j, TrieNode* node, vector<string>& result) {
        if (i < 0 || i >= board.size() || j < 0 || j >= board[0].size()) return;
        char c = board[i][j];
        if (c == '#' || !node->children[c - 'a']) return;

        node = node->children[c - 'a'];
        if (node->word) {
            result.push_back(*node->word);
            node->word = nullptr;
        }

        board[i][j] = '#';
        dfs(board, i + 1, j, node, result);
        dfs(board, i - 1, j, node, result);
        dfs(board, i, j + 1, node, result);
        dfs(board, i, j - 1, node, result);
        board[i][j] = c;
    }
};

int main() {
    vector<vector<char>> board = {
        {'o','a','a','n'},
        {'e','t','a','e'},
        {'i','h','k','r'},
        {'i','f','l','v'}
    };

    cout << "Word Search I:\n";
    cout << "exist 'eat': " << Solution79().exist(board, "eat") << "\n";
    cout << "exist 'oath': " << Solution79().exist(board, "oath") << "\n";

    vector<string> words = {"oath", "pea", "eat", "rain"};
    auto found = Solution212().findWords(board, words);
    cout << "\nWord Search II found: ";
    for (auto& w : found) cout << w << " ";
    cout << "\n";

    return 0;
}
