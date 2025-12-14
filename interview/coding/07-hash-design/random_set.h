/**
 * @file random_set.h
 * @brief O(1) 随机集合实现 - 题目说明与声明
 *
 * 设计支持 O(1) 插入、删除和随机获取的数据结构。
 * 核心思想：哈希表 + 动态数组
 */

#ifndef RANDOM_SET_H
#define RANDOM_SET_H

#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <random>

namespace RandomSetProblems {

// ==================== 题目声明 ====================

/**
 * 题目1: O(1) 时间插入、删除和获取随机元素
 *
 * 实现 RandomizedSet 类：
 *   - RandomizedSet() 初始化对象
 *   - bool insert(int val) 当元素 val 不存在时，向集合中插入该项，并返回 true；
 *     否则，返回 false
 *   - bool remove(int val) 当元素 val 存在时，从集合中移除该项，并返回 true；
 *     否则，返回 false
 *   - int getRandom() 随机返回现有集合中的一项（保证调用此方法时集合中至少存在一个元素）。
 *     每个元素应该有相同的概率被返回
 *
 * 你必须实现类的所有函数，并满足每个函数的平均时间复杂度为 O(1)。
 *
 * 示例:
 *   RandomizedSet randomizedSet;
 *   randomizedSet.insert(1);     // 返回 true
 *   randomizedSet.remove(2);     // 返回 false
 *   randomizedSet.insert(2);     // 返回 true
 *   randomizedSet.getRandom();   // 返回 1 或 2
 *   randomizedSet.remove(1);     // 返回 true
 *   randomizedSet.insert(2);     // 返回 false
 *   randomizedSet.getRandom();   // 返回 2
 *
 * 思路:
 *   - 数组：存储元素，支持 O(1) 随机访问
 *   - 哈希表：val -> index，支持 O(1) 查找
 *   - 删除时，将待删除元素与最后一个元素交换，然后删除最后一个
 */
class RandomizedSet {
public:
    RandomizedSet();
    bool insert(int val);
    bool remove(int val);
    int getRandom();
private:
    std::vector<int> nums_;
    std::unordered_map<int, int> valToIndex_;
    std::mt19937 rng_;
};

class RandomizedSetSolution {
public:
    RandomizedSetSolution();
    bool insert(int val);
    bool remove(int val);
    int getRandom();
private:
    std::vector<int> nums_;
    std::unordered_map<int, int> valToIndex_;
    std::mt19937 rng_;
};

/**
 * 题目2: O(1) 时间插入、删除和获取随机元素 - 允许重复
 *
 * RandomizedCollection 是一种包含数字集合（可能是重复的）的数据结构。
 * 它应该支持插入和删除特定元素，以及删除随机元素。
 *
 * 实现 RandomizedCollection 类:
 *   - RandomizedCollection() 初始化空的 RandomizedCollection 对象
 *   - bool insert(int val) 将一个 val 项插入到集合中，即使该项已经存在。
 *     如果该项不存在，返回 true；否则返回 false
 *   - bool remove(int val) 如果存在，从集合中移除一个 val 项。
 *     如果该项存在，返回 true；否则返回 false。
 *     注意，如果 val 在集合中出现多次，我们只删除其中一个
 *   - int getRandom() 从现有集合中随机获取一个元素。
 *     每个元素被返回的概率与其在集合中的数量呈正比。
 *
 * 示例:
 *   RandomizedCollection collection;
 *   collection.insert(1);   // 返回 true
 *   collection.insert(1);   // 返回 false
 *   collection.insert(2);   // 返回 true
 *   collection.getRandom(); // 返回 1 的概率为 2/3，返回 2 的概率为 1/3
 *   collection.remove(1);   // 返回 true
 *   collection.getRandom(); // 返回 1 和 2 的概率均为 1/2
 */
class RandomizedCollection {
public:
    RandomizedCollection();
    bool insert(int val);
    bool remove(int val);
    int getRandom();
private:
    std::vector<int> nums_;
    std::unordered_map<int, std::unordered_set<int>> valToIndices_;
    std::mt19937 rng_;
};

class RandomizedCollectionSolution {
public:
    RandomizedCollectionSolution();
    bool insert(int val);
    bool remove(int val);
    int getRandom();
private:
    std::vector<int> nums_;
    std::unordered_map<int, std::unordered_set<int>> valToIndices_;
    std::mt19937 rng_;
};

/**
 * 题目3: 黑名单中的随机数
 *
 * 给定一个整数 n 和一个无重复黑名单整数数组 blacklist。
 * 设计一个算法，从 [0, n - 1] 范围内的任意整数中选取一个未加入黑名单 blacklist 的整数。
 * 任何在上述范围内且不在黑名单 blacklist 中的整数都应该有同等的可能性被返回。
 *
 * 优化你的算法，使它最小化调用随机函数的次数。
 *
 * 实现 BlacklistRandom 类:
 *   - BlacklistRandom(int n, vector<int>& blacklist) 初始化对象
 *   - int pick() 返回一个在 [0, n - 1] 范围内的随机整数且该整数不在 blacklist 中
 *
 * 示例:
 *   BlacklistRandom blRandom(7, {2, 3, 5});
 *   blRandom.pick(); // 返回 0, 1, 4, 6 中的任一个，概率相等
 *
 * 思路: 将黑名单数字映射到 [n - blacklist.size(), n) 区间的白名单数字
 */
class BlacklistRandom {
public:
    BlacklistRandom(int n, std::vector<int>& blacklist);
    int pick();
private:
    int whitelistSize_;
    std::unordered_map<int, int> blackToWhite_;
    std::mt19937 rng_;
};

class BlacklistRandomSolution {
public:
    BlacklistRandomSolution(int n, std::vector<int>& blacklist);
    int pick();
private:
    int whitelistSize_;
    std::unordered_map<int, int> blackToWhite_;
    std::mt19937 rng_;
};

/**
 * 题目4: 带权重的随机选择
 *
 * 给你一个下标从 0 开始的正整数数组 w，其中 w[i] 代表第 i 个下标的权重。
 *
 * 实现 pickIndex() 函数，它可以随机地从范围 [0, w.length - 1]（含边界）内
 * 选出并返回一个下标。选取下标 i 的概率为 w[i] / sum(w)。
 *
 * 示例:
 *   WeightedRandom wr({1, 3});
 *   wr.pickIndex(); // 返回 0 的概率为 1/4，返回 1 的概率为 3/4
 */
class WeightedRandom {
public:
    WeightedRandom(std::vector<int>& w);
    int pickIndex();
private:
    std::vector<int> prefixSum_;
    std::mt19937 rng_;
};

class WeightedRandomSolution {
public:
    WeightedRandomSolution(std::vector<int>& w);
    int pickIndex();
private:
    std::vector<int> prefixSum_;
    std::mt19937 rng_;
};

/**
 * 题目5: 链表随机节点（蓄水池抽样）
 *
 * 给你一个单链表，随机选择链表的一个节点，并返回相应的节点值。
 * 每个节点被选中的概率一样。
 *
 * 进阶：如果链表非常大且长度未知，如何解决这个问题？你能否在不使用额外空间的情况下解决此问题？
 *
 * 思路：蓄水池抽样算法
 *   - 遍历链表，第 i 个节点以 1/i 的概率替换当前结果
 */
struct ListNode {
    int val;
    ListNode* next;
    ListNode(int x) : val(x), next(nullptr) {}
};

class ReservoirSampling {
public:
    ReservoirSampling(ListNode* head);
    int getRandom();
    std::vector<int> getKRandom(int k);
private:
    ListNode* head_;
    std::mt19937 rng_;
};

class ReservoirSamplingSolution {
public:
    ReservoirSamplingSolution(ListNode* head);
    int getRandom();
    std::vector<int> getKRandom(int k);
private:
    ListNode* head_;
    std::mt19937 rng_;
};

/**
 * 题目6: 随机打乱数组 (Fisher-Yates 洗牌算法)
 *
 * 给你一个整数数组 nums，设计算法来打乱一个没有重复元素的数组。
 * 打乱后，数组的所有排列应该是等可能的。
 *
 * 实现 ShuffleArray 类：
 *   - ShuffleArray(vector<int>& nums) 使用整数数组 nums 初始化对象
 *   - vector<int> reset() 重设数组到它的初始状态并返回
 *   - vector<int> shuffle() 返回数组随机打乱后的结果
 */
class ShuffleArray {
public:
    ShuffleArray(std::vector<int>& nums);
    std::vector<int> reset();
    std::vector<int> shuffle();
private:
    std::vector<int> original_;
    std::vector<int> shuffled_;
    std::mt19937 rng_;
};

class ShuffleArraySolution {
public:
    ShuffleArraySolution(std::vector<int>& nums);
    std::vector<int> reset();
    std::vector<int> shuffle();
private:
    std::vector<int> original_;
    std::vector<int> shuffled_;
    std::mt19937 rng_;
};

// ==================== 测试函数声明 ====================

void testRandomSet();          // 测试面试者实现
void testRandomSetSolution();  // 测试解答实现

// runTests 在头文件 inline 定义，调用两个测试函数
inline void runTests() {
    testRandomSet();
    testRandomSetSolution();
}

} // namespace RandomSetProblems

#endif // RANDOM_SET_H
