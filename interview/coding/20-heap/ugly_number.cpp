/**
 * @file ugly_number.cpp
 * @brief 丑数 - 面试题实现
 */

#include "ugly_number.h"

namespace UglyNumberImpl {

// ==================== 判断丑数 ====================

bool isUgly(int n) {
    // TODO: 实现判断丑数
    // 1. 检查 n 是否 <= 0（丑数必须是正整数）
    // 2. 不断除以 2, 3, 5
    // 3. 最后检查是否为 1
    return false;
}

// ==================== 第n个丑数 - 三指针法 ====================

int nthUglyNumber(int n) {
    // TODO: 实现三指针法
    // 1. 创建数组 ugly，ugly[0] = 1
    // 2. 初始化三个指针 p2, p3, p5 = 0
    // 3. 循环 n-1 次：
    //    - 计算 next2 = ugly[p2] * 2
    //    - 计算 next3 = ugly[p3] * 3
    //    - 计算 next5 = ugly[p5] * 5
    //    - ugly[i] = min(next2, next3, next5)
    //    - 如果 ugly[i] == next2，p2++
    //    - 如果 ugly[i] == next3，p3++
    //    - 如果 ugly[i] == next5，p5++
    // 4. 返回 ugly[n-1]
    return 0;
}

// ==================== 第n个丑数 - 最小堆法 ====================

int nthUglyNumberHeap(int n) {
    // TODO: 实现最小堆法
    // 1. 创建最小堆，初始值为 1
    // 2. 创建 set 用于去重
    // 3. 循环 n 次：
    //    - 取堆顶元素作为当前丑数
    //    - 弹出堆顶
    //    - 将当前丑数乘以 2, 3, 5 得到新候选
    //    - 如果新候选未出现过，加入堆和 set
    // 4. 返回第 n 次取出的丑数
    return 0;
}

} // namespace UglyNumberImpl
