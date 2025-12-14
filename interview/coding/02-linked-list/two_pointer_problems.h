/**
 * @file two_pointer_problems.h
 * @brief 链表双指针问题 - 题目说明与声明
 *
 * 双指针是链表问题中最重要的技巧之一，包括快慢指针、前后指针等。
 */

#ifndef TWO_POINTER_PROBLEMS_H
#define TWO_POINTER_PROBLEMS_H

#include "../../util/Util.h"

namespace TwoPointerProblems {

// ==================== 题目声明 ====================

/**
 * 题目1: 链表的中间结点
 *
 * 给定一个头结点为 head 的非空单链表，返回链表的中间结点。
 * 如果有两个中间结点，则返回第二个中间结点。
 */
ListNode* middleNode(ListNode* head);
ListNode* middleNodeSolution(ListNode* head);

/**
 * 题目2: 删除链表的倒数第 N 个结点
 *
 * 给你一个链表，删除链表的倒数第 n 个结点，并返回链表的头结点。
 * 进阶: 一趟扫描实现
 */
ListNode* removeNthFromEnd(ListNode* head, int n);
ListNode* removeNthFromEndSolution(ListNode* head, int n);

/**
 * 题目3: 链表中倒数第 k 个节点
 *
 * 输入一个链表，输出该链表中倒数第 k 个节点（从 1 开始计数）。
 */
ListNode* getKthFromEnd(ListNode* head, int k);
ListNode* getKthFromEndSolution(ListNode* head, int k);

/**
 * 题目4: 回文链表
 *
 * 判断链表是否为回文链表。
 * 进阶: O(n) 时间复杂度和 O(1) 空间复杂度
 */
bool isPalindrome(ListNode* head);
bool isPalindromeSolution(ListNode* head);

/**
 * 题目5: 重排链表
 *
 * 将 L0→L1→…→Ln-1→Ln 重排为 L0→Ln→L1→Ln-1→L2→Ln-2→…
 */
void reorderList(ListNode* head);
void reorderListSolution(ListNode* head);

/**
 * 题目6: 分隔链表
 *
 * 使所有小于 x 的节点都出现在大于或等于 x 的节点之前，保持相对顺序。
 */
ListNode* partition(ListNode* head, int x);
ListNode* partitionSolution(ListNode* head, int x);

/**
 * 题目7: 奇偶链表
 *
 * 将奇数索引节点和偶数索引节点分别组合，奇数在前偶数在后。
 */
ListNode* oddEvenList(ListNode* head);
ListNode* oddEvenListSolution(ListNode* head);

// ==================== 测试函数声明 ====================

void testTwoPointerProblems();          // 测试面试者实现
void testTwoPointerProblemsSolution();  // 测试解答实现

// runTests 在头文件 inline 定义，调用两个测试函数
inline void runTests() {
    testTwoPointerProblems();
    testTwoPointerProblemsSolution();
}

} // namespace TwoPointerProblems

#endif // TWO_POINTER_PROBLEMS_H
