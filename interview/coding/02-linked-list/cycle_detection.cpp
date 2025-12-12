/**
 * @file cycle_detection.cpp
 * @brief 链表环检测相关问题 - 题目文件
 *
 * 链表环检测是经典的双指针问题，掌握快慢指针技巧是关键。
 */

#include <iostream>

struct ListNode {
    int val;
    ListNode* next;
    ListNode(int x) : val(x), next(nullptr) {}
};

/**
 * 题目1: 环形链表
 *
 * 给你一个链表的头节点 head，判断链表中是否有环。
 *
 * 如果链表中有某个节点，可以通过连续跟踪 next 指针再次到达该节点，
 * 则链表中存在环。
 *
 * 示例:
 *   输入: head = [3,2,0,-4], pos = 1 (表示尾节点连接到下标为1的节点)
 *   输出: true
 *
 *   输入: head = [1,2], pos = 0
 *   输出: true
 *
 *   输入: head = [1], pos = -1
 *   输出: false
 *
 * 要求: 使用 O(1) 空间复杂度
 *
 * @param head 链表头节点
 * @return 是否有环
 */
bool hasCycle(ListNode* head) {
    // TODO: 实现你的解法
    return false;
}

/**
 * 题目2: 环形链表 II
 *
 * 给定一个链表的头节点 head，返回链表开始入环的第一个节点。
 * 如果链表无环，则返回 null。
 *
 * 数学推导（面试重点）:
 *   设环前长度为 a，环长为 b
 *   快指针走的距离: 2s = s + nb (快指针比慢指针多走 n 圈)
 *   所以 s = nb
 *   入环点 = a + nb，慢指针已走 nb，再走 a 步即可到达入环点
 *   让一个指针从头开始，两个同速前进，相遇点即为入环点
 *
 * 示例:
 *   输入: head = [3,2,0,-4], pos = 1
 *   输出: 返回下标为 1 的节点
 *
 * @param head 链表头节点
 * @return 入环的第一个节点，无环返回 nullptr
 */
ListNode* detectCycle(ListNode* head) {
    // TODO: 实现你的解法
    return nullptr;
}

/**
 * 题目3: 快乐数
 *
 * 编写一个算法来判断一个数 n 是不是快乐数。
 *
 * 「快乐数」定义:
 *   - 对于一个正整数，每一次将该数替换为它每个位置上的数字的平方和
 *   - 重复这个过程直到这个数变为 1，或者进入无限循环
 *   - 如果这个过程的结果为 1，则这个数是快乐数
 *
 * 示例:
 *   输入: n = 19
 *   输出: true
 *   解释:
 *     1² + 9² = 82
 *     8² + 2² = 68
 *     6² + 8² = 100
 *     1² + 0² + 0² = 1
 *
 *   输入: n = 2
 *   输出: false
 *
 * 提示: 可以将其看作链表环检测问题
 *
 * @param n 正整数
 * @return 是否为快乐数
 */
bool isHappy(int n) {
    // TODO: 实现你的解法
    return false;
}

/**
 * 题目4: 相交链表
 *
 * 给你两个单链表的头节点 headA 和 headB，
 * 请你找出并返回两个单链表相交的起始节点。
 * 如果两个链表不存在相交节点，返回 null。
 *
 * 注意: 相交是指节点相同（地址相同），不是值相同
 *
 * 示例:
 *   A:     4 -> 1 \
 *                   8 -> 4 -> 5
 *   B: 5 -> 6 -> 1 /
 *   输出: 节点 8
 *
 * 技巧:
 *   - 指针 A 走完链表 A 后走链表 B
 *   - 指针 B 走完链表 B 后走链表 A
 *   - 两指针会同时到达相交点（或同时到达 null）
 *
 * @param headA 链表 A 的头节点
 * @param headB 链表 B 的头节点
 * @return 相交的起始节点
 */
ListNode* getIntersectionNode(ListNode* headA, ListNode* headB) {
    // TODO: 实现你的解法
    return nullptr;
}


// ==================== 辅助函数 ====================

// 创建带环的链表
// vals: 节点值数组
// pos: 环的入口位置（-1 表示无环）
ListNode* createCyclicList(std::initializer_list<int> vals, int pos) {
    if (vals.size() == 0) return nullptr;

    ListNode dummy(0);
    ListNode* curr = &dummy;
    ListNode* cycleEntry = nullptr;
    int idx = 0;

    for (int val : vals) {
        curr->next = new ListNode(val);
        curr = curr->next;
        if (idx == pos) {
            cycleEntry = curr;
        }
        ++idx;
    }

    // 形成环
    if (pos >= 0 && cycleEntry) {
        curr->next = cycleEntry;
    }

    return dummy.next;
}

// 释放带环链表（需要特殊处理）
void deleteCyclicList(ListNode* head, int pos) {
    if (!head) return;

    // 先断开环
    if (pos >= 0) {
        ListNode* slow = head;
        ListNode* fast = head;
        while (fast && fast->next) {
            slow = slow->next;
            fast = fast->next->next;
            if (slow == fast) break;
        }
        if (slow == fast) {
            ListNode* ptr1 = head;
            ListNode* ptr2 = slow;
            ListNode* prev = nullptr;
            while (ptr1 != ptr2) {
                prev = ptr2;
                ptr1 = ptr1->next;
                ptr2 = ptr2->next;
            }
            // 找到环的最后一个节点并断开
            ListNode* temp = ptr1;
            while (temp->next != ptr1) {
                temp = temp->next;
            }
            temp->next = nullptr;
        }
    }

    // 正常释放
    while (head) {
        ListNode* temp = head;
        head = head->next;
        delete temp;
    }
}


// ==================== 测试代码 ====================
#include <cassert>

void testHasCycle() {
    // 有环
    ListNode* head = createCyclicList({3, 2, 0, -4}, 1);
    assert(hasCycle(head) == true);
    deleteCyclicList(head, 1);

    // 有环（单节点自环）
    ListNode* single = new ListNode(1);
    single->next = single;
    assert(hasCycle(single) == true);
    single->next = nullptr;
    delete single;

    // 无环
    head = createCyclicList({1, 2, 3}, -1);
    assert(hasCycle(head) == false);
    deleteCyclicList(head, -1);

    // 空链表
    assert(hasCycle(nullptr) == false);

    std::cout << "hasCycle: PASSED\n";
}

void testDetectCycle() {
    // 有环，入口在 pos=1
    ListNode* head = createCyclicList({3, 2, 0, -4}, 1);
    ListNode* entry = detectCycle(head);
    assert(entry != nullptr && entry->val == 2);
    deleteCyclicList(head, 1);

    // 无环
    head = createCyclicList({1, 2, 3}, -1);
    assert(detectCycle(head) == nullptr);
    deleteCyclicList(head, -1);

    std::cout << "detectCycle: PASSED\n";
}

void testIsHappy() {
    assert(isHappy(19) == true);
    assert(isHappy(2) == false);
    assert(isHappy(1) == true);
    assert(isHappy(7) == true);

    std::cout << "isHappy: PASSED\n";
}

void testGetIntersectionNode() {
    // 创建相交链表
    // A: 4 -> 1 -> 8 -> 4 -> 5
    // B: 5 -> 6 -> 1 -> 8 -> 4 -> 5
    ListNode* common = new ListNode(8);
    common->next = new ListNode(4);
    common->next->next = new ListNode(5);

    ListNode* headA = new ListNode(4);
    headA->next = new ListNode(1);
    headA->next->next = common;

    ListNode* headB = new ListNode(5);
    headB->next = new ListNode(6);
    headB->next->next = new ListNode(1);
    headB->next->next->next = common;

    ListNode* intersection = getIntersectionNode(headA, headB);
    assert(intersection == common);

    // 清理（注意不要重复删除）
    delete headA->next;
    delete headA;
    delete headB->next->next;
    delete headB->next;
    delete headB;
    delete common->next->next;
    delete common->next;
    delete common;

    // 不相交
    headA = new ListNode(1);
    headB = new ListNode(2);
    assert(getIntersectionNode(headA, headB) == nullptr);
    delete headA;
    delete headB;

    std::cout << "getIntersectionNode: PASSED\n";
}

int main() {
    std::cout << "=== Cycle Detection Tests ===\n";

    // 取消注释以运行测试（实现解法后）
    // testHasCycle();
    // testDetectCycle();
    // testIsHappy();
    // testGetIntersectionNode();

    std::cout << "All tests passed!\n";
    return 0;
}
