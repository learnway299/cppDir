/**
 * @file cycle_detection_solution.cpp
 * @brief 链表环检测相关问题 - 解答文件
 *
 * 包含 cycle_detection.cpp 中所有题目的解答和详细解析。
 */

#include <iostream>
#include <unordered_set>

struct ListNode {
    int val;
    ListNode* next;
    ListNode(int x) : val(x), next(nullptr) {}
};

/**
 * 题目1: 环形链表
 *
 * 解法: Floyd 快慢指针（龟兔赛跑）
 * 思路:
 *   - 快指针每次走 2 步，慢指针每次走 1 步
 *   - 如果有环，快指针一定会追上慢指针
 *   - 如果无环，快指针会先到达 null
 *
 * 时间复杂度: O(n)
 * 空间复杂度: O(1)
 */
bool hasCycle(ListNode* head) {
    if (!head || !head->next) return false;

    ListNode* slow = head;
    ListNode* fast = head;

    while (fast && fast->next) {
        slow = slow->next;         // 慢指针走 1 步
        fast = fast->next->next;   // 快指针走 2 步

        if (slow == fast) {
            return true;  // 相遇，有环
        }
    }

    return false;  // 快指针到达 null，无环
}

// 解法2: 哈希表（空间 O(n)，不推荐但易理解）
bool hasCycleHash(ListNode* head) {
    std::unordered_set<ListNode*> visited;

    while (head) {
        if (visited.count(head)) {
            return true;
        }
        visited.insert(head);
        head = head->next;
    }

    return false;
}

/**
 * 题目2: 环形链表 II - 找入环点
 *
 * 解法: Floyd 算法扩展
 *
 * 数学推导:
 *   设：
 *     a = 头到入环点的距离
 *     b = 入环点到相遇点的距离
 *     c = 相遇点到入环点的距离（环长 = b + c）
 *
 *   相遇时：
 *     慢指针走了: a + b
 *     快指针走了: a + b + n(b + c)，其中 n 是快指针多走的圈数
 *
 *   因为快指针速度是慢指针的 2 倍:
 *     2(a + b) = a + b + n(b + c)
 *     a + b = n(b + c)
 *     a = n(b + c) - b = (n-1)(b + c) + c
 *
 *   这意味着: 从头走 a 步 = 从相遇点走 c + (n-1)圈
 *   所以让两个指针分别从头和相遇点同速前进，会在入环点相遇
 *
 * 时间复杂度: O(n)
 * 空间复杂度: O(1)
 */
ListNode* detectCycle(ListNode* head) {
    if (!head || !head->next) return nullptr;

    ListNode* slow = head;
    ListNode* fast = head;

    // 第一阶段：找到相遇点
    while (fast && fast->next) {
        slow = slow->next;
        fast = fast->next->next;

        if (slow == fast) {
            // 第二阶段：找入环点
            ListNode* ptr1 = head;
            ListNode* ptr2 = slow;

            while (ptr1 != ptr2) {
                ptr1 = ptr1->next;
                ptr2 = ptr2->next;
            }

            return ptr1;  // 入环点
        }
    }

    return nullptr;  // 无环
}

// 哈希表解法（简单但空间 O(n)）
ListNode* detectCycleHash(ListNode* head) {
    std::unordered_set<ListNode*> visited;

    while (head) {
        if (visited.count(head)) {
            return head;  // 第一次重复访问的节点就是入环点
        }
        visited.insert(head);
        head = head->next;
    }

    return nullptr;
}

/**
 * 题目3: 快乐数
 *
 * 解法: 将其转化为链表环检测问题
 * 思路:
 *   - 每个数通过 "各位平方和" 映射到下一个数
 *   - 如果最终变成 1，则是快乐数
 *   - 如果进入循环（但不包含 1），则不是快乐数
 *   - 可以证明不会无限增长
 *
 * 时间复杂度: O(log n)
 * 空间复杂度: O(1)
 */
int getNext(int n) {
    int sum = 0;
    while (n > 0) {
        int digit = n % 10;
        sum += digit * digit;
        n /= 10;
    }
    return sum;
}

bool isHappy(int n) {
    int slow = n;
    int fast = getNext(n);

    while (fast != 1 && slow != fast) {
        slow = getNext(slow);           // 走 1 步
        fast = getNext(getNext(fast));  // 走 2 步
    }

    return fast == 1;
}

// 哈希表解法
bool isHappyHash(int n) {
    std::unordered_set<int> seen;

    while (n != 1 && !seen.count(n)) {
        seen.insert(n);
        n = getNext(n);
    }

    return n == 1;
}

/**
 * 题目4: 相交链表
 *
 * 解法: 双指针同步走
 * 思路:
 *   - 指针 A 走完链表 A 后从链表 B 头开始走
 *   - 指针 B 走完链表 B 后从链表 A 头开始走
 *   - 两指针会同时到达相交点
 *
 * 证明:
 *   设 A 独有部分长 a，B 独有部分长 b，公共部分长 c
 *   A 走的路程: a + c + b
 *   B 走的路程: b + c + a
 *   两者相等，所以会同时到达相交点
 *   如果不相交（c=0），两指针会同时到达 null
 *
 * 时间复杂度: O(m + n)
 * 空间复杂度: O(1)
 */
ListNode* getIntersectionNode(ListNode* headA, ListNode* headB) {
    if (!headA || !headB) return nullptr;

    ListNode* ptrA = headA;
    ListNode* ptrB = headB;

    // 当两指针相遇或同时为 null 时退出
    while (ptrA != ptrB) {
        ptrA = ptrA ? ptrA->next : headB;
        ptrB = ptrB ? ptrB->next : headA;
    }

    return ptrA;  // 相交点或 null
}

// 解法2: 先计算长度差
ListNode* getIntersectionNodeV2(ListNode* headA, ListNode* headB) {
    if (!headA || !headB) return nullptr;

    // 计算链表长度
    auto getLength = [](ListNode* head) {
        int len = 0;
        while (head) {
            ++len;
            head = head->next;
        }
        return len;
    };

    int lenA = getLength(headA);
    int lenB = getLength(headB);

    // 让较长的链表先走
    while (lenA > lenB) {
        headA = headA->next;
        --lenA;
    }
    while (lenB > lenA) {
        headB = headB->next;
        --lenB;
    }

    // 同步前进找相交点
    while (headA != headB) {
        headA = headA->next;
        headB = headB->next;
    }

    return headA;
}

// 解法3: 哈希表
ListNode* getIntersectionNodeHash(ListNode* headA, ListNode* headB) {
    std::unordered_set<ListNode*> visited;

    while (headA) {
        visited.insert(headA);
        headA = headA->next;
    }

    while (headB) {
        if (visited.count(headB)) {
            return headB;
        }
        headB = headB->next;
    }

    return nullptr;
}


// ==================== 辅助函数 ====================

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

    if (pos >= 0 && cycleEntry) {
        curr->next = cycleEntry;
    }

    return dummy.next;
}

void deleteCyclicList(ListNode* head, int pos) {
    if (!head) return;

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
            ListNode* temp = ptr1;
            while (temp->next != ptr1) {
                temp = temp->next;
            }
            temp->next = nullptr;
        }
    }

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

    // 有环，入口在 pos=0
    head = createCyclicList({1, 2}, 0);
    entry = detectCycle(head);
    assert(entry != nullptr && entry->val == 1);
    deleteCyclicList(head, 0);

    // 无环
    head = createCyclicList({1, 2, 3}, -1);
    assert(detectCycle(head) == nullptr);
    deleteCyclicList(head, -1);

    std::cout << "detectCycle: PASSED\n";
}

void testIsHappy() {
    assert(isHappy(19) == true);   // 19 -> 82 -> 68 -> 100 -> 1
    assert(isHappy(2) == false);
    assert(isHappy(1) == true);
    assert(isHappy(7) == true);    // 7 -> 49 -> 97 -> 130 -> 10 -> 1
    assert(isHappy(4) == false);

    std::cout << "isHappy: PASSED\n";
}

void testGetIntersectionNode() {
    // 创建相交链表
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

    // 清理
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
    headA->next = new ListNode(2);
    headB = new ListNode(3);
    assert(getIntersectionNode(headA, headB) == nullptr);
    delete headA->next;
    delete headA;
    delete headB;

    std::cout << "getIntersectionNode: PASSED\n";
}

int main() {
    std::cout << "=== Cycle Detection Solution Tests ===\n";

    testHasCycle();
    testDetectCycle();
    testIsHappy();
    testGetIntersectionNode();

    std::cout << "\nAll tests passed!\n";
    return 0;
}
