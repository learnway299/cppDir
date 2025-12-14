/**
 * @file linked_list_sort_solution.cpp
 * @brief 链表排序专题 - 参考答案
 */

#include "linked_list_sort.h"
#include <iostream>
#include <cassert>
#include <queue>
#include <algorithm>

namespace LinkedListSort {

// ==================== 辅助函数 ====================

static ListNode* merge(ListNode* l1, ListNode* l2) {
    ListNode dummy;
    ListNode* curr = &dummy;
    while (l1 && l2) {
        if (l1->val <= l2->val) {
            curr->next = l1;
            l1 = l1->next;
        } else {
            curr->next = l2;
            l2 = l2->next;
        }
        curr = curr->next;
    }
    curr->next = l1 ? l1 : l2;
    return dummy.next;
}

static ListNode* getMiddle(ListNode* head) {
    ListNode* slow = head;
    ListNode* fast = head->next;
    while (fast && fast->next) {
        slow = slow->next;
        fast = fast->next->next;
    }
    return slow;
}

// ==================== 参考答案实现 ====================

// 题目1: 排序链表 - 自顶向下归并 O(n log n)
ListNode* sortListSolution(ListNode* head) {
    if (!head || !head->next) return head;
    ListNode* mid = getMiddle(head);
    ListNode* rightHead = mid->next;
    mid->next = nullptr;
    ListNode* left = sortListSolution(head);
    ListNode* right = sortListSolution(rightHead);
    return merge(left, right);
}

// 题目2: 插入排序链表 O(n^2)
ListNode* insertionSortListSolution(ListNode* head) {
    if (!head || !head->next) return head;
    ListNode dummy(0);
    ListNode* curr = head;
    while (curr) {
        ListNode* next = curr->next;
        ListNode* prev = &dummy;
        while (prev->next && prev->next->val < curr->val) {
            prev = prev->next;
        }
        curr->next = prev->next;
        prev->next = curr;
        curr = next;
    }
    return dummy.next;
}

// 题目3: 合并K个升序链表 - 优先队列 O(N log K)
ListNode* mergeKListsSolution(std::vector<ListNode*>& lists) {
    auto cmp = [](ListNode* a, ListNode* b) { return a->val > b->val; };
    std::priority_queue<ListNode*, std::vector<ListNode*>, decltype(cmp)> pq(cmp);
    for (ListNode* list : lists) {
        if (list) pq.push(list);
    }
    ListNode dummy;
    ListNode* curr = &dummy;
    while (!pq.empty()) {
        ListNode* node = pq.top();
        pq.pop();
        curr->next = node;
        curr = curr->next;
        if (node->next) pq.push(node->next);
    }
    return dummy.next;
}

// 题目4: 链表中的第 K 大节点
int findKthLargestSolution(ListNode* head, int k) {
    std::priority_queue<int, std::vector<int>, std::greater<int>> minHeap;
    while (head) {
        minHeap.push(head->val);
        if (static_cast<int>(minHeap.size()) > k) minHeap.pop();
        head = head->next;
    }
    return minHeap.empty() ? -1 : minHeap.top();
}

// 题目5: 链表中的前 K 大节点
std::vector<int> topKLargestSolution(ListNode* head, int k) {
    std::priority_queue<int, std::vector<int>, std::greater<int>> minHeap;
    while (head) {
        minHeap.push(head->val);
        if (static_cast<int>(minHeap.size()) > k) minHeap.pop();
        head = head->next;
    }
    std::vector<int> result;
    while (!minHeap.empty()) {
        result.push_back(minHeap.top());
        minHeap.pop();
    }
    std::sort(result.rbegin(), result.rend());
    return result;
}

// 题目6: 链表三路划分
ListNode* partitionThreeWaySolution(ListNode* head, int pivot) {
    ListNode smallDummy, equalDummy, largeDummy;
    ListNode* small = &smallDummy;
    ListNode* equal = &equalDummy;
    ListNode* large = &largeDummy;
    while (head) {
        if (head->val < pivot) {
            small->next = head;
            small = small->next;
        } else if (head->val == pivot) {
            equal->next = head;
            equal = equal->next;
        } else {
            large->next = head;
            large = large->next;
        }
        head = head->next;
    }
    large->next = nullptr;
    equal->next = largeDummy.next;
    small->next = equalDummy.next;
    return smallDummy.next;
}

// 题目7: 有序链表交集
ListNode* getIntersectionSortedSolution(ListNode* l1, ListNode* l2) {
    ListNode dummy;
    ListNode* curr = &dummy;
    while (l1 && l2) {
        if (l1->val < l2->val) {
            l1 = l1->next;
        } else if (l1->val > l2->val) {
            l2 = l2->next;
        } else {
            curr->next = new ListNode(l1->val);
            curr = curr->next;
            l1 = l1->next;
            l2 = l2->next;
        }
    }
    return dummy.next;
}

// 题目8: 删除重复元素
ListNode* deleteDuplicatesSolution(ListNode* head) {
    ListNode* curr = head;
    while (curr && curr->next) {
        if (curr->val == curr->next->val) {
            ListNode* toDelete = curr->next;
            curr->next = curr->next->next;
            delete toDelete;
        } else {
            curr = curr->next;
        }
    }
    return head;
}

// 题目9: 删除所有重复元素
ListNode* deleteDuplicatesIISolution(ListNode* head) {
    ListNode dummy(0, head);
    ListNode* prev = &dummy;
    while (prev->next) {
        ListNode* curr = prev->next;
        if (curr->next && curr->val == curr->next->val) {
            int dupVal = curr->val;
            while (prev->next && prev->next->val == dupVal) {
                ListNode* toDelete = prev->next;
                prev->next = prev->next->next;
                delete toDelete;
            }
        } else {
            prev = prev->next;
        }
    }
    return dummy.next;
}

// 题目10: 旋转链表
ListNode* rotateRightSolution(ListNode* head, int k) {
    if (!head || !head->next || k == 0) return head;
    int len = 1;
    ListNode* tail = head;
    while (tail->next) {
        ++len;
        tail = tail->next;
    }
    k = k % len;
    if (k == 0) return head;
    tail->next = head;  // 成环
    ListNode* newTail = head;
    for (int i = 0; i < len - k - 1; ++i) {
        newTail = newTail->next;
    }
    ListNode* newHead = newTail->next;
    newTail->next = nullptr;
    return newHead;
}

// ==================== 测试函数 ====================

void testLinkedListSortSolution() {
    std::cout << "=== Linked List Sort Tests (Solution) ===\n";

    // sortList
    ListNode* head = Util::createList({4, 2, 1, 3});
    ListNode* sorted = sortListSolution(head);
    assert(Util::listToVector(sorted) == std::vector<int>({1, 2, 3, 4}));
    Util::deleteList(sorted);
    std::cout << "  sortListSolution: PASSED\n";

    // insertionSortList
    head = Util::createList({4, 2, 1, 3});
    sorted = insertionSortListSolution(head);
    assert(Util::listToVector(sorted) == std::vector<int>({1, 2, 3, 4}));
    Util::deleteList(sorted);
    std::cout << "  insertionSortListSolution: PASSED\n";

    // mergeKLists
    std::vector<ListNode*> lists = {
        Util::createList({1, 4, 5}),
        Util::createList({1, 3, 4}),
        Util::createList({2, 6})
    };
    ListNode* merged = mergeKListsSolution(lists);
    assert(Util::listToVector(merged) == std::vector<int>({1, 1, 2, 3, 4, 4, 5, 6}));
    Util::deleteList(merged);
    std::cout << "  mergeKListsSolution: PASSED\n";

    // deleteDuplicates
    head = Util::createList({1, 1, 2, 3, 3});
    ListNode* result = deleteDuplicatesSolution(head);
    assert(Util::listToVector(result) == std::vector<int>({1, 2, 3}));
    Util::deleteList(result);
    std::cout << "  deleteDuplicatesSolution: PASSED\n";

    // rotateRight
    head = Util::createList({1, 2, 3, 4, 5});
    result = rotateRightSolution(head, 2);
    assert(Util::listToVector(result) == std::vector<int>({4, 5, 1, 2, 3}));
    Util::deleteList(result);
    std::cout << "  rotateRightSolution: PASSED\n";
}

} // namespace LinkedListSort
