/**
 * @file lru_cache.h
 * @brief LRU 缓存实现 - 题目说明与声明
 *
 * LRU (Least Recently Used) 缓存是面试高频题，需要掌握哈希表+双向链表的实现。
 */

#ifndef LRU_CACHE_LINKED_LIST_H
#define LRU_CACHE_LINKED_LIST_H

#include "../../util/Util.h"
#include <unordered_map>
#include <list>

namespace LRUCacheLinkedList {

// ==================== 题目声明 ====================

/**
 * 题目: LRU 缓存
 *
 * 请你设计并实现一个满足 LRU (最近最少使用) 缓存约束的数据结构。
 *
 * 实现要求：
 *   - get(key): O(1) 时间获取值，不存在返回 -1
 *   - put(key, value): O(1) 时间插入/更新，超容量时淘汰最久未使用的
 *
 * 提示: 使用哈希表 + 双向链表
 */

// 面试者实现的 LRU 缓存
class LRUCache {
public:
    LRUCache(int capacity);
    int get(int key);
    void put(int key, int value);
};

// 参考答案：手动实现双向链表
class LRUCacheSolution {
private:
    struct DLinkedNode {
        int key, value;
        DLinkedNode* prev;
        DLinkedNode* next;
        DLinkedNode() : key(0), value(0), prev(nullptr), next(nullptr) {}
        DLinkedNode(int k, int v) : key(k), value(v), prev(nullptr), next(nullptr) {}
    };

    int capacity_, size_;
    DLinkedNode* head_;
    DLinkedNode* tail_;
    std::unordered_map<int, DLinkedNode*> cache_;

    void addToHead(DLinkedNode* node);
    void removeNode(DLinkedNode* node);
    void moveToHead(DLinkedNode* node);
    DLinkedNode* removeTail();

public:
    LRUCacheSolution(int capacity);
    ~LRUCacheSolution();
    int get(int key);
    void put(int key, int value);
};

// 参考答案：使用 std::list 的简洁版本
class LRUCacheSTLSolution {
private:
    int capacity_;
    std::list<std::pair<int, int>> lruList_;
    std::unordered_map<int, std::list<std::pair<int, int>>::iterator> cache_;

public:
    LRUCacheSTLSolution(int capacity);
    int get(int key);
    void put(int key, int value);
};

// ==================== 测试函数声明 ====================

void testLRUCacheLinkedList();          // 测试面试者实现
void testLRUCacheLinkedListSolution();  // 测试解答实现

// runTests 在头文件 inline 定义，调用两个测试函数
inline void runTests() {
    testLRUCacheLinkedList();
    testLRUCacheLinkedListSolution();
}

} // namespace LRUCacheLinkedList

#endif // LRU_CACHE_LINKED_LIST_H
