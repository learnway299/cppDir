/**
 * @file hashmap_impl.h
 * @brief 哈希表实现与应用 - 题目声明
 *
 * 哈希表是一种高效的数据结构，提供 O(1) 平均时间的查找、插入和删除。
 * 关键点：哈希函数设计、冲突解决（链地址法、开放定址法）
 */

#ifndef HASHMAP_IMPL_H
#define HASHMAP_IMPL_H

#include <vector>
#include <string>

namespace HashMapImplProblems {

/**
 * 题目1: 设计哈希集合
 *
 * 不使用任何内建的哈希表库设计一个哈希集合（HashSet）。
 *
 * 实现 MyHashSet 类：
 *   - void add(key) 向哈希集合中插入值 key
 *   - bool contains(key) 返回哈希集合中是否存在这个值 key
 *   - void remove(key) 将给定值 key 从哈希集合中删除。如果哈希集合中没有这个值，什么也不做
 *
 * 示例:
 *   MyHashSet myHashSet = new MyHashSet();
 *   myHashSet.add(1);
 *   myHashSet.add(2);
 *   myHashSet.contains(1); // 返回 true
 *   myHashSet.contains(3); // 返回 false
 *   myHashSet.add(2);
 *   myHashSet.contains(2); // 返回 true
 *   myHashSet.remove(2);
 *   myHashSet.contains(2); // 返回 false
 */
class MyHashSet {
public:
    MyHashSet();
    void add(int key);
    void remove(int key);
    bool contains(int key);
};

/**
 * 题目2: 设计哈希映射
 *
 * 不使用任何内建的哈希表库设计一个哈希映射（HashMap）。
 *
 * 实现 MyHashMap 类：
 *   - MyHashMap() 初始化对象
 *   - void put(int key, int value) 向 HashMap 插入一个键值对 (key, value)
 *     如果 key 已存在，则更新其对应的值 value
 *   - int get(int key) 返回特定的 key 所映射的 value；
 *     如果映射中不包含 key 的映射，返回 -1
 *   - void remove(key) 如果映射中存在 key 的映射，则移除 key 和它所对应的 value
 *
 * 示例:
 *   MyHashMap myHashMap = new MyHashMap();
 *   myHashMap.put(1, 1);
 *   myHashMap.put(2, 2);
 *   myHashMap.get(1);    // 返回 1
 *   myHashMap.get(3);    // 返回 -1
 *   myHashMap.put(2, 1);
 *   myHashMap.get(2);    // 返回 1
 *   myHashMap.remove(2);
 *   myHashMap.get(2);    // 返回 -1
 */
class MyHashMap {
public:
    MyHashMap();
    void put(int key, int value);
    int get(int key);
    void remove(int key);
};

/**
 * 题目3: 两数之和
 *
 * 给定一个整数数组 nums 和一个整数目标值 target，
 * 请你在该数组中找出和为目标值 target 的那两个整数，并返回它们的数组下标。
 *
 * 你可以假设每种输入只会对应一个答案。但是，数组中同一个元素在答案里不能重复出现。
 *
 * 示例:
 *   输入: nums = [2,7,11,15], target = 9
 *   输出: [0,1]
 *   解释: nums[0] + nums[1] == 9
 *
 *   输入: nums = [3,2,4], target = 6
 *   输出: [1,2]
 *
 *   输入: nums = [3,3], target = 6
 *   输出: [0,1]
 *
 * @param nums 整数数组
 * @param target 目标和
 * @return 两个数的下标
 */
std::vector<int> twoSum(std::vector<int>& nums, int target);

/**
 * 题目4: 存在重复元素
 *
 * 给你一个整数数组 nums。如果任一值在数组中出现至少两次，返回 true；
 * 如果数组中每个元素互不相同，返回 false。
 *
 * 示例:
 *   输入: nums = [1,2,3,1]
 *   输出: true
 *
 *   输入: nums = [1,2,3,4]
 *   输出: false
 *
 * @param nums 整数数组
 * @return 是否存在重复元素
 */
bool containsDuplicate(std::vector<int>& nums);

/**
 * 题目5: 存在重复元素 II
 *
 * 给你一个整数数组 nums 和一个整数 k，判断数组中是否存在两个不同的索引 i 和 j，
 * 满足 nums[i] == nums[j] 且 abs(i - j) <= k。如果存在，返回 true；否则返回 false。
 *
 * 示例:
 *   输入: nums = [1,2,3,1], k = 3
 *   输出: true
 *
 *   输入: nums = [1,0,1,1], k = 1
 *   输出: true
 *
 *   输入: nums = [1,2,3,1,2,3], k = 2
 *   输出: false
 *
 * @param nums 整数数组
 * @param k 最大索引差
 * @return 是否存在满足条件的重复元素
 */
bool containsNearbyDuplicate(std::vector<int>& nums, int k);

/**
 * 题目6: 字母异位词分组
 *
 * 给你一个字符串数组，请你将字母异位词组合在一起。可以按任意顺序返回结果列表。
 * 字母异位词是由重新排列源单词的字母得到的一个新单词，所有源单词中的字母通常恰好只用一次。
 *
 * 示例:
 *   输入: strs = ["eat","tea","tan","ate","nat","bat"]
 *   输出: [["bat"],["nat","tan"],["ate","eat","tea"]]
 *
 *   输入: strs = [""]
 *   输出: [[""]]
 *
 *   输入: strs = ["a"]
 *   输出: [["a"]]
 *
 * @param strs 字符串数组
 * @return 分组后的字母异位词
 */
std::vector<std::vector<std::string>> groupAnagrams(std::vector<std::string>& strs);

/**
 * 题目7: 同构字符串
 *
 * 给定两个字符串 s 和 t，判断它们是否是同构的。
 * 如果 s 中的字符可以按某种映射关系替换得到 t，那么这两个字符串是同构的。
 *
 * 每个出现的字符都应当映射到另一个字符，同时不改变字符的顺序。
 * 不同字符不能映射到同一个字符上，相同字符只能映射到同一个字符上，字符可以映射到自己本身。
 *
 * 示例:
 *   输入: s = "egg", t = "add"
 *   输出: true
 *
 *   输入: s = "foo", t = "bar"
 *   输出: false
 *
 *   输入: s = "paper", t = "title"
 *   输出: true
 *
 * @param s 字符串1
 * @param t 字符串2
 * @return 是否同构
 */
bool isIsomorphic(std::string s, std::string t);

/**
 * 题目8: 有效的字母异位词
 *
 * 给定两个字符串 s 和 t，编写一个函数来判断 t 是否是 s 的字母异位词。
 *
 * 注意：若 s 和 t 中每个字符出现的次数都相同，则称 s 和 t 互为字母异位词。
 *
 * 示例:
 *   输入: s = "anagram", t = "nagaram"
 *   输出: true
 *
 *   输入: s = "rat", t = "car"
 *   输出: false
 *
 * @param s 字符串1
 * @param t 字符串2
 * @return 是否为字母异位词
 */
bool isAnagram(std::string s, std::string t);

/**
 * 题目9: 最长连续序列
 *
 * 给定一个未排序的整数数组 nums，找出数字连续的最长序列（不要求序列元素在原数组中连续）的长度。
 * 请你设计并实现时间复杂度为 O(n) 的算法解决此问题。
 *
 * 示例:
 *   输入: nums = [100,4,200,1,3,2]
 *   输出: 4
 *   解释: 最长数字连续序列是 [1, 2, 3, 4]，长度为 4
 *
 *   输入: nums = [0,3,7,2,5,8,4,6,0,1]
 *   输出: 9
 *
 * @param nums 整数数组
 * @return 最长连续序列长度
 */
int longestConsecutive(std::vector<int>& nums);

/**
 * 题目10: 前 K 个高频元素
 *
 * 给你一个整数数组 nums 和一个整数 k，请你返回其中出现频率前 k 高的元素。
 * 你可以按任意顺序返回答案。
 *
 * 示例:
 *   输入: nums = [1,1,1,2,2,3], k = 2
 *   输出: [1,2]
 *
 *   输入: nums = [1], k = 1
 *   输出: [1]
 *
 * 进阶：时间复杂度必须优于 O(n log n)
 *
 * @param nums 整数数组
 * @param k 前 k 个
 * @return 前 k 个高频元素
 */
std::vector<int> topKFrequent(std::vector<int>& nums, int k);

// Solution 版本
class MyHashSetSolution {
public:
    MyHashSetSolution();
    void add(int key);
    void remove(int key);
    bool contains(int key);
};

class MyHashMapSolution {
public:
    MyHashMapSolution();
    void put(int key, int value);
    int get(int key);
    void remove(int key);
};

std::vector<int> twoSumSolution(std::vector<int>& nums, int target);
bool containsDuplicateSolution(std::vector<int>& nums);
bool containsNearbyDuplicateSolution(std::vector<int>& nums, int k);
std::vector<std::vector<std::string>> groupAnagramsSolution(std::vector<std::string>& strs);
bool isIsomorphicSolution(std::string s, std::string t);
bool isAnagramSolution(std::string s, std::string t);
int longestConsecutiveSolution(std::vector<int>& nums);
std::vector<int> topKFrequentSolution(std::vector<int>& nums, int k);

// ==================== 测试函数声明 ====================

void testHashMapImpl();          // 测试面试者实现
void testHashMapImplSolution();  // 测试解答实现

// runTests 在头文件 inline 定义，调用两个测试函数
inline void runTests() {
    testHashMapImpl();
    testHashMapImplSolution();
}

} // namespace HashMapImplProblems

#endif // HASHMAP_IMPL_H
