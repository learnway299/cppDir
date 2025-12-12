/**
 * @file trapping_rain_solution.cpp
 * @brief 接雨水 - 解答
 */
#include <vector>
#include <algorithm>
#include <iostream>
#include <stack>
using namespace std;

// 双指针法
int trap(vector<int>& height) {
    if (height.empty()) return 0;
    int left = 0, right = height.size() - 1;
    int leftMax = 0, rightMax = 0;
    int water = 0;

    while (left < right) {
        if (height[left] < height[right]) {
            if (height[left] >= leftMax) {
                leftMax = height[left];
            } else {
                water += leftMax - height[left];
            }
            ++left;
        } else {
            if (height[right] >= rightMax) {
                rightMax = height[right];
            } else {
                water += rightMax - height[right];
            }
            --right;
        }
    }
    return water;
}

// 单调栈法
int trapStack(vector<int>& height) {
    stack<int> st;
    int water = 0;
    for (int i = 0; i < height.size(); ++i) {
        while (!st.empty() && height[i] > height[st.top()]) {
            int top = st.top();
            st.pop();
            if (st.empty()) break;
            int width = i - st.top() - 1;
            int h = min(height[i], height[st.top()]) - height[top];
            water += width * h;
        }
        st.push(i);
    }
    return water;
}

int main() {
    vector<int> height = {0, 1, 0, 2, 1, 0, 1, 3, 2, 1, 2, 1};
    cout << "trap([0,1,0,2,1,0,1,3,2,1,2,1]): " << trap(height) << "\n";

    height = {4, 2, 0, 3, 2, 5};
    cout << "trap([4,2,0,3,2,5]): " << trap(height) << "\n";
    return 0;
}
