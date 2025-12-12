/**
 * @file missing_number_solution.cpp
 * @brief 丢失的数字 - 解答
 */
#include <vector>
#include <iostream>
using namespace std;

// 方法1: 异或法
// a ^ a = 0, 0 ^ a = a
// 将 [0,n] 与数组所有元素异或，成对的抵消
int missingNumber(vector<int>& nums) {
    int n = nums.size();
    int xorAll = n;  // 从n开始
    for (int i = 0; i < n; ++i) {
        xorAll ^= i ^ nums[i];
    }
    return xorAll;
}

// 方法2: 数学公式
int missingNumberSum(vector<int>& nums) {
    int n = nums.size();
    int expectedSum = n * (n + 1) / 2;
    int actualSum = 0;
    for (int num : nums) actualSum += num;
    return expectedSum - actualSum;
}

int main() {
    vector<int> nums1 = {3, 0, 1};
    cout << "missingNumber([3,0,1]): " << missingNumber(nums1) << "\n";  // 2

    vector<int> nums2 = {9,6,4,2,3,5,7,0,1};
    cout << "missingNumber([9,6,4,2,3,5,7,0,1]): " << missingNumber(nums2) << "\n";  // 8

    cout << "Using sum method: " << missingNumberSum(nums1) << "\n";  // 2
    return 0;
}
