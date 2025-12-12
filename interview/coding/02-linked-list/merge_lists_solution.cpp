/**
 * @file merge_lists_solution.cpp
 * @brief 链表合并相关问题 - 解答文件
 *
 * 包含 merge_lists.cpp 中所有题目的解答和详细解析。
 */

#include <iostream>
#include <vector>
#include <queue>

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
 * 解法1: 迭代（推荐）
 * 思路: 使用哑节点，比较两个链表节点，较小的接入结果链表
 *
 * 时间复杂度: O(m + n)
 * 空间复杂度: O(1)
 */
ListNode* mergeTwoLists(ListNode* l1, ListNode* l2) {
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

    // 接上剩余部分
    tail->next = l1 ? l1 : l2;

    return dummy.next;
}

// 解法2: 递归
ListNode* mergeTwoListsRecursive(ListNode* l1, ListNode* l2) {
    if (!l1) return l2;
    if (!l2) return l1;

    if (l1->val <= l2->val) {
        l1->next = mergeTwoListsRecursive(l1->next, l2);
        return l1;
    } else {
        l2->next = mergeTwoListsRecursive(l1, l2->next);
        return l2;
    }
}

/**
 * 题目2: 合并 K 个升序链表
 *
 * 解法1: 优先队列（最小堆）
 * 思路:
 *   - 将所有链表的头节点放入最小堆
 *   - 每次取出最小节点，将其 next 节点放入堆
 *
 * 时间复杂度: O(N log k)，N 是节点总数，k 是链表数
 * 空间复杂度: O(k)
 */
ListNode* mergeKLists(std::vector<ListNode*>& lists) {
    // 自定义比较器
    auto cmp = [](ListNode* a, ListNode* b) {
        return a->val > b->val;  // 最小堆
    };
    std::priority_queue<ListNode*, std::vector<ListNode*>, decltype(cmp)> pq(cmp);

    // 将所有链表头节点加入堆
    for (ListNode* list : lists) {
        if (list) {
            pq.push(list);
        }
    }

    ListNode dummy;
    ListNode* tail = &dummy;

    while (!pq.empty()) {
        ListNode* minNode = pq.top();
        pq.pop();

        tail->next = minNode;
        tail = tail->next;

        if (minNode->next) {
            pq.push(minNode->next);
        }
    }

    return dummy.next;
}

// 解法2: 分治法
ListNode* mergeKListsDivide(std::vector<ListNode*>& lists, int left, int right) {
    if (left > right) return nullptr;
    if (left == right) return lists[left];

    int mid = left + (right - left) / 2;
    ListNode* leftMerged = mergeKListsDivide(lists, left, mid);
    ListNode* rightMerged = mergeKListsDivide(lists, mid + 1, right);

    return mergeTwoLists(leftMerged, rightMerged);
}

ListNode* mergeKListsDivideConquer(std::vector<ListNode*>& lists) {
    if (lists.empty()) return nullptr;
    return mergeKListsDivide(lists, 0, static_cast<int>(lists.size()) - 1);
}

/**
 * 题目3: 排序链表
 *
 * 解法: 归并排序
 * 思路:
 *   1. 使用快慢指针找到中点
 *   2. 断开链表，递归排序两部分
 *   3. 合并两个有序链表
 *
 * 时间复杂度: O(n log n)
 * 空间复杂度: O(log n) 递归栈 / O(1) 迭代版
 */
ListNode* sortList(ListNode* head) {
    // 基准情况
    if (!head || !head->next) return head;

    // 找到中点（快慢指针）
    ListNode* slow = head;
    ListNode* fast = head->next;  // 注意：从 head->next 开始，确保左半部分更短或相等

    while (fast && fast->next) {
        slow = slow->next;
        fast = fast->next->next;
    }

    // 断开链表
    ListNode* mid = slow->next;
    slow->next = nullptr;

    // 递归排序
    ListNode* left = sortList(head);
    ListNode* right = sortList(mid);

    // 合并
    return mergeTwoLists(left, right);
}

// 迭代版归并排序（空间 O(1)）
ListNode* sortListIterative(ListNode* head) {
    if (!head || !head->next) return head;

    // 计算链表长度
    int length = 0;
    for (ListNode* p = head; p; p = p->next) ++length;

    ListNode dummy(0, head);

    // 子链表长度从 1 开始，每次翻倍
    for (int size = 1; size < length; size *= 2) {
        ListNode* prev = &dummy;
        ListNode* curr = dummy.next;

        while (curr) {
            // 获取第一段
            ListNode* left = curr;
            for (int i = 1; i < size && curr->next; ++i) {
                curr = curr->next;
            }

            // 获取第二段
            ListNode* right = curr->next;
            curr->next = nullptr;
            curr = right;
            for (int i = 1; i < size && curr && curr->next; ++i) {
                curr = curr->next;
            }

            // 保存下一段起点
            ListNode* next = nullptr;
            if (curr) {
                next = curr->next;
                curr->next = nullptr;
            }

            // 合并两段
            prev->next = mergeTwoLists(left, right);

            // 移动 prev 到合并后链表的末尾
            while (prev->next) prev = prev->next;

            curr = next;
        }
    }

    return dummy.next;
}

/**
 * 题目4: 合并两个链表（在中间插入）
 *
 * 解法: 指针操作
 * 思路:
 *   1. 找到位置 a-1 的节点（记为 prev）
 *   2. 找到位置 b+1 的节点（记为 suffix）
 *   3. prev->next = list2 的头
 *   4. list2 的尾->next = suffix
 *
 * 时间复杂度: O(m + n)
 * 空间复杂度: O(1)
 */
ListNode* mergeInBetween(ListNode* list1, int a, int b, ListNode* list2) {
    ListNode* prev = list1;

    // 找到位置 a-1
    for (int i = 0; i < a - 1; ++i) {
        prev = prev->next;
    }

    // 找到位置 b+1
    ListNode* suffix = prev->next;
    for (int i = a; i <= b; ++i) {
        ListNode* temp = suffix;
        suffix = suffix->next;
        delete temp;  // 删除被替换的节点（可选）
    }

    // 连接 list2
    prev->next = list2;

    // 找到 list2 的尾节点
    ListNode* tail = list2;
    while (tail->next) {
        tail = tail->next;
    }

    // 连接后续部分
    tail->next = suffix;

    return list1;
}

/**
 * 题目5: 两数相加
 *
 * 解法: 模拟加法
 * 思路:
 *   - 同时遍历两个链表，逐位相加
 *   - 维护进位 carry
 *   - 注意处理链表长度不同的情况
 *
 * 时间复杂度: O(max(m, n))
 * 空间复杂度: O(max(m, n))
 */
ListNode* addTwoNumbers(ListNode* l1, ListNode* l2) {
    ListNode dummy;
    ListNode* curr = &dummy;
    int carry = 0;

    while (l1 || l2 || carry) {
        int sum = carry;

        if (l1) {
            sum += l1->val;
            l1 = l1->next;
        }
        if (l2) {
            sum += l2->val;
            l2 = l2->next;
        }

        carry = sum / 10;
        curr->next = new ListNode(sum % 10);
        curr = curr->next;
    }

    return dummy.next;
}

// 递归解法
ListNode* addTwoNumbersRecursive(ListNode* l1, ListNode* l2, int carry = 0) {
    if (!l1 && !l2 && carry == 0) return nullptr;

    int sum = carry;
    if (l1) {
        sum += l1->val;
        l1 = l1->next;
    }
    if (l2) {
        sum += l2->val;
        l2 = l2->next;
    }

    ListNode* node = new ListNode(sum % 10);
    node->next = addTwoNumbersRecursive(l1, l2, sum / 10);

    return node;
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

void testMergeTwoLists() {
    ListNode* l1 = createList({1, 2, 4});
    ListNode* l2 = createList({1, 3, 4});
    ListNode* merged = mergeTwoLists(l1, l2);
    ListNode* expected = createList({1, 1, 2, 3, 4, 4});
    assert(compareLists(merged, expected));
    deleteList(merged);
    deleteList(expected);

    // 空链表
    l1 = nullptr;
    l2 = createList({1, 2});
    merged = mergeTwoLists(l1, l2);
    expected = createList({1, 2});
    assert(compareLists(merged, expected));
    deleteList(merged);
    deleteList(expected);

    std::cout << "mergeTwoLists: PASSED\n";
}

void testMergeKLists() {
    // 测试优先队列解法
    std::vector<ListNode*> lists1 = {
        createList({1, 4, 5}),
        createList({1, 3, 4}),
        createList({2, 6})
    };
    ListNode* merged = mergeKLists(lists1);
    ListNode* expected = createList({1, 1, 2, 3, 4, 4, 5, 6});
    assert(compareLists(merged, expected));
    deleteList(merged);
    deleteList(expected);

    // 测试分治解法
    std::vector<ListNode*> lists2 = {
        createList({1, 4, 5}),
        createList({1, 3, 4}),
        createList({2, 6})
    };
    merged = mergeKListsDivideConquer(lists2);
    expected = createList({1, 1, 2, 3, 4, 4, 5, 6});
    assert(compareLists(merged, expected));
    deleteList(merged);
    deleteList(expected);

    // 空列表
    std::vector<ListNode*> emptyLists;
    assert(mergeKLists(emptyLists) == nullptr);

    std::cout << "mergeKLists: PASSED\n";
}

void testSortList() {
    ListNode* head = createList({4, 2, 1, 3});
    ListNode* sorted = sortList(head);
    ListNode* expected = createList({1, 2, 3, 4});
    assert(compareLists(sorted, expected));
    deleteList(sorted);
    deleteList(expected);

    // 测试迭代版
    head = createList({-1, 5, 3, 4, 0});
    sorted = sortListIterative(head);
    expected = createList({-1, 0, 3, 4, 5});
    assert(compareLists(sorted, expected));
    deleteList(sorted);
    deleteList(expected);

    // 空链表
    assert(sortList(nullptr) == nullptr);

    // 单节点
    head = new ListNode(1);
    sorted = sortList(head);
    assert(sorted->val == 1);
    delete sorted;

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

    // 有进位
    l1 = createList({9, 9, 9, 9});
    l2 = createList({9, 9});
    sum = addTwoNumbers(l1, l2);
    expected = createList({8, 9, 0, 0, 1});
    assert(compareLists(sum, expected));
    deleteList(sum);
    deleteList(expected);
    deleteList(l1);
    deleteList(l2);

    std::cout << "addTwoNumbers: PASSED\n";
}

int main() {
    std::cout << "=== Merge Lists Solution Tests ===\n";

    testMergeTwoLists();
    testMergeKLists();
    testSortList();
    testMergeInBetween();
    testAddTwoNumbers();

    std::cout << "\nAll tests passed!\n";
    return 0;
}
