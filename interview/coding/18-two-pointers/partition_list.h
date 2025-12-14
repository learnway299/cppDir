/**
 * @file partition_list.h
 * @brief 分隔链表 - 题目说明与声明
 *
 * 双指针经典问题：分隔链表为两部分
 *
 * LeetCode 86: Partition List
 */

#ifndef PARTITION_LIST_H
#define PARTITION_LIST_H

#include <initializer_list>

namespace PartitionListImpl {

struct ListNode {
    int val;
    ListNode* next;
    ListNode(int x) : val(x), next(nullptr) {}
};

/**
 * 题目: 分隔链表 (LeetCode 86)
 *
 * 给定一个链表和一个特定值 x，
 * 对链表进行分隔，使得所有小于 x 的节点都在大于或等于 x 的节点之前。
 * 保留两个分区中每个节点的初始相对位置。
 *
 * 示例:
 *   输入: head = [1,4,3,2,5,2], x = 3
 *   输出: [1,2,2,4,3,5]
 *   解释: 小于 3 的节点 [1,2,2] 在前，大于等于 3 的节点 [4,3,5] 在后
 *
 * 提示:
 *   - 创建两个虚拟头节点 smallDummy 和 largeDummy
 *   - 遍历原链表，小于 x 的挂到 small 链，其他挂到 large 链
 *   - 最后连接两个链表：small->next = largeDummy.next
 *   - 注意：large->next 必须设为 nullptr，避免循环
 *
 * 要求:
 *   - 时间复杂度 O(n)
 *   - 空间复杂度 O(1)
 */
ListNode* partition(ListNode* head, int x);

// ==================== 辅助函数 ====================

ListNode* createList(std::initializer_list<int> values);
void deleteList(ListNode* head);
bool compareList(ListNode* l1, std::initializer_list<int> expected);

// ==================== 测试函数 ====================

void runTests();

} // namespace PartitionListImpl

#endif // PARTITION_LIST_H
