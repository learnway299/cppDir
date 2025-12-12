/**
 * @file merge_lists.cpp
 * @brief 链表合并相关问题 - 题目文件
 *
 * 链表合并是常见的面试题，涉及双指针、分治、堆等多种技巧。
 */

#include <iostream>
#include <vector>

struct ListNode {
    int val;
    ListNode* next;
    ListNode() : val(0), next(nullptr) {}
    ListNode(int x) : val(x), next(nullptr) {}
    ListNode(int x, ListNode* n) : val(x), next(n) {}
};

/**
 * 题目1: 合并两个有序链表
 *
 * 将两个升序链表合并为一个新的升序链表并返回。
 * 新链表是通过拼接给定的两个链表的所有节点组成的。
 *
 * 示例:
 *   输入: l1 = 1->2->4, l2 = 1->3->4
 *   输出: 1->1->2->3->4->4
 *
 *   输入: l1 = [], l2 = []
 *   输出: []
 *
 *   输入: l1 = [], l2 = 0
 *   输出: 0
 *
 * @param l1 第一个有序链表
 * @param l2 第二个有序链表
 * @return 合并后的有序链表
 */
ListNode* mergeTwoLists(ListNode* l1, ListNode* l2) {
    // TODO: 实现你的解法
    return nullptr;
}

/**
 * 题目2: 合并 K 个升序链表
 *
 * 给你一个链表数组，每个链表都已经按升序排列。
 * 请你将所有链表合并到一个升序链表中，返回合并后的链表。
 *
 * 示例:
 *   输入: lists = [[1,4,5],[1,3,4],[2,6]]
 *   输出: [1,1,2,3,4,4,5,6]
 *
 *   输入: lists = []
 *   输出: []
 *
 *   输入: lists = [[]]
 *   输出: []
 *
 * 要求: 时间复杂度优于 O(k*N)，其中 N 是节点总数
 *
 * 提示: 可以使用分治法或优先队列（堆）
 *
 * @param lists 链表数组
 * @return 合并后的有序链表
 */
ListNode* mergeKLists(std::vector<ListNode*>& lists) {
    // TODO: 实现你的解法
    return nullptr;
}

/**
 * 题目3: 排序链表
 *
 * 给你链表的头结点 head，请将其按升序排列并返回排序后的链表。
 *
 * 示例:
 *   输入: head = 4->2->1->3
 *   输出: 1->2->3->4
 *
 *   输入: head = -1->5->3->4->0
 *   输出: -1->0->3->4->5
 *
 * 进阶: 在 O(n log n) 时间复杂度和常数级空间复杂度下排序链表
 *
 * @param head 链表头节点
 * @return 排序后的链表头节点
 */
ListNode* sortList(ListNode* head) {
    // TODO: 实现你的解法
    return nullptr;
}

/**
 * 题目4: 合并两个链表
 *
 * 给你两个链表 list1 和 list2，它们分别包含 n 和 m 个节点。
 * 请你将 list1 中从第 a 个节点到第 b 个节点删除，并将 list2 接在被删除节点的位置。
 *
 * 示例:
 *   输入: list1 = 0->1->2->3->4->5, a = 3, b = 4,
 *         list2 = 1000000->1000001->1000002
 *   输出: 0->1->2->1000000->1000001->1000002->5
 *   解释: 删除 list1 中第 3 到第 4 节点，并将 list2 接在相应位置
 *
 * @param list1 第一个链表
 * @param a 删除起始位置（0-indexed）
 * @param b 删除结束位置（0-indexed）
 * @param list2 要插入的链表
 * @return 合并后的链表
 */
ListNode* mergeInBetween(ListNode* list1, int a, int b, ListNode* list2) {
    // TODO: 实现你的解法
    return nullptr;
}

/**
 * 题目5: 两数相加
 *
 * 给你两个非空的链表，表示两个非负的整数。
 * 它们每位数字都是按照逆序的方式存储的，每个节点只能存储一位数字。
 * 请你将两个数相加，并以相同形式返回一个表示和的链表。
 *
 * 示例:
 *   输入: l1 = 2->4->3 (表示 342), l2 = 5->6->4 (表示 465)
 *   输出: 7->0->8 (表示 807)
 *   解释: 342 + 465 = 807
 *
 *   输入: l1 = 9->9->9->9, l2 = 9->9
 *   输出: 8->9->0->0->1
 *   解释: 9999 + 99 = 10098
 *
 * @param l1 第一个链表
 * @param l2 第二个链表
 * @return 和的链表
 */
ListNode* addTwoNumbers(ListNode* l1, ListNode* l2) {
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

void testMergeTwoLists() {
    ListNode* l1 = createList({1, 2, 4});
    ListNode* l2 = createList({1, 3, 4});
    ListNode* merged = mergeTwoLists(l1, l2);
    ListNode* expected = createList({1, 1, 2, 3, 4, 4});
    assert(compareLists(merged, expected));
    deleteList(merged);
    deleteList(expected);

    // 空链表
    assert(mergeTwoLists(nullptr, nullptr) == nullptr);

    std::cout << "mergeTwoLists: PASSED\n";
}

void testMergeKLists() {
    std::vector<ListNode*> lists = {
        createList({1, 4, 5}),
        createList({1, 3, 4}),
        createList({2, 6})
    };
    ListNode* merged = mergeKLists(lists);
    ListNode* expected = createList({1, 1, 2, 3, 4, 4, 5, 6});
    assert(compareLists(merged, expected));
    deleteList(merged);
    deleteList(expected);

    std::cout << "mergeKLists: PASSED\n";
}

void testSortList() {
    ListNode* head = createList({4, 2, 1, 3});
    ListNode* sorted = sortList(head);
    ListNode* expected = createList({1, 2, 3, 4});
    assert(compareLists(sorted, expected));
    deleteList(sorted);
    deleteList(expected);

    std::cout << "sortList: PASSED\n";
}

void testMergeInBetween() {
    ListNode* list1 = createList({0, 1, 2, 3, 4, 5});
    ListNode* list2 = createList({1000000, 1000001, 1000002});
    ListNode* result = mergeInBetween(list1, 3, 4, list2);
    ListNode* expected = createList({0, 1, 2, 1000000, 1000001, 1000002, 5});
    assert(compareLists(result, expected));
    deleteList(result);
    deleteList(expected);

    std::cout << "mergeInBetween: PASSED\n";
}

void testAddTwoNumbers() {
    ListNode* l1 = createList({2, 4, 3});
    ListNode* l2 = createList({5, 6, 4});
    ListNode* sum = addTwoNumbers(l1, l2);
    ListNode* expected = createList({7, 0, 8});
    assert(compareLists(sum, expected));
    deleteList(sum);
    deleteList(expected);
    deleteList(l1);
    deleteList(l2);

    std::cout << "addTwoNumbers: PASSED\n";
}

int main() {
    std::cout << "=== Merge Lists Tests ===\n";

    // 取消注释以运行测试（实现解法后）
    // testMergeTwoLists();
    // testMergeKLists();
    // testSortList();
    // testMergeInBetween();
    // testAddTwoNumbers();

    std::cout << "All tests passed!\n";
    return 0;
}
