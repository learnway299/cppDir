/**
 * @file merge_lists_solution.cpp
 * @brief 链表合并相关问题 - 参考答案
 */

#include "merge_lists.h"
#include <iostream>
#include <cassert>
#include <queue>

namespace MergeLists {

// ==================== 参考答案实现 ====================

// 题目1: 合并两个有序链表 - 迭代 O(m+n)
ListNode* mergeTwoListsSolution(ListNode* l1, ListNode* l2) {
    ListNode dummy;
    ListNode* tail = &dummy;
    while (l1 && l2) {
        if (l1->val <= l2->val) {
            tail->next = l1;
            l1 = l1->next;
        } else {
            tail->next = l2;
            l2 = l2->next;
        }
        tail = tail->next;
    }
    tail->next = l1 ? l1 : l2;
    return dummy.next;
}

// 题目2: 合并 K 个升序链表 - 优先队列 O(N log k)
ListNode* mergeKListsSolution(std::vector<ListNode*>& lists) {
    auto cmp = [](ListNode* a, ListNode* b) { return a->val > b->val; };
    std::priority_queue<ListNode*, std::vector<ListNode*>, decltype(cmp)> pq(cmp);
    for (ListNode* list : lists) {
        if (list) pq.push(list);
    }
    ListNode dummy;
    ListNode* tail = &dummy;
    while (!pq.empty()) {
        ListNode* minNode = pq.top();
        pq.pop();
        tail->next = minNode;
        tail = tail->next;
        if (minNode->next) pq.push(minNode->next);
    }
    return dummy.next;
}

// 题目3: 排序链表 - 归并排序 O(n log n)
ListNode* sortListSolution(ListNode* head) {
    if (!head || !head->next) return head;
    ListNode* slow = head;
    ListNode* fast = head->next;
    while (fast && fast->next) {
        slow = slow->next;
        fast = fast->next->next;
    }
    ListNode* mid = slow->next;
    slow->next = nullptr;
    ListNode* left = sortListSolution(head);
    ListNode* right = sortListSolution(mid);
    return mergeTwoListsSolution(left, right);
}

// 题目4: 合并两个链表（区间替换）
ListNode* mergeInBetweenSolution(ListNode* list1, int a, int b, ListNode* list2) {
    ListNode* prevA = list1;
    for (int i = 0; i < a - 1; ++i) prevA = prevA->next;
    ListNode* afterB = prevA;
    for (int i = 0; i <= b - a + 1; ++i) afterB = afterB->next;
    prevA->next = list2;
    ListNode* tail2 = list2;
    while (tail2->next) tail2 = tail2->next;
    tail2->next = afterB;
    return list1;
}

// 题目5: 两数相加 O(max(m,n))
ListNode* addTwoNumbersSolution(ListNode* l1, ListNode* l2) {
    ListNode dummy;
    ListNode* curr = &dummy;
    int carry = 0;
    while (l1 || l2 || carry) {
        int sum = carry;
        if (l1) { sum += l1->val; l1 = l1->next; }
        if (l2) { sum += l2->val; l2 = l2->next; }
        carry = sum / 10;
        curr->next = new ListNode(sum % 10);
        curr = curr->next;
    }
    return dummy.next;
}

// ==================== 测试函数 ====================

void testMergeListsSolution() {
    std::cout << "=== Merge Lists Tests (Solution) ===\n";

    // mergeTwoLists
    ListNode* l1 = Util::createList({1, 2, 4});
    ListNode* l2 = Util::createList({1, 3, 4});
    ListNode* merged = mergeTwoListsSolution(l1, l2);
    ListNode* expected = Util::createList({1, 1, 2, 3, 4, 4});
    assert(Util::compareLists(merged, expected));
    Util::deleteList(merged);
    Util::deleteList(expected);
    std::cout << "  mergeTwoListsSolution: PASSED\n";

    // mergeKLists
    std::vector<ListNode*> lists = {
        Util::createList({1, 4, 5}),
        Util::createList({1, 3, 4}),
        Util::createList({2, 6})
    };
    merged = mergeKListsSolution(lists);
    expected = Util::createList({1, 1, 2, 3, 4, 4, 5, 6});
    assert(Util::compareLists(merged, expected));
    Util::deleteList(merged);
    Util::deleteList(expected);
    std::cout << "  mergeKListsSolution: PASSED\n";

    // sortList
    ListNode* head = Util::createList({4, 2, 1, 3});
    ListNode* sorted = sortListSolution(head);
    expected = Util::createList({1, 2, 3, 4});
    assert(Util::compareLists(sorted, expected));
    Util::deleteList(sorted);
    Util::deleteList(expected);
    std::cout << "  sortListSolution: PASSED\n";

    // addTwoNumbers
    l1 = Util::createList({2, 4, 3});
    l2 = Util::createList({5, 6, 4});
    ListNode* sum = addTwoNumbersSolution(l1, l2);
    expected = Util::createList({7, 0, 8});
    assert(Util::compareLists(sum, expected));
    Util::deleteList(sum);
    Util::deleteList(expected);
    Util::deleteList(l1);
    Util::deleteList(l2);
    std::cout << "  addTwoNumbersSolution: PASSED\n";
}

} // namespace MergeLists
