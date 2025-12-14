/**
 * @file find_median_solution.cpp
 * @brief 数据流的中位数 - 解答
 */

#include "find_median.h"
#include <queue>
#include <vector>
#include <iostream>
#include <cassert>
#include <cmath>

namespace FindMedianImpl {
namespace Solution {

// ==================== 数据流中位数 ====================
// 使用两个堆: 大顶堆存较小的一半，小顶堆存较大的一半

class MedianFinder {
    std::priority_queue<int> maxHeap;                             // 较小的一半
    std::priority_queue<int, std::vector<int>, std::greater<int>> minHeap;  // 较大的一半

public:
    MedianFinder() {}

    void addNum(int num) {
        // 先加入大顶堆
        maxHeap.push(num);
        // 把大顶堆最大的给小顶堆
        minHeap.push(maxHeap.top());
        maxHeap.pop();
        // 保持平衡: 大顶堆 >= 小顶堆
        if (maxHeap.size() < minHeap.size()) {
            maxHeap.push(minHeap.top());
            minHeap.pop();
        }
    }

    double findMedian() {
        if (maxHeap.size() > minHeap.size()) {
            return maxHeap.top();
        }
        return (maxHeap.top() + minHeap.top()) / 2.0;
    }
};

} // namespace Solution

// ==================== 测试函数 ====================

void runTests() {
    std::cout << "=== Find Median Tests ===" << std::endl;

    // 测试偶数个元素
    {
        Solution::MedianFinder mf;
        mf.addNum(1);
        mf.addNum(2);
        assert(std::abs(mf.findMedian() - 1.5) < 1e-9);
    }
    std::cout << "  MedianFinder (even): PASSED" << std::endl;

    // 测试奇数个元素
    {
        Solution::MedianFinder mf;
        mf.addNum(1);
        mf.addNum(2);
        mf.addNum(3);
        assert(std::abs(mf.findMedian() - 2.0) < 1e-9);
    }
    std::cout << "  MedianFinder (odd): PASSED" << std::endl;

    // 测试连续添加
    {
        Solution::MedianFinder mf;
        mf.addNum(1);
        mf.addNum(2);
        assert(std::abs(mf.findMedian() - 1.5) < 1e-9);

        mf.addNum(3);
        assert(std::abs(mf.findMedian() - 2.0) < 1e-9);

        mf.addNum(4);
        assert(std::abs(mf.findMedian() - 2.5) < 1e-9);

        mf.addNum(5);
        assert(std::abs(mf.findMedian() - 3.0) < 1e-9);
    }
    std::cout << "  MedianFinder (sequence): PASSED" << std::endl;
}

} // namespace FindMedianImpl
