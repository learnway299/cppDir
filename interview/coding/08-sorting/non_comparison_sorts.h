/**
 * @file non_comparison_sorts.h
 * @brief 非比较排序算法实现 - 题目说明与声明
 *
 * 非比较排序：不通过比较元素大小，利用元素本身的特性排序
 * 可以突破 O(n log n) 的下界，达到 O(n) 时间复杂度
 *
 * 主要算法：
 * - 计数排序 O(n + k)：适用于范围有限的整数
 * - 桶排序 O(n + k)：适用于均匀分布的数据
 * - 基数排序 O(d * n)：适用于位数有限的整数
 */

#ifndef NON_COMPARISON_SORTS_H
#define NON_COMPARISON_SORTS_H

#include <vector>
#include <string>

namespace NonComparisonSorts {

// ==================== 题目声明 ====================

/**
 * 题目1: 计数排序
 *
 * 原理：
 *   - 统计每个元素出现的次数
 *   - 根据统计信息计算每个元素的最终位置
 *   - 将元素放到正确位置
 *
 * 时间复杂度: O(n + k)，k 是数据范围
 * 空间复杂度: O(k)
 * 稳定性: 稳定
 *
 * 适用场景：数据范围不大的整数排序
 */
void countingSort(std::vector<int>& nums);
void countingSortSolution(std::vector<int>& nums);

/**
 * 题目2: 桶排序
 *
 * 原理：
 *   - 将数据分到有限数量的桶中
 *   - 每个桶内部单独排序
 *   - 将各桶的数据合并
 *
 * 时间复杂度: O(n + k) 平均，O(n²) 最坏
 * 空间复杂度: O(n + k)
 * 稳定性: 取决于桶内排序算法
 *
 * 适用场景：数据均匀分布
 */
void bucketSort(std::vector<float>& nums);
void bucketSortSolution(std::vector<float>& nums);

/**
 * 题目3: 基数排序
 *
 * 原理：
 *   - 从最低位开始，按每一位进行稳定排序
 *   - 重复直到最高位
 *
 * 时间复杂度: O(d * n)，d 是最大位数
 * 空间复杂度: O(n + k)，k 是基数（通常为 10）
 * 稳定性: 稳定
 *
 * 适用场景：整数、字符串排序
 */
void radixSort(std::vector<int>& nums);
void radixSortSolution(std::vector<int>& nums);

/**
 * 题目4: 最大间距
 * LeetCode 164
 *
 * 给定一个无序的数组 nums，返回数组在排序之后，相邻元素之间最大的差值。
 * 如果数组元素个数小于 2，则返回 0。
 *
 * 要求：时间复杂度 O(n)，空间复杂度 O(n)
 *
 * 示例:
 *   输入: nums = [3,6,9,1]
 *   输出: 3
 *   解释: 排序后的数组是 [1,3,6,9]，最大间距是 9-6=3
 *
 * 思路:
 *   - 使用桶排序思想
 *   - 将数据分到 n 个桶中
 *   - 最大间距一定出现在不同桶之间（桶内间距 < 平均间距）
 */
int maximumGap(std::vector<int>& nums);
int maximumGapSolution(std::vector<int>& nums);

/**
 * 题目5: 数组中的第K个最大元素（线性时间）
 * LeetCode 215
 *
 * 给定整数数组 nums 和整数 k，请返回数组中第 k 个最大的元素。
 * 要求：平均时间复杂度 O(n)
 *
 * 示例:
 *   输入: nums = [3,2,1,5,6,4], k = 2
 *   输出: 5
 *
 * 思路:
 *   - 方法1：快速选择（QuickSelect）
 *   - 方法2：BFPRT 算法（最坏 O(n)）
 */
int findKthLargest(std::vector<int>& nums, int k);
int findKthLargestSolution(std::vector<int>& nums, int k);

/**
 * 题目6: 按奇偶排序数组
 * LeetCode 905
 *
 * 给你一个整数数组 nums，将 nums 中的的所有偶数元素移动到数组的前面，
 * 后跟所有奇数元素。
 *
 * 示例:
 *   输入: nums = [3,1,2,4]
 *   输出: [2,4,3,1]（[4,2,1,3] 或其他也正确）
 *
 * 思路: 双指针或计数排序思想
 */
std::vector<int> sortArrayByParity(std::vector<int>& nums);
std::vector<int> sortArrayByParitySolution(std::vector<int>& nums);

/**
 * 题目7: 按奇偶排序数组 II
 * LeetCode 922
 *
 * 给定一个非负整数数组 nums，nums 中一半整数是奇数，一半整数是偶数。
 * 对数组进行排序，以便当 nums[i] 为奇数时，i 也是奇数；
 * 当 nums[i] 为偶数时，i 也是偶数。
 *
 * 示例:
 *   输入: nums = [4,2,5,7]
 *   输出: [4,5,2,7]
 */
std::vector<int> sortArrayByParityII(std::vector<int>& nums);
std::vector<int> sortArrayByParityIISolution(std::vector<int>& nums);

/**
 * 题目8: H 指数
 * LeetCode 274
 *
 * 给你一个整数数组 citations，其中 citations[i] 表示研究者的第 i 篇论文被引用的次数。
 * 计算并返回该研究者的 h 指数。
 *
 * h 指数的定义：h 代表"高引用次数"，
 * 一名科研人员的 h 指数是指他（她）至少有 h 篇论文分别被引用了至少 h 次。
 *
 * 示例:
 *   输入: citations = [3,0,6,1,5]
 *   输出: 3
 *   解释: 至少有 3 篇论文被引用了至少 3 次
 *
 * 思路: 计数排序，从高到低累计
 */
int hIndex(std::vector<int>& citations);
int hIndexSolution(std::vector<int>& citations);

/**
 * 题目9: 根据字符出现频率排序
 * LeetCode 451
 *
 * 给定一个字符串 s，根据字符出现的频率对其进行降序排序。
 * 字符串中字符的频率是其出现的次数。
 *
 * 示例:
 *   输入: s = "tree"
 *   输出: "eert" 或 "eetr"
 *
 * 思路: 计数 + 桶排序
 */
std::string frequencySort(std::string s);
std::string frequencySortSolution(std::string s);

/**
 * 题目10: 排序颜色
 * LeetCode 75
 *
 * 给定一个包含红色、白色和蓝色、共 n 个元素的数组 nums，
 * 原地对它们进行排序，使得相同颜色的元素相邻，
 * 并按照红色、白色、蓝色顺序排列。
 *
 * 使用整数 0、1 和 2 分别表示红色、白色和蓝色。
 *
 * 要求：不使用排序函数，一趟扫描完成
 *
 * 示例:
 *   输入: nums = [2,0,2,1,1,0]
 *   输出: [0,0,1,1,2,2]
 *
 * 思路: 荷兰国旗问题，三路划分
 */
void sortColors(std::vector<int>& nums);
void sortColorsSolution(std::vector<int>& nums);

/**
 * 题目11: 有效的字母异位词分组
 * LeetCode 49 变体
 *
 * 给定一个字符串数组，将字母异位词组合在一起。
 * 字母异位词指字母相同，但排列不同的字符串。
 *
 * 示例:
 *   输入: strs = ["eat","tea","tan","ate","nat","bat"]
 *   输出: [["bat"],["nat","tan"],["ate","eat","tea"]]
 *
 * 思路: 使用计数排序生成字符串的标准化表示
 */
std::vector<std::vector<std::string>> groupAnagrams(std::vector<std::string>& strs);
std::vector<std::vector<std::string>> groupAnagramsSolution(std::vector<std::string>& strs);

// ==================== 测试函数声明 ====================

void testNonComparisonSorts();          // 测试面试者实现
void testNonComparisonSortsSolution();  // 测试解答实现

// runTests 在头文件 inline 定义，调用两个测试函数
inline void runTests() {
    testNonComparisonSorts();
    testNonComparisonSortsSolution();
}

} // namespace NonComparisonSorts

#endif // NON_COMPARISON_SORTS_H
