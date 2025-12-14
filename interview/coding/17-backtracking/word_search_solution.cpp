/**
 * @file word_search_solution.cpp
 * @brief 单词搜索 - 参考答案
 */
#include "word_search.h"
#include <iostream>
#include <cassert>

namespace WordSearch {

// ==================== 辅助函数 ====================
static bool dfsExist(std::vector<std::vector<char>>& board, const std::string& word,
                      int i, int j, int k) {
    if (k == static_cast<int>(word.size())) return true;
    if (i < 0 || i >= static_cast<int>(board.size()) ||
        j < 0 || j >= static_cast<int>(board[0].size())) return false;
    if (board[i][j] != word[k]) return false;

    char temp = board[i][j];
    board[i][j] = '#';
    bool found = dfsExist(board, word, i + 1, j, k + 1) ||
                 dfsExist(board, word, i - 1, j, k + 1) ||
                 dfsExist(board, word, i, j + 1, k + 1) ||
                 dfsExist(board, word, i, j - 1, k + 1);
    board[i][j] = temp;
    return found;
}

// Trie 节点
struct TrieNode {
    TrieNode* children[26] = {};
    std::string* word = nullptr;

    ~TrieNode() {
        for (auto child : children) {
            delete child;
        }
    }
};

static void dfsFindWords(std::vector<std::vector<char>>& board, int i, int j,
                          TrieNode* node, std::vector<std::string>& result) {
    if (i < 0 || i >= static_cast<int>(board.size()) ||
        j < 0 || j >= static_cast<int>(board[0].size())) return;

    char c = board[i][j];
    if (c == '#' || !node->children[c - 'a']) return;

    node = node->children[c - 'a'];
    if (node->word) {
        result.push_back(*node->word);
        node->word = nullptr;  // 避免重复添加
    }

    board[i][j] = '#';
    dfsFindWords(board, i + 1, j, node, result);
    dfsFindWords(board, i - 1, j, node, result);
    dfsFindWords(board, i, j + 1, node, result);
    dfsFindWords(board, i, j - 1, node, result);
    board[i][j] = c;
}

// ==================== 参考答案实现 ====================

bool existSolution(std::vector<std::vector<char>>& board, const std::string& word) {
    int m = static_cast<int>(board.size());
    int n = static_cast<int>(board[0].size());
    for (int i = 0; i < m; ++i) {
        for (int j = 0; j < n; ++j) {
            if (dfsExist(board, word, i, j, 0)) return true;
        }
    }
    return false;
}

std::vector<std::string> findWordsSolution(std::vector<std::vector<char>>& board,
                                            std::vector<std::string>& words) {
    TrieNode* root = new TrieNode();

    // 构建 Trie
    for (auto& w : words) {
        TrieNode* node = root;
        for (char c : w) {
            if (!node->children[c - 'a'])
                node->children[c - 'a'] = new TrieNode();
            node = node->children[c - 'a'];
        }
        node->word = &w;
    }

    std::vector<std::string> result;
    int m = static_cast<int>(board.size());
    int n = static_cast<int>(board[0].size());
    for (int i = 0; i < m; ++i) {
        for (int j = 0; j < n; ++j) {
            dfsFindWords(board, i, j, root, result);
        }
    }

    delete root;
    return result;
}

// ==================== 测试函数 ====================

void runTests() {
    std::cout << "\n=== Word Search Tests ===" << std::endl;

    // 测试单词搜索 I
    {
        std::vector<std::vector<char>> board = {
            {'A','B','C','E'},
            {'S','F','C','S'},
            {'A','D','E','E'}
        };
        assert(existSolution(board, "ABCCED") == true);
        assert(existSolution(board, "SEE") == true);
        assert(existSolution(board, "ABCB") == false);
        std::cout << "  [PASS] Word Search I" << std::endl;
    }

    // 测试单词搜索 II
    {
        std::vector<std::vector<char>> board = {
            {'o','a','a','n'},
            {'e','t','a','e'},
            {'i','h','k','r'},
            {'i','f','l','v'}
        };
        std::vector<std::string> words = {"oath", "pea", "eat", "rain"};
        auto found = findWordsSolution(board, words);
        assert(found.size() == 2);  // "oath" and "eat"
        std::cout << "  [PASS] Word Search II" << std::endl;
    }

    std::cout << "=== All Word Search Tests Passed! ===" << std::endl;
}

} // namespace WordSearch

/**
 * 关键要点：
 *
 * 1. 单词搜索 I：
 *    - DFS + 回溯
 *    - 标记已访问的单元格
 *    - 四个方向搜索
 *    - 恢复标记（回溯）
 *
 * 2. 单词搜索 II：
 *    - 使用 Trie 树优化
 *    - 一次 DFS 搜索多个单词
 *    - 剪枝：前缀不存在时提前返回
 *
 * 3. 标记技巧：
 *    - 使用特殊字符 '#' 标记已访问
 *    - 回溯时恢复原字符
 *    - 避免额外的 visited 数组
 */
