/**
 * @file heap_impl.h
 * @brief 堆实现 - 面试题
 *
 * 题目: 手写实现最小堆
 * 支持: push, pop, top, size, empty
 *
 * 要求：
 * 1. 堆的性质：
 *    - 最小堆: 父节点 <= 子节点
 *    - 最大堆: 父节点 >= 子节点
 *    - 完全二叉树，用数组存储
 *
 * 2. 索引关系：
 *    - 父节点: (i - 1) / 2
 *    - 左子节点: 2 * i + 1
 *    - 右子节点: 2 * i + 2
 *
 * 3. 核心操作：
 *    - siftUp: 新元素加入后上浮
 *    - siftDown: 删除堆顶后下沉
 *
 * 4. 时间复杂度：
 *    - push: O(log n)
 *    - pop: O(log n)
 *    - top: O(1)
 *
 * 5. 空间复杂度：O(n)
 */

#ifndef HEAP_IMPL_H
#define HEAP_IMPL_H

#include <vector>

namespace HeapImplImpl {

// ==================== 最小堆实现 ====================
class MinHeap {
public:
    void push(int val);
    void pop();
    int top() const;
    int size() const;
    bool empty() const;

private:
    std::vector<int> data_;

    void siftUp(int idx);
    void siftDown(int idx);
};

// ==================== 测试函数 ====================
void runTests();

} // namespace HeapImplImpl

#endif // HEAP_IMPL_H
