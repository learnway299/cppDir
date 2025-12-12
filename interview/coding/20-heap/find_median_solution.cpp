/**
 * @file find_median_solution.cpp
 * @brief 数据流的中位数 - 解答
 */
#include <queue>
#include <iostream>
using namespace std;

// 使用两个堆: 大顶堆存较小的一半，小顶堆存较大的一半
class MedianFinder {
    priority_queue<int> maxHeap;                             // 较小的一半
    priority_queue<int, vector<int>, greater<int>> minHeap;  // 较大的一半

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

int main() {
    MedianFinder mf;
    mf.addNum(1);
    mf.addNum(2);
    cout << "Median after [1,2]: " << mf.findMedian() << "\n";  // 1.5

    mf.addNum(3);
    cout << "Median after [1,2,3]: " << mf.findMedian() << "\n";  // 2

    mf.addNum(4);
    cout << "Median after [1,2,3,4]: " << mf.findMedian() << "\n";  // 2.5

    mf.addNum(5);
    cout << "Median after [1,2,3,4,5]: " << mf.findMedian() << "\n";  // 3
    return 0;
}
