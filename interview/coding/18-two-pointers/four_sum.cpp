/**
 * @file four_sum.cpp
 * @brief 四数之和 - 面试者实现文件
 */

#include "four_sum.h"

namespace FourSumImpl {

std::vector<std::vector<int>> fourSum(std::vector<int>& nums, int target) {
    // TODO: 实现四数之和
    // 提示:
    // 1. 先对数组排序
    // 2. 外层两重循环固定前两个数 i, j
    // 3. 内层使用双指针 left, right 寻找剩余两数
    // 4. 去重：
    //    - 第一层：i > 0 && nums[i] == nums[i-1] continue
    //    - 第二层：j > i+1 && nums[j] == nums[j-1] continue
    //    - 双指针：找到解后跳过相同元素
    // 5. 注意溢出：使用 long long 计算 sum
    return {};
}

} // namespace FourSumImpl
