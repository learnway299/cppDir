/**
 * @file sort_colors_solution.cpp
 * @brief 颜色分类 - 解答
 */
#include <vector>
#include <iostream>
using namespace std;

// 三指针（荷兰国旗）
void sortColors(vector<int>& nums) {
    int p0 = 0, curr = 0, p2 = nums.size() - 1;
    while (curr <= p2) {
        if (nums[curr] == 0) {
            swap(nums[p0++], nums[curr++]);
        } else if (nums[curr] == 2) {
            swap(nums[curr], nums[p2--]);
        } else {
            ++curr;
        }
    }
}

int main() {
    vector<int> nums = {2, 0, 2, 1, 1, 0};
    cout << "Before: ";
    for (int n : nums) cout << n << " ";

    sortColors(nums);

    cout << "\nAfter:  ";
    for (int n : nums) cout << n << " ";
    cout << "\n";
    return 0;
}
