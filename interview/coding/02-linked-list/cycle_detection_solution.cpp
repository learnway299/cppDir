/**
 * @file cycle_detection_solution.cpp
 * @brief 链表环检测相关问题 - 参考答案
 */

#include "cycle_detection.h"
#include <iostream>
#include <cassert>

namespace CycleDetection {

// ==================== 辅助函数 ====================

static int getNext(int n) {
    int sum = 0;
    while (n > 0) {
        int digit = n % 10;
        sum += digit * digit;
        n /= 10;
    }
    return sum;
}

static ListNode* createCyclicList(std::initializer_list<int> vals, int pos) {
    if (vals.size() == 0) return nullptr;
    ListNode dummy(0);
    ListNode* curr = &dummy;
    ListNode* cycleEntry = nullptr;
    int idx = 0;
    for (int val : vals) {
        curr->next = new ListNode(val);
        curr = curr->next;
        if (idx == pos) cycleEntry = curr;
        ++idx;
    }
    if (pos >= 0 && cycleEntry) curr->next = cycleEntry;
    return dummy.next;
}

static void breakCycle(ListNode* head, int pos) {
    if (!head || pos < 0) return;
    ListNode* slow = head;
    ListNode* fast = head;
    while (fast && fast->next) {
        slow = slow->next;
        fast = fast->next->next;
        if (slow == fast) break;
    }
    if (slow == fast) {
        ListNode* ptr = head;
        while (ptr != slow) {
            ptr = ptr->next;
            slow = slow->next;
        }
        ListNode* temp = ptr;
        while (temp->next != ptr) temp = temp->next;
        temp->next = nullptr;
    }
}

// ==================== 参考答案实现 ====================

// 题目1: 环形链表 - Floyd快慢指针 O(n)
bool hasCycleSolution(ListNode* head) {
    if (!head || !head->next) return false;
    ListNode* slow = head;
    ListNode* fast = head;
    while (fast && fast->next) {
        slow = slow->next;
        fast = fast->next->next;
        if (slow == fast) return true;
    }
    return false;
}

// 题目2: 环形链表 II - 找入环点
ListNode* detectCycleSolution(ListNode* head) {
    if (!head || !head->next) return nullptr;
    ListNode* slow = head;
    ListNode* fast = head;
    while (fast && fast->next) {
        slow = slow->next;
        fast = fast->next->next;
        if (slow == fast) {
            ListNode* ptr1 = head;
            ListNode* ptr2 = slow;
            while (ptr1 != ptr2) {
                ptr1 = ptr1->next;
                ptr2 = ptr2->next;
            }
            return ptr1;
        }
    }
    return nullptr;
}

// 题目3: 快乐数 - 转化为环检测
bool isHappySolution(int n) {
    int slow = n;
    int fast = getNext(n);
    while (fast != 1 && slow != fast) {
        slow = getNext(slow);
        fast = getNext(getNext(fast));
    }
    return fast == 1;
}

// 题目4: 相交链表 - 双指针
ListNode* getIntersectionNodeSolution(ListNode* headA, ListNode* headB) {
    if (!headA || !headB) return nullptr;
    ListNode* ptrA = headA;
    ListNode* ptrB = headB;
    while (ptrA != ptrB) {
        ptrA = ptrA ? ptrA->next : headB;
        ptrB = ptrB ? ptrB->next : headA;
    }
    return ptrA;
}

// ==================== 测试函数 ====================

void testCycleDetectionSolution() {
    std::cout << "=== Cycle Detection Tests (Solution) ===\n";

    // hasCycle
    ListNode* head = createCyclicList({3, 2, 0, -4}, 1);
    assert(hasCycleSolution(head) == true);
    breakCycle(head, 1);
    Util::deleteList(head);

    head = createCyclicList({1, 2, 3}, -1);
    assert(hasCycleSolution(head) == false);
    Util::deleteList(head);
    std::cout << "  hasCycleSolution: PASSED\n";

    // detectCycle
    head = createCyclicList({3, 2, 0, -4}, 1);
    ListNode* entry = detectCycleSolution(head);
    assert(entry != nullptr && entry->val == 2);
    breakCycle(head, 1);
    Util::deleteList(head);
    std::cout << "  detectCycleSolution: PASSED\n";

    // isHappy
    assert(isHappySolution(19) == true);
    assert(isHappySolution(2) == false);
    assert(isHappySolution(1) == true);
    std::cout << "  isHappySolution: PASSED\n";

    // getIntersectionNode
    ListNode* common = new ListNode(8);
    common->next = new ListNode(4);
    ListNode* headA = new ListNode(4);
    headA->next = common;
    ListNode* headB = new ListNode(5);
    headB->next = new ListNode(6);
    headB->next->next = common;
    assert(getIntersectionNodeSolution(headA, headB) == common);
    delete headA;
    delete headB->next;
    delete headB;
    delete common->next;
    delete common;
    std::cout << "  getIntersectionNodeSolution: PASSED\n";
}

} // namespace CycleDetection
