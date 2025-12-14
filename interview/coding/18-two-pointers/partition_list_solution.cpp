/**
 * @file partition_list_solution.cpp
 * @brief 分隔链表 - 参考答案
 */

#include "partition_list.h"
#include <iostream>
#include <cassert>

namespace PartitionListImpl {

namespace Solution {

ListNode* partition(ListNode* head, int x) {
    ListNode smallDummy(0), largeDummy(0);
    ListNode* small = &smallDummy;
    ListNode* large = &largeDummy;

    while (head) {
        if (head->val < x) {
            small->next = head;
            small = small->next;
        } else {
            large->next = head;
            large = large->next;
        }
        head = head->next;
    }

    large->next = nullptr;  // 避免循环
    small->next = largeDummy.next;  // 连接两个链表
    return smallDummy.next;
}

} // namespace Solution

// ==================== 测试函数 ====================

void runTests() {
    std::cout << "=== Partition List Tests ===" << std::endl;

    {
        ListNode* head = createList({1, 4, 3, 2, 5, 2});
        head = Solution::partition(head, 3);
        assert(compareList(head, {1, 2, 2, 4, 3, 5}));
        deleteList(head);
    }
    std::cout << "  Partition List (basic): PASSED" << std::endl;

    {
        ListNode* head = createList({2, 1});
        head = Solution::partition(head, 2);
        assert(compareList(head, {1, 2}));
        deleteList(head);
    }
    std::cout << "  Partition List (simple): PASSED" << std::endl;

    {
        ListNode* head = createList({1});
        head = Solution::partition(head, 0);
        assert(compareList(head, {1}));
        deleteList(head);
    }
    std::cout << "  Partition List (single): PASSED" << std::endl;

    {
        ListNode* head = createList({3, 1, 2});
        head = Solution::partition(head, 3);
        assert(compareList(head, {1, 2, 3}));
        deleteList(head);
    }
    std::cout << "  Partition List (boundary): PASSED" << std::endl;
}

} // namespace PartitionListImpl

/**
 * 关键要点：
 *
 * 1. 双链表分隔策略：
 *    - 创建两个虚拟头节点：smallDummy 和 largeDummy
 *    - small 链：收集所有 < x 的节点
 *    - large 链：收集所有 >= x 的节点
 *    - 最后连接两个链表
 *
 * 2. 遍历过程：
 *    - 遍历原链表的每个节点
 *    - 根据节点值与 x 的关系，决定挂到哪个链
 *    - small 和 large 指针不断后移
 *
 * 3. 连接两个链表的关键点：
 *    - large->next = nullptr（必须！避免循环）
 *    - small->next = largeDummy.next（连接两个链表）
 *    - 返回 smallDummy.next
 *
 * 4. 为什么要设置 large->next = nullptr：
 *    - large 链的最后一个节点可能原本指向其他节点
 *    - 如果不切断，会形成循环或包含不应该包含的节点
 *    - 例如：[1,4,3,2,5,2] x=3，如果不设置 nullptr
 *      large 链最后一个节点 5 可能还指向 2
 *
 * 5. 保持相对顺序：
 *    - 原链表中小于 x 的元素顺序不变
 *    - 原链表中大于等于 x 的元素顺序不变
 *    - 因为我们是按遍历顺序依次挂到两个链表
 *
 * 6. 虚拟头节点的好处：
 *    - 简化边界条件处理
 *    - 不需要特殊处理空链表
 *    - 不需要特殊处理第一个节点
 *
 * 7. 时间复杂度：O(n)
 *    - 遍历原链表一次
 *
 * 8. 空间复杂度：O(1)
 *    - 只使用常数额外空间
 *    - 没有创建新节点，只是重新组织原有节点
 *
 * 9. 推广应用：
 *    - 奇偶链表（LeetCode 328）
 *    - 重排链表（LeetCode 143）
 *    - 任何需要将链表分为两部分的问题
 */
