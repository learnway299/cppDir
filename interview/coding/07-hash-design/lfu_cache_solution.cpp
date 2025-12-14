/**
 * @file lfu_cache_solution.cpp
 * @brief LFU 缓存实现 - 参考答案
 */

#include "lfu_cache.h"
#include <iostream>
#include <cassert>

namespace LFUCacheProblems {

// ==================== 参考答案实现 ====================

// LFUCacheSolution 实现
LFUCacheSolution::LFUCacheSolution(int capacity) : capacity_(capacity), minFreq_(0) {}

void LFUCacheSolution::increaseFreq(int key) {
    Node& node = keyMap_[key];
    int oldFreq = node.freq;
    int newFreq = oldFreq + 1;

    freqMap_[oldFreq].erase(node.iter);

    if (freqMap_[oldFreq].empty()) {
        freqMap_.erase(oldFreq);
        if (minFreq_ == oldFreq) {
            minFreq_ = newFreq;
        }
    }

    freqMap_[newFreq].push_front(key);
    node.freq = newFreq;
    node.iter = freqMap_[newFreq].begin();
}

int LFUCacheSolution::get(int key) {
    if (capacity_ == 0) return -1;

    auto it = keyMap_.find(key);
    if (it == keyMap_.end()) {
        return -1;
    }

    increaseFreq(key);
    return it->second.value;
}

void LFUCacheSolution::put(int key, int value) {
    if (capacity_ == 0) return;

    auto it = keyMap_.find(key);

    if (it != keyMap_.end()) {
        it->second.value = value;
        increaseFreq(key);
        return;
    }

    if (static_cast<int>(keyMap_.size()) >= capacity_) {
        int evictKey = freqMap_[minFreq_].back();
        freqMap_[minFreq_].pop_back();
        if (freqMap_[minFreq_].empty()) {
            freqMap_.erase(minFreq_);
        }
        keyMap_.erase(evictKey);
    }

    freqMap_[1].push_front(key);
    keyMap_[key] = {value, 1, freqMap_[1].begin()};
    minFreq_ = 1;
}

// LFUCacheManualSolution 实现
struct LFUCacheManualSolution::DLinkedNode {
    int key, value, freq;
    DLinkedNode* prev;
    DLinkedNode* next;
    DLinkedNode() : key(0), value(0), freq(1), prev(nullptr), next(nullptr) {}
    DLinkedNode(int k, int v) : key(k), value(v), freq(1), prev(nullptr), next(nullptr) {}
};

struct LFUCacheManualSolution::FreqList {
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
        node->prev = head;
        node->next = head->next;
        head->next->prev = node;
        head->next = node;
        ++size;
    }

    void removeNode(DLinkedNode* node) {
        node->prev->next = node->next;
        node->next->prev = node->prev;
        --size;
    }

    DLinkedNode* removeTail() {
        if (size == 0) return nullptr;
        DLinkedNode* node = tail->prev;
        removeNode(node);
        return node;
    }

    bool empty() const { return size == 0; }
};

LFUCacheManualSolution::LFUCacheManualSolution(int capacity) : capacity_(capacity), minFreq_(0) {}

LFUCacheManualSolution::~LFUCacheManualSolution() {
    for (auto& [freq, list] : freqMap_) {
        delete list;
    }
}

LFUCacheManualSolution::FreqList* LFUCacheManualSolution::getOrCreateFreqList(int freq) {
    if (!freqMap_.count(freq)) {
        freqMap_[freq] = new FreqList();
    }
    return freqMap_[freq];
}

void LFUCacheManualSolution::increaseFreq(DLinkedNode* node) {
    int oldFreq = node->freq;
    int newFreq = oldFreq + 1;

    freqMap_[oldFreq]->removeNode(node);
    if (freqMap_[oldFreq]->empty()) {
        delete freqMap_[oldFreq];
        freqMap_.erase(oldFreq);
        if (minFreq_ == oldFreq) {
            minFreq_ = newFreq;
        }
    }

    node->freq = newFreq;
    getOrCreateFreqList(newFreq)->addToHead(node);
}

int LFUCacheManualSolution::get(int key) {
    if (capacity_ == 0) return -1;

    auto it = keyMap_.find(key);
    if (it == keyMap_.end()) {
        return -1;
    }

    DLinkedNode* node = it->second;
    increaseFreq(node);
    return node->value;
}

void LFUCacheManualSolution::put(int key, int value) {
    if (capacity_ == 0) return;

    auto it = keyMap_.find(key);

    if (it != keyMap_.end()) {
        DLinkedNode* node = it->second;
        node->value = value;
        increaseFreq(node);
        return;
    }

    if (static_cast<int>(keyMap_.size()) >= capacity_) {
        DLinkedNode* evictNode = freqMap_[minFreq_]->removeTail();
        keyMap_.erase(evictNode->key);
        if (freqMap_[minFreq_]->empty()) {
            delete freqMap_[minFreq_];
            freqMap_.erase(minFreq_);
        }
        delete evictNode;
    }

    DLinkedNode* newNode = new DLinkedNode(key, value);
    keyMap_[key] = newNode;
    getOrCreateFreqList(1)->addToHead(newNode);
    minFreq_ = 1;
}

// ==================== 测试函数 ====================

void testLFUCacheSolution() {
    std::cout << "=== LFU Cache Tests (Solution) ===\n";

    // LFUCacheSolution 测试
    LFUCacheSolution lfu(2);
    lfu.put(1, 1);
    lfu.put(2, 2);
    assert(lfu.get(1) == 1);
    lfu.put(3, 3);           // 淘汰 key 2
    assert(lfu.get(2) == -1);
    assert(lfu.get(3) == 3);
    lfu.put(4, 4);           // 淘汰 key 1
    assert(lfu.get(1) == -1);
    assert(lfu.get(3) == 3);
    assert(lfu.get(4) == 4);
    std::cout << "  LFUCacheSolution: PASSED\n";

    // LFUCacheManualSolution 测试
    LFUCacheManualSolution lfuManual(2);
    lfuManual.put(1, 1);
    lfuManual.put(2, 2);
    assert(lfuManual.get(1) == 1);
    lfuManual.put(3, 3);
    assert(lfuManual.get(2) == -1);
    assert(lfuManual.get(3) == 3);
    lfuManual.put(4, 4);
    assert(lfuManual.get(1) == -1);
    assert(lfuManual.get(3) == 3);
    assert(lfuManual.get(4) == 4);
    std::cout << "  LFUCacheManualSolution: PASSED\n";

    // 边界情况测试
    LFUCacheSolution lfu0(0);
    lfu0.put(0, 0);
    assert(lfu0.get(0) == -1);

    LFUCacheSolution lfu1(1);
    lfu1.put(1, 1);
    assert(lfu1.get(1) == 1);
    lfu1.put(2, 2);
    assert(lfu1.get(1) == -1);
    assert(lfu1.get(2) == 2);

    LFUCacheSolution lfu2(2);
    lfu2.put(1, 1);
    lfu2.put(2, 2);
    lfu2.put(1, 10);  // 更新已存在的 key
    assert(lfu2.get(1) == 10);
    std::cout << "  LFUCacheSolution Edge Cases: PASSED\n";
}

} // namespace LFUCacheProblems
