/**
 * @file two_sum_sorted_solution.cpp
 * @brief 两数之和（有序数组）- 解答
 */
#include <vector>
#include <iostream>
using namespace std;

vector<int> twoSum(vector<int>& numbers, int target) {
    int left = 0, right = numbers.size() - 1;
    while (left < right) {
        int sum = numbers[left] + numbers[right];
        if (sum == target) {
            return {left + 1, right + 1};
        } else if (sum < target) {
            ++left;
        } else {
            --right;
        }
    }
    return {};
}

int main() {
    vector<int> nums = {2, 7, 11, 15};
    auto result = twoSum(nums, 9);
    cout << "twoSum([2,7,11,15], 9): [" << result[0] << ", " << result[1] << "]\n";

    nums = {2, 3, 4};
    result = twoSum(nums, 6);
    cout << "twoSum([2,3,4], 6): [" << result[0] << ", " << result[1] << "]\n";
    return 0;
}
