/**
 * @file lru_cache_solution.cpp
 * @brief LRU 缓存实现 - 参考答案
 */

#include "lru_cache.h"
#include <iostream>
#include <cassert>

namespace LRUCacheLinkedList {

// ==================== 手动实现双向链表版本 ====================

void LRUCacheSolution::addToHead(DLinkedNode* node) {
    node->prev = head_;
    node->next = head_->next;
    head_->next->prev = node;
    head_->next = node;
}

void LRUCacheSolution::removeNode(DLinkedNode* node) {
    node->prev->next = node->next;
    node->next->prev = node->prev;
}

void LRUCacheSolution::moveToHead(DLinkedNode* node) {
    removeNode(node);
    addToHead(node);
}

LRUCacheSolution::DLinkedNode* LRUCacheSolution::removeTail() {
    DLinkedNode* node = tail_->prev;
    removeNode(node);
    return node;
}

LRUCacheSolution::LRUCacheSolution(int capacity) : capacity_(capacity), size_(0) {
    head_ = new DLinkedNode();
    tail_ = new DLinkedNode();
    head_->next = tail_;
    tail_->prev = head_;
}

LRUCacheSolution::~LRUCacheSolution() {
    DLinkedNode* curr = head_;
    while (curr) {
        DLinkedNode* next = curr->next;
        delete curr;
        curr = next;
    }
}

int LRUCacheSolution::get(int key) {
    auto it = cache_.find(key);
    if (it == cache_.end()) return -1;
    DLinkedNode* node = it->second;
    moveToHead(node);
    return node->value;
}

void LRUCacheSolution::put(int key, int value) {
    auto it = cache_.find(key);
    if (it != cache_.end()) {
        DLinkedNode* node = it->second;
        node->value = value;
        moveToHead(node);
    } else {
        DLinkedNode* newNode = new DLinkedNode(key, value);
        cache_[key] = newNode;
        addToHead(newNode);
        ++size_;
        if (size_ > capacity_) {
            DLinkedNode* removed = removeTail();
            cache_.erase(removed->key);
            delete removed;
            --size_;
        }
    }
}

// ==================== STL list 版本 ====================

LRUCacheSTLSolution::LRUCacheSTLSolution(int capacity) : capacity_(capacity) {}

int LRUCacheSTLSolution::get(int key) {
    auto it = cache_.find(key);
    if (it == cache_.end()) return -1;
    lruList_.splice(lruList_.begin(), lruList_, it->second);
    return it->second->second;
}

void LRUCacheSTLSolution::put(int key, int value) {
    auto it = cache_.find(key);
    if (it != cache_.end()) {
        it->second->second = value;
        lruList_.splice(lruList_.begin(), lruList_, it->second);
    } else {
        if (static_cast<int>(cache_.size()) >= capacity_) {
            int oldKey = lruList_.back().first;
            lruList_.pop_back();
            cache_.erase(oldKey);
        }
        lruList_.emplace_front(key, value);
        cache_[key] = lruList_.begin();
    }
}

// ==================== 测试函数 ====================

void testLRUCacheLinkedListSolution() {
    std::cout << "=== LRU Cache (Linked List) Tests (Solution) ===\n";

    // 测试手动实现版本
    LRUCacheSolution cache(2);
    cache.put(1, 1);
    cache.put(2, 2);
    assert(cache.get(1) == 1);
    cache.put(3, 3);
    assert(cache.get(2) == -1);
    cache.put(4, 4);
    assert(cache.get(1) == -1);
    assert(cache.get(3) == 3);
    assert(cache.get(4) == 4);
    std::cout << "  LRUCacheSolution: PASSED\n";

    // 测试 STL 版本
    LRUCacheSTLSolution cacheSTL(2);
    cacheSTL.put(1, 1);
    cacheSTL.put(2, 2);
    assert(cacheSTL.get(1) == 1);
    cacheSTL.put(3, 3);
    assert(cacheSTL.get(2) == -1);
    std::cout << "  LRUCacheSTLSolution: PASSED\n";
}

} // namespace LRUCacheLinkedList
