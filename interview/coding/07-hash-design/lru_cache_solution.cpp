/**
 * @file lru_cache_solution.cpp
 * @brief LRU 缓存实现 - 解答文件
 *
 * LRU 缓存核心：
 * - 哈希表 + 双向链表
 * - 哈希表提供 O(1) 查找
 * - 双向链表维护访问顺序，支持 O(1) 删除和移动
 */

#include <iostream>
#include <unordered_map>
#include <list>
#include <mutex>
#include <shared_mutex>
#include <chrono>

/**
 * 题目1: LRU 缓存（使用 STL list）
 *
 * 时间复杂度: get/put O(1)
 * 空间复杂度: O(capacity)
 */
class LRUCache {
private:
    int capacity;
    std::list<std::pair<int, int>> cache;  // (key, value)
    std::unordered_map<int, std::list<std::pair<int, int>>::iterator> map;

public:
    LRUCache(int capacity) : capacity(capacity) {}

    int get(int key) {
        auto it = map.find(key);
        if (it == map.end()) {
            return -1;
        }

        // 移动到链表头部
        cache.splice(cache.begin(), cache, it->second);
        return it->second->second;
    }

    void put(int key, int value) {
        auto it = map.find(key);

        if (it != map.end()) {
            // 已存在，更新值并移动到头部
            it->second->second = value;
            cache.splice(cache.begin(), cache, it->second);
            return;
        }

        // 不存在，插入新节点
        if (static_cast<int>(cache.size()) >= capacity) {
            // 淘汰最久未使用的（链表尾部）
            int oldKey = cache.back().first;
            map.erase(oldKey);
            cache.pop_back();
        }

        cache.emplace_front(key, value);
        map[key] = cache.begin();
    }
};

/**
 * 题目2: LRU 缓存（手写双向链表）
 *
 * 面试常考版本，需要手写链表节点和操作
 */
class LRUCacheManual {
private:
    struct DLinkedNode {
        int key, value;
        DLinkedNode* prev;
        DLinkedNode* next;
        DLinkedNode() : key(0), value(0), prev(nullptr), next(nullptr) {}
        DLinkedNode(int k, int v) : key(k), value(v), prev(nullptr), next(nullptr) {}
    };

    int capacity;
    int size;
    DLinkedNode* head;  // 虚拟头节点
    DLinkedNode* tail;  // 虚拟尾节点
    std::unordered_map<int, DLinkedNode*> map;

    void addToHead(DLinkedNode* node) {
        node->prev = head;
        node->next = head->next;
        head->next->prev = node;
        head->next = node;
    }

    void removeNode(DLinkedNode* node) {
        node->prev->next = node->next;
        node->next->prev = node->prev;
    }

    void moveToHead(DLinkedNode* node) {
        removeNode(node);
        addToHead(node);
    }

    DLinkedNode* removeTail() {
        DLinkedNode* node = tail->prev;
        removeNode(node);
        return node;
    }

public:
    LRUCacheManual(int capacity) : capacity(capacity), size(0) {
        head = new DLinkedNode();
        tail = new DLinkedNode();
        head->next = tail;
        tail->prev = head;
    }

    ~LRUCacheManual() {
        DLinkedNode* curr = head;
        while (curr) {
            DLinkedNode* next = curr->next;
            delete curr;
            curr = next;
        }
    }

    int get(int key) {
        auto it = map.find(key);
        if (it == map.end()) {
            return -1;
        }

        DLinkedNode* node = it->second;
        moveToHead(node);
        return node->value;
    }

    void put(int key, int value) {
        auto it = map.find(key);

        if (it != map.end()) {
            DLinkedNode* node = it->second;
            node->value = value;
            moveToHead(node);
            return;
        }

        DLinkedNode* newNode = new DLinkedNode(key, value);
        map[key] = newNode;
        addToHead(newNode);
        ++size;

        if (size > capacity) {
            DLinkedNode* removed = removeTail();
            map.erase(removed->key);
            delete removed;
            --size;
        }
    }
};

/**
 * 题目3: 带过期时间的 LRU 缓存
 */
class LRUCacheWithTTL {
private:
    struct CacheEntry {
        int value;
        std::chrono::steady_clock::time_point expireTime;
    };

    int capacity;
    std::list<std::pair<int, CacheEntry>> cache;
    std::unordered_map<int, std::list<std::pair<int, CacheEntry>>::iterator> map;

    bool isExpired(const CacheEntry& entry) {
        return std::chrono::steady_clock::now() > entry.expireTime;
    }

    void cleanupExpired() {
        auto it = cache.begin();
        while (it != cache.end()) {
            if (isExpired(it->second)) {
                map.erase(it->first);
                it = cache.erase(it);
            } else {
                ++it;
            }
        }
    }

public:
    LRUCacheWithTTL(int capacity) : capacity(capacity) {}

    int get(int key) {
        auto it = map.find(key);
        if (it == map.end()) {
            return -1;
        }

        // 检查是否过期
        if (isExpired(it->second->second)) {
            cache.erase(it->second);
            map.erase(it);
            return -1;
        }

        cache.splice(cache.begin(), cache, it->second);
        return it->second->second.value;
    }

    void put(int key, int value, int ttlSeconds) {
        auto expireTime = std::chrono::steady_clock::now() + std::chrono::seconds(ttlSeconds);
        CacheEntry entry{value, expireTime};

        auto it = map.find(key);

        if (it != map.end()) {
            it->second->second = entry;
            cache.splice(cache.begin(), cache, it->second);
            return;
        }

        if (static_cast<int>(cache.size()) >= capacity) {
            int oldKey = cache.back().first;
            map.erase(oldKey);
            cache.pop_back();
        }

        cache.emplace_front(key, entry);
        map[key] = cache.begin();
    }
};

/**
 * 题目4: 线程安全的 LRU 缓存
 *
 * 使用读写锁优化并发性能：
 * - 读操作：共享锁
 * - 写操作：独占锁
 */
class ThreadSafeLRUCache {
private:
    int capacity;
    std::list<std::pair<int, int>> cache;
    std::unordered_map<int, std::list<std::pair<int, int>>::iterator> map;
    mutable std::shared_mutex mutex;

public:
    ThreadSafeLRUCache(int capacity) : capacity(capacity) {}

    int get(int key) {
        std::unique_lock<std::shared_mutex> lock(mutex);

        auto it = map.find(key);
        if (it == map.end()) {
            return -1;
        }

        cache.splice(cache.begin(), cache, it->second);
        return it->second->second;
    }

    void put(int key, int value) {
        std::unique_lock<std::shared_mutex> lock(mutex);

        auto it = map.find(key);

        if (it != map.end()) {
            it->second->second = value;
            cache.splice(cache.begin(), cache, it->second);
            return;
        }

        if (static_cast<int>(cache.size()) >= capacity) {
            int oldKey = cache.back().first;
            map.erase(oldKey);
            cache.pop_back();
        }

        cache.emplace_front(key, value);
        map[key] = cache.begin();
    }

    // 只读操作可以使用共享锁
    bool contains(int key) const {
        std::shared_lock<std::shared_mutex> lock(mutex);
        return map.find(key) != map.end();
    }

    int size() const {
        std::shared_lock<std::shared_mutex> lock(mutex);
        return cache.size();
    }
};

/**
 * 分片 LRU 缓存（进一步提高并发性能）
 *
 * 将缓存分成多个分片，每个分片独立加锁
 */
class ShardedLRUCache {
private:
    static const int SHARD_COUNT = 16;
    std::vector<ThreadSafeLRUCache> shards;

    int getShard(int key) {
        return std::hash<int>{}(key) % SHARD_COUNT;
    }

public:
    ShardedLRUCache(int totalCapacity) {
        int capacityPerShard = (totalCapacity + SHARD_COUNT - 1) / SHARD_COUNT;
        shards.reserve(SHARD_COUNT);
        for (int i = 0; i < SHARD_COUNT; ++i) {
            shards.emplace_back(capacityPerShard);
        }
    }

    int get(int key) {
        return shards[getShard(key)].get(key);
    }

    void put(int key, int value) {
        shards[getShard(key)].put(key, value);
    }
};


// ==================== 测试代码 ====================
#include <cassert>
#include <thread>

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

void testLRUCacheWithTTL() {
    LRUCacheWithTTL cache(2);
    cache.put(1, 100, 2);  // TTL 2秒
    assert(cache.get(1) == 100);

    std::cout << "LRUCacheWithTTL: PASSED\n";
}

void testThreadSafeLRUCache() {
    ThreadSafeLRUCache cache(100);

    // 多线程写入
    std::vector<std::thread> threads;
    for (int i = 0; i < 10; ++i) {
        threads.emplace_back([&cache, i]() {
            for (int j = 0; j < 100; ++j) {
                cache.put(i * 100 + j, j);
            }
        });
    }

    for (auto& t : threads) {
        t.join();
    }

    std::cout << "ThreadSafeLRUCache: PASSED\n";
}

int main() {
    std::cout << "=== LRU Cache Solution Tests ===\n";

    testLRUCache();
    testLRUCacheManual();
    testLRUCacheWithTTL();
    testThreadSafeLRUCache();

    std::cout << "\nAll tests passed!\n";
    return 0;
}
