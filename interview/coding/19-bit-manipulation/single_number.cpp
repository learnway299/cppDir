/**
 * @file single_number.cpp
 * @brief 只出现一次的数字 - 面试者实现
 */

#include "single_number.h"

namespace SingleNumberImpl {

// 题目1: 只出现一次的数字 (LeetCode 136)
// 数组中除一个数外，其他都出现两次
int singleNumber(std::vector<int>& nums) {
    // TODO: 使用异或运算实现
    return 0;
}

// 题目2: 只出现一次的数字 II (LeetCode 137)
// 其他数都出现三次
int singleNumber2(std::vector<int>& nums) {
    // TODO: 统计每一位上1的个数，对3取模
    return 0;
}

// 题目3: 只出现一次的数字 III (LeetCode 260)
// 有两个数只出现一次
std::vector<int> singleNumber3(std::vector<int>& nums) {
    // TODO: 先异或得到两数异或结果，然后分组
    return {};
}

} // namespace SingleNumberImpl
