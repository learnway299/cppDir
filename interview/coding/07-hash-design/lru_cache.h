/**
 * @file lru_cache.h
 * @brief LRU 缓存实现 - 题目说明与声明
 *
 * LRU (Least Recently Used) 是一种常用的缓存淘汰策略。
 * 当缓存满时，淘汰最久未使用的数据。
 *
 * 核心数据结构：哈希表 + 双向链表
 */

#ifndef LRU_CACHE_HASH_H
#define LRU_CACHE_HASH_H

#include <unordered_map>
#include <list>

namespace LRUCacheHash {

// ==================== 题目声明 ====================

/**
 * 题目1: LRU 缓存
 *
 * 请你设计并实现一个满足 LRU (最近最少使用) 缓存约束的数据结构。
 *
 * 实现 LRUCache 类：
 *   - LRUCache(int capacity) 以正整数作为容量 capacity 初始化 LRU 缓存
 *   - int get(int key) 如果关键字 key 存在于缓存中，则返回关键字的值，否则返回 -1
 *   - void put(int key, int value) 如果关键字 key 已经存在，则变更其数据值 value；
 *     如果不存在，则向缓存中插入该组 key-value。
 *     如果插入操作导致关键字数量超过 capacity，则应该逐出最久未使用的关键字。
 *
 * 函数 get 和 put 必须以 O(1) 的平均时间复杂度运行。
 *
 * 示例:
 *   LRUCache lRUCache(2);
 *   lRUCache.put(1, 1);
 *   lRUCache.put(2, 2);
 *   lRUCache.get(1);    // 返回 1
 *   lRUCache.put(3, 3); // 淘汰 key 2
 *   lRUCache.get(2);    // 返回 -1
 *
 * 思路:
 *   - 哈希表：O(1) 查找
 *   - 双向链表：O(1) 删除和移动节点
 *   - 链表头部是最近使用的，尾部是最久未使用的
 */
class LRUCache {
public:
    LRUCache(int capacity);
    int get(int key);
    void put(int key, int value);
private:
    int capacity_;
    std::list<std::pair<int, int>> cache_;
    std::unordered_map<int, std::list<std::pair<int, int>>::iterator> map_;
};

class LRUCacheSolution {
public:
    LRUCacheSolution(int capacity);
    int get(int key);
    void put(int key, int value);
private:
    int capacity_;
    std::list<std::pair<int, int>> cache_;
    std::unordered_map<int, std::list<std::pair<int, int>>::iterator> map_;
};

/**
 * 题目2: LRU 缓存（手写双向链表版本）
 *
 * 不使用 STL 的 list，手写双向链表实现 LRU。
 *
 * 这是面试中更常见的要求，考察对底层数据结构的理解。
 */
class LRUCacheManual {
public:
    LRUCacheManual(int capacity);
    ~LRUCacheManual();
    int get(int key);
    void put(int key, int value);
private:
    struct DLinkedNode;
    int capacity_;
    int size_;
    DLinkedNode* head_;
    DLinkedNode* tail_;
    std::unordered_map<int, DLinkedNode*> map_;

    void addToHead(DLinkedNode* node);
    void removeNode(DLinkedNode* node);
    void moveToHead(DLinkedNode* node);
    DLinkedNode* removeTail();
};

class LRUCacheManualSolution {
public:
    LRUCacheManualSolution(int capacity);
    ~LRUCacheManualSolution();
    int get(int key);
    void put(int key, int value);
private:
    struct DLinkedNode;
    int capacity_;
    int size_;
    DLinkedNode* head_;
    DLinkedNode* tail_;
    std::unordered_map<int, DLinkedNode*> map_;

    void addToHead(DLinkedNode* node);
    void removeNode(DLinkedNode* node);
    void moveToHead(DLinkedNode* node);
    DLinkedNode* removeTail();
};

/**
 * 题目3: 带过期时间的 LRU 缓存
 *
 * 在 LRU 缓存基础上，增加过期时间功能：
 *   - put(key, value, ttl) 插入带过期时间的数据
 *   - get(key) 如果数据已过期，返回 -1 并删除
 *
 * 这是实际工程中常见的需求（如 Redis 的 EXPIRE 命令）。
 */
class LRUCacheWithTTL {
public:
    LRUCacheWithTTL(int capacity);
    int get(int key);
    void put(int key, int value, int ttlSeconds);
private:
    struct CacheEntry;
    int capacity_;
};

class LRUCacheWithTTLSolution {
public:
    LRUCacheWithTTLSolution(int capacity);
    int get(int key);
    void put(int key, int value, int ttlSeconds);
private:
    struct CacheEntry;
    int capacity_;
    std::list<std::pair<int, CacheEntry>> cache_;
    std::unordered_map<int, std::list<std::pair<int, CacheEntry>>::iterator> map_;
};

/**
 * 题目4: 线程安全的 LRU 缓存
 *
 * 在 LRU 缓存基础上，支持多线程并发访问：
 *   - 读写操作需要保证线程安全
 *   - 考虑使用读写锁提高并发性能
 */
class ThreadSafeLRUCache {
public:
    ThreadSafeLRUCache(int capacity);
    int get(int key);
    void put(int key, int value);
private:
    int capacity_;
};

class ThreadSafeLRUCacheSolution {
public:
    ThreadSafeLRUCacheSolution(int capacity);
    int get(int key);
    void put(int key, int value);
    bool contains(int key) const;
    int size() const;
private:
    int capacity_;
    std::list<std::pair<int, int>> cache_;
    std::unordered_map<int, std::list<std::pair<int, int>>::iterator> map_;
    mutable void* mutex_; // 使用 void* 避免包含 mutex 头文件
};

// ==================== 测试函数声明 ====================

void testLRUCacheHash();          // 测试面试者实现
void testLRUCacheHashSolution();  // 测试解答实现

// runTests 在头文件 inline 定义，调用两个测试函数
inline void runTests() {
    testLRUCacheHash();
    testLRUCacheHashSolution();
}

} // namespace LRUCacheHash

#endif // LRU_CACHE_HASH_H
