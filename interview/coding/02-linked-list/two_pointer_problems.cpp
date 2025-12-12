/**
 * @file two_pointer_problems.cpp
 * @brief 链表双指针问题 - 题目文件
 *
 * 双指针是链表问题中最重要的技巧之一，包括快慢指针、前后指针等。
 */

#include <iostream>

struct ListNode {
    int val;
    ListNode* next;
    ListNode() : val(0), next(nullptr) {}
    ListNode(int x) : val(x), next(nullptr) {}
    ListNode(int x, ListNode* n) : val(x), next(n) {}
};

/**
 * 题目1: 链表的中间结点
 *
 * 给定一个头结点为 head 的非空单链表，返回链表的中间结点。
 * 如果有两个中间结点，则返回第二个中间结点。
 *
 * 示例:
 *   输入: 1->2->3->4->5
 *   输出: 节点 3
 *
 *   输入: 1->2->3->4->5->6
 *   输出: 节点 4（两个中间节点 3 和 4，返回第二个）
 *
 * @param head 链表头节点
 * @return 中间节点
 */
ListNode* middleNode(ListNode* head) {
    // TODO: 实现你的解法
    return nullptr;
}

/**
 * 题目2: 删除链表的倒数第 N 个结点
 *
 * 给你一个链表，删除链表的倒数第 n 个结点，并且返回链表的头结点。
 *
 * 示例:
 *   输入: head = 1->2->3->4->5, n = 2
 *   输出: 1->2->3->5
 *
 *   输入: head = 1, n = 1
 *   输出: []
 *
 *   输入: head = 1->2, n = 1
 *   输出: 1
 *
 * 进阶: 能否使用一趟扫描实现？
 *
 * @param head 链表头节点
 * @param n 倒数第 n 个
 * @return 删除后的链表头节点
 */
ListNode* removeNthFromEnd(ListNode* head, int n) {
    // TODO: 实现你的解法
    return nullptr;
}

/**
 * 题目3: 链表中倒数第 k 个节点
 *
 * 输入一个链表，输出该链表中倒数第 k 个节点。
 * 从 1 开始计数，即链表的尾节点是倒数第 1 个节点。
 *
 * 示例:
 *   输入: 1->2->3->4->5, k = 2
 *   输出: 节点 4
 *
 * @param head 链表头节点
 * @param k 倒数第 k 个
 * @return 倒数第 k 个节点
 */
ListNode* getKthFromEnd(ListNode* head, int k) {
    // TODO: 实现你的解法
    return nullptr;
}

/**
 * 题目4: 回文链表
 *
 * 给你一个单链表的头节点 head，请你判断该链表是否为回文链表。
 * 如果是，返回 true；否则，返回 false。
 *
 * 示例:
 *   输入: 1->2->2->1
 *   输出: true
 *
 *   输入: 1->2
 *   输出: false
 *
 * 进阶: 你能否用 O(n) 时间复杂度和 O(1) 空间复杂度解决此题？
 *
 * @param head 链表头节点
 * @return 是否为回文链表
 */
bool isPalindrome(ListNode* head) {
    // TODO: 实现你的解法
    return false;
}

/**
 * 题目5: 重排链表
 *
 * 给定一个单链表 L: L0→L1→…→Ln-1→Ln
 * 将其重新排列后变为: L0→Ln→L1→Ln-1→L2→Ln-2→…
 *
 * 不能只是单纯的改变节点内部的值，而是需要实际的进行节点交换。
 *
 * 示例:
 *   输入: 1->2->3->4
 *   输出: 1->4->2->3
 *
 *   输入: 1->2->3->4->5
 *   输出: 1->5->2->4->3
 *
 * @param head 链表头节点
 */
void reorderList(ListNode* head) {
    // TODO: 实现你的解法
}

/**
 * 题目6: 分隔链表
 *
 * 给你一个链表的头节点 head 和一个特定值 x，
 * 请你对链表进行分隔，使得所有小于 x 的节点都出现在大于或等于 x 的节点之前。
 * 你应当保留两个分区中每个节点的初始相对位置。
 *
 * 示例:
 *   输入: head = 1->4->3->2->5->2, x = 3
 *   输出: 1->2->2->4->3->5
 *
 *   输入: head = 2->1, x = 2
 *   输出: 1->2
 *
 * @param head 链表头节点
 * @param x 分隔值
 * @return 分隔后的链表头节点
 */
ListNode* partition(ListNode* head, int x) {
    // TODO: 实现你的解法
    return nullptr;
}

/**
 * 题目7: 奇偶链表
 *
 * 给定单链表的头节点 head，将所有索引为奇数的节点和索引为偶数的节点分别组合在一起，
 * 然后返回重新排序的列表。
 *
 * 第一个节点的索引被认为是奇数，第二个节点的索引为偶数，以此类推。
 * 请注意，奇数组和偶数组内部的相对顺序应该与输入时保持一致。
 *
 * 示例:
 *   输入: 1->2->3->4->5
 *   输出: 1->3->5->2->4
 *
 *   输入: 2->1->3->5->6->4->7
 *   输出: 2->3->6->7->1->5->4
 *
 * @param head 链表头节点
 * @return 重排后的链表头节点
 */
ListNode* oddEvenList(ListNode* head) {
    // TODO: 实现你的解法
    return nullptr;
}


// ==================== 辅助函数 ====================

ListNode* createList(std::initializer_list<int> vals) {
    ListNode dummy;
    ListNode* curr = &dummy;
    for (int val : vals) {
        curr->next = new ListNode(val);
        curr = curr->next;
    }
    return dummy.next;
}

void printList(ListNode* head) {
    while (head) {
        std::cout << head->val;
        if (head->next) std::cout << " -> ";
        head = head->next;
    }
    std::cout << " -> NULL\n";
}

void deleteList(ListNode* head) {
    while (head) {
        ListNode* temp = head;
        head = head->next;
        delete temp;
    }
}

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

void testMiddleNode() {
    ListNode* head = createList({1, 2, 3, 4, 5});
    ListNode* mid = middleNode(head);
    assert(mid->val == 3);
    deleteList(head);

    head = createList({1, 2, 3, 4, 5, 6});
    mid = middleNode(head);
    assert(mid->val == 4);
    deleteList(head);

    std::cout << "middleNode: PASSED\n";
}

void testRemoveNthFromEnd() {
    ListNode* head = createList({1, 2, 3, 4, 5});
    ListNode* result = removeNthFromEnd(head, 2);
    ListNode* expected = createList({1, 2, 3, 5});
    assert(compareLists(result, expected));
    deleteList(result);
    deleteList(expected);

    head = createList({1});
    result = removeNthFromEnd(head, 1);
    assert(result == nullptr);

    std::cout << "removeNthFromEnd: PASSED\n";
}

void testGetKthFromEnd() {
    ListNode* head = createList({1, 2, 3, 4, 5});
    ListNode* kth = getKthFromEnd(head, 2);
    assert(kth->val == 4);
    deleteList(head);

    std::cout << "getKthFromEnd: PASSED\n";
}

void testIsPalindrome() {
    ListNode* head = createList({1, 2, 2, 1});
    assert(isPalindrome(head) == true);
    deleteList(head);

    head = createList({1, 2});
    assert(isPalindrome(head) == false);
    deleteList(head);

    head = createList({1, 2, 3, 2, 1});
    assert(isPalindrome(head) == true);
    deleteList(head);

    std::cout << "isPalindrome: PASSED\n";
}

void testReorderList() {
    ListNode* head = createList({1, 2, 3, 4});
    reorderList(head);
    ListNode* expected = createList({1, 4, 2, 3});
    assert(compareLists(head, expected));
    deleteList(head);
    deleteList(expected);

    head = createList({1, 2, 3, 4, 5});
    reorderList(head);
    expected = createList({1, 5, 2, 4, 3});
    assert(compareLists(head, expected));
    deleteList(head);
    deleteList(expected);

    std::cout << "reorderList: PASSED\n";
}

void testPartition() {
    ListNode* head = createList({1, 4, 3, 2, 5, 2});
    ListNode* result = partition(head, 3);
    ListNode* expected = createList({1, 2, 2, 4, 3, 5});
    assert(compareLists(result, expected));
    deleteList(result);
    deleteList(expected);

    std::cout << "partition: PASSED\n";
}

void testOddEvenList() {
    ListNode* head = createList({1, 2, 3, 4, 5});
    ListNode* result = oddEvenList(head);
    ListNode* expected = createList({1, 3, 5, 2, 4});
    assert(compareLists(result, expected));
    deleteList(result);
    deleteList(expected);

    std::cout << "oddEvenList: PASSED\n";
}

int main() {
    std::cout << "=== Two Pointer Problems Tests ===\n";

    // 取消注释以运行测试（实现解法后）
    // testMiddleNode();
    // testRemoveNthFromEnd();
    // testGetKthFromEnd();
    // testIsPalindrome();
    // testReorderList();
    // testPartition();
    // testOddEvenList();

    std::cout << "All tests passed!\n";
    return 0;
}
