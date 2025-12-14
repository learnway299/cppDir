/**
 * @file merge_lists.h
 * @brief 链表合并相关问题 - 题目说明与声明
 *
 * 链表合并是常见的面试题，涉及双指针、分治、堆等多种技巧。
 */

#ifndef MERGE_LISTS_H
#define MERGE_LISTS_H

#include "../../util/Util.h"
#include <vector>

namespace MergeLists {

// ==================== 题目声明 ====================

/**
 * 题目1: 合并两个有序链表
 *
 * 将两个升序链表合并为一个新的升序链表并返回。
 *
 * 示例:
 *   输入: l1 = 1->2->4, l2 = 1->3->4
 *   输出: 1->1->2->3->4->4
 */
ListNode* mergeTwoLists(ListNode* l1, ListNode* l2);
ListNode* mergeTwoListsSolution(ListNode* l1, ListNode* l2);

/**
 * 题目2: 合并 K 个升序链表
 *
 * 给你一个链表数组，每个链表都已经按升序排列。
 * 请你将所有链表合并到一个升序链表中，返回合并后的链表。
 *
 * 要求: 时间复杂度优于 O(k*N)，使用分治法或优先队列
 */
ListNode* mergeKLists(std::vector<ListNode*>& lists);
ListNode* mergeKListsSolution(std::vector<ListNode*>& lists);

/**
 * 题目3: 排序链表
 *
 * 给你链表的头结点 head，请将其按升序排列并返回。
 * 进阶: O(n log n) 时间复杂度和常数级空间复杂度
 */
ListNode* sortList(ListNode* head);
ListNode* sortListSolution(ListNode* head);

/**
 * 题目4: 合并两个链表（区间替换）
 *
 * 将 list1 中从第 a 个到第 b 个节点删除，将 list2 接在该位置。
 */
ListNode* mergeInBetween(ListNode* list1, int a, int b, ListNode* list2);
ListNode* mergeInBetweenSolution(ListNode* list1, int a, int b, ListNode* list2);

/**
 * 题目5: 两数相加
 *
 * 两个逆序存储的链表表示两个整数，返回它们的和（同样逆序存储）。
 *
 * 示例:
 *   输入: l1 = 2->4->3 (342), l2 = 5->6->4 (465)
 *   输出: 7->0->8 (807)
 */
ListNode* addTwoNumbers(ListNode* l1, ListNode* l2);
ListNode* addTwoNumbersSolution(ListNode* l1, ListNode* l2);

// ==================== 测试函数声明 ====================

void testMergeLists();          // 测试面试者实现
void testMergeListsSolution();  // 测试解答实现

// runTests 在头文件 inline 定义，调用两个测试函数
inline void runTests() {
    testMergeLists();
    testMergeListsSolution();
}

} // namespace MergeLists

#endif // MERGE_LISTS_H
