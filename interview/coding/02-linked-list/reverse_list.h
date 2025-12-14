/**
 * @file reverse_list.h
 * @brief 链表反转相关问题 - 题目说明与声明
 *
 * 链表反转是面试中最常考的链表题目之一，需要掌握迭代和递归两种解法。
 */

#ifndef REVERSE_LIST_H
#define REVERSE_LIST_H

#include "../../util/Util.h"

namespace ReverseList {

// ==================== 题目声明 ====================

/**
 * 题目1: 反转链表
 *
 * 给你单链表的头节点 head，请你反转链表，并返回反转后的链表。
 *
 * 示例:
 *   输入: 1 -> 2 -> 3 -> 4 -> 5 -> NULL
 *   输出: 5 -> 4 -> 3 -> 2 -> 1 -> NULL
 *
 * 要求: 分别用迭代和递归两种方法实现
 */
ListNode* reverseList(ListNode* head);
ListNode* reverseListSolution(ListNode* head);

/**
 * 题目2: 反转链表 II（区间反转）
 *
 * 给你单链表的头指针 head 和两个整数 left 和 right，
 * 其中 left <= right。请你反转从位置 left 到位置 right 的链表节点，
 * 返回反转后的链表。（位置从 1 开始）
 *
 * 示例:
 *   输入: head = 1->2->3->4->5, left = 2, right = 4
 *   输出: 1->4->3->2->5
 */
ListNode* reverseBetween(ListNode* head, int left, int right);
ListNode* reverseBetweenSolution(ListNode* head, int left, int right);

/**
 * 题目3: K 个一组翻转链表
 *
 * 给你链表的头节点 head，每 k 个节点一组进行翻转，请你返回修改后的链表。
 * k 是一个正整数，它的值小于或等于链表的长度。
 * 如果节点总数不是 k 的整数倍，那么请将最后剩余的节点保持原有顺序。
 *
 * 示例:
 *   输入: head = 1->2->3->4->5, k = 2
 *   输出: 2->1->4->3->5
 *
 *   输入: head = 1->2->3->4->5, k = 3
 *   输出: 3->2->1->4->5
 */
ListNode* reverseKGroup(ListNode* head, int k);
ListNode* reverseKGroupSolution(ListNode* head, int k);

/**
 * 题目4: 两两交换链表中的节点
 *
 * 给你一个链表，两两交换其中相邻的节点，并返回交换后链表的头节点。
 * 你必须在不修改节点内部的值的情况下完成本题（即只能进行节点交换）。
 *
 * 示例:
 *   输入: head = 1->2->3->4
 *   输出: 2->1->4->3
 */
ListNode* swapPairs(ListNode* head);
ListNode* swapPairsSolution(ListNode* head);

// ==================== 测试函数声明 ====================

void testReverseList();          // 测试面试者实现
void testReverseListSolution();  // 测试解答实现

// runTests 在头文件 inline 定义，调用两个测试函数
inline void runTests() {
    testReverseList();
    testReverseListSolution();
}

} // namespace ReverseList

#endif // REVERSE_LIST_H
