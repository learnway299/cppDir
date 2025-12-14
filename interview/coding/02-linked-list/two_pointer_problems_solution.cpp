/**
 * @file two_pointer_problems_solution.cpp
 * @brief 链表双指针问题 - 参考答案
 */

#include "two_pointer_problems.h"
#include <iostream>
#include <cassert>

namespace TwoPointerProblems {

// ==================== 辅助函数 ====================

static ListNode* reverseList(ListNode* head) {
    ListNode* prev = nullptr;
    while (head) {
        ListNode* next = head->next;
        head->next = prev;
        prev = head;
        head = next;
    }
    return prev;
}

// ==================== 参考答案实现 ====================

// 题目1: 链表的中间结点 - 快慢指针 O(n)
ListNode* middleNodeSolution(ListNode* head) {
    ListNode* slow = head;
    ListNode* fast = head;
    while (fast && fast->next) {
        slow = slow->next;
        fast = fast->next->next;
    }
    return slow;
}

// 题目2: 删除链表的倒数第 N 个结点
ListNode* removeNthFromEndSolution(ListNode* head, int n) {
    ListNode dummy(0, head);
    ListNode* fast = &dummy;
    ListNode* slow = &dummy;
    for (int i = 0; i <= n; ++i) fast = fast->next;
    while (fast) {
        slow = slow->next;
        fast = fast->next;
    }
    ListNode* toDelete = slow->next;
    slow->next = slow->next->next;
    delete toDelete;
    return dummy.next;
}

// 题目3: 链表中倒数第 k 个节点
ListNode* getKthFromEndSolution(ListNode* head, int k) {
    ListNode* fast = head;
    ListNode* slow = head;
    for (int i = 0; i < k; ++i) fast = fast->next;
    while (fast) {
        slow = slow->next;
        fast = fast->next;
    }
    return slow;
}

// 题目4: 回文链表 - 快慢指针+反转后半部分
bool isPalindromeSolution(ListNode* head) {
    if (!head || !head->next) return true;
    ListNode* slow = head;
    ListNode* fast = head;
    while (fast->next && fast->next->next) {
        slow = slow->next;
        fast = fast->next->next;
    }
    ListNode* secondHalf = reverseList(slow->next);
    ListNode* p1 = head;
    ListNode* p2 = secondHalf;
    bool result = true;
    while (p2) {
        if (p1->val != p2->val) { result = false; break; }
        p1 = p1->next;
        p2 = p2->next;
    }
    slow->next = reverseList(secondHalf);
    return result;
}

// 题目5: 重排链表
void reorderListSolution(ListNode* head) {
    if (!head || !head->next || !head->next->next) return;
    ListNode* slow = head;
    ListNode* fast = head;
    while (fast->next && fast->next->next) {
        slow = slow->next;
        fast = fast->next->next;
    }
    ListNode* second = reverseList(slow->next);
    slow->next = nullptr;
    ListNode* first = head;
    while (second) {
        ListNode* tmp1 = first->next;
        ListNode* tmp2 = second->next;
        first->next = second;
        second->next = tmp1;
        first = tmp1;
        second = tmp2;
    }
}

// 题目6: 分隔链表
ListNode* partitionSolution(ListNode* head, int x) {
    ListNode smallDummy, largeDummy;
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
    large->next = nullptr;
    small->next = largeDummy.next;
    return smallDummy.next;
}

// 题目7: 奇偶链表
ListNode* oddEvenListSolution(ListNode* head) {
    if (!head || !head->next) return head;
    ListNode* odd = head;
    ListNode* even = head->next;
    ListNode* evenHead = even;
    while (even && even->next) {
        odd->next = even->next;
        odd = odd->next;
        even->next = odd->next;
        even = even->next;
    }
    odd->next = evenHead;
    return head;
}

// ==================== 测试函数 ====================

void testTwoPointerProblemsSolution() {
    std::cout << "=== Two Pointer Problems Tests (Solution) ===\n";

    // middleNode
    ListNode* head = Util::createList({1, 2, 3, 4, 5});
    assert(middleNodeSolution(head)->val == 3);
    Util::deleteList(head);
    std::cout << "  middleNodeSolution: PASSED\n";

    // removeNthFromEnd
    head = Util::createList({1, 2, 3, 4, 5});
    ListNode* result = removeNthFromEndSolution(head, 2);
    ListNode* expected = Util::createList({1, 2, 3, 5});
    assert(Util::compareLists(result, expected));
    Util::deleteList(result);
    Util::deleteList(expected);
    std::cout << "  removeNthFromEndSolution: PASSED\n";

    // getKthFromEnd
    head = Util::createList({1, 2, 3, 4, 5});
    assert(getKthFromEndSolution(head, 2)->val == 4);
    Util::deleteList(head);
    std::cout << "  getKthFromEndSolution: PASSED\n";

    // isPalindrome
    head = Util::createList({1, 2, 2, 1});
    assert(isPalindromeSolution(head) == true);
    Util::deleteList(head);
    head = Util::createList({1, 2});
    assert(isPalindromeSolution(head) == false);
    Util::deleteList(head);
    std::cout << "  isPalindromeSolution: PASSED\n";

    // reorderList
    head = Util::createList({1, 2, 3, 4});
    reorderListSolution(head);
    expected = Util::createList({1, 4, 2, 3});
    assert(Util::compareLists(head, expected));
    Util::deleteList(head);
    Util::deleteList(expected);
    std::cout << "  reorderListSolution: PASSED\n";

    // partition
    head = Util::createList({1, 4, 3, 2, 5, 2});
    result = partitionSolution(head, 3);
    expected = Util::createList({1, 2, 2, 4, 3, 5});
    assert(Util::compareLists(result, expected));
    Util::deleteList(result);
    Util::deleteList(expected);
    std::cout << "  partitionSolution: PASSED\n";

    // oddEvenList
    head = Util::createList({1, 2, 3, 4, 5});
    result = oddEvenListSolution(head);
    expected = Util::createList({1, 3, 5, 2, 4});
    assert(Util::compareLists(result, expected));
    Util::deleteList(result);
    Util::deleteList(expected);
    std::cout << "  oddEvenListSolution: PASSED\n";
}

} // namespace TwoPointerProblems
