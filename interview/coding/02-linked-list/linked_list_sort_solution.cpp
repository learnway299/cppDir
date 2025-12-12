/**
 * @file linked_list_sort_solution.cpp
 * @brief 链表排序专题 - 解答文件
 *
 * 链表排序算法复杂度对比：
 * | 算法         | 时间         | 空间      | 稳定 | 说明                    |
 * |--------------|--------------|-----------|------|-------------------------|
 * | 归并(递归)   | O(n log n)   | O(log n)  | ✓    | 递归栈空间              |
 * | 归并(迭代)   | O(n log n)   | O(1)      | ✓    | 最优选择                |
 * | 快速排序     | O(n log n)   | O(log n)  | ✗    | 链表快排较复杂          |
 * | 插入排序     | O(n²)        | O(1)      | ✓    | 适合近乎有序的链表      |
 */

#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
#include <functional>
#include <random>

struct ListNode {
    int val;
    ListNode* next;
    ListNode() : val(0), next(nullptr) {}
    ListNode(int x) : val(x), next(nullptr) {}
    ListNode(int x, ListNode* next) : val(x), next(next) {}
};

// ==================== 辅助函数 ====================

// 找链表中点（快慢指针）
ListNode* getMiddle(ListNode* head) {
    ListNode* slow = head;
    ListNode* fast = head->next;
    while (fast && fast->next) {
        slow = slow->next;
        fast = fast->next->next;
    }
    return slow;
}

// 合并两个有序链表（迭代版）
ListNode* merge(ListNode* l1, ListNode* l2) {
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

// 合并两个有序链表（递归版）
ListNode* mergeRecursive(ListNode* l1, ListNode* l2) {
    if (!l1) return l2;
    if (!l2) return l1;
    if (l1->val <= l2->val) {
        l1->next = mergeRecursive(l1->next, l2);
        return l1;
    } else {
        l2->next = mergeRecursive(l1, l2->next);
        return l2;
    }
}

// ==================== 题目1: 排序链表 ====================

/**
 * 方法1: 自顶向下归并排序（递归）
 * 时间: O(n log n)
 * 空间: O(log n) 递归栈
 */
ListNode* sortListTopDown(ListNode* head) {
    if (!head || !head->next) return head;

    // 找中点
    ListNode* mid = getMiddle(head);
    ListNode* rightHead = mid->next;
    mid->next = nullptr;

    // 递归排序
    ListNode* left = sortListTopDown(head);
    ListNode* right = sortListTopDown(rightHead);

    // 合并
    return merge(left, right);
}

/**
 * 方法2: 自底向上归并排序（迭代）
 * 时间: O(n log n)
 * 空间: O(1)
 */
ListNode* sortListBottomUp(ListNode* head) {
    if (!head || !head->next) return head;

    // 计算链表长度
    int length = 0;
    for (ListNode* curr = head; curr; curr = curr->next) {
        ++length;
    }

    ListNode dummy(0);
    dummy.next = head;

    // 分割函数：从 head 开始分割出 n 个节点，返回剩余部分
    auto split = [](ListNode* head, int n) -> ListNode* {
        for (int i = 1; head && i < n; ++i) {
            head = head->next;
        }
        if (!head) return nullptr;
        ListNode* second = head->next;
        head->next = nullptr;
        return second;
    };

    // 合并函数：合并 l1 和 l2，将结果接到 tail 后面，返回新的 tail
    auto mergeAndGetTail = [](ListNode* l1, ListNode* l2, ListNode* tail) -> ListNode* {
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
        while (tail->next) tail = tail->next;
        return tail;
    };

    // 从 size=1 开始，每次翻倍
    for (int size = 1; size < length; size *= 2) {
        ListNode* curr = dummy.next;
        ListNode* tail = &dummy;

        while (curr) {
            ListNode* left = curr;
            ListNode* right = split(left, size);
            curr = split(right, size);
            tail = mergeAndGetTail(left, right, tail);
        }
    }

    return dummy.next;
}

/**
 * 方法3: 链表快速排序
 * 时间: O(n log n) 平均，O(n²) 最坏
 * 空间: O(log n) 递归栈
 */
ListNode* sortListQuickSort(ListNode* head) {
    if (!head || !head->next) return head;

    // 以头节点为 pivot，划分为三部分
    ListNode lessHead(0), equalHead(0), greaterHead(0);
    ListNode* less = &lessHead;
    ListNode* equal = &equalHead;
    ListNode* greater = &greaterHead;

    int pivot = head->val;
    ListNode* curr = head;

    while (curr) {
        if (curr->val < pivot) {
            less->next = curr;
            less = less->next;
        } else if (curr->val > pivot) {
            greater->next = curr;
            greater = greater->next;
        } else {
            equal->next = curr;
            equal = equal->next;
        }
        curr = curr->next;
    }

    less->next = nullptr;
    equal->next = nullptr;
    greater->next = nullptr;

    // 递归排序
    ListNode* sortedLess = sortListQuickSort(lessHead.next);
    ListNode* sortedGreater = sortListQuickSort(greaterHead.next);

    // 连接三部分
    ListNode dummy(0);
    ListNode* tail = &dummy;

    // 连接 less 部分
    if (sortedLess) {
        tail->next = sortedLess;
        while (tail->next) tail = tail->next;
    }

    // 连接 equal 部分
    tail->next = equalHead.next;
    while (tail->next) tail = tail->next;

    // 连接 greater 部分
    tail->next = sortedGreater;

    return dummy.next;
}

/**
 * 方法4: 链表插入排序
 * 时间: O(n²)
 * 空间: O(1)
 */
ListNode* sortListInsertion(ListNode* head) {
    if (!head || !head->next) return head;

    ListNode dummy(0);
    ListNode* curr = head;

    while (curr) {
        ListNode* next = curr->next;

        // 找到插入位置
        ListNode* prev = &dummy;
        while (prev->next && prev->next->val < curr->val) {
            prev = prev->next;
        }

        // 插入
        curr->next = prev->next;
        prev->next = curr;

        curr = next;
    }

    return dummy.next;
}

// 默认使用自底向上归并
ListNode* sortList(ListNode* head) {
    return sortListBottomUp(head);
}

// ==================== 题目2: 链表插入排序 ====================

ListNode* insertionSortList(ListNode* head) {
    return sortListInsertion(head);
}

// ==================== 题目3: 合并两个有序链表 ====================

/**
 * 方法1: 迭代
 */
ListNode* mergeTwoListsIterative(ListNode* l1, ListNode* l2) {
    return merge(l1, l2);
}

/**
 * 方法2: 递归
 */
ListNode* mergeTwoLists(ListNode* l1, ListNode* l2) {
    return mergeRecursive(l1, l2);
}

// ==================== 题目4: 合并K个升序链表 ====================

/**
 * 方法1: 优先队列（最小堆）
 * 时间: O(N log K)，N 是总节点数，K 是链表数
 * 空间: O(K)
 */
ListNode* mergeKListsHeap(std::vector<ListNode*>& lists) {
    auto cmp = [](ListNode* a, ListNode* b) { return a->val > b->val; };
    std::priority_queue<ListNode*, std::vector<ListNode*>, decltype(cmp)> minHeap(cmp);

    for (ListNode* list : lists) {
        if (list) minHeap.push(list);
    }

    ListNode dummy;
    ListNode* curr = &dummy;

    while (!minHeap.empty()) {
        ListNode* node = minHeap.top();
        minHeap.pop();
        curr->next = node;
        curr = curr->next;
        if (node->next) {
            minHeap.push(node->next);
        }
    }

    return dummy.next;
}

/**
 * 方法2: 分治归并
 * 时间: O(N log K)
 * 空间: O(log K) 递归栈
 */
ListNode* mergeKListsDivide(std::vector<ListNode*>& lists, int left, int right) {
    if (left > right) return nullptr;
    if (left == right) return lists[left];

    int mid = left + (right - left) / 2;
    ListNode* leftList = mergeKListsDivide(lists, left, mid);
    ListNode* rightList = mergeKListsDivide(lists, mid + 1, right);
    return merge(leftList, rightList);
}

ListNode* mergeKListsDivideConquer(std::vector<ListNode*>& lists) {
    if (lists.empty()) return nullptr;
    return mergeKListsDivide(lists, 0, lists.size() - 1);
}

/**
 * 方法3: 逐一合并
 * 时间: O(NK)
 * 空间: O(1)
 */
ListNode* mergeKListsOneByOne(std::vector<ListNode*>& lists) {
    ListNode* result = nullptr;
    for (ListNode* list : lists) {
        result = merge(result, list);
    }
    return result;
}

ListNode* mergeKLists(std::vector<ListNode*>& lists) {
    return mergeKListsHeap(lists);
}

// ==================== 题目5: 链表中的第 K 大节点 ====================

/**
 * 方法1: 排序后遍历
 * 时间: O(n log n)
 */
int findKthLargestSort(ListNode* head, int k) {
    std::vector<int> values;
    while (head) {
        values.push_back(head->val);
        head = head->next;
    }
    if (k > static_cast<int>(values.size())) return -1;

    std::sort(values.begin(), values.end(), std::greater<int>());
    return values[k - 1];
}

/**
 * 方法2: 最小堆
 * 时间: O(n log k)
 */
int findKthLargestHeap(ListNode* head, int k) {
    std::priority_queue<int, std::vector<int>, std::greater<int>> minHeap;

    while (head) {
        minHeap.push(head->val);
        if (static_cast<int>(minHeap.size()) > k) {
            minHeap.pop();
        }
        head = head->next;
    }

    if (static_cast<int>(minHeap.size()) < k) return -1;
    return minHeap.top();
}

/**
 * 方法3: 快速选择
 * 时间: O(n) 平均
 */
int findKthLargestQuickSelect(ListNode* head, int k) {
    std::vector<int> values;
    while (head) {
        values.push_back(head->val);
        head = head->next;
    }
    if (k > static_cast<int>(values.size())) return -1;

    std::mt19937 rng(std::random_device{}());

    std::function<int(int, int, int)> quickSelect = [&](int left, int right, int target) {
        if (left == right) return values[left];

        std::uniform_int_distribution<int> dist(left, right);
        int pivotIdx = dist(rng);
        std::swap(values[pivotIdx], values[right]);

        int pivot = values[right];
        int i = left;
        for (int j = left; j < right; ++j) {
            if (values[j] > pivot) {
                std::swap(values[i++], values[j]);
            }
        }
        std::swap(values[i], values[right]);

        if (i == target) return values[i];
        else if (i < target) return quickSelect(i + 1, right, target);
        else return quickSelect(left, i - 1, target);
    };

    return quickSelect(0, values.size() - 1, k - 1);
}

int findKthLargest(ListNode* head, int k) {
    return findKthLargestHeap(head, k);
}

// ==================== 题目6: 链表中的前 K 大节点 ====================

/**
 * 方法1: 排序后取前 K 个
 */
std::vector<int> topKLargestSort(ListNode* head, int k) {
    std::vector<int> values;
    while (head) {
        values.push_back(head->val);
        head = head->next;
    }

    std::sort(values.begin(), values.end(), std::greater<int>());

    std::vector<int> result;
    for (int i = 0; i < k && i < static_cast<int>(values.size()); ++i) {
        result.push_back(values[i]);
    }
    return result;
}

/**
 * 方法2: 最小堆
 */
std::vector<int> topKLargest(ListNode* head, int k) {
    std::priority_queue<int, std::vector<int>, std::greater<int>> minHeap;

    while (head) {
        minHeap.push(head->val);
        if (static_cast<int>(minHeap.size()) > k) {
            minHeap.pop();
        }
        head = head->next;
    }

    std::vector<int> result;
    while (!minHeap.empty()) {
        result.push_back(minHeap.top());
        minHeap.pop();
    }
    std::reverse(result.begin(), result.end());
    return result;
}

// ==================== 题目7: 链表中倒数第 K 个节点 ====================

/**
 * 方法1: 两次遍历
 */
ListNode* getKthFromEndTwoPass(ListNode* head, int k) {
    int length = 0;
    for (ListNode* curr = head; curr; curr = curr->next) {
        ++length;
    }

    if (k > length) return nullptr;

    ListNode* curr = head;
    for (int i = 0; i < length - k; ++i) {
        curr = curr->next;
    }
    return curr;
}

/**
 * 方法2: 快慢指针（一次遍历）
 */
ListNode* getKthFromEnd(ListNode* head, int k) {
    ListNode* fast = head;
    ListNode* slow = head;

    // fast 先走 k 步
    for (int i = 0; i < k; ++i) {
        if (!fast) return nullptr;
        fast = fast->next;
    }

    // 同时移动
    while (fast) {
        fast = fast->next;
        slow = slow->next;
    }

    return slow;
}

// ==================== 题目8: 分隔链表 ====================

ListNode* partition(ListNode* head, int x) {
    ListNode lessHead(0), greaterHead(0);
    ListNode* less = &lessHead;
    ListNode* greater = &greaterHead;

    while (head) {
        if (head->val < x) {
            less->next = head;
            less = less->next;
        } else {
            greater->next = head;
            greater = greater->next;
        }
        head = head->next;
    }

    greater->next = nullptr;
    less->next = greaterHead.next;

    return lessHead.next;
}

// ==================== 题目9: 链表三路划分 ====================

ListNode* partitionThreeWay(ListNode* head, int pivot) {
    ListNode lessHead(0), equalHead(0), greaterHead(0);
    ListNode* less = &lessHead;
    ListNode* equal = &equalHead;
    ListNode* greater = &greaterHead;

    while (head) {
        if (head->val < pivot) {
            less->next = head;
            less = less->next;
        } else if (head->val > pivot) {
            greater->next = head;
            greater = greater->next;
        } else {
            equal->next = head;
            equal = equal->next;
        }
        head = head->next;
    }

    // 连接三部分
    greater->next = nullptr;
    equal->next = greaterHead.next;
    less->next = equalHead.next;

    return lessHead.next;
}

// ==================== 题目10: 奇偶链表 ====================

ListNode* oddEvenList(ListNode* head) {
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

// ==================== 题目11: 重排链表 ====================

void reorderList(ListNode* head) {
    if (!head || !head->next) return;

    // 1. 找中点
    ListNode* slow = head;
    ListNode* fast = head;
    while (fast->next && fast->next->next) {
        slow = slow->next;
        fast = fast->next->next;
    }

    // 2. 反转后半部分
    ListNode* second = slow->next;
    slow->next = nullptr;

    ListNode* prev = nullptr;
    while (second) {
        ListNode* next = second->next;
        second->next = prev;
        prev = second;
        second = next;
    }
    second = prev;

    // 3. 合并两个链表
    ListNode* first = head;
    while (second) {
        ListNode* firstNext = first->next;
        ListNode* secondNext = second->next;
        first->next = second;
        second->next = firstNext;
        first = firstNext;
        second = secondNext;
    }
}

// ==================== 题目12: 有序链表交集 ====================

ListNode* getIntersectionSorted(ListNode* l1, ListNode* l2) {
    ListNode dummy;
    ListNode* curr = &dummy;

    while (l1 && l2) {
        if (l1->val == l2->val) {
            curr->next = new ListNode(l1->val);
            curr = curr->next;
            l1 = l1->next;
            l2 = l2->next;
        } else if (l1->val < l2->val) {
            l1 = l1->next;
        } else {
            l2 = l2->next;
        }
    }

    return dummy.next;
}

// ==================== 题目13: 删除排序链表中的重复元素 ====================

ListNode* deleteDuplicates(ListNode* head) {
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

// ==================== 题目14: 删除排序链表中的重复元素 II ====================

/**
 * 方法1: 迭代
 */
ListNode* deleteDuplicatesIIIterative(ListNode* head) {
    ListNode dummy(0, head);
    ListNode* prev = &dummy;

    while (head) {
        // 检查是否有重复
        if (head->next && head->val == head->next->val) {
            // 跳过所有重复节点
            while (head->next && head->val == head->next->val) {
                ListNode* toDelete = head;
                head = head->next;
                delete toDelete;
            }
            // 删除最后一个重复节点
            ListNode* toDelete = head;
            head = head->next;
            delete toDelete;
            prev->next = head;
        } else {
            prev = prev->next;
            head = head->next;
        }
    }

    return dummy.next;
}

/**
 * 方法2: 递归
 */
ListNode* deleteDuplicatesIIRecursive(ListNode* head) {
    if (!head || !head->next) return head;

    if (head->val == head->next->val) {
        // 跳过所有重复节点
        while (head->next && head->val == head->next->val) {
            ListNode* toDelete = head;
            head = head->next;
            delete toDelete;
        }
        // 删除最后一个重复节点，递归处理剩余部分
        ListNode* toDelete = head;
        head = head->next;
        delete toDelete;
        return deleteDuplicatesIIRecursive(head);
    } else {
        head->next = deleteDuplicatesIIRecursive(head->next);
        return head;
    }
}

ListNode* deleteDuplicatesII(ListNode* head) {
    return deleteDuplicatesIIIterative(head);
}

// ==================== 题目15: 旋转链表 ====================

ListNode* rotateRight(ListNode* head, int k) {
    if (!head || !head->next || k == 0) return head;

    // 计算长度并找到尾节点
    int length = 1;
    ListNode* tail = head;
    while (tail->next) {
        ++length;
        tail = tail->next;
    }

    // 计算实际旋转次数
    k = k % length;
    if (k == 0) return head;

    // 找到新的尾节点（倒数第 k+1 个节点）
    ListNode* newTail = head;
    for (int i = 0; i < length - k - 1; ++i) {
        newTail = newTail->next;
    }

    // 重新连接
    ListNode* newHead = newTail->next;
    newTail->next = nullptr;
    tail->next = head;

    return newHead;
}


// ==================== 测试代码 ====================
#include <cassert>

ListNode* createList(const std::vector<int>& vals) {
    ListNode dummy;
    ListNode* curr = &dummy;
    for (int val : vals) {
        curr->next = new ListNode(val);
        curr = curr->next;
    }
    return dummy.next;
}

std::vector<int> listToVector(ListNode* head) {
    std::vector<int> result;
    while (head) {
        result.push_back(head->val);
        head = head->next;
    }
    return result;
}

void freeList(ListNode* head) {
    while (head) {
        ListNode* next = head->next;
        delete head;
        head = next;
    }
}

void testSortList() {
    // 测试自顶向下归并
    ListNode* head1 = createList({4, 2, 1, 3});
    ListNode* sorted1 = sortListTopDown(head1);
    assert((listToVector(sorted1) == std::vector<int>{1, 2, 3, 4}));
    freeList(sorted1);

    // 测试自底向上归并
    ListNode* head2 = createList({4, 2, 1, 3});
    ListNode* sorted2 = sortListBottomUp(head2);
    assert((listToVector(sorted2) == std::vector<int>{1, 2, 3, 4}));
    freeList(sorted2);

    // 测试快速排序
    ListNode* head3 = createList({4, 2, 1, 3});
    ListNode* sorted3 = sortListQuickSort(head3);
    assert((listToVector(sorted3) == std::vector<int>{1, 2, 3, 4}));
    freeList(sorted3);

    // 测试插入排序
    ListNode* head4 = createList({4, 2, 1, 3});
    ListNode* sorted4 = sortListInsertion(head4);
    assert((listToVector(sorted4) == std::vector<int>{1, 2, 3, 4}));
    freeList(sorted4);

    std::cout << "sortList (all methods): PASSED\n";
}

void testMergeTwoLists() {
    ListNode* l1 = createList({1, 2, 4});
    ListNode* l2 = createList({1, 3, 4});
    ListNode* merged = mergeTwoLists(l1, l2);
    assert((listToVector(merged) == std::vector<int>{1, 1, 2, 3, 4, 4}));
    freeList(merged);
    std::cout << "mergeTwoLists: PASSED\n";
}

void testMergeKLists() {
    // 测试堆方法
    std::vector<ListNode*> lists1 = {
        createList({1, 4, 5}),
        createList({1, 3, 4}),
        createList({2, 6})
    };
    ListNode* merged1 = mergeKListsHeap(lists1);
    assert((listToVector(merged1) == std::vector<int>{1, 1, 2, 3, 4, 4, 5, 6}));
    freeList(merged1);

    // 测试分治方法
    std::vector<ListNode*> lists2 = {
        createList({1, 4, 5}),
        createList({1, 3, 4}),
        createList({2, 6})
    };
    ListNode* merged2 = mergeKListsDivideConquer(lists2);
    assert((listToVector(merged2) == std::vector<int>{1, 1, 2, 3, 4, 4, 5, 6}));
    freeList(merged2);

    std::cout << "mergeKLists (all methods): PASSED\n";
}

void testFindKthLargest() {
    ListNode* head1 = createList({3, 1, 4, 1, 5, 9, 2, 6});
    assert(findKthLargestSort(head1, 3) == 5);
    freeList(head1);

    ListNode* head2 = createList({3, 1, 4, 1, 5, 9, 2, 6});
    assert(findKthLargestHeap(head2, 3) == 5);
    freeList(head2);

    ListNode* head3 = createList({3, 1, 4, 1, 5, 9, 2, 6});
    assert(findKthLargestQuickSelect(head3, 3) == 5);
    freeList(head3);

    std::cout << "findKthLargest (all methods): PASSED\n";
}

void testTopKLargest() {
    ListNode* head = createList({3, 1, 4, 1, 5, 9, 2, 6});
    auto result = topKLargest(head, 3);
    assert((result == std::vector<int>{9, 6, 5}));
    freeList(head);
    std::cout << "topKLargest: PASSED\n";
}

void testGetKthFromEnd() {
    ListNode* head1 = createList({1, 2, 3, 4, 5});
    ListNode* result1 = getKthFromEndTwoPass(head1, 2);
    assert(result1->val == 4);

    ListNode* head2 = createList({1, 2, 3, 4, 5});
    ListNode* result2 = getKthFromEnd(head2, 2);
    assert(result2->val == 4);

    freeList(head1);
    freeList(head2);
    std::cout << "getKthFromEnd (all methods): PASSED\n";
}

void testPartition() {
    ListNode* head = createList({1, 4, 3, 2, 5, 2});
    ListNode* result = partition(head, 3);
    auto vec = listToVector(result);
    int i = 0;
    while (i < static_cast<int>(vec.size()) && vec[i] < 3) ++i;
    while (i < static_cast<int>(vec.size())) {
        assert(vec[i] >= 3);
        ++i;
    }
    freeList(result);
    std::cout << "partition: PASSED\n";
}

void testPartitionThreeWay() {
    ListNode* head = createList({3, 5, 8, 5, 10, 2, 1});
    ListNode* result = partitionThreeWay(head, 5);
    auto vec = listToVector(result);
    // 验证：小于5的在前，等于5的在中间，大于5的在后
    int i = 0;
    while (i < static_cast<int>(vec.size()) && vec[i] < 5) ++i;
    while (i < static_cast<int>(vec.size()) && vec[i] == 5) ++i;
    while (i < static_cast<int>(vec.size())) {
        assert(vec[i] > 5);
        ++i;
    }
    freeList(result);
    std::cout << "partitionThreeWay: PASSED\n";
}

void testOddEvenList() {
    ListNode* head = createList({1, 2, 3, 4, 5});
    ListNode* result = oddEvenList(head);
    assert((listToVector(result) == std::vector<int>{1, 3, 5, 2, 4}));
    freeList(result);
    std::cout << "oddEvenList: PASSED\n";
}

void testReorderList() {
    ListNode* head = createList({1, 2, 3, 4});
    reorderList(head);
    assert((listToVector(head) == std::vector<int>{1, 4, 2, 3}));
    freeList(head);
    std::cout << "reorderList: PASSED\n";
}

void testGetIntersectionSorted() {
    ListNode* l1 = createList({1, 2, 3, 4, 5});
    ListNode* l2 = createList({2, 4, 6, 8});
    ListNode* result = getIntersectionSorted(l1, l2);
    assert((listToVector(result) == std::vector<int>{2, 4}));
    freeList(l1);
    freeList(l2);
    freeList(result);
    std::cout << "getIntersectionSorted: PASSED\n";
}

void testDeleteDuplicates() {
    ListNode* head = createList({1, 1, 2, 3, 3});
    ListNode* result = deleteDuplicates(head);
    assert((listToVector(result) == std::vector<int>{1, 2, 3}));
    freeList(result);
    std::cout << "deleteDuplicates: PASSED\n";
}

void testDeleteDuplicatesII() {
    ListNode* head1 = createList({1, 2, 3, 3, 4, 4, 5});
    ListNode* result1 = deleteDuplicatesIIIterative(head1);
    assert((listToVector(result1) == std::vector<int>{1, 2, 5}));
    freeList(result1);

    ListNode* head2 = createList({1, 2, 3, 3, 4, 4, 5});
    ListNode* result2 = deleteDuplicatesIIRecursive(head2);
    assert((listToVector(result2) == std::vector<int>{1, 2, 5}));
    freeList(result2);

    std::cout << "deleteDuplicatesII (all methods): PASSED\n";
}

void testRotateRight() {
    ListNode* head = createList({1, 2, 3, 4, 5});
    ListNode* result = rotateRight(head, 2);
    assert((listToVector(result) == std::vector<int>{4, 5, 1, 2, 3}));
    freeList(result);
    std::cout << "rotateRight: PASSED\n";
}

int main() {
    std::cout << "=== Linked List Sort Solution Tests ===\n";

    testSortList();
    testMergeTwoLists();
    testMergeKLists();
    testFindKthLargest();
    testTopKLargest();
    testGetKthFromEnd();
    testPartition();
    testPartitionThreeWay();
    testOddEvenList();
    testReorderList();
    testGetIntersectionSorted();
    testDeleteDuplicates();
    testDeleteDuplicatesII();
    testRotateRight();

    std::cout << "\nAll tests passed!\n";
    return 0;
}
