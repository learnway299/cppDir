/**
 * @file remove_duplicates_solution.cpp
 * @brief 删除有序数组中的重复项 - 解答
 */
#include <vector>
#include <iostream>
using namespace std;

int removeDuplicates(vector<int>& nums) {
    if (nums.empty()) return 0;
    int slow = 0;
    for (int fast = 1; fast < nums.size(); ++fast) {
        if (nums[fast] != nums[slow]) {
            nums[++slow] = nums[fast];
        }
    }
    return slow + 1;
}

int removeDuplicates2(vector<int>& nums) {
    if (nums.size() <= 2) return nums.size();
    int slow = 2;
    for (int fast = 2; fast < nums.size(); ++fast) {
        if (nums[fast] != nums[slow - 2]) {
            nums[slow++] = nums[fast];
        }
    }
    return slow;
}

int main() {
    vector<int> nums = {1, 1, 2};
    int len = removeDuplicates(nums);
    cout << "removeDuplicates([1,1,2]): " << len << " -> ";
    for (int i = 0; i < len; ++i) cout << nums[i] << " ";
    cout << "\n";

    nums = {1, 1, 1, 2, 2, 3};
    len = removeDuplicates2(nums);
    cout << "removeDuplicates2([1,1,1,2,2,3]): " << len << " -> ";
    for (int i = 0; i < len; ++i) cout << nums[i] << " ";
    cout << "\n";
    return 0;
}
