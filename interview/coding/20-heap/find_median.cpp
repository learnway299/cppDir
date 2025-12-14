/**
 * @file find_median.cpp
 * @brief 数据流的中位数 - 面试题实现
 */

#include "find_median.h"

namespace FindMedianImpl {

// ==================== 数据流中位数 ====================

MedianFinder::MedianFinder() {
    // TODO: 初始化
}

void MedianFinder::addNum(int num) {
    // TODO: 实现添加数字
    // 1. 先将数字加入大顶堆（较小的一半）
    // 2. 把大顶堆的最大值转移到小顶堆（较大的一半）
    // 3. 保持平衡：确保大顶堆的大小 >= 小顶堆的大小
    //    如果小顶堆更大，从小顶堆移一个元素到大顶堆
}

double MedianFinder::findMedian() {
    // TODO: 实现查找中位数
    // 1. 如果大顶堆大小 > 小顶堆大小，返回大顶堆堆顶
    // 2. 否则返回两个堆顶的平均值
    return 0.0;
}

} // namespace FindMedianImpl
