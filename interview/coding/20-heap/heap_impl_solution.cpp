/**
 * @file heap_impl_solution.cpp
 * @brief 堆实现 - 解答
 */
#include <vector>
#include <iostream>
#include <stdexcept>
using namespace std;

class MinHeap {
public:
    void push(int val) {
        data_.push_back(val);
        siftUp(data_.size() - 1);
    }

    void pop() {
        if (empty()) throw runtime_error("Heap is empty");
        data_[0] = data_.back();
        data_.pop_back();
        if (!empty()) siftDown(0);
    }

    int top() const {
        if (empty()) throw runtime_error("Heap is empty");
        return data_[0];
    }

    int size() const { return data_.size(); }
    bool empty() const { return data_.empty(); }

private:
    vector<int> data_;

    void siftUp(int idx) {
        while (idx > 0) {
            int parent = (idx - 1) / 2;
            if (data_[idx] >= data_[parent]) break;
            swap(data_[idx], data_[parent]);
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
            swap(data_[idx], data_[smallest]);
            idx = smallest;
        }
    }
};

int main() {
    MinHeap heap;
    heap.push(5);
    heap.push(3);
    heap.push(7);
    heap.push(1);

    cout << "Top: " << heap.top() << "\n";  // 1

    heap.pop();
    cout << "After pop, top: " << heap.top() << "\n";  // 3

    heap.push(2);
    cout << "After push(2), top: " << heap.top() << "\n";  // 2

    cout << "Size: " << heap.size() << "\n";  // 4
    return 0;
}
