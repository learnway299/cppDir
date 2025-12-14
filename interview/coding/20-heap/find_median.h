/**
 * @file find_median.h
 * @brief 数据流的中位数 - 面试题
 *
 * 题目: 数据流的中位数 (LeetCode 295)
 * 设计一个支持以下操作的数据结构:
 * - addNum(int num): 添加一个整数
 * - findMedian(): 返回中位数
 *
 * 关键要点：
 *
 * 1. 双堆策略：
 *    - maxHeap (大顶堆): 存较小的一半
 *    - minHeap (小顶堆): 存较大的一半
 *    - maxHeap.top() <= minHeap.top()
 *
 * 2. 平衡维护：
 *    - maxHeap.size() >= minHeap.size()
 *    - 差值最多为 1
 *
 * 3. 添加元素流程：
 *    - 先加入 maxHeap
 *    - 把 maxHeap.top() 给 minHeap
 *    - 如果 minHeap 更大，移回一个
 *
 * 4. 中位数计算：
 *    - 奇数个: maxHeap.top()
 *    - 偶数个: (maxHeap.top() + minHeap.top()) / 2
 *
 * 5. 时间复杂度：
 *    - addNum: O(log n)
 *    - findMedian: O(1)
 */

#ifndef FIND_MEDIAN_H
#define FIND_MEDIAN_H

namespace FindMedianImpl {

// ==================== 数据流中位数 ====================
class MedianFinder {
public:
    MedianFinder();
    void addNum(int num);
    double findMedian();

private:
    // TODO: 添加必要的成员变量（两个堆）
};

// ==================== 测试函数 ====================
void runTests();

} // namespace FindMedianImpl

#endif // FIND_MEDIAN_H
