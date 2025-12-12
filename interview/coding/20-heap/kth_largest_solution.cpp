/**
 * @file kth_largest_solution.cpp
 * @brief 第K大元素 - 解答
 */
#include <vector>
#include <queue>
#include <iostream>
#include <algorithm>
using namespace std;

// 方法1: 最小堆，维护k个最大元素
int findKthLargest(vector<int>& nums, int k) {
    priority_queue<int, vector<int>, greater<int>> minHeap;
    for (int n : nums) {
        minHeap.push(n);
        if (minHeap.size() > k) minHeap.pop();
    }
    return minHeap.top();
}

// 方法2: 快速选择 (平均O(n))
int quickSelect(vector<int>& nums, int left, int right, int k) {
    int pivot = nums[right];
    int i = left;
    for (int j = left; j < right; ++j) {
        if (nums[j] >= pivot) {
            swap(nums[i++], nums[j]);
        }
    }
    swap(nums[i], nums[right]);

    if (i == k) return nums[i];
    else if (i < k) return quickSelect(nums, i + 1, right, k);
    else return quickSelect(nums, left, i - 1, k);
}

int findKthLargestQuickSelect(vector<int>& nums, int k) {
    return quickSelect(nums, 0, nums.size() - 1, k - 1);
}

// 数据流中的第K大
class KthLargest {
    priority_queue<int, vector<int>, greater<int>> minHeap;
    int k_;
public:
    KthLargest(int k, vector<int>& nums) : k_(k) {
        for (int n : nums) add(n);
    }

    int add(int val) {
        minHeap.push(val);
        if (minHeap.size() > k_) minHeap.pop();
        return minHeap.top();
    }
};

int main() {
    vector<int> nums = {3,2,1,5,6,4};
    cout << "findKthLargest([3,2,1,5,6,4], k=2): " << findKthLargest(nums, 2) << "\n";  // 5

    vector<int> nums2 = {3,2,3,1,2,4,5,5,6};
    cout << "findKthLargest([3,2,3,1,2,4,5,5,6], k=4): " << findKthLargest(nums2, 4) << "\n";  // 4

    vector<int> init = {4, 5, 8, 2};
    KthLargest kl(3, init);
    cout << "KthLargest add(3): " << kl.add(3) << "\n";  // 4
    cout << "KthLargest add(5): " << kl.add(5) << "\n";  // 5
    cout << "KthLargest add(10): " << kl.add(10) << "\n"; // 5
    return 0;
}
