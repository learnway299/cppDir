/**
 * @file reverse_list.cpp
 * @brief 链表反转相关问题 - 题目文件
 *
 * 链表反转是面试中最常考的链表题目之一，需要掌握迭代和递归两种解法。
 */

#include <iostream>

// 链表节点定义
struct ListNode {
    int val;
    ListNode* next;
    ListNode() : val(0), next(nullptr) {}
    ListNode(int x) : val(x), next(nullptr) {}
    ListNode(int x, ListNode* n) : val(x), next(n) {}
};

/**
 * 题目1: 反转链表
 *
 * 给你单链表的头节点 head，请你反转链表，并返回反转后的链表。
 *
 * 示例:
 *   输入: 1 -> 2 -> 3 -> 4 -> 5 -> NULL
 *   输出: 5 -> 4 -> 3 -> 2 -> 1 -> NULL
 *
 *   输入: 1 -> 2 -> NULL
 *   输出: 2 -> 1 -> NULL
 *
 *   输入: NULL
 *   输出: NULL
 *
 * 要求: 分别用迭代和递归两种方法实现
 *
 * @param head 链表头节点
 * @return 反转后的链表头节点
 */
ListNode* reverseList(ListNode* head) {
    // TODO: 实现你的解法
    return nullptr;
}

/**
 * 题目2: 反转链表 II（区间反转）
 *
 * 给你单链表的头指针 head 和两个整数 left 和 right，
 * 其中 left <= right。请你反转从位置 left 到位置 right 的链表节点，
 * 返回反转后的链表。（位置从 1 开始）
 *
 * 示例:
 *   输入: head = 1->2->3->4->5, left = 2, right = 4
 *   输出: 1->4->3->2->5
 *
 *   输入: head = 5, left = 1, right = 1
 *   输出: 5
 *
 * @param head 链表头节点
 * @param left 反转起始位置（从1开始）
 * @param right 反转结束位置
 * @return 反转后的链表头节点
 */
ListNode* reverseBetween(ListNode* head, int left, int right) {
    // TODO: 实现你的解法
    return nullptr;
}

/**
 * 题目3: K 个一组翻转链表
 *
 * 给你链表的头节点 head，每 k 个节点一组进行翻转，请你返回修改后的链表。
 * k 是一个正整数，它的值小于或等于链表的长度。
 * 如果节点总数不是 k 的整数倍，那么请将最后剩余的节点保持原有顺序。
 *
 * 示例:
 *   输入: head = 1->2->3->4->5, k = 2
 *   输出: 2->1->4->3->5
 *
 *   输入: head = 1->2->3->4->5, k = 3
 *   输出: 3->2->1->4->5
 *
 * 进阶: 你可以设计一个只用 O(1) 额外内存空间的算法解决此问题吗？
 *
 * @param head 链表头节点
 * @param k 每组节点数
 * @return 反转后的链表头节点
 */
ListNode* reverseKGroup(ListNode* head, int k) {
    // TODO: 实现你的解法
    return nullptr;
}

/**
 * 题目4: 两两交换链表中的节点
 *
 * 给你一个链表，两两交换其中相邻的节点，并返回交换后链表的头节点。
 * 你必须在不修改节点内部的值的情况下完成本题（即只能进行节点交换）。
 *
 * 示例:
 *   输入: head = 1->2->3->4
 *   输出: 2->1->4->3
 *
 *   输入: head = 1->2->3
 *   输出: 2->1->3
 *
 *   输入: head = 1
 *   输出: 1
 *
 * @param head 链表头节点
 * @return 交换后的链表头节点
 */
ListNode* swapPairs(ListNode* head) {
    // TODO: 实现你的解法
    return nullptr;
}


// ==================== 辅助函数 ====================

// 从数组创建链表
ListNode* createList(std::initializer_list<int> vals) {
    ListNode dummy;
    ListNode* curr = &dummy;
    for (int val : vals) {
        curr->next = new ListNode(val);
        curr = curr->next;
    }
    return dummy.next;
}

// 打印链表
void printList(ListNode* head) {
    while (head) {
        std::cout << head->val;
        if (head->next) std::cout << " -> ";
        head = head->next;
    }
    std::cout << " -> NULL\n";
}

// 释放链表内存
void deleteList(ListNode* head) {
    while (head) {
        ListNode* temp = head;
        head = head->next;
        delete temp;
    }
}

// 比较两个链表是否相等
bool compareLists(ListNode* l1, ListNode* l2) {
    while (l1 && l2) {
        if (l1->val != l2->val) return false;
        l1 = l1->next;
        l2 = l2->next;
    }
    return l1 == nullptr && l2 == nullptr;
}


// ==================== 测试代码 ====================
#include <cassert>

void testReverseList() {
    ListNode* head = createList({1, 2, 3, 4, 5});
    ListNode* reversed = reverseList(head);
    ListNode* expected = createList({5, 4, 3, 2, 1});
    assert(compareLists(reversed, expected));
    deleteList(reversed);
    deleteList(expected);

    // 空链表
    assert(reverseList(nullptr) == nullptr);

    // 单节点
    ListNode* single = new ListNode(1);
    ListNode* singleReversed = reverseList(single);
    assert(singleReversed->val == 1);
    delete singleReversed;

    std::cout << "reverseList: PASSED\n";
}

void testReverseBetween() {
    ListNode* head = createList({1, 2, 3, 4, 5});
    ListNode* result = reverseBetween(head, 2, 4);
    ListNode* expected = createList({1, 4, 3, 2, 5});
    assert(compareLists(result, expected));
    deleteList(result);
    deleteList(expected);

    std::cout << "reverseBetween: PASSED\n";
}

void testReverseKGroup() {
    ListNode* head = createList({1, 2, 3, 4, 5});
    ListNode* result = reverseKGroup(head, 2);
    ListNode* expected = createList({2, 1, 4, 3, 5});
    assert(compareLists(result, expected));
    deleteList(result);
    deleteList(expected);

    std::cout << "reverseKGroup: PASSED\n";
}

void testSwapPairs() {
    ListNode* head = createList({1, 2, 3, 4});
    ListNode* result = swapPairs(head);
    ListNode* expected = createList({2, 1, 4, 3});
    assert(compareLists(result, expected));
    deleteList(result);
    deleteList(expected);

    std::cout << "swapPairs: PASSED\n";
}

int main() {
    std::cout << "=== Reverse List Tests ===\n";

    // 取消注释以运行测试（实现解法后）
    // testReverseList();
    // testReverseBetween();
    // testReverseKGroup();
    // testSwapPairs();

    std::cout << "All tests passed!\n";
    return 0;
}
