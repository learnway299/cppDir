/**
 * @file lru_cache_solution.cpp
 * @brief LRU 缓存实现 - 解答文件
 *
 * 包含 lru_cache.cpp 中题目的解答和详细解析。
 */

#include <iostream>
#include <unordered_map>
#include <list>

/**
 * 解法1: 哈希表 + 双向链表（手动实现）
 *
 * 数据结构:
 *   - 哈希表: key -> 链表节点指针，O(1) 查找
 *   - 双向链表: 维护访问顺序，头部是最近访问，尾部是最久未访问
 *
 * 操作:
 *   - get: O(1) 查找 + O(1) 移动到头部
 *   - put: O(1) 查找/插入 + O(1) 移动/删除
 *
 * 时间复杂度: O(1)
 * 空间复杂度: O(capacity)
 */
class LRUCache {
private:
    // 双向链表节点
    struct DLinkedNode {
        int key;
        int value;
        DLinkedNode* prev;
        DLinkedNode* next;

        DLinkedNode() : key(0), value(0), prev(nullptr), next(nullptr) {}
        DLinkedNode(int k, int v) : key(k), value(v), prev(nullptr), next(nullptr) {}
    };

    int capacity_;
    int size_;
    DLinkedNode* head_;  // 虚拟头节点
    DLinkedNode* tail_;  // 虚拟尾节点
    std::unordered_map<int, DLinkedNode*> cache_;

    // 添加节点到头部（最近访问）
    void addToHead(DLinkedNode* node) {
        node->prev = head_;
        node->next = head_->next;
        head_->next->prev = node;
        head_->next = node;
    }

    // 从链表中移除节点
    void removeNode(DLinkedNode* node) {
        node->prev->next = node->next;
        node->next->prev = node->prev;
    }

    // 将节点移动到头部
    void moveToHead(DLinkedNode* node) {
        removeNode(node);
        addToHead(node);
    }

    // 移除尾部节点（最久未使用）
    DLinkedNode* removeTail() {
        DLinkedNode* node = tail_->prev;
        removeNode(node);
        return node;
    }

public:
    LRUCache(int capacity) : capacity_(capacity), size_(0) {
        // 创建虚拟头尾节点
        head_ = new DLinkedNode();
        tail_ = new DLinkedNode();
        head_->next = tail_;
        tail_->prev = head_;
    }

    ~LRUCache() {
        // 清理所有节点
        DLinkedNode* curr = head_;
        while (curr) {
            DLinkedNode* next = curr->next;
            delete curr;
            curr = next;
        }
    }

    int get(int key) {
        auto it = cache_.find(key);
        if (it == cache_.end()) {
            return -1;
        }

        // 移动到头部（标记为最近访问）
        DLinkedNode* node = it->second;
        moveToHead(node);
        return node->value;
    }

    void put(int key, int value) {
        auto it = cache_.find(key);

        if (it != cache_.end()) {
            // key 已存在，更新值并移动到头部
            DLinkedNode* node = it->second;
            node->value = value;
            moveToHead(node);
        } else {
            // key 不存在，创建新节点
            DLinkedNode* newNode = new DLinkedNode(key, value);
            cache_[key] = newNode;
            addToHead(newNode);
            ++size_;

            // 超过容量，删除最久未使用的
            if (size_ > capacity_) {
                DLinkedNode* removed = removeTail();
                cache_.erase(removed->key);
                delete removed;
                --size_;
            }
        }
    }
};


/**
 * 解法2: 使用 std::list（简洁版）
 *
 * 使用 STL 的双向链表 std::list 简化实现。
 * splice 操作可以 O(1) 移动元素。
 */
class LRUCacheSTL {
private:
    int capacity_;
    std::list<std::pair<int, int>> lruList_;  // {key, value}
    std::unordered_map<int, std::list<std::pair<int, int>>::iterator> cache_;

public:
    LRUCacheSTL(int capacity) : capacity_(capacity) {}

    int get(int key) {
        auto it = cache_.find(key);
        if (it == cache_.end()) {
            return -1;
        }

        // 移动到链表头部
        lruList_.splice(lruList_.begin(), lruList_, it->second);
        return it->second->second;
    }

    void put(int key, int value) {
        auto it = cache_.find(key);

        if (it != cache_.end()) {
            // 更新值并移动到头部
            it->second->second = value;
            lruList_.splice(lruList_.begin(), lruList_, it->second);
        } else {
            // 检查容量
            if (static_cast<int>(cache_.size()) >= capacity_) {
                // 删除最久未使用的（链表尾部）
                int oldKey = lruList_.back().first;
                lruList_.pop_back();
                cache_.erase(oldKey);
            }

            // 插入新元素到头部
            lruList_.emplace_front(key, value);
            cache_[key] = lruList_.begin();
        }
    }
};


/**
 * 扩展: LRU 缓存的变体
 *
 * 1. LRU-K: 记录最近 K 次访问，根据第 K 次访问时间淘汰
 * 2. 2Q: 两个队列，一个用于首次访问，一个用于多次访问
 * 3. LFU: 根据访问频率淘汰（最不经常使用）
 * 4. ARC: 自适应替换缓存，结合 LRU 和 LFU 的优点
 */


// ==================== 测试代码 ====================
#include <cassert>

void testLRUCache() {
    // 基本测试
    LRUCache cache(2);

    cache.put(1, 1);
    cache.put(2, 2);
    assert(cache.get(1) == 1);

    cache.put(3, 3);
    assert(cache.get(2) == -1);

    cache.put(4, 4);
    assert(cache.get(1) == -1);
    assert(cache.get(3) == 3);
    assert(cache.get(4) == 4);

    std::cout << "LRUCache basic test: PASSED\n";

    // 更新测试
    LRUCache cache2(2);
    cache2.put(1, 1);
    cache2.put(2, 2);
    cache2.put(1, 10);
    assert(cache2.get(1) == 10);

    cache2.put(3, 3);
    assert(cache2.get(2) == -1);

    std::cout << "LRUCache update test: PASSED\n";

    // 容量为 1 的测试
    LRUCache cache3(1);
    cache3.put(1, 1);
    assert(cache3.get(1) == 1);
    cache3.put(2, 2);
    assert(cache3.get(1) == -1);
    assert(cache3.get(2) == 2);

    std::cout << "LRUCache capacity=1 test: PASSED\n";

    // 复杂操作测试
    LRUCache cache4(3);
    cache4.put(1, 1);
    cache4.put(2, 2);
    cache4.put(3, 3);
    cache4.get(1);       // 访问 1，顺序变为 2,3,1
    cache4.put(4, 4);    // 淘汰 2
    assert(cache4.get(2) == -1);
    assert(cache4.get(1) == 1);
    assert(cache4.get(3) == 3);
    assert(cache4.get(4) == 4);

    std::cout << "LRUCache complex test: PASSED\n";
}

void testLRUCacheSTL() {
    LRUCacheSTL cache(2);

    cache.put(1, 1);
    cache.put(2, 2);
    assert(cache.get(1) == 1);

    cache.put(3, 3);
    assert(cache.get(2) == -1);

    cache.put(4, 4);
    assert(cache.get(1) == -1);
    assert(cache.get(3) == 3);
    assert(cache.get(4) == 4);

    std::cout << "LRUCacheSTL: PASSED\n";
}

int main() {
    std::cout << "=== LRU Cache Solution Tests ===\n";

    testLRUCache();
    testLRUCacheSTL();

    std::cout << "\nAll tests passed!\n";
    return 0;
}
