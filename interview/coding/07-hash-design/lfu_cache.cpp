/**
 * @file lfu_cache.cpp
 * @brief LFU 缓存实现 - 面试者实现文件
 */

#include "lfu_cache.h"
#include <iostream>

namespace LFUCacheProblems {

// ==================== 面试者实现 ====================

// LFUCache 实现
LFUCache::LFUCache(int capacity) : capacity_(capacity), minFreq_(0) {
    // TODO: 初始化
}

void LFUCache::increaseFreq(int key) {
    // TODO: 实现频率增加逻辑
}

int LFUCache::get(int key) {
    // TODO: 实现获取
    return -1;
}

void LFUCache::put(int key, int value) {
    // TODO: 实现插入/更新
}

// LFUCacheManual 实现
struct LFUCacheManual::DLinkedNode {
    int key, value, freq;
    DLinkedNode* prev;
    DLinkedNode* next;
    DLinkedNode() : key(0), value(0), freq(1), prev(nullptr), next(nullptr) {}
    DLinkedNode(int k, int v) : key(k), value(v), freq(1), prev(nullptr), next(nullptr) {}
};

struct LFUCacheManual::FreqList {
    DLinkedNode* head;
    DLinkedNode* tail;
    int size;

    FreqList() : size(0) {
        head = new DLinkedNode();
        tail = new DLinkedNode();
        head->next = tail;
        tail->prev = head;
    }

    ~FreqList() {
        DLinkedNode* curr = head;
        while (curr) {
            DLinkedNode* next = curr->next;
            delete curr;
            curr = next;
        }
    }

    void addToHead(DLinkedNode* node) {
        // TODO: 实现添加到头部
    }

    void removeNode(DLinkedNode* node) {
        // TODO: 实现删除节点
    }

    DLinkedNode* removeTail() {
        // TODO: 实现删除尾部
        return nullptr;
    }

    bool empty() const { return size == 0; }
};

LFUCacheManual::LFUCacheManual(int capacity) : capacity_(capacity), minFreq_(0) {
    // TODO: 初始化
}

LFUCacheManual::~LFUCacheManual() {
    // TODO: 释放内存
}

LFUCacheManual::FreqList* LFUCacheManual::getOrCreateFreqList(int freq) {
    // TODO: 实现获取或创建频率链表
    return nullptr;
}

void LFUCacheManual::increaseFreq(DLinkedNode* node) {
    // TODO: 实现频率增加逻辑
}

int LFUCacheManual::get(int key) {
    // TODO: 实现获取
    return -1;
}

void LFUCacheManual::put(int key, int value) {
    // TODO: 实现插入/更新
}

// ==================== 测试函数 ====================

void testLFUCache() {
    std::cout << "=== LFU Cache Tests (User Implementation) ===\n";
    // TODO: 添加面试者实现的测试
    std::cout << "  (No tests yet - implement your solutions first)\n";
}

} // namespace LFUCacheProblems
