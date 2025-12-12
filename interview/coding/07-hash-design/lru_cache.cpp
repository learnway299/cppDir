/**
 * @file lru_cache.cpp
 * @brief LRU 缓存实现 - 题目文件
 *
 * LRU (Least Recently Used) 是一种常用的缓存淘汰策略。
 * 当缓存满时，淘汰最久未使用的数据。
 *
 * 核心数据结构：哈希表 + 双向链表
 */

#include <iostream>
#include <unordered_map>
#include <list>

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
 *   LRUCache lRUCache = new LRUCache(2);
 *   lRUCache.put(1, 1);
 *   lRUCache.put(2, 2);
 *   lRUCache.get(1);    // 返回 1
 *   lRUCache.put(3, 3); // 淘汰 key 2
 *   lRUCache.get(2);    // 返回 -1
 *   lRUCache.put(4, 4); // 淘汰 key 1
 *   lRUCache.get(1);    // 返回 -1
 *   lRUCache.get(3);    // 返回 3
 *   lRUCache.get(4);    // 返回 4
 *
 * 思路:
 *   - 哈希表：O(1) 查找
 *   - 双向链表：O(1) 删除和移动节点
 *   - 链表头部是最近使用的，尾部是最久未使用的
 */
class LRUCache {
public:
    LRUCache(int capacity) {
        // TODO: 初始化
    }

    int get(int key) {
        // TODO: 实现获取
        return -1;
    }

    void put(int key, int value) {
        // TODO: 实现插入/更新
    }
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
    LRUCacheManual(int capacity) {
        // TODO: 初始化
    }

    int get(int key) {
        // TODO: 实现获取
        return -1;
    }

    void put(int key, int value) {
        // TODO: 实现插入/更新
    }
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
    LRUCacheWithTTL(int capacity) {
        // TODO: 初始化
    }

    int get(int key) {
        // TODO: 实现获取（检查过期）
        return -1;
    }

    void put(int key, int value, int ttl) {
        // TODO: 实现插入（带过期时间）
    }
};

/**
 * 题目4: 线程安全的 LRU 缓存
 *
 * 在 LRU 缓存基础上，支持多线程并发访问：
 *   - 读写操作需要保证线程安全
 *   - 考虑使用读写锁提高并发性能
 *
 * 这是实际工程中必须考虑的问题。
 */
class ThreadSafeLRUCache {
public:
    ThreadSafeLRUCache(int capacity) {
        // TODO: 初始化
    }

    int get(int key) {
        // TODO: 线程安全的获取
        return -1;
    }

    void put(int key, int value) {
        // TODO: 线程安全的插入
    }
};


// ==================== 测试代码 ====================
#include <cassert>

void testLRUCache() {
    LRUCache cache(2);
    cache.put(1, 1);
    cache.put(2, 2);
    assert(cache.get(1) == 1);
    cache.put(3, 3);        // 淘汰 key 2
    assert(cache.get(2) == -1);
    cache.put(4, 4);        // 淘汰 key 1
    assert(cache.get(1) == -1);
    assert(cache.get(3) == 3);
    assert(cache.get(4) == 4);

    std::cout << "LRUCache: PASSED\n";
}

void testLRUCacheManual() {
    LRUCacheManual cache(2);
    cache.put(1, 1);
    cache.put(2, 2);
    assert(cache.get(1) == 1);
    cache.put(3, 3);
    assert(cache.get(2) == -1);
    cache.put(4, 4);
    assert(cache.get(1) == -1);
    assert(cache.get(3) == 3);
    assert(cache.get(4) == 4);

    std::cout << "LRUCacheManual: PASSED\n";
}

int main() {
    std::cout << "=== LRU Cache Tests ===\n";

    // 取消注释以运行测试（实现解法后）
    // testLRUCache();
    // testLRUCacheManual();

    std::cout << "All tests passed!\n";
    return 0;
}
