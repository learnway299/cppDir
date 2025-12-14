/**
 * @file copy_random_list_solution.cpp
 * @brief 复制带随机指针的链表 - 参考答案
 */

#include "copy_random_list.h"
#include <iostream>
#include <cassert>
#include <vector>

namespace CopyRandomList {

// ==================== 参考答案实现 ====================

// 解法1: 哈希表 O(n) 时间，O(n) 空间
RandomListNode* copyRandomListHashSolution(RandomListNode* head) {
    if (!head) return nullptr;
    std::unordered_map<RandomListNode*, RandomListNode*> oldToNew;
    RandomListNode* curr = head;
    while (curr) {
        oldToNew[curr] = new RandomListNode(curr->val);
        curr = curr->next;
    }
    curr = head;
    while (curr) {
        RandomListNode* newNode = oldToNew[curr];
        newNode->next = oldToNew[curr->next];
        newNode->random = oldToNew[curr->random];
        curr = curr->next;
    }
    return oldToNew[head];
}

// 解法2: 原地复制 O(n) 时间，O(1) 空间
RandomListNode* copyRandomListSolution(RandomListNode* head) {
    if (!head) return nullptr;
    // 步骤1：在每个节点后插入复制节点
    RandomListNode* curr = head;
    while (curr) {
        RandomListNode* copy = new RandomListNode(curr->val);
        copy->next = curr->next;
        curr->next = copy;
        curr = copy->next;
    }
    // 步骤2：设置复制节点的 random 指针
    curr = head;
    while (curr) {
        RandomListNode* copy = curr->next;
        if (curr->random) {
            copy->random = curr->random->next;
        }
        curr = copy->next;
    }
    // 步骤3：分离两个链表
    RandomListNode* newHead = head->next;
    curr = head;
    while (curr) {
        RandomListNode* copy = curr->next;
        curr->next = copy->next;
        if (copy->next) {
            copy->next = copy->next->next;
        }
        curr = curr->next;
    }
    return newHead;
}

// ==================== 辅助函数 ====================

static void deleteRandomList(RandomListNode* head) {
    while (head) {
        RandomListNode* temp = head;
        head = head->next;
        delete temp;
    }
}

static bool validateCopy(RandomListNode* original, RandomListNode* copied) {
    if (!original && !copied) return true;
    if (!original || !copied) return false;

    std::unordered_map<RandomListNode*, int> originalIndex;
    RandomListNode* curr = original;
    int idx = 0;
    while (curr) {
        originalIndex[curr] = idx++;
        curr = curr->next;
    }

    std::unordered_map<RandomListNode*, int> copiedIndex;
    curr = copied;
    idx = 0;
    while (curr) {
        copiedIndex[curr] = idx++;
        curr = curr->next;
    }

    RandomListNode* o = original;
    RandomListNode* c = copied;
    while (o && c) {
        if (o->val != c->val) return false;
        if (o == c) return false;  // 必须是深拷贝
        if (o->random && c->random) {
            if (originalIndex[o->random] != copiedIndex[c->random]) return false;
        } else if (o->random || c->random) {
            return false;
        }
        o = o->next;
        c = c->next;
    }
    return o == nullptr && c == nullptr;
}

// ==================== 测试函数 ====================

void testCopyRandomListSolution() {
    std::cout << "=== Copy Random List Tests (Solution) ===\n";

    // 创建测试链表: [[7,null],[13,0],[11,4],[10,2],[1,0]]
    std::vector<RandomListNode*> nodes;
    for (int val : {7, 13, 11, 10, 1}) {
        nodes.push_back(new RandomListNode(val));
    }
    nodes[0]->next = nodes[1];
    nodes[1]->next = nodes[2];
    nodes[2]->next = nodes[3];
    nodes[3]->next = nodes[4];
    nodes[0]->random = nullptr;
    nodes[1]->random = nodes[0];
    nodes[2]->random = nodes[4];
    nodes[3]->random = nodes[2];
    nodes[4]->random = nodes[0];

    // 测试哈希表解法
    RandomListNode* copied = copyRandomListHashSolution(nodes[0]);
    assert(validateCopy(nodes[0], copied));
    deleteRandomList(copied);
    std::cout << "  copyRandomListHashSolution: PASSED\n";

    // 测试原地复制解法
    copied = copyRandomListSolution(nodes[0]);
    assert(validateCopy(nodes[0], copied));
    deleteRandomList(nodes[0]);
    deleteRandomList(copied);
    std::cout << "  copyRandomListSolution: PASSED\n";

    // 测试空链表
    assert(copyRandomListSolution(nullptr) == nullptr);
    std::cout << "  copyRandomListSolution (empty): PASSED\n";
}

} // namespace CopyRandomList
