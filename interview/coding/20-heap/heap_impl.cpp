/**
 * @file heap_impl.cpp
 * @brief 堆实现 - 面试题实现
 */

#include "heap_impl.h"

namespace HeapImplImpl {

// ==================== 最小堆实现 ====================

void MinHeap::push(int val) {
    // TODO: 实现 push 操作
    // 1. 将元素添加到数组末尾
    // 2. 执行上浮操作
}

void MinHeap::pop() {
    // TODO: 实现 pop 操作
    // 1. 检查堆是否为空
    // 2. 将堆顶元素替换为最后一个元素
    // 3. 删除最后一个元素
    // 4. 执行下沉操作
}

int MinHeap::top() const {
    // TODO: 返回堆顶元素
    return 0;
}

int MinHeap::size() const {
    // TODO: 返回堆的大小
    return 0;
}

bool MinHeap::empty() const {
    // TODO: 判断堆是否为空
    return true;
}

void MinHeap::siftUp(int idx) {
    // TODO: 实现上浮操作
    // 比较当前节点和父节点，如果当前节点更小则交换
}

void MinHeap::siftDown(int idx) {
    // TODO: 实现下沉操作
    // 比较当前节点和子节点，选择最小的子节点进行交换
}

} // namespace HeapImplImpl
