/**
 * @file move_zeroes_solution.cpp
 * @brief 移动零 - 解答
 */
#include <vector>
#include <iostream>
using namespace std;

void moveZeroes(vector<int>& nums) {
    int slow = 0;
    for (int fast = 0; fast < nums.size(); ++fast) {
        if (nums[fast] != 0) {
            swap(nums[slow++], nums[fast]);
        }
    }
}

int main() {
    vector<int> nums = {0, 1, 0, 3, 12};
    cout << "Before: ";
    for (int n : nums) cout << n << " ";

    moveZeroes(nums);

    cout << "\nAfter:  ";
    for (int n : nums) cout << n << " ";
    cout << "\n";
    return 0;
}
