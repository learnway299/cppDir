/**
 * @file three_sum_solution.cpp
 * @brief 三数之和 - 解答
 */
#include <vector>
#include <algorithm>
#include <iostream>
using namespace std;

vector<vector<int>> threeSum(vector<int>& nums) {
    vector<vector<int>> result;
    sort(nums.begin(), nums.end());
    int n = nums.size();

    for (int i = 0; i < n - 2; ++i) {
        if (i > 0 && nums[i] == nums[i - 1]) continue;
        if (nums[i] > 0) break;

        int left = i + 1, right = n - 1;
        while (left < right) {
            int sum = nums[i] + nums[left] + nums[right];
            if (sum == 0) {
                result.push_back({nums[i], nums[left], nums[right]});
                while (left < right && nums[left] == nums[left + 1]) ++left;
                while (left < right && nums[right] == nums[right - 1]) --right;
                ++left;
                --right;
            } else if (sum < 0) {
                ++left;
            } else {
                --right;
            }
        }
    }
    return result;
}

int threeSumClosest(vector<int>& nums, int target) {
    sort(nums.begin(), nums.end());
    int n = nums.size();
    int closest = nums[0] + nums[1] + nums[2];

    for (int i = 0; i < n - 2; ++i) {
        int left = i + 1, right = n - 1;
        while (left < right) {
            int sum = nums[i] + nums[left] + nums[right];
            if (abs(sum - target) < abs(closest - target)) {
                closest = sum;
            }
            if (sum < target) ++left;
            else if (sum > target) --right;
            else return target;
        }
    }
    return closest;
}

int main() {
    vector<int> nums = {-1, 0, 1, 2, -1, -4};
    auto result = threeSum(nums);
    cout << "threeSum([-1,0,1,2,-1,-4]):\n";
    for (auto& v : result) {
        cout << "[" << v[0] << "," << v[1] << "," << v[2] << "]\n";
    }

    nums = {-1, 2, 1, -4};
    cout << "\nthreeSumClosest([-1,2,1,-4], 1): " << threeSumClosest(nums, 1) << "\n";
    return 0;
}
