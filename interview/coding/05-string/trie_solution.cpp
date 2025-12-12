/**
 * @file trie_solution.cpp
 * @brief 前缀树（字典树）问题 - 解答文件
 *
 * Trie 特点：
 * - 根节点不包含字符
 * - 每个节点包含多个子节点（26 个字母或更多）
 * - 从根到某节点的路径表示一个前缀
 * - 通常用 isEnd 标记单词结尾
 */

#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <functional>
#include <sstream>

/**
 * 题目1: 实现 Trie（前缀树）
 *
 * 时间复杂度: insert/search/startsWith 均为 O(m)，m 为字符串长度
 * 空间复杂度: O(总字符数 * 26)
 */
class Trie {
private:
    struct TrieNode {
        TrieNode* children[26] = {nullptr};
        bool isEnd = false;
    };

    TrieNode* root;

public:
    Trie() {
        root = new TrieNode();
    }

    ~Trie() {
        // 递归删除节点（实际应用中需要）
        std::function<void(TrieNode*)> deleteNode = [&](TrieNode* node) {
            if (!node) return;
            for (int i = 0; i < 26; ++i) {
                deleteNode(node->children[i]);
            }
            delete node;
        };
        deleteNode(root);
    }

    void insert(std::string word) {
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

    bool search(std::string word) {
        TrieNode* node = searchPrefix(word);
        return node != nullptr && node->isEnd;
    }

    bool startsWith(std::string prefix) {
        return searchPrefix(prefix) != nullptr;
    }

private:
    TrieNode* searchPrefix(const std::string& prefix) {
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
};

/**
 * 使用 unordered_map 的版本（支持更大字符集）
 */
class TrieMap {
private:
    struct TrieNode {
        std::unordered_map<char, TrieNode*> children;
        bool isEnd = false;
    };

    TrieNode* root;

public:
    TrieMap() : root(new TrieNode()) {}

    void insert(const std::string& word) {
        TrieNode* node = root;
        for (char c : word) {
            if (!node->children.count(c)) {
                node->children[c] = new TrieNode();
            }
            node = node->children[c];
        }
        node->isEnd = true;
    }

    bool search(const std::string& word) {
        TrieNode* node = searchPrefix(word);
        return node && node->isEnd;
    }

    bool startsWith(const std::string& prefix) {
        return searchPrefix(prefix) != nullptr;
    }

private:
    TrieNode* searchPrefix(const std::string& prefix) {
        TrieNode* node = root;
        for (char c : prefix) {
            if (!node->children.count(c)) return nullptr;
            node = node->children[c];
        }
        return node;
    }
};

/**
 * 题目2: 添加与搜索单词（支持通配符）
 *
 * 解法: Trie + DFS 处理通配符
 */
class WordDictionary {
private:
    struct TrieNode {
        TrieNode* children[26] = {nullptr};
        bool isEnd = false;
    };

    TrieNode* root;

public:
    WordDictionary() : root(new TrieNode()) {}

    void addWord(std::string word) {
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

    bool search(std::string word) {
        return searchHelper(word, 0, root);
    }

private:
    bool searchHelper(const std::string& word, int pos, TrieNode* node) {
        if (pos == static_cast<int>(word.size())) {
            return node->isEnd;
        }

        char c = word[pos];
        if (c == '.') {
            // 通配符：尝试所有可能的子节点
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
};

/**
 * 题目3: 单词搜索 II
 *
 * 解法: Trie + DFS 回溯
 * 时间复杂度: O(m * n * 4^L)，L 为单词平均长度
 * 空间复杂度: O(总单词长度)
 */
class Solution3 {
private:
    struct TrieNode {
        TrieNode* children[26] = {nullptr};
        std::string word;  // 存储完整单词，方便获取结果
    };

    TrieNode* root;
    std::vector<std::string> result;
    int rows, cols;

public:
    std::vector<std::string> findWords(std::vector<std::vector<char>>& board, std::vector<std::string>& words) {
        root = new TrieNode();
        result.clear();
        rows = board.size();
        cols = board[0].size();

        // 构建 Trie
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

        // DFS 搜索
        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < cols; ++j) {
                dfs(board, i, j, root);
            }
        }

        return result;
    }

private:
    void dfs(std::vector<std::vector<char>>& board, int i, int j, TrieNode* node) {
        if (i < 0 || i >= rows || j < 0 || j >= cols) return;

        char c = board[i][j];
        if (c == '#' || !node->children[c - 'a']) return;

        node = node->children[c - 'a'];

        // 找到一个单词
        if (!node->word.empty()) {
            result.push_back(node->word);
            node->word.clear();  // 避免重复添加
        }

        // 标记访问
        board[i][j] = '#';

        // 四个方向搜索
        dfs(board, i + 1, j, node);
        dfs(board, i - 1, j, node);
        dfs(board, i, j + 1, node);
        dfs(board, i, j - 1, node);

        // 恢复
        board[i][j] = c;
    }
};

std::vector<std::string> findWords(std::vector<std::vector<char>>& board, std::vector<std::string>& words) {
    Solution3 sol;
    return sol.findWords(board, words);
}

/**
 * 题目4: 键值映射
 *
 * 解法: Trie + DFS 求和
 */
class MapSum {
private:
    struct TrieNode {
        TrieNode* children[26] = {nullptr};
        int val = 0;
    };

    TrieNode* root;
    std::unordered_map<std::string, int> keyVal;  // 存储原值，用于更新

public:
    MapSum() : root(new TrieNode()) {}

    void insert(std::string key, int val) {
        int delta = val;
        if (keyVal.count(key)) {
            delta -= keyVal[key];
        }
        keyVal[key] = val;

        TrieNode* node = root;
        for (char c : key) {
            int idx = c - 'a';
            if (!node->children[idx]) {
                node->children[idx] = new TrieNode();
            }
            node = node->children[idx];
            node->val += delta;  // 更新路径上所有节点的值
        }
    }

    int sum(std::string prefix) {
        TrieNode* node = root;
        for (char c : prefix) {
            int idx = c - 'a';
            if (!node->children[idx]) return 0;
            node = node->children[idx];
        }
        return node->val;
    }
};

/**
 * 题目5: 回文对
 *
 * 解法: Trie 存储逆序单词
 * 时间复杂度: O(n * k²)
 * 空间复杂度: O(n * k)
 */
class Solution5 {
private:
    struct TrieNode {
        TrieNode* children[26] = {nullptr};
        int wordIdx = -1;
        std::vector<int> palindromeSuffixes;  // 后缀是回文的单词索引
    };

    TrieNode* root;

    bool isPalindrome(const std::string& s, int left, int right) {
        while (left < right) {
            if (s[left] != s[right]) return false;
            ++left;
            --right;
        }
        return true;
    }

public:
    std::vector<std::vector<int>> palindromePairs(std::vector<std::string>& words) {
        root = new TrieNode();
        std::vector<std::vector<int>> result;

        // 构建逆序 Trie
        for (int i = 0; i < static_cast<int>(words.size()); ++i) {
            TrieNode* node = root;
            const std::string& word = words[i];

            for (int j = word.size() - 1; j >= 0; --j) {
                // 如果 word[0..j] 是回文，记录下来
                if (isPalindrome(word, 0, j)) {
                    node->palindromeSuffixes.push_back(i);
                }

                int idx = word[j] - 'a';
                if (!node->children[idx]) {
                    node->children[idx] = new TrieNode();
                }
                node = node->children[idx];
            }
            node->palindromeSuffixes.push_back(i);  // 空后缀也是回文
            node->wordIdx = i;
        }

        // 搜索
        for (int i = 0; i < static_cast<int>(words.size()); ++i) {
            TrieNode* node = root;
            const std::string& word = words[i];

            for (int j = 0; j < static_cast<int>(word.size()); ++j) {
                // 如果在 Trie 中找到一个完整单词，且剩余部分是回文
                if (node->wordIdx != -1 && node->wordIdx != i &&
                    isPalindrome(word, j, word.size() - 1)) {
                    result.push_back({i, node->wordIdx});
                }

                int idx = word[j] - 'a';
                if (!node->children[idx]) {
                    node = nullptr;
                    break;
                }
                node = node->children[idx];
            }

            // 当前单词完全匹配后，查找 Trie 中后缀是回文的单词
            if (node) {
                for (int idx : node->palindromeSuffixes) {
                    if (idx != i) {
                        result.push_back({i, idx});
                    }
                }
            }
        }

        return result;
    }
};

std::vector<std::vector<int>> palindromePairs(std::vector<std::string>& words) {
    Solution5 sol;
    return sol.palindromePairs(words);
}

/**
 * 题目6: 最长单词
 *
 * 解法: Trie + DFS
 * 时间复杂度: O(总字符数)
 * 空间复杂度: O(总字符数)
 */
std::string longestWord(std::vector<std::string>& words) {
    // 构建 Trie
    struct TrieNode {
        TrieNode* children[26] = {nullptr};
        bool isEnd = false;
    };

    TrieNode* root = new TrieNode();
    root->isEnd = true;  // 根节点视为有效

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

    // DFS 找最长单词
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

/**
 * 题目7: 单词替换
 *
 * 解法: Trie 查找最短前缀
 * 时间复杂度: O(d + s)，d 为词典总长度，s 为句子总长度
 * 空间复杂度: O(d)
 */
std::string replaceWords(std::vector<std::string>& dictionary, std::string sentence) {
    struct TrieNode {
        TrieNode* children[26] = {nullptr};
        bool isEnd = false;
    };

    TrieNode* root = new TrieNode();

    // 构建 Trie
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

    // 查找最短词根
    auto findRoot = [&](const std::string& word) -> std::string {
        TrieNode* node = root;
        for (int i = 0; i < static_cast<int>(word.size()); ++i) {
            int idx = word[i] - 'a';
            if (!node->children[idx]) {
                return word;  // 没有匹配的词根
            }
            node = node->children[idx];
            if (node->isEnd) {
                return word.substr(0, i + 1);  // 找到最短词根
            }
        }
        return word;
    };

    // 处理句子
    std::istringstream iss(sentence);
    std::string word, result;

    while (iss >> word) {
        if (!result.empty()) result += ' ';
        result += findRoot(word);
    }

    return result;
}

/**
 * 题目8: 数组中两个数的最大异或值
 *
 * 解法: 二进制 Trie
 * 时间复杂度: O(n * L)，L = 32
 * 空间复杂度: O(n * L)
 */
int findMaximumXOR(std::vector<int>& nums) {
    struct TrieNode {
        TrieNode* children[2] = {nullptr};
    };

    TrieNode* root = new TrieNode();

    // 插入数字
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

    // 查找与 num 异或最大的数
    auto findMax = [&](int num) -> int {
        TrieNode* node = root;
        int maxXor = 0;
        for (int i = 31; i >= 0; --i) {
            int bit = (num >> i) & 1;
            int wanted = 1 - bit;  // 想要相反的位

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

/**
 * 题目9: 实现魔术字典
 *
 * 解法: Trie + DFS（允许恰好一次不匹配）
 */
class MagicDictionary {
private:
    struct TrieNode {
        TrieNode* children[26] = {nullptr};
        bool isEnd = false;
    };

    TrieNode* root;

public:
    MagicDictionary() : root(new TrieNode()) {}

    void buildDict(std::vector<std::string> dictionary) {
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

    bool search(std::string searchWord) {
        return dfs(searchWord, 0, root, false);
    }

private:
    bool dfs(const std::string& word, int pos, TrieNode* node, bool modified) {
        if (pos == static_cast<int>(word.size())) {
            return modified && node->isEnd;
        }

        int idx = word[pos] - 'a';

        // 尝试不修改
        if (node->children[idx]) {
            if (dfs(word, pos + 1, node->children[idx], modified)) {
                return true;
            }
        }

        // 尝试修改（如果还没修改过）
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
};


// ==================== 测试代码 ====================
#include <cassert>

void testTrie() {
    Trie trie;
    trie.insert("apple");
    assert(trie.search("apple") == true);
    assert(trie.search("app") == false);
    assert(trie.startsWith("app") == true);
    trie.insert("app");
    assert(trie.search("app") == true);

    std::cout << "Trie: PASSED\n";
}

void testWordDictionary() {
    WordDictionary wordDictionary;
    wordDictionary.addWord("bad");
    wordDictionary.addWord("dad");
    wordDictionary.addWord("mad");
    assert(wordDictionary.search("pad") == false);
    assert(wordDictionary.search("bad") == true);
    assert(wordDictionary.search(".ad") == true);
    assert(wordDictionary.search("b..") == true);

    std::cout << "WordDictionary: PASSED\n";
}

void testFindWords() {
    std::vector<std::vector<char>> board = {
        {'o','a','a','n'},
        {'e','t','a','e'},
        {'i','h','k','r'},
        {'i','f','l','v'}
    };
    std::vector<std::string> words = {"oath", "pea", "eat", "rain"};
    auto result = findWords(board, words);
    assert(result.size() == 2);

    std::cout << "findWords: PASSED\n";
}

void testMapSum() {
    MapSum mapSum;
    mapSum.insert("apple", 3);
    assert(mapSum.sum("ap") == 3);
    mapSum.insert("app", 2);
    assert(mapSum.sum("ap") == 5);

    std::cout << "MapSum: PASSED\n";
}

void testLongestWord() {
    std::vector<std::string> words1 = {"w", "wo", "wor", "worl", "world"};
    assert(longestWord(words1) == "world");

    std::vector<std::string> words2 = {"a", "banana", "app", "appl", "ap", "apply", "apple"};
    assert(longestWord(words2) == "apple");

    std::cout << "longestWord: PASSED\n";
}

void testReplaceWords() {
    std::vector<std::string> dict = {"cat", "bat", "rat"};
    std::string sentence = "the cattle was rattled by the battery";
    assert(replaceWords(dict, sentence) == "the cat was rat by the bat");

    std::cout << "replaceWords: PASSED\n";
}

void testFindMaximumXOR() {
    std::vector<int> nums = {3, 10, 5, 25, 2, 8};
    assert(findMaximumXOR(nums) == 28);

    std::cout << "findMaximumXOR: PASSED\n";
}

void testMagicDictionary() {
    MagicDictionary magicDictionary;
    magicDictionary.buildDict({"hello", "leetcode"});
    assert(magicDictionary.search("hello") == false);
    assert(magicDictionary.search("hhllo") == true);
    assert(magicDictionary.search("hell") == false);
    assert(magicDictionary.search("leetcoded") == false);

    std::cout << "MagicDictionary: PASSED\n";
}

int main() {
    std::cout << "=== Trie Solution Tests ===\n";

    testTrie();
    testWordDictionary();
    testFindWords();
    testMapSum();
    testLongestWord();
    testReplaceWords();
    testFindMaximumXOR();
    testMagicDictionary();

    std::cout << "\nAll tests passed!\n";
    return 0;
}
