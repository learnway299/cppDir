/**
 * @file trie_solution.cpp
 * @brief 前缀树（字典树）问题 - 参考答案
 */

#include "trie.h"
#include <iostream>
#include <cassert>
#include <functional>
#include <sstream>
#include <unordered_map>

namespace TrieProblems {

// ==================== 参考答案实现 ====================

// TrieSolution 实现
struct TrieSolution::TrieNode {
    TrieNode* children[26] = {nullptr};
    bool isEnd = false;
};

TrieSolution::TrieSolution() {
    root = new TrieNode();
}

TrieSolution::~TrieSolution() {
    std::function<void(TrieNode*)> deleteNode = [&](TrieNode* node) {
        if (!node) return;
        for (int i = 0; i < 26; ++i) {
            deleteNode(node->children[i]);
        }
        delete node;
    };
    deleteNode(root);
}

void TrieSolution::insert(const std::string& word) {
    TrieNode* node = root;
    for (char c : word) {
        int idx = c - 'a';
        if (!node->children[idx]) {
            node->children[idx] = new TrieNode();
        }
        node = node->children[idx];
    }
    node->isEnd = true;
}

bool TrieSolution::search(const std::string& word) {
    TrieNode* node = searchPrefix(word);
    return node != nullptr && node->isEnd;
}

bool TrieSolution::startsWith(const std::string& prefix) {
    return searchPrefix(prefix) != nullptr;
}

TrieSolution::TrieNode* TrieSolution::searchPrefix(const std::string& prefix) {
    TrieNode* node = root;
    for (char c : prefix) {
        int idx = c - 'a';
        if (!node->children[idx]) {
            return nullptr;
        }
        node = node->children[idx];
    }
    return node;
}

// WordDictionarySolution 实现
struct WordDictionarySolution::TrieNode {
    TrieNode* children[26] = {nullptr};
    bool isEnd = false;
};

WordDictionarySolution::WordDictionarySolution() : root(new TrieNode()) {}

WordDictionarySolution::~WordDictionarySolution() {
    std::function<void(TrieNode*)> deleteNode = [&](TrieNode* node) {
        if (!node) return;
        for (int i = 0; i < 26; ++i) {
            deleteNode(node->children[i]);
        }
        delete node;
    };
    deleteNode(root);
}

void WordDictionarySolution::addWord(const std::string& word) {
    TrieNode* node = root;
    for (char c : word) {
        int idx = c - 'a';
        if (!node->children[idx]) {
            node->children[idx] = new TrieNode();
        }
        node = node->children[idx];
    }
    node->isEnd = true;
}

bool WordDictionarySolution::search(const std::string& word) {
    return searchHelper(word, 0, root);
}

bool WordDictionarySolution::searchHelper(const std::string& word, int pos, TrieNode* node) {
    if (pos == static_cast<int>(word.size())) {
        return node->isEnd;
    }

    char c = word[pos];
    if (c == '.') {
        for (int i = 0; i < 26; ++i) {
            if (node->children[i] && searchHelper(word, pos + 1, node->children[i])) {
                return true;
            }
        }
        return false;
    } else {
        int idx = c - 'a';
        if (!node->children[idx]) return false;
        return searchHelper(word, pos + 1, node->children[idx]);
    }
}

// findWordsSolution
std::vector<std::string> findWordsSolution(std::vector<std::vector<char>>& board, std::vector<std::string>& words) {
    struct TrieNode {
        TrieNode* children[26] = {nullptr};
        std::string word;
    };

    TrieNode* root = new TrieNode();
    std::vector<std::string> result;
    int rows = static_cast<int>(board.size());
    int cols = static_cast<int>(board[0].size());

    for (const std::string& word : words) {
        TrieNode* node = root;
        for (char c : word) {
            int idx = c - 'a';
            if (!node->children[idx]) {
                node->children[idx] = new TrieNode();
            }
            node = node->children[idx];
        }
        node->word = word;
    }

    std::function<void(int, int, TrieNode*)> dfs = [&](int i, int j, TrieNode* node) {
        if (i < 0 || i >= rows || j < 0 || j >= cols) return;

        char c = board[i][j];
        if (c == '#' || !node->children[c - 'a']) return;

        node = node->children[c - 'a'];

        if (!node->word.empty()) {
            result.push_back(node->word);
            node->word.clear();
        }

        board[i][j] = '#';
        dfs(i + 1, j, node);
        dfs(i - 1, j, node);
        dfs(i, j + 1, node);
        dfs(i, j - 1, node);
        board[i][j] = c;
    };

    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            dfs(i, j, root);
        }
    }

    return result;
}

// MapSumSolution 实现
struct MapSumSolution::TrieNode {
    TrieNode* children[26] = {nullptr};
    int val = 0;
};

MapSumSolution::MapSumSolution() : root(new TrieNode()) {}

void MapSumSolution::insert(const std::string& key, int val) {
    int delta = val;
    for (auto& [k, v] : keyVal) {
        if (k == key) {
            delta -= v;
            v = val;
            break;
        }
    }
    if (delta == val) {
        keyVal.push_back({key, val});
    }

    TrieNode* node = root;
    for (char c : key) {
        int idx = c - 'a';
        if (!node->children[idx]) {
            node->children[idx] = new TrieNode();
        }
        node = node->children[idx];
        node->val += delta;
    }
}

int MapSumSolution::sum(const std::string& prefix) {
    TrieNode* node = root;
    for (char c : prefix) {
        int idx = c - 'a';
        if (!node->children[idx]) return 0;
        node = node->children[idx];
    }
    return node->val;
}

// longestWordSolution
std::string longestWordSolution(std::vector<std::string>& words) {
    struct TrieNode {
        TrieNode* children[26] = {nullptr};
        bool isEnd = false;
    };

    TrieNode* root = new TrieNode();
    root->isEnd = true;

    for (const std::string& word : words) {
        TrieNode* node = root;
        for (char c : word) {
            int idx = c - 'a';
            if (!node->children[idx]) {
                node->children[idx] = new TrieNode();
            }
            node = node->children[idx];
        }
        node->isEnd = true;
    }

    std::string result;
    std::string current;

    std::function<void(TrieNode*)> dfs = [&](TrieNode* node) {
        if (!node || !node->isEnd) return;

        if (current.size() > result.size()) {
            result = current;
        }

        for (int i = 0; i < 26; ++i) {
            if (node->children[i] && node->children[i]->isEnd) {
                current += ('a' + i);
                dfs(node->children[i]);
                current.pop_back();
            }
        }
    };

    dfs(root);
    return result;
}

// replaceWordsSolution
std::string replaceWordsSolution(std::vector<std::string>& dictionary, const std::string& sentence) {
    struct TrieNode {
        TrieNode* children[26] = {nullptr};
        bool isEnd = false;
    };

    TrieNode* root = new TrieNode();

    for (const std::string& word : dictionary) {
        TrieNode* node = root;
        for (char c : word) {
            int idx = c - 'a';
            if (!node->children[idx]) {
                node->children[idx] = new TrieNode();
            }
            node = node->children[idx];
        }
        node->isEnd = true;
    }

    auto findRoot = [&](const std::string& word) -> std::string {
        TrieNode* node = root;
        for (int i = 0; i < static_cast<int>(word.size()); ++i) {
            int idx = word[i] - 'a';
            if (!node->children[idx]) {
                return word;
            }
            node = node->children[idx];
            if (node->isEnd) {
                return word.substr(0, i + 1);
            }
        }
        return word;
    };

    std::istringstream iss(sentence);
    std::string word, result;

    while (iss >> word) {
        if (!result.empty()) result += ' ';
        result += findRoot(word);
    }

    return result;
}

// findMaximumXORSolution
int findMaximumXORSolution(std::vector<int>& nums) {
    struct TrieNode {
        TrieNode* children[2] = {nullptr};
    };

    TrieNode* root = new TrieNode();

    auto insert = [&](int num) {
        TrieNode* node = root;
        for (int i = 31; i >= 0; --i) {
            int bit = (num >> i) & 1;
            if (!node->children[bit]) {
                node->children[bit] = new TrieNode();
            }
            node = node->children[bit];
        }
    };

    auto findMax = [&](int num) -> int {
        TrieNode* node = root;
        int maxXor = 0;
        for (int i = 31; i >= 0; --i) {
            int bit = (num >> i) & 1;
            int wanted = 1 - bit;

            if (node->children[wanted]) {
                maxXor |= (1 << i);
                node = node->children[wanted];
            } else {
                node = node->children[bit];
            }
        }
        return maxXor;
    };

    int maxResult = 0;
    for (int num : nums) {
        insert(num);
        maxResult = std::max(maxResult, findMax(num));
    }

    return maxResult;
}

// MagicDictionarySolution 实现
struct MagicDictionarySolution::TrieNode {
    TrieNode* children[26] = {nullptr};
    bool isEnd = false;
};

MagicDictionarySolution::MagicDictionarySolution() : root(new TrieNode()) {}

void MagicDictionarySolution::buildDict(const std::vector<std::string>& dictionary) {
    for (const std::string& word : dictionary) {
        TrieNode* node = root;
        for (char c : word) {
            int idx = c - 'a';
            if (!node->children[idx]) {
                node->children[idx] = new TrieNode();
            }
            node = node->children[idx];
        }
        node->isEnd = true;
    }
}

bool MagicDictionarySolution::search(const std::string& searchWord) {
    return dfs(searchWord, 0, root, false);
}

bool MagicDictionarySolution::dfs(const std::string& word, int pos, TrieNode* node, bool modified) {
    if (pos == static_cast<int>(word.size())) {
        return modified && node->isEnd;
    }

    int idx = word[pos] - 'a';

    if (node->children[idx]) {
        if (dfs(word, pos + 1, node->children[idx], modified)) {
            return true;
        }
    }

    if (!modified) {
        for (int i = 0; i < 26; ++i) {
            if (i != idx && node->children[i]) {
                if (dfs(word, pos + 1, node->children[i], true)) {
                    return true;
                }
            }
        }
    }

    return false;
}

// ==================== 测试函数 ====================

void testTrieSolution() {
    std::cout << "=== Trie Tests (Solution) ===\n";

    // TrieSolution
    TrieSolution trie;
    trie.insert("apple");
    assert(trie.search("apple") == true);
    assert(trie.search("app") == false);
    assert(trie.startsWith("app") == true);
    trie.insert("app");
    assert(trie.search("app") == true);
    std::cout << "  TrieSolution: PASSED\n";

    // WordDictionarySolution
    WordDictionarySolution wordDictionary;
    wordDictionary.addWord("bad");
    wordDictionary.addWord("dad");
    wordDictionary.addWord("mad");
    assert(wordDictionary.search("pad") == false);
    assert(wordDictionary.search("bad") == true);
    assert(wordDictionary.search(".ad") == true);
    assert(wordDictionary.search("b..") == true);
    std::cout << "  WordDictionarySolution: PASSED\n";

    // findWordsSolution
    std::vector<std::vector<char>> board = {
        {'o','a','a','n'},
        {'e','t','a','e'},
        {'i','h','k','r'},
        {'i','f','l','v'}
    };
    std::vector<std::string> words = {"oath", "pea", "eat", "rain"};
    auto findResult = findWordsSolution(board, words);
    assert(findResult.size() == 2);
    std::cout << "  findWordsSolution: PASSED\n";

    // MapSumSolution
    MapSumSolution mapSum;
    mapSum.insert("apple", 3);
    assert(mapSum.sum("ap") == 3);
    mapSum.insert("app", 2);
    assert(mapSum.sum("ap") == 5);
    std::cout << "  MapSumSolution: PASSED\n";

    // longestWordSolution
    std::vector<std::string> words1 = {"w", "wo", "wor", "worl", "world"};
    assert(longestWordSolution(words1) == "world");
    std::vector<std::string> words2 = {"a", "banana", "app", "appl", "ap", "apply", "apple"};
    assert(longestWordSolution(words2) == "apple");
    std::cout << "  longestWordSolution: PASSED\n";

    // replaceWordsSolution
    std::vector<std::string> dict = {"cat", "bat", "rat"};
    std::string sentence = "the cattle was rattled by the battery";
    assert(replaceWordsSolution(dict, sentence) == "the cat was rat by the bat");
    std::cout << "  replaceWordsSolution: PASSED\n";

    // findMaximumXORSolution
    std::vector<int> nums = {3, 10, 5, 25, 2, 8};
    assert(findMaximumXORSolution(nums) == 28);
    std::cout << "  findMaximumXORSolution: PASSED\n";

    // MagicDictionarySolution
    MagicDictionarySolution magicDictionary;
    magicDictionary.buildDict({"hello", "leetcode"});
    assert(magicDictionary.search("hello") == false);
    assert(magicDictionary.search("hhllo") == true);
    assert(magicDictionary.search("hell") == false);
    assert(magicDictionary.search("leetcoded") == false);
    std::cout << "  MagicDictionarySolution: PASSED\n";
}

} // namespace TrieProblems
