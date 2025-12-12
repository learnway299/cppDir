/**
 * @file trie.cpp
 * @brief 前缀树（字典树）问题 - 题目文件
 *
 * Trie（前缀树/字典树）是一种树形数据结构，用于高效地存储和检索字符串数据集中的键。
 * 常见应用：自动补全、拼写检查、IP 路由、词频统计等。
 */

#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>

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
    Trie() {
        // TODO: 初始化
    }

    void insert(std::string word) {
        // TODO: 实现插入
    }

    bool search(std::string word) {
        // TODO: 实现搜索
        return false;
    }

    bool startsWith(std::string prefix) {
        // TODO: 实现前缀搜索
        return false;
    }
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
 *   WordDictionary wordDictionary = new WordDictionary();
 *   wordDictionary.addWord("bad");
 *   wordDictionary.addWord("dad");
 *   wordDictionary.addWord("mad");
 *   wordDictionary.search("pad"); // 返回 false
 *   wordDictionary.search("bad"); // 返回 true
 *   wordDictionary.search(".ad"); // 返回 true
 *   wordDictionary.search("b.."); // 返回 true
 */
class WordDictionary {
public:
    WordDictionary() {
        // TODO: 初始化
    }

    void addWord(std::string word) {
        // TODO: 实现添加
    }

    bool search(std::string word) {
        // TODO: 实现搜索（支持 '.' 通配符）
        return false;
    }
};

/**
 * 题目3: 单词搜索 II
 *
 * 给定一个 m x n 二维字符网格 board 和一个单词（字符串）列表 words，
 * 返回所有二维网格上的单词。
 *
 * 单词必须按照字母顺序，通过相邻的单元格内的字母构成，
 * 其中"相邻"单元格是那些水平相邻或垂直相邻的单元格。
 * 同一个单元格内的字母在一个单词中不允许被重复使用。
 *
 * 示例:
 *   输入: board = [["o","a","a","n"],["e","t","a","e"],["i","h","k","r"],["i","f","l","v"]]
 *         words = ["oath","pea","eat","rain"]
 *   输出: ["eat","oath"]
 *
 * 思路: Trie + DFS 回溯
 *
 * @param board 字符网格
 * @param words 单词列表
 * @return 网格中存在的单词
 */
std::vector<std::string> findWords(std::vector<std::vector<char>>& board, std::vector<std::string>& words) {
    // TODO: 实现你的解法
    return {};
}

/**
 * 题目4: 键值映射
 *
 * 设计一个 map，满足以下几点:
 *   - 字符串表示键，整数表示值
 *   - 返回具有前缀等于给定字符串的键的值的总和
 *
 * 实现 MapSum 类：
 *   - MapSum() 初始化对象
 *   - void insert(String key, int val) 插入 key-val 键值对
 *     - 如果 key 已存在，则用新的 val 替换原来的值
 *   - int sum(String prefix) 返回所有以 prefix 为前缀的键的值的总和
 *
 * 示例:
 *   MapSum mapSum = new MapSum();
 *   mapSum.insert("apple", 3);
 *   mapSum.sum("ap");           // 返回 3
 *   mapSum.insert("app", 2);
 *   mapSum.sum("ap");           // 返回 5
 */
class MapSum {
public:
    MapSum() {
        // TODO: 初始化
    }

    void insert(std::string key, int val) {
        // TODO: 实现插入
    }

    int sum(std::string prefix) {
        // TODO: 实现前缀求和
        return 0;
    }
};

/**
 * 题目5: 回文对
 *
 * 给定一组唯一的单词，找出所有不同的索引对 (i, j)，
 * 使得两个单词 words[i] + words[j] 拼接后是回文串。
 *
 * 示例:
 *   输入: words = ["abcd","dcba","lls","s","sssll"]
 *   输出: [[0,1],[1,0],[3,2],[2,4]]
 *
 * 思路: Trie 存储逆序单词
 *
 * @param words 单词数组
 * @return 所有回文对的索引
 */
std::vector<std::vector<int>> palindromePairs(std::vector<std::string>& words) {
    // TODO: 实现你的解法
    return {};
}

/**
 * 题目6: 最长单词
 *
 * 给出一个字符串数组 words 组成的英语词典。
 * 返回 words 中最长的一个单词，该单词是由 words 词典中其他单词逐步添加一个字母组成。
 * 若其中有多个可行的答案，则返回答案中字典序最小的单词。
 * 若无答案，则返回空字符串。
 *
 * 示例:
 *   输入: words = ["w","wo","wor","worl","world"]
 *   输出: "world"
 *
 *   输入: words = ["a","banana","app","appl","ap","apply","apple"]
 *   输出: "apple"
 *
 * @param words 单词数组
 * @return 最长单词
 */
std::string longestWord(std::vector<std::string>& words) {
    // TODO: 实现你的解法
    return "";
}

/**
 * 题目7: 单词替换
 *
 * 在英语中，有一个叫做 词根 (root) 的概念，它可以跟着其他一些词组成另一个较长的单词
 * 我们称这个较长的单词为 继承词 (successor)。
 *
 * 给定一个由许多词根组成的词典 dictionary 和一个句子 sentence。
 * 用句子中的继承词用词根替换掉。如果继承词有许多可以形成它的词根，则用最短的词根替换它。
 *
 * 示例:
 *   输入: dictionary = ["cat","bat","rat"], sentence = "the cattle was rattled by the battery"
 *   输出: "the cat was rat by the bat"
 *
 *   输入: dictionary = ["a","b","c"], sentence = "aadsfasf absbs bbab cadsfabd"
 *   输出: "a]a b c"
 *
 * @param dictionary 词根字典
 * @param sentence 句子
 * @return 替换后的句子
 */
std::string replaceWords(std::vector<std::string>& dictionary, std::string sentence) {
    // TODO: 实现你的解法
    return "";
}

/**
 * 题目8: 数组中两个数的最大异或值
 *
 * 给你一个整数数组 nums，返回 nums[i] XOR nums[j] 的最大运算结果，其中 0 ≤ i ≤ j < n。
 *
 * 示例:
 *   输入: nums = [3,10,5,25,2,8]
 *   输出: 28
 *   解释: 最大运算结果是 5 XOR 25 = 28
 *
 *   输入: nums = [14,70,53,83,49,91,36,80,92,51,66,70]
 *   输出: 127
 *
 * 思路: 二进制 Trie
 *
 * @param nums 整数数组
 * @return 最大异或值
 */
int findMaximumXOR(std::vector<int>& nums) {
    // TODO: 实现你的解法
    return 0;
}

/**
 * 题目9: 实现魔术字典
 *
 * 设计一个使用单词列表进行初始化的数据结构，单词列表中的单词互不相同。
 * 如果给出一个单词，请判定能否只将这个单词中一个字母换成另一个字母，
 * 使得所形成的新单词存在于你构建的字典中。
 *
 * 实现 MagicDictionary 类：
 *   - MagicDictionary() 初始化对象
 *   - void buildDict(String[] dictionary) 使用字符串数组 dictionary 设定该数据结构
 *   - bool search(String searchWord) 给定一个字符串 searchWord，判定能否只改变一个字符使其匹配
 *
 * 示例:
 *   MagicDictionary magicDictionary = new MagicDictionary();
 *   magicDictionary.buildDict(["hello", "leetcode"]);
 *   magicDictionary.search("hello"); // 返回 false
 *   magicDictionary.search("hhllo"); // 返回 true
 *   magicDictionary.search("hell");  // 返回 false
 *   magicDictionary.search("leetcoded"); // 返回 false
 */
class MagicDictionary {
public:
    MagicDictionary() {
        // TODO: 初始化
    }

    void buildDict(std::vector<std::string> dictionary) {
        // TODO: 构建字典
    }

    bool search(std::string searchWord) {
        // TODO: 搜索（恰好改变一个字符）
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
    // 结果应包含 "eat" 和 "oath"
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
    std::cout << "=== Trie Tests ===\n";

    // 取消注释以运行测试（实现解法后）
    // testTrie();
    // testWordDictionary();
    // testFindWords();
    // testMapSum();
    // testLongestWord();
    // testReplaceWords();
    // testFindMaximumXOR();
    // testMagicDictionary();

    std::cout << "All tests passed!\n";
    return 0;
}
