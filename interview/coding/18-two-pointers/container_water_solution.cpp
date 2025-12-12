/**
 * @file container_water_solution.cpp
 * @brief 盛最多水的容器 - 解答
 */
#include <vector>
#include <algorithm>
#include <iostream>
using namespace std;

int maxArea(vector<int>& height) {
    int left = 0, right = height.size() - 1;
    int maxWater = 0;
    while (left < right) {
        int water = min(height[left], height[right]) * (right - left);
        maxWater = max(maxWater, water);
        if (height[left] < height[right]) {
            ++left;
        } else {
            --right;
        }
    }
    return maxWater;
}

int main() {
    vector<int> height = {1, 8, 6, 2, 5, 4, 8, 3, 7};
    cout << "maxArea([1,8,6,2,5,4,8,3,7]): " << maxArea(height) << "\n";

    height = {1, 1};
    cout << "maxArea([1,1]): " << maxArea(height) << "\n";
    return 0;
}
