/**
 * @file trie.h
 * @brief 前缀树（字典树）问题 - 题目说明与声明
 *
 * Trie（前缀树/字典树）是一种树形数据结构，用于高效地存储和检索字符串数据集中的键。
 * 常见应用：自动补全、拼写检查、IP 路由、词频统计等。
 */

#ifndef TRIE_H
#define TRIE_H

#include <string>
#include <vector>

namespace TrieProblems {

// ==================== 题目声明 ====================

/**
 * 题目1: 实现 Trie（前缀树）
 *
 * 实现 Trie 类：
 *   - Trie() 初始化前缀树对象
 *   - void insert(String word) 向前缀树中插入字符串 word
 *   - boolean search(String word) 如果字符串 word 在前缀树中，返回 true；否则返回 false
 *   - boolean startsWith(String prefix) 如果之前插入的字符串 word 的前缀之一为 prefix，返回 true；否则返回 false
 *
 * 示例:
 *   Trie trie = new Trie();
 *   trie.insert("apple");
 *   trie.search("apple");   // 返回 true
 *   trie.search("app");     // 返回 false
 *   trie.startsWith("app"); // 返回 true
 *   trie.insert("app");
 *   trie.search("app");     // 返回 true
 */
class Trie {
public:
    Trie();
    ~Trie();
    void insert(const std::string& word);
    bool search(const std::string& word);
    bool startsWith(const std::string& prefix);
private:
    struct TrieNode;
    TrieNode* root;
};

class TrieSolution {
public:
    TrieSolution();
    ~TrieSolution();
    void insert(const std::string& word);
    bool search(const std::string& word);
    bool startsWith(const std::string& prefix);
private:
    struct TrieNode;
    TrieNode* root;
    TrieNode* searchPrefix(const std::string& prefix);
};

/**
 * 题目2: 添加与搜索单词（支持通配符）
 *
 * 设计一个数据结构，支持添加新单词和查找字符串是否与任何先前添加的字符串匹配。
 *
 * 实现 WordDictionary 类：
 *   - WordDictionary() 初始化对象
 *   - void addWord(word) 将 word 添加到数据结构中
 *   - bool search(word) 如果数据结构中存在与 word 匹配的字符串，返回 true；否则返回 false
 *     - word 中可能包含一些 '.'，每个 '.' 都可以表示任何一个字母
 *
 * 示例:
 *   wordDictionary.addWord("bad");
 *   wordDictionary.search(".ad"); // 返回 true
 *   wordDictionary.search("b.."); // 返回 true
 */
class WordDictionary {
public:
    WordDictionary();
    ~WordDictionary();
    void addWord(const std::string& word);
    bool search(const std::string& word);
private:
    struct TrieNode;
    TrieNode* root;
};

class WordDictionarySolution {
public:
    WordDictionarySolution();
    ~WordDictionarySolution();
    void addWord(const std::string& word);
    bool search(const std::string& word);
private:
    struct TrieNode;
    TrieNode* root;
    bool searchHelper(const std::string& word, int pos, TrieNode* node);
};

/**
 * 题目3: 单词搜索 II
 *
 * 给定一个 m x n 二维字符网格 board 和一个单���（字符串）列表 words，
 * 返回所有二维网格上的单词。
 *
 * 示例:
 *   输入: board = [["o","a","a","n"],["e","t","a","e"],["i","h","k","r"],["i","f","l","v"]]
 *         words = ["oath","pea","eat","rain"]
 *   输出: ["eat","oath"]
 *
 * 思路: Trie + DFS 回溯
 */
std::vector<std::string> findWords(std::vector<std::vector<char>>& board, std::vector<std::string>& words);
std::vector<std::string> findWordsSolution(std::vector<std::vector<char>>& board, std::vector<std::string>& words);

/**
 * 题目4: 键值映射
 *
 * 设计一个 map，满足以下几点:
 *   - 字符串表示键，整数表示值
 *   - 返回具有前缀等于给定字符串的键的值的总和
 *
 * 示例:
 *   mapSum.insert("apple", 3);
 *   mapSum.sum("ap");           // 返回 3
 *   mapSum.insert("app", 2);
 *   mapSum.sum("ap");           // 返回 5
 */
class MapSum {
public:
    MapSum();
    void insert(const std::string& key, int val);
    int sum(const std::string& prefix);
private:
    struct TrieNode;
    TrieNode* root;
};

class MapSumSolution {
public:
    MapSumSolution();
    void insert(const std::string& key, int val);
    int sum(const std::string& prefix);
private:
    struct TrieNode;
    TrieNode* root;
    std::vector<std::pair<std::string, int>> keyVal;
};

/**
 * 题目5: 最长单词
 *
 * 给出一个字符串数组 words 组成的英语词典。
 * 返回 words 中最长的一个单词，该单词是由 words 词典中其他单词逐步添加一个字母组成。
 *
 * 示例:
 *   输入: words = ["w","wo","wor","worl","world"]
 *   输出: "world"
 *
 *   输入: words = ["a","banana","app","appl","ap","apply","apple"]
 *   输出: "apple"
 */
std::string longestWord(std::vector<std::string>& words);
std::string longestWordSolution(std::vector<std::string>& words);

/**
 * 题目6: 单词替换
 *
 * 给定一个由许多词根组成的词典 dictionary 和一个句子 sentence。
 * 用句子中的继承词用词根替换掉。
 *
 * 示例:
 *   输入: dictionary = ["cat","bat","rat"], sentence = "the cattle was rattled by the battery"
 *   输出: "the cat was rat by the bat"
 */
std::string replaceWords(std::vector<std::string>& dictionary, const std::string& sentence);
std::string replaceWordsSolution(std::vector<std::string>& dictionary, const std::string& sentence);

/**
 * 题目7: 数组中两个数的最大异或值
 *
 * 给你一个整数数组 nums，返回 nums[i] XOR nums[j] 的最大运算结果。
 *
 * 示例:
 *   输入: nums = [3,10,5,25,2,8]
 *   输出: 28
 *   解释: 最大运算结果是 5 XOR 25 = 28
 *
 * 思路: 二进制 Trie
 */
int findMaximumXOR(std::vector<int>& nums);
int findMaximumXORSolution(std::vector<int>& nums);

/**
 * 题目8: 实现魔术字典
 *
 * 设计一个数据结构，判定能否只将一个单词中一个字母换成另一个字母，
 * 使得所形成的新单词存在于字典中。
 *
 * 示例:
 *   magicDictionary.buildDict(["hello", "leetcode"]);
 *   magicDictionary.search("hello"); // 返回 false
 *   magicDictionary.search("hhllo"); // 返回 true
 */
class MagicDictionary {
public:
    MagicDictionary();
    void buildDict(const std::vector<std::string>& dictionary);
    bool search(const std::string& searchWord);
private:
    struct TrieNode;
    TrieNode* root;
};

class MagicDictionarySolution {
public:
    MagicDictionarySolution();
    void buildDict(const std::vector<std::string>& dictionary);
    bool search(const std::string& searchWord);
private:
    struct TrieNode;
    TrieNode* root;
    bool dfs(const std::string& word, int pos, TrieNode* node, bool modified);
};

// ==================== 测试函数声明 ====================

void testTrie();          // 测试面试者实现
void testTrieSolution();  // 测试解答实现

// runTests 在头文件 inline 定义，调用两个测试函数
inline void runTests() {
    testTrie();
    testTrieSolution();
}

} // namespace TrieProblems

#endif // TRIE_H
