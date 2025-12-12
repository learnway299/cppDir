/**
 * @file lfu_cache.cpp
 * @brief LFU 缓存实现 - 题目文件
 *
 * LFU (Least Frequently Used) 是一种缓存淘汰策略。
 * 当缓存满时，淘汰使用频率最低的数据；频率相同时，淘汰最久未使用的。
 *
 * 核心数据结构：
 * - 哈希表：key -> (value, freq)
 * - 频率表：freq -> 双向链表（按访问时间排序）
 */

#include <iostream>
#include <unordered_map>
#include <list>

/**
 * 题目: LFU 缓存
 *
 * 请你为最不经常使用（LFU）缓存算法设计并实现数据结构。
 *
 * 实现 LFUCache 类：
 *   - LFUCache(int capacity) 用数据结构的容量 capacity 初始化对象
 *   - int get(int key) 如果键 key 存在于缓存中，则获取键的值，否则返回 -1
 *   - void put(int key, int value) 如果键 key 已存在，则变更其值；
 *     如果键不存在，请插入键值对。
 *     当缓存达到其容量 capacity 时，则应该在插入新项之前，移除最不经常使用的项。
 *     在此问题中，当存在平局（即两个或更多个键具有相同使用频率）时，应该去除最近最久未使用的键。
 *
 * 为了确定最不常使用的键，可以为缓存中的每个键维护一个使用计数器。
 * 使用计数最小的键是最久未使用的键。
 * 当一个键首次插入到缓存中时，它的使用计数器被设置为 1 (由于 put 操作)。
 * 对缓存中的键执行 get 或 put 操作，使用计数器的值将会递增。
 *
 * 函数 get 和 put 必须以 O(1) 的平均时间复杂度运行。
 *
 * 示例:
 *   LFUCache lfu = new LFUCache(2);
 *   lfu.put(1, 1);   // cache=[1,_], cnt(1)=1
 *   lfu.put(2, 2);   // cache=[2,1], cnt(2)=1, cnt(1)=1
 *   lfu.get(1);      // 返回 1, cache=[1,2], cnt(2)=1, cnt(1)=2
 *   lfu.put(3, 3);   // 去除 key 2, cache=[3,1], cnt(3)=1, cnt(1)=2
 *   lfu.get(2);      // 返回 -1
 *   lfu.get(3);      // 返回 3, cache=[3,1], cnt(3)=2, cnt(1)=2
 *   lfu.put(4, 4);   // 去除 key 1, cache=[4,3], cnt(4)=1, cnt(3)=2
 *   lfu.get(1);      // 返回 -1
 *   lfu.get(3);      // 返回 3, cache=[3,4], cnt(4)=1, cnt(3)=3
 *   lfu.get(4);      // 返回 4, cache=[4,3], cnt(4)=2, cnt(3)=3
 *
 * 思路:
 *   - keyMap: key -> Node*（O(1) 查找）
 *   - freqMap: freq -> 双向链表（相同频率的节点按时间排序）
 *   - minFreq: 记录当前最小频率
 */
class LFUCache {
public:
    LFUCache(int capacity) {
        // TODO: 初始化
    }

    int get(int key) {
        // TODO: 实现获取
        return -1;
    }

    void put(int key, int value) {
        // TODO: 实现插入/更新
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

    std::cout << "LFUCache Edge Cases: PASSED\n";
}

int main() {
    std::cout << "=== LFU Cache Tests ===\n";

    // 取消注释以运行测试（实现解法后）
    // testLFUCache();
    // testLFUCacheEdgeCases();

    std::cout << "All tests passed!\n";
    return 0;
}
