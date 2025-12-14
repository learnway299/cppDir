/**
 * @file lfu_cache.h
 * @brief LFU 缓存实现 - 题目说明与声明
 *
 * LFU (Least Frequently Used) 是一种缓存淘汰策略。
 * 当缓存满时，淘汰使用频率最低的数据；频率相同时，淘汰最久未使用的。
 *
 * 核心数据结构：
 * - 哈希表：key -> (value, freq)
 * - 频率表：freq -> 双向链表（按访问时间排序）
 */

#ifndef LFU_CACHE_H
#define LFU_CACHE_H

#include <unordered_map>
#include <list>

namespace LFUCacheProblems {

// ==================== 题目声明 ====================

/**
 * 题目1: LFU 缓存
 *
 * 请你为最不经常使用（LFU）缓存算法设计并实现数据结构。
 *
 * 实现 LFUCache 类：
 *   - LFUCache(int capacity) 用数据结构的容量 capacity 初始化对象
 *   - int get(int key) 如果键 key 存在于缓存中，则获取键的值，否则返回 -1
 *   - void put(int key, int value) 如果键 key 已存在，则变更其值；
 *     如果键不存在，请插入键值对。
 *     当缓存达到其容量 capacity 时，则应该在插入新项之前，移除最不经常使用的项。
 *     在此问题中，当存在平局（即两个或更多个键具有相同使用频率）时，应该去除最近最久未使用的键。
 *
 * 为了确定最不常使用的键，可以为缓存中的每个键维护一个使用计数器。
 * 使用计数最小的键是最久未使用的键。
 * 当一个键首次插入到缓存中时，它的使用计数器被设置为 1 (由于 put 操作)。
 * 对缓存中的键执行 get 或 put 操作，使用计数器的值将会递增。
 *
 * 函数 get 和 put 必须以 O(1) 的平均时间复杂度运行。
 *
 * 示例:
 *   LFUCache lfu(2);
 *   lfu.put(1, 1);   // cache=[1,_], cnt(1)=1
 *   lfu.put(2, 2);   // cache=[2,1], cnt(2)=1, cnt(1)=1
 *   lfu.get(1);      // 返回 1, cache=[1,2], cnt(2)=1, cnt(1)=2
 *   lfu.put(3, 3);   // 去除 key 2, cache=[3,1], cnt(3)=1, cnt(1)=2
 *   lfu.get(2);      // 返回 -1
 *   lfu.get(3);      // 返回 3, cache=[3,1], cnt(3)=2, cnt(1)=2
 *   lfu.put(4, 4);   // 去除 key 1, cache=[4,3], cnt(4)=1, cnt(3)=2
 *   lfu.get(1);      // 返回 -1
 *   lfu.get(3);      // 返回 3, cache=[3,4], cnt(4)=1, cnt(3)=3
 *   lfu.get(4);      // 返回 4, cache=[4,3], cnt(4)=2, cnt(3)=3
 *
 * 思路:
 *   - keyMap: key -> Node（O(1) 查找）
 *   - freqMap: freq -> 双向链表（相同频率的节点按时间排序）
 *   - minFreq: 记录当前最小频率
 */
class LFUCache {
public:
    LFUCache(int capacity);
    int get(int key);
    void put(int key, int value);
private:
    struct Node {
        int value;
        int freq;
        std::list<int>::iterator iter;
    };

    int capacity_;
    int minFreq_;
    std::unordered_map<int, Node> keyMap_;
    std::unordered_map<int, std::list<int>> freqMap_;

    void increaseFreq(int key);
};

class LFUCacheSolution {
public:
    LFUCacheSolution(int capacity);
    int get(int key);
    void put(int key, int value);
private:
    struct Node {
        int value;
        int freq;
        std::list<int>::iterator iter;
    };

    int capacity_;
    int minFreq_;
    std::unordered_map<int, Node> keyMap_;
    std::unordered_map<int, std::list<int>> freqMap_;

    void increaseFreq(int key);
};

/**
 * 题目2: LFU 缓存（手写双向链表版本）
 *
 * 不使用 STL 的 list，手写双向链表实现 LFU。
 *
 * 这是面试中更常见的要求，考察对底层数据结构的理解。
 *
 * 数据结构设计：
 *   - DLinkedNode: 双向链表节点，包含 key, value, freq
 *   - FreqList: 每个频率对应的双向链表
 *   - keyMap: key -> DLinkedNode*
 *   - freqMap: freq -> FreqList*
 */
class LFUCacheManual {
public:
    LFUCacheManual(int capacity);
    ~LFUCacheManual();
    int get(int key);
    void put(int key, int value);
private:
    struct DLinkedNode;
    struct FreqList;

    int capacity_;
    int minFreq_;
    std::unordered_map<int, DLinkedNode*> keyMap_;
    std::unordered_map<int, FreqList*> freqMap_;

    FreqList* getOrCreateFreqList(int freq);
    void increaseFreq(DLinkedNode* node);
};

class LFUCacheManualSolution {
public:
    LFUCacheManualSolution(int capacity);
    ~LFUCacheManualSolution();
    int get(int key);
    void put(int key, int value);
private:
    struct DLinkedNode;
    struct FreqList;

    int capacity_;
    int minFreq_;
    std::unordered_map<int, DLinkedNode*> keyMap_;
    std::unordered_map<int, FreqList*> freqMap_;

    FreqList* getOrCreateFreqList(int freq);
    void increaseFreq(DLinkedNode* node);
};

// ==================== 测试函数声明 ====================

void testLFUCache();          // 测试面试者实现
void testLFUCacheSolution();  // 测试解答实现

// runTests 在头文件 inline 定义，调用两个测试函数
inline void runTests() {
    testLFUCache();
    testLFUCacheSolution();
}

} // namespace LFUCacheProblems

#endif // LFU_CACHE_H
