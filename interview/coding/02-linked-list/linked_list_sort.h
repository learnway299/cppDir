/**
 * @file linked_list_sort.h
 * @brief 链表排序专题 - 题目说明与声明
 *
 * 链表排序的特点：
 * 1. 不支持随机访问，无法使用快排的 Hoare 划分
 * 2. 插入/删除 O(1)，适合归并排序
 * 3. 需要特别注意指针操作和边界条件
 */

#ifndef LINKED_LIST_SORT_H
#define LINKED_LIST_SORT_H

#include "../../util/Util.h"
#include <vector>

namespace LinkedListSort {

// ==================== 题目声明 ====================

/**
 * 题目1: 排序链表 (LeetCode 148)
 * 进阶：O(n log n) 时间 O(1) 空间
 */
ListNode* sortList(ListNode* head);
ListNode* sortListSolution(ListNode* head);

/**
 * 题目2: 对链表进行插入排序 (LeetCode 147)
 */
ListNode* insertionSortList(ListNode* head);
ListNode* insertionSortListSolution(ListNode* head);

/**
 * 题目3: 合并K个升序链表 (LeetCode 23)
 * 方法: 优先队列 O(N log K) 或 分治归并
 */
ListNode* mergeKLists(std::vector<ListNode*>& lists);
ListNode* mergeKListsSolution(std::vector<ListNode*>& lists);

/**
 * 题目4: 链表中的第 K 大节点
 */
int findKthLargest(ListNode* head, int k);
int findKthLargestSolution(ListNode* head, int k);

/**
 * 题目5: 链表中的前 K 大节点
 */
std::vector<int> topKLargest(ListNode* head, int k);
std::vector<int> topKLargestSolution(ListNode* head, int k);

/**
 * 题目6: 链表三路划分（类似荷兰国旗）
 */
ListNode* partitionThreeWay(ListNode* head, int pivot);
ListNode* partitionThreeWaySolution(ListNode* head, int pivot);

/**
 * 题目7: 按升序排列的两个链表的交集
 */
ListNode* getIntersectionSorted(ListNode* l1, ListNode* l2);
ListNode* getIntersectionSortedSolution(ListNode* l1, ListNode* l2);

/**
 * 题目8: 删除排序链表中的重复元素 (LeetCode 83)
 */
ListNode* deleteDuplicates(ListNode* head);
ListNode* deleteDuplicatesSolution(ListNode* head);

/**
 * 题目9: 删除排序链表中的重复元素 II (LeetCode 82)
 * 删除所有重复数字的节点
 */
ListNode* deleteDuplicatesII(ListNode* head);
ListNode* deleteDuplicatesIISolution(ListNode* head);

/**
 * 题目10: 旋转链表 (LeetCode 61)
 */
ListNode* rotateRight(ListNode* head, int k);
ListNode* rotateRightSolution(ListNode* head, int k);

// ==================== 测试函数声明 ====================

void testLinkedListSort();          // 测试面试者实现
void testLinkedListSortSolution();  // 测试解答实现

// runTests 在头文件 inline 定义，调用两个测试函数
inline void runTests() {
    testLinkedListSort();
    testLinkedListSortSolution();
}

} // namespace LinkedListSort

#endif // LINKED_LIST_SORT_H
