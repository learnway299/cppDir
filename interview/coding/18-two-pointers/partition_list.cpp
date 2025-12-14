/**
 * @file partition_list.cpp
 * @brief 分隔链表 - 面试者实现文件
 */

#include "partition_list.h"

namespace PartitionListImpl {

ListNode* partition(ListNode* head, int x) {
    // TODO: 分隔链表，小于 x 的节点在前，大于等于 x 的在后
    // 提示:
    // 1. 创建两个虚拟头节点：
    //    ListNode smallDummy(0), largeDummy(0);
    //    ListNode* small = &smallDummy;
    //    ListNode* large = &largeDummy;
    // 2. 遍历原链表：
    //    - 如果 head->val < x，挂到 small 链
    //    - 否则，挂到 large 链
    // 3. 连接两个链表：
    //    - large->next = nullptr（重要：避免循环）
    //    - small->next = largeDummy.next
    // 4. 返回 smallDummy.next
    return nullptr;
}

// ==================== 辅助函数 ====================

ListNode* createList(std::initializer_list<int> values) {
    ListNode dummy(0);
    ListNode* tail = &dummy;
    for (int val : values) {
        tail->next = new ListNode(val);
        tail = tail->next;
    }
    return dummy.next;
}

void deleteList(ListNode* head) {
    while (head) {
        ListNode* next = head->next;
        delete head;
        head = next;
    }
}

bool compareList(ListNode* l1, std::initializer_list<int> expected) {
    for (int val : expected) {
        if (!l1 || l1->val != val) return false;
        l1 = l1->next;
    }
    return l1 == nullptr;
}

} // namespace PartitionListImpl
