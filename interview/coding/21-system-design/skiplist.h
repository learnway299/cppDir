/**
 * @file skiplist.h
 * @brief 跳表 - 面试题
 *
 * 题目：实现跳表 (LeetCode 1206)
 *
 * 要求：
 * 1. 实现 Skiplist 类，支持以下操作：
 *    - bool search(int target): 查找元素是否存在
 *    - void add(int num): 添加元素（允许重复）
 *    - bool erase(int num): 删除一个元素，返回是否成功
 *
 * 2. 跳表的特性：
 *    - 多层链表结构
 *    - 底层是完整的有序链表
 *    - 上层是下层的子集，用于快速跳跃
 *    - 随机层数，平衡性概率保证
 *
 * 3. 实现要点：
 *    - 使用随机数生成节点层数
 *    - 维护多层指针数组
 *    - 查找时从高层向低层逐步逼近
 *
 * 时间复杂度：
 * - search: 平均 O(log N)
 * - add: 平均 O(log N)
 * - erase: 平均 O(log N)
 *
 * 空间复杂度：O(N log N)
 */

#ifndef SKIPLIST_H
#define SKIPLIST_H

#include <vector>
#include <random>

namespace SkiplistImpl {

class Skiplist {
private:
    static const int MAX_LEVEL = 16;

    struct Node {
        int val;
        std::vector<Node*> forward;  // 指向不同层的下一个节点
        Node(int v, int level);
    };

    Node* head_;
    int level_;                // 当前最大层数
    std::mt19937 rng_;         // 随机数生成器

    int randomLevel();

public:
    Skiplist();
    ~Skiplist();

    bool search(int target);
    void add(int num);
    bool erase(int num);
    void print();
};

void runTests();

} // namespace SkiplistImpl

#endif // SKIPLIST_H
