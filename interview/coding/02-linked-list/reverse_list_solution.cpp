/**
 * @file reverse_list_solution.cpp
 * @brief 链表反转相关问题 - 解答文件
 *
 * 包含 reverse_list.cpp 中所有题目的解答和详细解析。
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
 * 题目1: 反转链表
 *
 * 解法1: 迭代（推荐）
 * 思路:
 *   - 使用三个指针: prev, curr, next
 *   - 遍历链表，逐个节点反转指向
 *
 * 时间复杂度: O(n)
 * 空间复杂度: O(1)
 */
ListNode* reverseList(ListNode* head) {
    ListNode* prev = nullptr;
    ListNode* curr = head;

    while (curr) {
        ListNode* next = curr->next;  // 保存下一个节点
        curr->next = prev;            // 反转当前节点的指向
        prev = curr;                  // prev 前进
        curr = next;                  // curr 前进
    }

    return prev;  // prev 指向新的头节点
}

/**
 * 解法2: 递归
 * 思路:
 *   - 递归到链表末尾
 *   - 回溯时反转指向
 *
 * 时间复杂度: O(n)
 * 空间复杂度: O(n) - 递归栈
 */
ListNode* reverseListRecursive(ListNode* head) {
    // 基准情况：空链表或只有一个节点
    if (!head || !head->next) {
        return head;
    }

    // 递归反转后续节点
    ListNode* newHead = reverseListRecursive(head->next);

    // 反转当前节点与下一节点的关系
    // head->next 是反转后子链表的尾节点
    head->next->next = head;
    head->next = nullptr;

    return newHead;
}

/**
 * 题目2: 反转链表 II（区间反转）
 *
 * 解法: 穿针引线
 * 思路:
 *   1. 找到 left 的前一个节点 prev
 *   2. 反转 [left, right] 区间内的节点
 *   3. 连接反转后的部分
 *
 * 时间复杂度: O(n)
 * 空间复杂度: O(1)
 */
ListNode* reverseBetween(ListNode* head, int left, int right) {
    if (!head || left == right) return head;

    // 使用哑节点简化边界处理
    ListNode dummy(0, head);
    ListNode* prev = &dummy;

    // 找到 left 的前一个节点
    for (int i = 1; i < left; ++i) {
        prev = prev->next;
    }

    // curr 指向要开始反转的第一个节点
    ListNode* curr = prev->next;

    // 反转 [left, right] 区间
    // 使用头插法：依次将 curr 后面的节点移动到 prev 后面
    for (int i = 0; i < right - left; ++i) {
        ListNode* next = curr->next;
        curr->next = next->next;
        next->next = prev->next;
        prev->next = next;
    }

    return dummy.next;
}

// 另一种解法：先切断再反转再连接
ListNode* reverseBetweenV2(ListNode* head, int left, int right) {
    ListNode dummy(0, head);
    ListNode* prev = &dummy;

    // 找到 left 前一个节点
    for (int i = 1; i < left; ++i) {
        prev = prev->next;
    }

    // 切断并反转
    ListNode* start = prev->next;
    ListNode* end = start;
    for (int i = left; i < right; ++i) {
        end = end->next;
    }
    ListNode* suffix = end->next;
    end->next = nullptr;

    // 反转 [start, end]
    ListNode* reversed = reverseList(start);

    // 连接
    prev->next = reversed;
    start->next = suffix;  // start 现在是反转后的尾节点

    return dummy.next;
}

/**
 * 题目3: K 个一组翻转链表
 *
 * 解法: 分组反转
 * 思路:
 *   1. 检查剩余节点是否足够 k 个
 *   2. 反转 k 个节点
 *   3. 递归或迭代处理后续节点
 *
 * 时间复杂度: O(n)
 * 空间复杂度: O(1) 迭代 / O(n/k) 递归
 */
ListNode* reverseKGroup(ListNode* head, int k) {
    if (!head || k == 1) return head;

    ListNode dummy(0, head);
    ListNode* prevGroupEnd = &dummy;

    while (true) {
        // 检查剩余节点是否足够 k 个
        ListNode* kthNode = prevGroupEnd;
        for (int i = 0; i < k; ++i) {
            kthNode = kthNode->next;
            if (!kthNode) {
                return dummy.next;  // 不足 k 个，直接返回
            }
        }

        // 记录下一组的起始位置
        ListNode* nextGroupStart = kthNode->next;
        ListNode* groupStart = prevGroupEnd->next;

        // 反转当前组 [groupStart, kthNode]
        ListNode* prev = nextGroupStart;
        ListNode* curr = groupStart;
        while (curr != nextGroupStart) {
            ListNode* next = curr->next;
            curr->next = prev;
            prev = curr;
            curr = next;
        }

        // 连接：prevGroupEnd -> kthNode (反转后的头)
        prevGroupEnd->next = kthNode;
        // groupStart 现在是反转后的尾
        prevGroupEnd = groupStart;
    }

    return dummy.next;
}

// 递归版本
ListNode* reverseKGroupRecursive(ListNode* head, int k) {
    // 检查是否有 k 个节点
    ListNode* curr = head;
    int count = 0;
    while (curr && count < k) {
        curr = curr->next;
        ++count;
    }
    if (count < k) return head;  // 不足 k 个，不反转

    // 反转前 k 个节点
    ListNode* prev = nullptr;
    curr = head;
    for (int i = 0; i < k; ++i) {
        ListNode* next = curr->next;
        curr->next = prev;
        prev = curr;
        curr = next;
    }

    // 递归处理剩余部分，并连接
    head->next = reverseKGroupRecursive(curr, k);

    return prev;  // prev 是反转后的新头
}

/**
 * 题目4: 两两交换链表中的节点
 *
 * 解法1: 迭代
 * 思路: 实际上是 k=2 的 reverseKGroup
 *
 * 时间复杂度: O(n)
 * 空间复杂度: O(1)
 */
ListNode* swapPairs(ListNode* head) {
    ListNode dummy(0, head);
    ListNode* prev = &dummy;

    while (prev->next && prev->next->next) {
        ListNode* first = prev->next;
        ListNode* second = prev->next->next;

        // 交换
        first->next = second->next;
        second->next = first;
        prev->next = second;

        // 移动到下一对
        prev = first;
    }

    return dummy.next;
}

// 解法2: 递归
ListNode* swapPairsRecursive(ListNode* head) {
    if (!head || !head->next) return head;

    ListNode* first = head;
    ListNode* second = head->next;

    // 递归处理后续节点
    first->next = swapPairsRecursive(second->next);
    second->next = first;

    return second;  // second 成为新的头
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

void testReverseList() {
    // 测试迭代版本
    ListNode* head = createList({1, 2, 3, 4, 5});
    ListNode* reversed = reverseList(head);
    ListNode* expected = createList({5, 4, 3, 2, 1});
    assert(compareLists(reversed, expected));
    deleteList(reversed);
    deleteList(expected);

    // 测试递归版本
    head = createList({1, 2, 3, 4, 5});
    reversed = reverseListRecursive(head);
    expected = createList({5, 4, 3, 2, 1});
    assert(compareLists(reversed, expected));
    deleteList(reversed);
    deleteList(expected);

    // 空链表
    assert(reverseList(nullptr) == nullptr);
    assert(reverseListRecursive(nullptr) == nullptr);

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

    // 反转整个链表
    head = createList({1, 2, 3});
    result = reverseBetween(head, 1, 3);
    expected = createList({3, 2, 1});
    assert(compareLists(result, expected));
    deleteList(result);
    deleteList(expected);

    // 单元素范围
    head = createList({1, 2, 3});
    result = reverseBetween(head, 2, 2);
    expected = createList({1, 2, 3});
    assert(compareLists(result, expected));
    deleteList(result);
    deleteList(expected);

    std::cout << "reverseBetween: PASSED\n";
}

void testReverseKGroup() {
    // k = 2
    ListNode* head = createList({1, 2, 3, 4, 5});
    ListNode* result = reverseKGroup(head, 2);
    ListNode* expected = createList({2, 1, 4, 3, 5});
    assert(compareLists(result, expected));
    deleteList(result);
    deleteList(expected);

    // k = 3
    head = createList({1, 2, 3, 4, 5});
    result = reverseKGroup(head, 3);
    expected = createList({3, 2, 1, 4, 5});
    assert(compareLists(result, expected));
    deleteList(result);
    deleteList(expected);

    // k = 1 (不变)
    head = createList({1, 2, 3});
    result = reverseKGroup(head, 1);
    expected = createList({1, 2, 3});
    assert(compareLists(result, expected));
    deleteList(result);
    deleteList(expected);

    // 递归版本测试
    head = createList({1, 2, 3, 4, 5});
    result = reverseKGroupRecursive(head, 2);
    expected = createList({2, 1, 4, 3, 5});
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

    // 奇数个节点
    head = createList({1, 2, 3});
    result = swapPairs(head);
    expected = createList({2, 1, 3});
    assert(compareLists(result, expected));
    deleteList(result);
    deleteList(expected);

    // 单节点
    head = new ListNode(1);
    result = swapPairs(head);
    assert(result->val == 1);
    delete result;

    // 空链表
    assert(swapPairs(nullptr) == nullptr);

    // 递归版本测试
    head = createList({1, 2, 3, 4});
    result = swapPairsRecursive(head);
    expected = createList({2, 1, 4, 3});
    assert(compareLists(result, expected));
    deleteList(result);
    deleteList(expected);

    std::cout << "swapPairs: PASSED\n";
}

int main() {
    std::cout << "=== Reverse List Solution Tests ===\n";

    testReverseList();
    testReverseBetween();
    testReverseKGroup();
    testSwapPairs();

    std::cout << "\nAll tests passed!\n";
    return 0;
}
