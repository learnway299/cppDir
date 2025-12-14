/**
 * @file lru_cache.cpp
 * @brief LRU 缓存实现 - 面试者实现文件
 */

#include "lru_cache.h"
#include <iostream>

namespace LRUCacheHash {

// ==================== 面试者实现 ====================

// LRUCache 实现
LRUCache::LRUCache(int capacity) : capacity_(capacity) {
    // TODO: 初始化
}

int LRUCache::get(int key) {
    // TODO: 实现获取
    return -1;
}

void LRUCache::put(int key, int value) {
    // TODO: 实现插入/更新
}

// LRUCacheManual 实现
struct LRUCacheManual::DLinkedNode {
    int key, value;
    DLinkedNode* prev;
    DLinkedNode* next;
    DLinkedNode() : key(0), value(0), prev(nullptr), next(nullptr) {}
    DLinkedNode(int k, int v) : key(k), value(v), prev(nullptr), next(nullptr) {}
};

LRUCacheManual::LRUCacheManual(int capacity) : capacity_(capacity), size_(0) {
    // TODO: 初始化
    head_ = nullptr;
    tail_ = nullptr;
}

LRUCacheManual::~LRUCacheManual() {
    // TODO: 释放内存
}

void LRUCacheManual::addToHead(DLinkedNode* node) {
    // TODO: 实现添加到头部
}

void LRUCacheManual::removeNode(DLinkedNode* node) {
    // TODO: 实现删除节点
}

void LRUCacheManual::moveToHead(DLinkedNode* node) {
    // TODO: 实现移动到头部
}

LRUCacheManual::DLinkedNode* LRUCacheManual::removeTail() {
    // TODO: 实现删除尾部
    return nullptr;
}

int LRUCacheManual::get(int key) {
    // TODO: 实现获取
    return -1;
}

void LRUCacheManual::put(int key, int value) {
    // TODO: 实现插入/更新
}

// LRUCacheWithTTL 实现
struct LRUCacheWithTTL::CacheEntry {
    int value;
    long long expireTime;
};

LRUCacheWithTTL::LRUCacheWithTTL(int capacity) : capacity_(capacity) {
    // TODO: 初始化
}

int LRUCacheWithTTL::get(int key) {
    // TODO: 实现获取（检查过期）
    return -1;
}

void LRUCacheWithTTL::put(int key, int value, int ttlSeconds) {
    // TODO: 实现插入（带过期时间）
}

// ThreadSafeLRUCache 实现
ThreadSafeLRUCache::ThreadSafeLRUCache(int capacity) : capacity_(capacity) {
    // TODO: 初始化
}

int ThreadSafeLRUCache::get(int key) {
    // TODO: 线程安全的获取
    return -1;
}

void ThreadSafeLRUCache::put(int key, int value) {
    // TODO: 线程安全的插入
}

// ==================== 测试函数 ====================

void testLRUCacheHash() {
    std::cout << "=== LRU Cache Hash Tests (User Implementation) ===\n";
    // TODO: 添加面试者实现的测试
    std::cout << "  (No tests yet - implement your solutions first)\n";
}

} // namespace LRUCacheHash
