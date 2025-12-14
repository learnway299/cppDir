/**
 * @file sort_colors.cpp
 * @brief 颜色分类 - 面试者实现文件
 */

#include "sort_colors.h"
#include <algorithm>

namespace SortColorsImpl {

void sortColors(std::vector<int>& nums) {
    // TODO: 使用三指针（荷兰国旗问题）对颜色进行分类
    // 提示:
    // 1. 初始化 p0 = 0, curr = 0, p2 = size - 1
    // 2. 当 curr <= p2 时循环：
    //    - 如果 nums[curr] == 0:
    //      * 交换 nums[p0] 和 nums[curr]
    //      * p0++, curr++
    //    - 如果 nums[curr] == 1:
    //      * curr++
    //    - 如果 nums[curr] == 2:
    //      * 交换 nums[curr] 和 nums[p2]
    //      * p2--（注意：curr 不动，因为换过来的元素还未检查）
    // 3. 循环结束后，数组被分为三部分：[0,p0) 全是 0，[p0,curr) 全是 1，(p2,n-1] 全是 2
}

} // namespace SortColorsImpl
