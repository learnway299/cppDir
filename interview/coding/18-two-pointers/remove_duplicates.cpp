/**
 * @file remove_duplicates.cpp
 * @brief 删除有序数组中的重复项 - 面试者实现文件
 */

#include "remove_duplicates.h"

namespace RemoveDuplicatesImpl {

int removeDuplicates(std::vector<int>& nums) {
    // TODO: 删除重复项，每个元素最多出现一次
    // 提示:
    // 1. 如果数组为空，返回 0
    // 2. 初始化 slow = 0
    // 3. 从 fast = 1 开始遍历：
    //    - 如果 nums[fast] != nums[slow]
    //    - 将 nums[fast] 写入 nums[++slow]
    // 4. 返回 slow + 1（长度）
    return 0;
}

int removeDuplicates2(std::vector<int>& nums) {
    // TODO: 删除重复项，每个元素最多出现两次
    // 提示:
    // 1. 如果数组长度 <= 2，直接返回长度
    // 2. 初始化 slow = 2
    // 3. 从 fast = 2 开始遍历：
    //    - 如果 nums[fast] != nums[slow-2]
    //    - 将 nums[fast] 写入 nums[slow++]
    // 4. 返回 slow（长度）
    return 0;
}

} // namespace RemoveDuplicatesImpl
