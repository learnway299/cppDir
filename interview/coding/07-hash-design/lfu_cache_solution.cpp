/**
 * @file lfu_cache_solution.cpp
 * @brief LFU 缓存实现 - 解答文件
 *
 * LFU 实现关键：
 * 1. 维护每个 key 的使用频率
 * 2. 相同频率的 key 按访问时间排序（LRU 顺序）
 * 3. 记录最小频率，用于 O(1) 淘汰
 */

#include <iostream>
#include <unordered_map>
#include <list>

/**
 * LFU 缓存实现
 *
 * 数据结构：
 * - keyMap: key -> {value, freq, iterator}
 * - freqMap: freq -> list<key>（双向链表，头部最新，尾部最旧）
 * - minFreq: 当前最小频率
 *
 * 时间复杂度: get/put O(1)
 * 空间复杂度: O(capacity)
 */
class LFUCache {
private:
    struct Node {
        int value;
        int freq;
        std::list<int>::iterator iter;  // 在 freqList 中的位置
    };

    int capacity;
    int minFreq;
    std::unordered_map<int, Node> keyMap;  // key -> Node
    std::unordered_map<int, std::list<int>> freqMap;  // freq -> list<key>

    void increaseFreq(int key) {
        Node& node = keyMap[key];
        int oldFreq = node.freq;
        int newFreq = oldFreq + 1;

        // 从旧频率链表中移除
        freqMap[oldFreq].erase(node.iter);

        // 如果旧频率链表为空，删除并可能更新 minFreq
        if (freqMap[oldFreq].empty()) {
            freqMap.erase(oldFreq);
            if (minFreq == oldFreq) {
                minFreq = newFreq;
            }
        }

        // 添加到新频率链表头部
        freqMap[newFreq].push_front(key);
        node.freq = newFreq;
        node.iter = freqMap[newFreq].begin();
    }

public:
    LFUCache(int capacity) : capacity(capacity), minFreq(0) {}

    int get(int key) {
        if (capacity == 0) return -1;

        auto it = keyMap.find(key);
        if (it == keyMap.end()) {
            return -1;
        }

        increaseFreq(key);
        return it->second.value;
    }

    void put(int key, int value) {
        if (capacity == 0) return;

        auto it = keyMap.find(key);

        if (it != keyMap.end()) {
            // key 已存在，更新值并增加频率
            it->second.value = value;
            increaseFreq(key);
            return;
        }

        // key 不存在，需要插入
        if (static_cast<int>(keyMap.size()) >= capacity) {
            // 淘汰最小频率链表的最后一个元素
            int evictKey = freqMap[minFreq].back();
            freqMap[minFreq].pop_back();
            if (freqMap[minFreq].empty()) {
                freqMap.erase(minFreq);
            }
            keyMap.erase(evictKey);
        }

        // 插入新节点，频率为 1
        freqMap[1].push_front(key);
        keyMap[key] = {value, 1, freqMap[1].begin()};
        minFreq = 1;
    }
};

/**
 * 使用自定义双向链表的 LFU 实现
 *
 * 更底层的实现，便于理解原理
 */
class LFUCacheManual {
private:
    struct DLinkedNode {
        int key, value, freq;
        DLinkedNode* prev;
        DLinkedNode* next;
        DLinkedNode() : key(0), value(0), freq(1), prev(nullptr), next(nullptr) {}
        DLinkedNode(int k, int v) : key(k), value(v), freq(1), prev(nullptr), next(nullptr) {}
    };

    struct FreqList {
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

    int capacity;
    int minFreq;
    std::unordered_map<int, DLinkedNode*> keyMap;
    std::unordered_map<int, FreqList*> freqMap;

    FreqList* getOrCreateFreqList(int freq) {
        if (!freqMap.count(freq)) {
            freqMap[freq] = new FreqList();
        }
        return freqMap[freq];
    }

    void increaseFreq(DLinkedNode* node) {
        int oldFreq = node->freq;
        int newFreq = oldFreq + 1;

        // 从旧频率链表移除
        freqMap[oldFreq]->removeNode(node);
        if (freqMap[oldFreq]->empty()) {
            delete freqMap[oldFreq];
            freqMap.erase(oldFreq);
            if (minFreq == oldFreq) {
                minFreq = newFreq;
            }
        }

        // 添加到新频率链表
        node->freq = newFreq;
        getOrCreateFreqList(newFreq)->addToHead(node);
    }

public:
    LFUCacheManual(int capacity) : capacity(capacity), minFreq(0) {}

    ~LFUCacheManual() {
        for (auto& [freq, list] : freqMap) {
            delete list;
        }
        // keyMap 中的节点已在 freqMap 的 FreqList 析构中删除
    }

    int get(int key) {
        if (capacity == 0) return -1;

        auto it = keyMap.find(key);
        if (it == keyMap.end()) {
            return -1;
        }

        DLinkedNode* node = it->second;
        increaseFreq(node);
        return node->value;
    }

    void put(int key, int value) {
        if (capacity == 0) return;

        auto it = keyMap.find(key);

        if (it != keyMap.end()) {
            DLinkedNode* node = it->second;
            node->value = value;
            increaseFreq(node);
            return;
        }

        if (static_cast<int>(keyMap.size()) >= capacity) {
            // 淘汰
            DLinkedNode* evictNode = freqMap[minFreq]->removeTail();
            keyMap.erase(evictNode->key);
            if (freqMap[minFreq]->empty()) {
                delete freqMap[minFreq];
                freqMap.erase(minFreq);
            }
            delete evictNode;
        }

        // 插入新节点
        DLinkedNode* newNode = new DLinkedNode(key, value);
        keyMap[key] = newNode;
        getOrCreateFreqList(1)->addToHead(newNode);
        minFreq = 1;
    }
};


// ==================== 测试代码 ====================
#include <cassert>

void testLFUCache() {
    LFUCache lfu(2);
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

    std::cout << "LFUCache: PASSED\n";
}

void testLFUCacheManual() {
    LFUCacheManual lfu(2);
    lfu.put(1, 1);
    lfu.put(2, 2);
    assert(lfu.get(1) == 1);
    lfu.put(3, 3);
    assert(lfu.get(2) == -1);
    assert(lfu.get(3) == 3);
    lfu.put(4, 4);
    assert(lfu.get(1) == -1);
    assert(lfu.get(3) == 3);
    assert(lfu.get(4) == 4);

    std::cout << "LFUCacheManual: PASSED\n";
}

void testLFUCacheEdgeCases() {
    // 容量为 0
    LFUCache lfu0(0);
    lfu0.put(0, 0);
    assert(lfu0.get(0) == -1);

    // 容量为 1
    LFUCache lfu1(1);
    lfu1.put(1, 1);
    assert(lfu1.get(1) == 1);
    lfu1.put(2, 2);
    assert(lfu1.get(1) == -1);
    assert(lfu1.get(2) == 2);

    // 更新已存在的 key
    LFUCache lfu2(2);
    lfu2.put(1, 1);
    lfu2.put(2, 2);
    lfu2.put(1, 10);  // 更新 key 1
    assert(lfu2.get(1) == 10);

    std::cout << "LFUCache Edge Cases: PASSED\n";
}

int main() {
    std::cout << "=== LFU Cache Solution Tests ===\n";

    testLFUCache();
    testLFUCacheManual();
    testLFUCacheEdgeCases();

    std::cout << "\nAll tests passed!\n";
    return 0;
}
