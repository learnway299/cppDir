/**
 * @file lru_cache.cpp
 * @brief LRU 缓存实现 - 题目文件
 *
 * LRU (Least Recently Used) 缓存是面试高频题，需要掌握哈希表+双向链表的实现。
 */

#include <iostream>
#include <unordered_map>

/**
 * 题目: LRU 缓存
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
 *   LRUCache cache(2);  // 容量为 2
 *
 *   cache.put(1, 1);     // 缓存是 {1=1}
 *   cache.put(2, 2);     // 缓存是 {1=1, 2=2}
 *   cache.get(1);        // 返回 1，缓存是 {2=2, 1=1}
 *   cache.put(3, 3);     // 该操作会使得关键字 2 作废，缓存是 {1=1, 3=3}
 *   cache.get(2);        // 返回 -1 (未找到)
 *   cache.put(4, 4);     // 该操作会使得关键字 1 作废，缓存是 {3=3, 4=4}
 *   cache.get(1);        // 返回 -1 (未找到)
 *   cache.get(3);        // 返回 3
 *   cache.get(4);        // 返回 4
 *
 * 提示:
 *   - 使用哈希表实现 O(1) 查找
 *   - 使用双向链表维护访问顺序
 *   - 最近访问的放在链表头部，最久未访问的在尾部
 */
class LRUCache {
public:
    /**
     * 构造函数
     * @param capacity 缓存容量
     */
    LRUCache(int capacity) {
        // TODO: 实现你的解法
    }

    /**
     * 获取缓存值
     * @param key 键
     * @return 值，如果不存在返回 -1
     */
    int get(int key) {
        // TODO: 实现你的解法
        return -1;
    }

    /**
     * 插入或更新缓存
     * @param key 键
     * @param value 值
     */
    void put(int key, int value) {
        // TODO: 实现你的解法
    }
};


// ==================== 测试代码 ====================
#include <cassert>

void testLRUCache() {
    // 基本测试
    LRUCache cache(2);

    cache.put(1, 1);
    cache.put(2, 2);
    assert(cache.get(1) == 1);      // 返回 1

    cache.put(3, 3);                 // 淘汰 key=2
    assert(cache.get(2) == -1);     // 返回 -1

    cache.put(4, 4);                 // 淘汰 key=1
    assert(cache.get(1) == -1);     // 返回 -1
    assert(cache.get(3) == 3);      // 返回 3
    assert(cache.get(4) == 4);      // 返回 4

    std::cout << "LRUCache basic test: PASSED\n";

    // 更新测试
    LRUCache cache2(2);
    cache2.put(1, 1);
    cache2.put(2, 2);
    cache2.put(1, 10);               // 更新 key=1 的值
    assert(cache2.get(1) == 10);

    cache2.put(3, 3);                // 淘汰 key=2（因为 key=1 刚被更新）
    assert(cache2.get(2) == -1);

    std::cout << "LRUCache update test: PASSED\n";

    // 容量为 1 的测试
    LRUCache cache3(1);
    cache3.put(1, 1);
    assert(cache3.get(1) == 1);
    cache3.put(2, 2);                // 淘汰 key=1
    assert(cache3.get(1) == -1);
    assert(cache3.get(2) == 2);

    std::cout << "LRUCache capacity=1 test: PASSED\n";
}

int main() {
    std::cout << "=== LRU Cache Tests ===\n";

    // 取消注释以运行测试（实现解法后）
    // testLRUCache();

    std::cout << "All tests passed!\n";
    return 0;
}
