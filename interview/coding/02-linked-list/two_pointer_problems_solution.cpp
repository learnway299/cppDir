/**
 * @file two_pointer_problems_solution.cpp
 * @brief 链表双指针问题 - 解答文件
 *
 * 包含 two_pointer_problems.cpp 中所有题目的解答和详细解析。
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
 * 解法: 快慢指针
 * 思路:
 *   - 快指针每次走 2 步，慢指针每次走 1 步
 *   - 当快指针到达末尾时，慢指针正好在中间
 *
 * 时间复杂度: O(n)
 * 空间复杂度: O(1)
 */
ListNode* middleNode(ListNode* head) {
    ListNode* slow = head;
    ListNode* fast = head;

    while (fast && fast->next) {
        slow = slow->next;
        fast = fast->next->next;
    }

    return slow;  // 偶数个节点时返回第二个中间节点
}

// 如果要返回第一个中间节点（偶数时）
ListNode* middleNodeFirst(ListNode* head) {
    ListNode* slow = head;
    ListNode* fast = head;

    while (fast->next && fast->next->next) {
        slow = slow->next;
        fast = fast->next->next;
    }

    return slow;
}

/**
 * 题目2: 删除链表的倒数第 N 个结点
 *
 * 解法: 快慢指针
 * 思路:
 *   - 快指针先走 n 步
 *   - 然后快慢指针一起走
 *   - 当快指针到达末尾时，慢指针正好在倒数第 n+1 个节点
 *
 * 时间复杂度: O(n)
 * 空间复杂度: O(1)
 */
ListNode* removeNthFromEnd(ListNode* head, int n) {
    ListNode dummy(0, head);
    ListNode* fast = &dummy;
    ListNode* slow = &dummy;

    // 快指针先走 n+1 步
    for (int i = 0; i <= n; ++i) {
        fast = fast->next;
    }

    // 快慢指针一起走
    while (fast) {
        slow = slow->next;
        fast = fast->next;
    }

    // 删除 slow->next
    ListNode* toDelete = slow->next;
    slow->next = slow->next->next;
    delete toDelete;

    return dummy.next;
}

/**
 * 题目3: 链表中倒数第 k 个节点
 *
 * 解法: 快慢指针
 * 思路: 与上题类似，但返回节点而不是删除
 *
 * 时间复杂度: O(n)
 * 空间复杂度: O(1)
 */
ListNode* getKthFromEnd(ListNode* head, int k) {
    ListNode* fast = head;
    ListNode* slow = head;

    // 快指针先走 k 步
    for (int i = 0; i < k; ++i) {
        fast = fast->next;
    }

    // 快慢指针一起走
    while (fast) {
        slow = slow->next;
        fast = fast->next;
    }

    return slow;
}

/**
 * 题目4: 回文链表
 *
 * 解法: 快慢指针 + 反转后半部分
 * 思路:
 *   1. 用快慢指针找到中点
 *   2. 反转后半部分链表
 *   3. 比较前半部分和反转后的后半部分
 *   4. （可选）恢复链表
 *
 * 时间复杂度: O(n)
 * 空间复杂度: O(1)
 */
ListNode* reverseList(ListNode* head) {
    ListNode* prev = nullptr;
    while (head) {
        ListNode* next = head->next;
        head->next = prev;
        prev = head;
        head = next;
    }
    return prev;
}

bool isPalindrome(ListNode* head) {
    if (!head || !head->next) return true;

    // 1. 找中点（慢指针最终指向中点或中点偏右）
    ListNode* slow = head;
    ListNode* fast = head;
    while (fast->next && fast->next->next) {
        slow = slow->next;
        fast = fast->next->next;
    }

    // 2. 反转后半部分
    ListNode* secondHalf = reverseList(slow->next);

    // 3. 比较
    ListNode* p1 = head;
    ListNode* p2 = secondHalf;
    bool result = true;
    while (p2) {
        if (p1->val != p2->val) {
            result = false;
            break;
        }
        p1 = p1->next;
        p2 = p2->next;
    }

    // 4. 恢复链表（可选）
    slow->next = reverseList(secondHalf);

    return result;
}

/**
 * 题目5: 重排链表
 *
 * 解法: 快慢指针 + 反转 + 合并
 * 思路:
 *   1. 找到中点，分成两半
 *   2. 反转后半部分
 *   3. 交替合并两个链表
 *
 * 时间复杂度: O(n)
 * 空间复杂度: O(1)
 */
void reorderList(ListNode* head) {
    if (!head || !head->next || !head->next->next) return;

    // 1. 找中点
    ListNode* slow = head;
    ListNode* fast = head;
    while (fast->next && fast->next->next) {
        slow = slow->next;
        fast = fast->next->next;
    }

    // 2. 断开并反转后半部分
    ListNode* second = reverseList(slow->next);
    slow->next = nullptr;

    // 3. 交替合并
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

/**
 * 题目6: 分隔链表
 *
 * 解法: 双链表
 * 思路:
 *   - 创建两个虚拟头节点：一个存小于 x 的节点，一个存大于等于 x 的节点
 *   - 遍历原链表，将节点分别添加到两个链表
 *   - 最后连接两个链表
 *
 * 时间复杂度: O(n)
 * 空间复杂度: O(1)
 */
ListNode* partition(ListNode* head, int x) {
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

    // 连接两个链表
    large->next = nullptr;  // 重要：断开原链表的尾部
    small->next = largeDummy.next;

    return smallDummy.next;
}

/**
 * 题目7: 奇偶链表
 *
 * 解法: 分离奇偶节点
 * 思路:
 *   - 用两个指针分别指向奇数位和偶数位节点
 *   - 将奇数位节点串成一条链，偶数位节点串成另一条链
 *   - 最后将偶数链接在奇数链后面
 *
 * 时间复杂度: O(n)
 * 空间复杂度: O(1)
 */
ListNode* oddEvenList(ListNode* head) {
    if (!head || !head->next) return head;

    ListNode* odd = head;           // 奇数链的当前节点
    ListNode* even = head->next;    // 偶数链的当前节点
    ListNode* evenHead = even;      // 偶数链的头节点

    while (even && even->next) {
        odd->next = even->next;     // 奇数指向下一个奇数位
        odd = odd->next;

        even->next = odd->next;     // 偶数指向下一个偶数位
        even = even->next;
    }

    odd->next = evenHead;           // 连接奇数链和偶数链

    return head;
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
    // 奇数个节点
    ListNode* head = createList({1, 2, 3, 4, 5});
    ListNode* mid = middleNode(head);
    assert(mid->val == 3);
    deleteList(head);

    // 偶数个节点（返回第二个中间节点）
    head = createList({1, 2, 3, 4, 5, 6});
    mid = middleNode(head);
    assert(mid->val == 4);
    deleteList(head);

    // 单节点
    head = new ListNode(1);
    mid = middleNode(head);
    assert(mid->val == 1);
    delete head;

    std::cout << "middleNode: PASSED\n";
}

void testRemoveNthFromEnd() {
    ListNode* head = createList({1, 2, 3, 4, 5});
    ListNode* result = removeNthFromEnd(head, 2);
    ListNode* expected = createList({1, 2, 3, 5});
    assert(compareLists(result, expected));
    deleteList(result);
    deleteList(expected);

    // 删除头节点
    head = createList({1, 2});
    result = removeNthFromEnd(head, 2);
    expected = createList({2});
    assert(compareLists(result, expected));
    deleteList(result);
    deleteList(expected);

    // 单节点
    head = createList({1});
    result = removeNthFromEnd(head, 1);
    assert(result == nullptr);

    std::cout << "removeNthFromEnd: PASSED\n";
}

void testGetKthFromEnd() {
    ListNode* head = createList({1, 2, 3, 4, 5});
    ListNode* kth = getKthFromEnd(head, 2);
    assert(kth->val == 4);

    kth = getKthFromEnd(head, 1);
    assert(kth->val == 5);

    kth = getKthFromEnd(head, 5);
    assert(kth->val == 1);

    deleteList(head);

    std::cout << "getKthFromEnd: PASSED\n";
}

void testIsPalindrome() {
    // 回文（偶数）
    ListNode* head = createList({1, 2, 2, 1});
    assert(isPalindrome(head) == true);
    deleteList(head);

    // 不是回文
    head = createList({1, 2});
    assert(isPalindrome(head) == false);
    deleteList(head);

    // 回文（奇数）
    head = createList({1, 2, 3, 2, 1});
    assert(isPalindrome(head) == true);
    deleteList(head);

    // 单节点
    head = new ListNode(1);
    assert(isPalindrome(head) == true);
    delete head;

    std::cout << "isPalindrome: PASSED\n";
}

void testReorderList() {
    // 偶数个节点
    ListNode* head = createList({1, 2, 3, 4});
    reorderList(head);
    ListNode* expected = createList({1, 4, 2, 3});
    assert(compareLists(head, expected));
    deleteList(head);
    deleteList(expected);

    // 奇数个节点
    head = createList({1, 2, 3, 4, 5});
    reorderList(head);
    expected = createList({1, 5, 2, 4, 3});
    assert(compareLists(head, expected));
    deleteList(head);
    deleteList(expected);

    // 两个节点
    head = createList({1, 2});
    reorderList(head);
    expected = createList({1, 2});
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

    // 所有元素都小于 x
    head = createList({1, 2});
    result = partition(head, 3);
    expected = createList({1, 2});
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

    head = createList({2, 1, 3, 5, 6, 4, 7});
    result = oddEvenList(head);
    expected = createList({2, 3, 6, 7, 1, 5, 4});
    assert(compareLists(result, expected));
    deleteList(result);
    deleteList(expected);

    // 两个节点
    head = createList({1, 2});
    result = oddEvenList(head);
    expected = createList({1, 2});
    assert(compareLists(result, expected));
    deleteList(result);
    deleteList(expected);

    std::cout << "oddEvenList: PASSED\n";
}

int main() {
    std::cout << "=== Two Pointer Problems Solution Tests ===\n";

    testMiddleNode();
    testRemoveNthFromEnd();
    testGetKthFromEnd();
    testIsPalindrome();
    testReorderList();
    testPartition();
    testOddEvenList();

    std::cout << "\nAll tests passed!\n";
    return 0;
}
