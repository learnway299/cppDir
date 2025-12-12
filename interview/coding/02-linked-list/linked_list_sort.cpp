/**
 * @file linked_list_sort.cpp
 * @brief 链表排序专题 - 题目文件
 *
 * 链表排序的特点：
 * 1. 不支持随机访问，无法使用快排的 Hoare 划分
 * 2. 插入/删除 O(1)，适合归并排序
 * 3. 需要特别注意指针操作和边界条件
 *
 * 常见面试题：
 * - 链表排序（归并、插入、快排）
 * - 链表 Top K 问题
 * - 链表重排问题
 * - 链表划分问题
 */

#include <iostream>
#include <vector>
#include <queue>

/**
 * 链表节点定义
 */
struct ListNode {
    int val;
    ListNode* next;
    ListNode() : val(0), next(nullptr) {}
    ListNode(int x) : val(x), next(nullptr) {}
    ListNode(int x, ListNode* next) : val(x), next(next) {}
};

/**
 * 题目1: 排序链表
 * LeetCode 148
 *
 * 给你链表的头结点 head，请将其按升序排列并返回排序后的链表。
 *
 * 进阶：你可以在 O(n log n) 时间复杂度和 O(1) 空间复杂度下完成吗？
 *
 * 示例:
 *   输入: head = [4,2,1,3]
 *   输出: [1,2,3,4]
 *
 * 要求实现：
 *   - 方法1：自顶向下归并排序（递归）O(log n) 空间
 *   - 方法2：自底向上归并排序（迭代）O(1) 空间
 *   - 方法3：快速排序
 *   - 方法4：插入排序
 */
ListNode* sortList(ListNode* head) {
    // TODO: 实现链表排序
    return nullptr;
}

/**
 * 题目2: 对链表进行插入排序
 * LeetCode 147
 *
 * 给定单链表的头节点 head，使用插入排序对链表进行排序，并返回排序后链表的头。
 *
 * 插入排序算法：
 * 1. 从第一个元素开始，该元素可以认为已被排序
 * 2. 取出下一个元素，在已排序的元素序列中从后向前扫描
 * 3. 如果该元素大于新元素，将该元素移到下一位置
 * 4. 重复步骤3，直到找到已排序的元素小于或者等于新元素的位置
 * 5. 将新元素插入到该位置后
 * 6. 重复步骤2~5
 *
 * 示例:
 *   输入: head = [4,2,1,3]
 *   输出: [1,2,3,4]
 */
ListNode* insertionSortList(ListNode* head) {
    // TODO: 实现链表插入排序
    return nullptr;
}

/**
 * 题目3: 合并两个有序链表
 * LeetCode 21
 *
 * 将两个升序链表合并为一个新的升序链表并返回。
 * 新链表是通过拼接给定的两个链表的所有节点组成的。
 *
 * 示例:
 *   输入: l1 = [1,2,4], l2 = [1,3,4]
 *   输出: [1,1,2,3,4,4]
 *
 * 要求实现：
 *   - 方法1：迭代
 *   - 方法2：递归
 */
ListNode* mergeTwoLists(ListNode* l1, ListNode* l2) {
    // TODO: 实现合并两个有序链表
    return nullptr;
}

/**
 * 题目4: 合并K个升序链表
 * LeetCode 23
 *
 * 给你一个链表数组，每个链表都已经按升序排列。
 * 请你将所有链表合并到一个升序链表中，返回合并后的链表。
 *
 * 示例:
 *   输入: lists = [[1,4,5],[1,3,4],[2,6]]
 *   输出: [1,1,2,3,4,4,5,6]
 *
 * 要求实现：
 *   - 方法1：优先队列（最小堆）O(N log K)
 *   - 方法2：分治归并 O(N log K)
 *   - 方法3：逐一合并 O(NK)
 */
ListNode* mergeKLists(std::vector<ListNode*>& lists) {
    // TODO: 实现合并K个有序链表
    return nullptr;
}

/**
 * 题目5: 链表中的第 K 大节点
 *
 * 给定一个单链表，返回链表中第 K 大的节点值。
 * 如果 K 大于链表长度，返回 -1。
 *
 * 示例:
 *   输入: head = [3,1,4,1,5,9,2,6], k = 3
 *   输出: 5
 *   解释: 排序后 [1,1,2,3,4,5,6,9]，第 3 大是 5
 *
 * 要求实现：
 *   - 方法1：排序后遍历 O(n log n)
 *   - 方法2：最小堆 O(n log k)
 *   - 方法3：快速选择 O(n) 平均
 */
int findKthLargest(ListNode* head, int k) {
    // TODO: 实现链表第 K 大
    return -1;
}

/**
 * 题目6: 链表中的前 K 大节点
 *
 * 给定一个单链表，返回链表中前 K 大的节点值（降序排列）。
 *
 * 示例:
 *   输入: head = [3,1,4,1,5,9,2,6], k = 3
 *   输出: [9,6,5]
 *
 * 要求实现：
 *   - 方法1：排序后取前 K 个
 *   - 方法2：最小堆维护 K 个最大值
 */
std::vector<int> topKLargest(ListNode* head, int k) {
    // TODO: 实现链表前 K 大
    return {};
}

/**
 * 题目7: 链表中倒数第 K 个节点
 * 剑指 Offer 22
 *
 * 输入一个链表，输出该链表中倒数第 k 个节点。
 * 链表的尾节点是倒数第1个节点。
 *
 * 示例:
 *   输入: head = [1,2,3,4,5], k = 2
 *   输出: 该链表倒数第 2 个节点（值为 4 的节点）
 *
 * 要求实现：
 *   - 方法1：两次遍历
 *   - 方法2：快慢指针（一次遍历）
 */
ListNode* getKthFromEnd(ListNode* head, int k) {
    // TODO: 实现倒数第 K 个节点
    return nullptr;
}

/**
 * 题目8: 分隔链表
 * LeetCode 86
 *
 * 给你一个链表的头节点 head 和一个特定值 x，
 * 请你对链表进行分隔，使得所有小于 x 的节点都出现在大于或等于 x 的节点之前。
 * 你应当保留两个分区中每个节点的初始相对位置。
 *
 * 示例:
 *   输入: head = [1,4,3,2,5,2], x = 3
 *   输出: [1,2,2,4,3,5]
 */
ListNode* partition(ListNode* head, int x) {
    // TODO: 实现链表分隔
    return nullptr;
}

/**
 * 题目9: 链表三路划分（类似荷兰国旗）
 *
 * 给定一个链表和一个值 pivot，将链表分为三部分：
 * 小于 pivot | 等于 pivot | 大于 pivot
 * 保持各部分内节点的相对顺序不变。
 *
 * 示例:
 *   输入: head = [3,5,8,5,10,2,1], pivot = 5
 *   输出: [3,2,1,5,5,8,10]
 */
ListNode* partitionThreeWay(ListNode* head, int pivot) {
    // TODO: 实现链表三路划分
    return nullptr;
}

/**
 * 题目10: 奇偶链表
 * LeetCode 328
 *
 * 给定单链表的头节点 head，将所有索引为奇数的节点和索引为偶数的节点分别组合在一起，
 * 然后返回重新排序的列表。
 * 第一个节点的索引被认为是奇数，第二个节点的索引为偶数，以此类推。
 *
 * 示例:
 *   输入: head = [1,2,3,4,5]
 *   输出: [1,3,5,2,4]
 */
ListNode* oddEvenList(ListNode* head) {
    // TODO: 实现奇偶链表
    return nullptr;
}

/**
 * 题目11: 重排链表
 * LeetCode 143
 *
 * 给定一个单链表 L: L0 → L1 → … → Ln-1 → Ln
 * 将其重新排列后变为: L0 → Ln → L1 → Ln-1 → L2 → Ln-2 → …
 *
 * 你不能只是单纯地改变节点内部的值，而是需要实际进行节点交换。
 *
 * 示例:
 *   输入: head = [1,2,3,4]
 *   输出: [1,4,2,3]
 *
 * 思路：找中点 + 反转后半部分 + 合并
 */
void reorderList(ListNode* head) {
    // TODO: 实现重排链表
}

/**
 * 题目12: 按升序排列的两个链表的交集
 *
 * 给定两个按升序排列的链表，返回一个新链表，包含两个链表的交集元素。
 * 结果链表也应该按升序排列。
 *
 * 示例:
 *   输入: l1 = [1,2,3,4,5], l2 = [2,4,6,8]
 *   输出: [2,4]
 */
ListNode* getIntersectionSorted(ListNode* l1, ListNode* l2) {
    // TODO: 实现有序链表交集
    return nullptr;
}

/**
 * 题目13: 删除排序链表中的重复元素
 * LeetCode 83
 *
 * 给定一个已排序的链表的头 head，删除所有重复的元素，使每个元素只出现一次。
 * 返回已排序的链表。
 *
 * 示例:
 *   输入: head = [1,1,2,3,3]
 *   输出: [1,2,3]
 */
ListNode* deleteDuplicates(ListNode* head) {
    // TODO: 实现删除重复元素
    return nullptr;
}

/**
 * 题目14: 删除排序链表中的重复元素 II
 * LeetCode 82
 *
 * 给定一个已排序的链表的头 head，删除原始链表中所有重复数字的节点，
 * 只留下不同的数字。返回已排序的链表。
 *
 * 示例:
 *   输入: head = [1,2,3,3,4,4,5]
 *   输出: [1,2,5]
 *
 * 要求实现：
 *   - 方法1：迭代
 *   - 方法2：递归
 */
ListNode* deleteDuplicatesII(ListNode* head) {
    // TODO: 实现删除所有重复元素
    return nullptr;
}

/**
 * 题目15: 旋转链表
 * LeetCode 61
 *
 * 给你一个链表的头节点 head，旋转链表，将链表每个节点向右移动 k 个位置。
 *
 * 示例:
 *   输入: head = [1,2,3,4,5], k = 2
 *   输出: [4,5,1,2,3]
 */
ListNode* rotateRight(ListNode* head, int k) {
    // TODO: 实现旋转链表
    return nullptr;
}


// ==================== 测试代码 ====================
#include <cassert>

// 辅助函数：创建链表
ListNode* createList(const std::vector<int>& vals) {
    ListNode dummy;
    ListNode* curr = &dummy;
    for (int val : vals) {
        curr->next = new ListNode(val);
        curr = curr->next;
    }
    return dummy.next;
}

// 辅助函数：链表转数组
std::vector<int> listToVector(ListNode* head) {
    std::vector<int> result;
    while (head) {
        result.push_back(head->val);
        head = head->next;
    }
    return result;
}

// 辅助函数：释放链表
void freeList(ListNode* head) {
    while (head) {
        ListNode* next = head->next;
        delete head;
        head = next;
    }
}

void testSortList() {
    ListNode* head = createList({4, 2, 1, 3});
    ListNode* sorted = sortList(head);
    assert((listToVector(sorted) == std::vector<int>{1, 2, 3, 4}));
    freeList(sorted);
    std::cout << "sortList: PASSED\n";
}

void testInsertionSortList() {
    ListNode* head = createList({4, 2, 1, 3});
    ListNode* sorted = insertionSortList(head);
    assert((listToVector(sorted) == std::vector<int>{1, 2, 3, 4}));
    freeList(sorted);
    std::cout << "insertionSortList: PASSED\n";
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
    std::vector<ListNode*> lists = {
        createList({1, 4, 5}),
        createList({1, 3, 4}),
        createList({2, 6})
    };
    ListNode* merged = mergeKLists(lists);
    assert((listToVector(merged) == std::vector<int>{1, 1, 2, 3, 4, 4, 5, 6}));
    freeList(merged);
    std::cout << "mergeKLists: PASSED\n";
}

void testFindKthLargest() {
    ListNode* head = createList({3, 1, 4, 1, 5, 9, 2, 6});
    assert(findKthLargest(head, 3) == 5);
    freeList(head);
    std::cout << "findKthLargest: PASSED\n";
}

void testPartition() {
    ListNode* head = createList({1, 4, 3, 2, 5, 2});
    ListNode* result = partition(head, 3);
    auto vec = listToVector(result);
    // 验证小于 3 的在前，大于等于 3 的在后
    int i = 0;
    while (i < static_cast<int>(vec.size()) && vec[i] < 3) ++i;
    while (i < static_cast<int>(vec.size())) {
        assert(vec[i] >= 3);
        ++i;
    }
    freeList(result);
    std::cout << "partition: PASSED\n";
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

void testDeleteDuplicates() {
    ListNode* head = createList({1, 1, 2, 3, 3});
    ListNode* result = deleteDuplicates(head);
    assert((listToVector(result) == std::vector<int>{1, 2, 3}));
    freeList(result);
    std::cout << "deleteDuplicates: PASSED\n";
}

void testRotateRight() {
    ListNode* head = createList({1, 2, 3, 4, 5});
    ListNode* result = rotateRight(head, 2);
    assert((listToVector(result) == std::vector<int>{4, 5, 1, 2, 3}));
    freeList(result);
    std::cout << "rotateRight: PASSED\n";
}

int main() {
    std::cout << "=== Linked List Sort Tests ===\n";

    // 取消注释以运行测试
    // testSortList();
    // testInsertionSortList();
    // testMergeTwoLists();
    // testMergeKLists();
    // testFindKthLargest();
    // testPartition();
    // testOddEvenList();
    // testReorderList();
    // testDeleteDuplicates();
    // testRotateRight();

    std::cout << "All tests passed!\n";
    return 0;
}
