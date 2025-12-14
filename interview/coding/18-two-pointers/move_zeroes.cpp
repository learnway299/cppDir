/**
 * @file move_zeroes.cpp
 * @brief 移动零 - 面试者实现文件
 */

#include "move_zeroes.h"
#include <algorithm>

namespace MoveZeroesImpl {

void moveZeroes(std::vector<int>& nums) {
    // TODO: 将所有 0 移到数组末尾，保持非零元素相对顺序
    // 提示:
    // 1. 初始化 slow = 0
    // 2. 从 fast = 0 开始遍历：
    //    - 如果 nums[fast] != 0
    //    - 交换 nums[slow] 和 nums[fast]
    //    - slow++
    // 3. 遍历结束后，[0, slow) 是非零元素，[slow, n) 是零
}

} // namespace MoveZeroesImpl
