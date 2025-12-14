/**
 * @file trapping_rain.cpp
 * @brief 接雨水 - 面试者实现文件
 */

#include "trapping_rain.h"

namespace TrappingRainImpl {

int trap(std::vector<int>& height) {
    // TODO: 使用双指针法计算能接的雨水量
    // 提示:
    // 1. 初始化 left = 0, right = size - 1
    // 2. 初始化 leftMax = 0, rightMax = 0, water = 0
    // 3. 当 left < right 时循环：
    //    - 如果 height[left] < height[right]:
    //      * 如果 height[left] >= leftMax: 更新 leftMax
    //      * 否则: water += leftMax - height[left]
    //      * left++
    //    - 否则：
    //      * 如果 height[right] >= rightMax: 更新 rightMax
    //      * 否则: water += rightMax - height[right]
    //      * right--
    // 4. 返回 water
    return 0;
}

int trapStack(std::vector<int>& height) {
    // TODO: 使用单调栈法计算能接的雨水量（选做）
    // 提示:
    // 1. 维护一个递减的单调栈（存储索引）
    // 2. 遍历每个柱子：
    //    - 当栈非空且当前柱子高度 > 栈顶柱子高度时：
    //      * 弹出栈顶（凹槽底部）
    //      * 如果栈为空，break
    //      * 计算宽度 = i - 栈顶 - 1
    //      * 计算高度 = min(当前柱子, 栈顶柱子) - 凹槽底部
    //      * 累加水量 = 宽度 * 高度
    //    - 压入当前索引
    // 3. 返回总水量
    return 0;
}

} // namespace TrappingRainImpl
