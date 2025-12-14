/**
 * @file merge_k_lists.h
 * @brief K 路归并问题集合 - 题目说明与声明
 *
 * K 路归并是排序和堆的经典应用场景
 * 核心思想：使用最小堆维护 K 个有序序列的当前最小值
 */

#ifndef MERGE_K_LISTS_H
#define MERGE_K_LISTS_H

#include <vector>
#include <string>

namespace MergeKLists {

// ==================== 链表节点定义 ====================

struct ListNode {
    int val;
    ListNode* next;
    ListNode() : val(0), next(nullptr) {}
    ListNode(int x) : val(x), next(nullptr) {}
    ListNode(int x, ListNode* next) : val(x), next(next) {}
};

// ==================== 题目声明 ====================

/**
 * 题目1: 合并K个升序链表
 * LeetCode 23
 *
 * 给你一个链表数组，每个链表都已经按升序排列。
 * 请你将所有链表合并到一个升序链表中，返回合并后的链表。
 *
 * 示例:
 *   输入: lists = [[1,4,5],[1,3,4],[2,6]]
 *   输出: [1,1,2,3,4,4,5,6]
 *
 * 思路:
 *   - 方法1：优先队列（最小堆）O(N log K)
 *   - 方法2：分治归并 O(N log K)
 *   - 方法3：逐一合并 O(NK)
 */
ListNode* mergeKLists(std::vector<ListNode*>& lists);
ListNode* mergeKListsSolution(std::vector<ListNode*>& lists);

/**
 * 题目2: 合并两个有序数组
 * LeetCode 88
 *
 * 给你两个按非递减顺序排列的整数数组 nums1 和 nums2，
 * 另有两个整数 m 和 n，分别表示 nums1 和 nums2 中的元素数目。
 * 请你合并 nums2 到 nums1 中，使合并后的数组同样按非递减顺序排列。
 *
 * 注意：nums1 的长度为 m + n，后 n 个元素为 0，应忽略。
 *
 * 示例:
 *   输入: nums1 = [1,2,3,0,0,0], m = 3, nums2 = [2,5,6], n = 3
 *   输出: [1,2,2,3,5,6]
 *
 * 思路: 从后往前填充，避免覆盖
 */
void merge(std::vector<int>& nums1, int m, std::vector<int>& nums2, int n);
void mergeSolution(std::vector<int>& nums1, int m, std::vector<int>& nums2, int n);

/**
 * 题目3: 合并K个有序数组
 *
 * 给你 K 个有序数组，将它们合并成一个有序数组。
 *
 * 示例:
 *   输入: arrays = [[1,4,5],[1,3,4],[2,6]]
 *   输出: [1,1,2,3,4,4,5,6]
 *
 * 思路: 最小堆存储 {值, 数组索引, 元素索引}
 */
std::vector<int> mergeKArrays(std::vector<std::vector<int>>& arrays);
std::vector<int> mergeKArraysSolution(std::vector<std::vector<int>>& arrays);

/**
 * 题目4: 合并区间
 * LeetCode 56
 *
 * 以数组 intervals 表示若干个区间的集合，其中单个区间为 intervals[i] = [starti, endi]。
 * 请你合并所有重叠的区间，并返回一个不重叠的区间数组。
 *
 * 示例:
 *   输入: intervals = [[1,3],[2,6],[8,10],[15,18]]
 *   输出: [[1,6],[8,10],[15,18]]
 *
 * 思路: 按起点排序，然后顺序合并
 */
std::vector<std::vector<int>> mergeIntervals(std::vector<std::vector<int>>& intervals);
std::vector<std::vector<int>> mergeIntervalsSolution(std::vector<std::vector<int>>& intervals);

/**
 * 题目5: 插入区间
 * LeetCode 57
 *
 * 给你一个无重叠的、按照区间起始端点排序的区间列表。
 * 在列表中插入一个新的区间，你需要确保列表中的区间仍然有序且不重叠。
 *
 * 示例:
 *   输入: intervals = [[1,3],[6,9]], newInterval = [2,5]
 *   输出: [[1,5],[6,9]]
 *
 * 思路: 分三部分处理：左边不重叠、重叠合并、右边不重叠
 */
std::vector<std::vector<int>> insert(std::vector<std::vector<int>>& intervals,
                                      std::vector<int>& newInterval);
std::vector<std::vector<int>> insertSolution(std::vector<std::vector<int>>& intervals,
                                              std::vector<int>& newInterval);

/**
 * 题目6: 外部排序
 *
 * 模拟外部排序：当数据量超过内存限制时，使用多路归并进行排序。
 *
 * 步骤：
 * 1. 将大文件分成多个小块，每块在内存中排序
 * 2. 将排好序的小块写入临时文件
 * 3. 使用 K 路归并合并所有临时文件
 *
 * 实现一个简化版本：给定多个已排序的数据块，合并成一个有序数组。
 */
std::vector<int> externalSort(std::vector<std::vector<int>>& chunks);
std::vector<int> externalSortSolution(std::vector<std::vector<int>>& chunks);

/**
 * 题目7: 找出第 K 小的数对距离
 * LeetCode 719
 *
 * 数对 (a, b) 由整数 a 和 b 组成，其数对距离定义为 a 和 b 的绝对差值。
 * 给你一个整数数组 nums 和一个整数 k，返回所有数对距离中第 k 小的数对距离。
 *
 * 示例:
 *   输入: nums = [1,3,1], k = 1
 *   输出: 0
 *   解释: 所有数对距离为 (1,3)->2, (1,1)->0, (3,1)->2
 *
 * 思路: 二分答案 + 双指针统计
 */
int smallestDistancePair(std::vector<int>& nums, int k);
int smallestDistancePairSolution(std::vector<int>& nums, int k);

/**
 * 题目8: 第 K 个最小的乘法表数
 * LeetCode 668
 *
 * 几乎每一个人都用乘法表。但是你能在乘法表中快速找到第 k 小的数字吗？
 * 乘法表是大小为 m x n 的表格，其中每个格子 (i, j) 为 i * j。
 *
 * 示例:
 *   输入: m = 3, n = 3, k = 5
 *   输出: 3
 *   解释: 乘法表 [[1,2,3],[2,4,6],[3,6,9]]，第 5 小是 3
 *
 * 思路: 二分答案，统计 <= mid 的数的个数
 */
int findKthNumber(int m, int n, int k);
int findKthNumberSolution(int m, int n, int k);

/**
 * 题目9: 有序矩阵中的第 K 小的元素
 * LeetCode 378
 *
 * 给你一个 n x n 矩阵 matrix，其中每行和每列元素均按升序排序，
 * 找到矩阵中第 k 小的元素。
 *
 * 示例:
 *   输入: matrix = [[1,5,9],[10,11,13],[12,13,15]], k = 8
 *   输出: 13
 *
 * 思路:
 *   - 方法1：K 路归并
 *   - 方法2：二分查找
 */
int kthSmallestInMatrix(std::vector<std::vector<int>>& matrix, int k);
int kthSmallestInMatrixSolution(std::vector<std::vector<int>>& matrix, int k);

/**
 * 题目10: 最小区间
 * LeetCode 632
 *
 * 你有 k 个非递减排列的整数列表。找到一个最小区间，
 * 使得 k 个列表中的每个列表至少有一个数包含在其中。
 *
 * 示例:
 *   输入: nums = [[4,10,15,24,26], [0,9,12,20], [5,18,22,30]]
 *   输出: [20,24]
 *
 * 思路: 使用最小堆，同时跟踪当前窗口的最大值
 */
std::vector<int> smallestRange(std::vector<std::vector<int>>& nums);
std::vector<int> smallestRangeSolution(std::vector<std::vector<int>>& nums);

/**
 * 附加题: 丑数 III
 * LeetCode 1201
 *
 * 给你四个整数 n, a, b, c，返回第 n 个丑数。
 * 丑数是能被 a 或 b 或 c 整除的正整数。
 *
 * 思路: 二分 + 容斥原理
 */
int nthUglyNumber3(int n, int a, int b, int c);
int nthUglyNumber3Solution(int n, int a, int b, int c);

// ==================== 辅助函数 ====================

ListNode* createList(const std::vector<int>& vals);
std::vector<int> listToVector(ListNode* head);
void freeList(ListNode* head);

// ==================== 测试函数声明 ====================

void testMergeKLists();          // 测试面试者实现
void testMergeKListsSolution();  // 测试解答实现

// runTests 在头文件 inline 定义，调用两个测试函数
inline void runTests() {
    testMergeKLists();
    testMergeKListsSolution();
}

} // namespace MergeKLists

#endif // MERGE_K_LISTS_H
