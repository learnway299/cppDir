/**
 * @file lru_cache_solution.cpp
 * @brief LRU 缓存实现 - 参考答案
 */

#include "lru_cache.h"
#include <mutex>
#include <chrono>
#include <cassert>
#include <iostream>
#include <shared_mutex>
namespace LRUCacheHash {

// ==================== 参考答案实现 ====================

// LRUCacheSolution 实现
LRUCacheSolution::LRUCacheSolution(int capacity) : capacity_(capacity) {}

int LRUCacheSolution::get(int key) {
    auto it = map_.find(key);
    if (it == map_.end()) {
        return -1;
    }

    cache_.splice(cache_.begin(), cache_, it->second);
    return it->second->second;
}

void LRUCacheSolution::put(int key, int value) {
    auto it = map_.find(key);

    if (it != map_.end()) {
        it->second->second = value;
        cache_.splice(cache_.begin(), cache_, it->second);
        return;
    }

    if (static_cast<int>(cache_.size()) >= capacity_) {
        int oldKey = cache_.back().first;
        map_.erase(oldKey);
        cache_.pop_back();
    }

    cache_.emplace_front(key, value);
    map_[key] = cache_.begin();
}

// LRUCacheManualSolution 实现
struct LRUCacheManualSolution::DLinkedNode {
    int key, value;
    DLinkedNode* prev;
    DLinkedNode* next;
    DLinkedNode() : key(0), value(0), prev(nullptr), next(nullptr) {}
    DLinkedNode(int k, int v) : key(k), value(v), prev(nullptr), next(nullptr) {}
};

LRUCacheManualSolution::LRUCacheManualSolution(int capacity) : capacity_(capacity), size_(0) {
    head_ = new DLinkedNode();
    tail_ = new DLinkedNode();
    head_->next = tail_;
    tail_->prev = head_;
}

LRUCacheManualSolution::~LRUCacheManualSolution() {
    DLinkedNode* curr = head_;
    while (curr) {
        DLinkedNode* next = curr->next;
        delete curr;
        curr = next;
    }
}

void LRUCacheManualSolution::addToHead(DLinkedNode* node) {
    node->prev = head_;
    node->next = head_->next;
    head_->next->prev = node;
    head_->next = node;
}

void LRUCacheManualSolution::removeNode(DLinkedNode* node) {
    node->prev->next = node->next;
    node->next->prev = node->prev;
}

void LRUCacheManualSolution::moveToHead(DLinkedNode* node) {
    removeNode(node);
    addToHead(node);
}

LRUCacheManualSolution::DLinkedNode* LRUCacheManualSolution::removeTail() {
    DLinkedNode* node = tail_->prev;
    removeNode(node);
    return node;
}

int LRUCacheManualSolution::get(int key) {
    auto it = map_.find(key);
    if (it == map_.end()) {
        return -1;
    }

    DLinkedNode* node = it->second;
    moveToHead(node);
    return node->value;
}

void LRUCacheManualSolution::put(int key, int value) {
    auto it = map_.find(key);

    if (it != map_.end()) {
        DLinkedNode* node = it->second;
        node->value = value;
        moveToHead(node);
        return;
    }

    DLinkedNode* newNode = new DLinkedNode(key, value);
    map_[key] = newNode;
    addToHead(newNode);
    ++size_;

    if (size_ > capacity_) {
        DLinkedNode* removed = removeTail();
        map_.erase(removed->key);
        delete removed;
        --size_;
    }
}

// LRUCacheWithTTLSolution 实现
struct LRUCacheWithTTLSolution::CacheEntry {
    int value;
    std::chrono::steady_clock::time_point expireTime;
};

LRUCacheWithTTLSolution::LRUCacheWithTTLSolution(int capacity) : capacity_(capacity) {}

int LRUCacheWithTTLSolution::get(int key) {
    auto it = map_.find(key);
    if (it == map_.end()) {
        return -1;
    }

    if (std::chrono::steady_clock::now() > it->second->second.expireTime) {
        cache_.erase(it->second);
        map_.erase(it);
        return -1;
    }

    cache_.splice(cache_.begin(), cache_, it->second);
    return it->second->second.value;
}

void LRUCacheWithTTLSolution::put(int key, int value, int ttlSeconds) {
    auto expireTime = std::chrono::steady_clock::now() + std::chrono::seconds(ttlSeconds);
    CacheEntry entry{value, expireTime};

    auto it = map_.find(key);

    if (it != map_.end()) {
        it->second->second = entry;
        cache_.splice(cache_.begin(), cache_, it->second);
        return;
    }

    if (static_cast<int>(cache_.size()) >= capacity_) {
        int oldKey = cache_.back().first;
        map_.erase(oldKey);
        cache_.pop_back();
    }

    cache_.emplace_front(key, entry);
    map_[key] = cache_.begin();
}

// ThreadSafeLRUCacheSolution 实现
ThreadSafeLRUCacheSolution::ThreadSafeLRUCacheSolution(int capacity) : capacity_(capacity) {
    mutex_ = new std::shared_mutex();
}

int ThreadSafeLRUCacheSolution::get(int key) {
    std::unique_lock<std::shared_mutex> lock(*static_cast<std::shared_mutex*>(mutex_));

    auto it = map_.find(key);
    if (it == map_.end()) {
        return -1;
    }

    cache_.splice(cache_.begin(), cache_, it->second);
    return it->second->second;
}

void ThreadSafeLRUCacheSolution::put(int key, int value) {
    std::unique_lock<std::shared_mutex> lock(*static_cast<std::shared_mutex*>(mutex_));

    auto it = map_.find(key);

    if (it != map_.end()) {
        it->second->second = value;
        cache_.splice(cache_.begin(), cache_, it->second);
        return;
    }

    if (static_cast<int>(cache_.size()) >= capacity_) {
        int oldKey = cache_.back().first;
        map_.erase(oldKey);
        cache_.pop_back();
    }

    cache_.emplace_front(key, value);
    map_[key] = cache_.begin();
}

bool ThreadSafeLRUCacheSolution::contains(int key) const {
    std::shared_lock<std::shared_mutex> lock(*static_cast<std::shared_mutex*>(mutex_));
    return map_.find(key) != map_.end();
}

int ThreadSafeLRUCacheSolution::size() const {
    std::shared_lock<std::shared_mutex> lock(*static_cast<std::shared_mutex*>(mutex_));
    return static_cast<int>(cache_.size());
}

// ==================== 测试函数 ====================

void testLRUCacheHashSolution() {
    std::cout << "=== LRU Cache Hash Tests (Solution) ===\n";

    // LRUCacheSolution
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

    // LRUCacheManualSolution
    LRUCacheManualSolution cacheManual(2);
    cacheManual.put(1, 1);
    cacheManual.put(2, 2);
    assert(cacheManual.get(1) == 1);
    cacheManual.put(3, 3);
    assert(cacheManual.get(2) == -1);
    cacheManual.put(4, 4);
    assert(cacheManual.get(1) == -1);
    assert(cacheManual.get(3) == 3);
    assert(cacheManual.get(4) == 4);
    std::cout << "  LRUCacheManualSolution: PASSED\n";

    // LRUCacheWithTTLSolution
    LRUCacheWithTTLSolution cacheTTL(2);
    cacheTTL.put(1, 100, 10);
    assert(cacheTTL.get(1) == 100);
    std::cout << "  LRUCacheWithTTLSolution: PASSED\n";

    // ThreadSafeLRUCacheSolution
    ThreadSafeLRUCacheSolution threadSafeCache(100);
    threadSafeCache.put(1, 1);
    assert(threadSafeCache.get(1) == 1);
    std::cout << "  ThreadSafeLRUCacheSolution: PASSED\n";
}

} // namespace LRUCacheHash
