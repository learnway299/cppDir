/**
 * @file reverse_list_solution.cpp
 * @brief 链表反转相关问题 - 参考答案
 */

#include "reverse_list.h"
#include <iostream>
#include <cassert>

namespace ReverseList {

// ==================== 参考答案实现 ====================

// 题目1: 反转链表 - 迭代 O(n)
ListNode* reverseListSolution(ListNode* head) {
    ListNode* prev = nullptr;
    ListNode* curr = head;
    while (curr) {
        ListNode* next = curr->next;
        curr->next = prev;
        prev = curr;
        curr = next;
    }
    return prev;
}

// 题目2: 反转链表 II（区间反转）
ListNode* reverseBetweenSolution(ListNode* head, int left, int right) {
    if (!head || left == right) return head;
    ListNode dummy(0, head);
    ListNode* prev = &dummy;
    for (int i = 1; i < left; ++i) prev = prev->next;
    ListNode* curr = prev->next;
    for (int i = 0; i < right - left; ++i) {
        ListNode* next = curr->next;
        curr->next = next->next;
        next->next = prev->next;
        prev->next = next;
    }
    return dummy.next;
}

// 题目3: K 个一组翻转链表
ListNode* reverseKGroupSolution(ListNode* head, int k) {
    if (!head || k == 1) return head;
    ListNode dummy(0, head);
    ListNode* prevGroupEnd = &dummy;
    while (true) {
        ListNode* kthNode = prevGroupEnd;
        for (int i = 0; i < k; ++i) {
            kthNode = kthNode->next;
            if (!kthNode) return dummy.next;
        }
        ListNode* nextGroupStart = kthNode->next;
        ListNode* groupStart = prevGroupEnd->next;
        ListNode* prev = nextGroupStart;
        ListNode* curr = groupStart;
        while (curr != nextGroupStart) {
            ListNode* next = curr->next;
            curr->next = prev;
            prev = curr;
            curr = next;
        }
        prevGroupEnd->next = kthNode;
        prevGroupEnd = groupStart;
    }
    return dummy.next;
}

// 题目4: 两两交换链表中的节点
ListNode* swapPairsSolution(ListNode* head) {
    ListNode dummy(0, head);
    ListNode* prev = &dummy;
    while (prev->next && prev->next->next) {
        ListNode* first = prev->next;
        ListNode* second = prev->next->next;
        first->next = second->next;
        second->next = first;
        prev->next = second;
        prev = first;
    }
    return dummy.next;
}

// ==================== 测试函数 ====================

void testReverseListSolution() {
    std::cout << "=== Reverse List Tests (Solution) ===\n";

    // reverseList
    ListNode* head = Util::createList({1, 2, 3, 4, 5});
    ListNode* reversed = reverseListSolution(head);
    ListNode* expected = Util::createList({5, 4, 3, 2, 1});
    assert(Util::compareLists(reversed, expected));
    Util::deleteList(reversed);
    Util::deleteList(expected);
    std::cout << "  reverseListSolution: PASSED\n";

    // reverseBetween
    head = Util::createList({1, 2, 3, 4, 5});
    ListNode* result = reverseBetweenSolution(head, 2, 4);
    expected = Util::createList({1, 4, 3, 2, 5});
    assert(Util::compareLists(result, expected));
    Util::deleteList(result);
    Util::deleteList(expected);
    std::cout << "  reverseBetweenSolution: PASSED\n";

    // reverseKGroup
    head = Util::createList({1, 2, 3, 4, 5});
    result = reverseKGroupSolution(head, 2);
    expected = Util::createList({2, 1, 4, 3, 5});
    assert(Util::compareLists(result, expected));
    Util::deleteList(result);
    Util::deleteList(expected);
    std::cout << "  reverseKGroupSolution: PASSED\n";

    // swapPairs
    head = Util::createList({1, 2, 3, 4});
    result = swapPairsSolution(head);
    expected = Util::createList({2, 1, 4, 3});
    assert(Util::compareLists(result, expected));
    Util::deleteList(result);
    Util::deleteList(expected);
    std::cout << "  swapPairsSolution: PASSED\n";
}

} // namespace ReverseList
