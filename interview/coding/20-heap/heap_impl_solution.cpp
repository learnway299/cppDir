/**
 * @file heap_impl_solution.cpp
 * @brief 堆实现 - 解答
 */

#include "heap_impl.h"
#include <iostream>
#include <stdexcept>
#include <cassert>
#include <algorithm>

namespace HeapImplImpl {
namespace Solution {

// ==================== 最小堆实现 ====================

class MinHeap {
public:
    void push(int val) {
        data_.push_back(val);
        siftUp(data_.size() - 1);
    }

    void pop() {
        if (empty()) throw std::runtime_error("Heap is empty");
        data_[0] = data_.back();
        data_.pop_back();
        if (!empty()) siftDown(0);
    }

    int top() const {
        if (empty()) throw std::runtime_error("Heap is empty");
        return data_[0];
    }

    int size() const { return data_.size(); }
    bool empty() const { return data_.empty(); }

private:
    std::vector<int> data_;

    void siftUp(int idx) {
        while (idx > 0) {
            int parent = (idx - 1) / 2;
            if (data_[idx] >= data_[parent]) break;
            std::swap(data_[idx], data_[parent]);
            idx = parent;
        }
    }

    void siftDown(int idx) {
        int n = data_.size();
        while (true) {
            int smallest = idx;
            int left = 2 * idx + 1;
            int right = 2 * idx + 2;

            if (left < n && data_[left] < data_[smallest])
                smallest = left;
            if (right < n && data_[right] < data_[smallest])
                smallest = right;

            if (smallest == idx) break;
            std::swap(data_[idx], data_[smallest]);
            idx = smallest;
        }
    }
};

} // namespace Solution

// ==================== 测试函数 ====================

void runTests() {
    std::cout << "=== Heap Impl Tests ===" << std::endl;

    // 测试基本操作
    {
        Solution::MinHeap heap;
        heap.push(5);
        heap.push(3);
        heap.push(7);
        heap.push(1);

        assert(heap.top() == 1);
        assert(heap.size() == 4);
    }
    std::cout << "  MinHeap push/top: PASSED" << std::endl;

    // 测试 pop
    {
        Solution::MinHeap heap;
        heap.push(5);
        heap.push(3);
        heap.push(7);
        heap.push(1);

        heap.pop();
        assert(heap.top() == 3);

        heap.push(2);
        assert(heap.top() == 2);
    }
    std::cout << "  MinHeap pop: PASSED" << std::endl;

    // 测试空堆
    {
        Solution::MinHeap heap;
        assert(heap.empty());
        heap.push(1);
        assert(!heap.empty());
    }
    std::cout << "  MinHeap empty: PASSED" << std::endl;
}

} // namespace HeapImplImpl
