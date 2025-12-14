/**
 * @file cycle_detection.h
 * @brief 链表环检测相关问题 - 题目说明与声明
 *
 * 链表环检测是经典的双指针问题，掌握快慢指针技巧是关键。
 */

#ifndef CYCLE_DETECTION_H
#define CYCLE_DETECTION_H

#include "../../util/Util.h"

namespace CycleDetection {

// ==================== 题目声明 ====================

/**
 * 题目1: 环形链表
 *
 * 给你一个链表的头节点 head，判断链表中是否有环。
 * 要求: 使用 O(1) 空间复杂度
 *
 * 示例:
 *   输入: head = [3,2,0,-4], pos = 1 (表示尾节点连接到下标为1的节点)
 *   输出: true
 */
bool hasCycle(ListNode* head);
bool hasCycleSolution(ListNode* head);

/**
 * 题目2: 环形链表 II
 *
 * 给定一个链表的头节点 head，返回链表开始入环的第一个节点。
 * 如果链表无环，则返回 null。
 *
 * 数学推导（面试重点）:
 *   设环前长度为 a，环长为 b
 *   快指针走的距离: 2s = s + nb (快指针比慢指针多走 n 圈)
 *   所以 s = nb
 *   入环点 = a + nb，慢指针已走 nb，再走 a 步即可到达入环点
 */
ListNode* detectCycle(ListNode* head);
ListNode* detectCycleSolution(ListNode* head);

/**
 * 题目3: 快乐数
 *
 * 编写一个算法来判断一个数 n 是不是快乐数。
 *
 * 「快乐数」定义:
 *   - 对于一个正整数，每一次将该数替换为它每个位置上的数字的平方和
 *   - 重复这个过程直到这个数变为 1，或者进入无限循环
 *   - 如果这个过程的结果为 1，则这个数是快乐数
 *
 * 提示: 可以将其看作链表环检测问题
 */
bool isHappy(int n);
bool isHappySolution(int n);

/**
 * 题目4: 相交链表
 *
 * 给你两个单链表的头节点 headA 和 headB，
 * 请你找出并返回两个单链表相交的起始节点。
 *
 * 技巧:
 *   - 指针 A 走完链表 A 后走链表 B
 *   - 指针 B 走完链表 B 后走链表 A
 *   - 两指针会同时到达相交点（或同时到达 null）
 */
ListNode* getIntersectionNode(ListNode* headA, ListNode* headB);
ListNode* getIntersectionNodeSolution(ListNode* headA, ListNode* headB);

// ==================== 测试函数声明 ====================

void testCycleDetection();          // 测试面试者实现
void testCycleDetectionSolution();  // 测试解答实现

// runTests 在头文件 inline 定义，调用两个测试函数
inline void runTests() {
    testCycleDetection();
    testCycleDetectionSolution();
}

} // namespace CycleDetection

#endif // CYCLE_DETECTION_H
