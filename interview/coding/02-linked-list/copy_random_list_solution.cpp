/**
 * @file copy_random_list_solution.cpp
 * @brief 复制带随机指针的链表 - 解答文件
 *
 * 包含 copy_random_list.cpp 中题目的解答和详细解析。
 */

#include <iostream>
#include <unordered_map>
#include <vector>

class Node {
public:
    int val;
    Node* next;
    Node* random;

    Node(int _val) : val(_val), next(nullptr), random(nullptr) {}
};

/**
 * 解法1: 哈希表
 *
 * 思路:
 *   1. 第一次遍历：创建所有新节点，建立原节点到新节点的映射
 *   2. 第二次遍历：设置新节点的 next 和 random 指针
 *
 * 时间复杂度: O(n)
 * 空间复杂度: O(n) - 哈希表存储
 */
Node* copyRandomListHash(Node* head) {
    if (!head) return nullptr;

    std::unordered_map<Node*, Node*> oldToNew;

    // 第一次遍历：创建新节点
    Node* curr = head;
    while (curr) {
        oldToNew[curr] = new Node(curr->val);
        curr = curr->next;
    }

    // 第二次遍历：设置指针
    curr = head;
    while (curr) {
        Node* newNode = oldToNew[curr];
        newNode->next = oldToNew[curr->next];       // 可能为 nullptr
        newNode->random = oldToNew[curr->random];   // 可能为 nullptr
        curr = curr->next;
    }

    return oldToNew[head];
}

/**
 * 解法2: 原地复制（推荐，空间 O(1)）
 *
 * 思路:
 *   1. 在每个原节点后面插入一个复制节点
 *      A -> A' -> B -> B' -> C -> C'
 *   2. 设置复制节点的 random 指针
 *      A'.random = A.random.next
 *   3. 分离原链表和复制链表
 *
 * 时间复杂度: O(n)
 * 空间复杂度: O(1)
 */
Node* copyRandomList(Node* head) {
    if (!head) return nullptr;

    // 步骤1：在每个节点后插入复制节点
    // A -> B -> C 变成 A -> A' -> B -> B' -> C -> C'
    Node* curr = head;
    while (curr) {
        Node* copy = new Node(curr->val);
        copy->next = curr->next;
        curr->next = copy;
        curr = copy->next;
    }

    // 步骤2：设置复制节点的 random 指针
    curr = head;
    while (curr) {
        Node* copy = curr->next;
        if (curr->random) {
            copy->random = curr->random->next;  // random 的复制节点
        }
        curr = copy->next;
    }

    // 步骤3：分离两个链表
    Node* newHead = head->next;
    curr = head;
    while (curr) {
        Node* copy = curr->next;
        curr->next = copy->next;
        if (copy->next) {
            copy->next = copy->next->next;
        }
        curr = curr->next;
    }

    return newHead;
}

/**
 * 解法3: 递归 + 哈希表
 *
 * 思路: 递归遍历节点，使用哈希表记录已复制的节点避免重复
 *
 * 时间复杂度: O(n)
 * 空间复杂度: O(n)
 */
std::unordered_map<Node*, Node*> visited;

Node* copyRandomListRecursive(Node* head) {
    if (!head) return nullptr;

    // 如果已经复制过，直接返回
    if (visited.count(head)) {
        return visited[head];
    }

    // 创建新节点
    Node* newNode = new Node(head->val);
    visited[head] = newNode;

    // 递归复制 next 和 random
    newNode->next = copyRandomListRecursive(head->next);
    newNode->random = copyRandomListRecursive(head->random);

    return newNode;
}

// 包装函数，清理静态变量
Node* copyRandomListRecursiveWrapper(Node* head) {
    visited.clear();
    return copyRandomListRecursive(head);
}


// ==================== 辅助函数 ====================

void deleteList(Node* head) {
    while (head) {
        Node* temp = head;
        head = head->next;
        delete temp;
    }
}

bool validateCopy(Node* original, Node* copied) {
    if (!original && !copied) return true;
    if (!original || !copied) return false;

    std::unordered_map<Node*, int> originalIndex;
    Node* curr = original;
    int idx = 0;
    while (curr) {
        originalIndex[curr] = idx++;
        curr = curr->next;
    }

    std::unordered_map<Node*, int> copiedIndex;
    curr = copied;
    idx = 0;
    while (curr) {
        copiedIndex[curr] = idx++;
        curr = curr->next;
    }

    Node* o = original;
    Node* c = copied;
    while (o && c) {
        if (o->val != c->val) return false;
        if (o == c) return false;

        if (o->random && c->random) {
            if (originalIndex[o->random] != copiedIndex[c->random]) {
                return false;
            }
        } else if (o->random || c->random) {
            return false;
        }

        o = o->next;
        c = c->next;
    }

    return o == nullptr && c == nullptr;
}


// ==================== 测试代码 ====================
#include <cassert>

void testCopyRandomList() {
    // 测试用例 1: [[7,null],[13,0],[11,4],[10,2],[1,0]]
    std::vector<Node*> nodes1;
    for (int val : {7, 13, 11, 10, 1}) {
        nodes1.push_back(new Node(val));
    }
    nodes1[0]->next = nodes1[1];
    nodes1[1]->next = nodes1[2];
    nodes1[2]->next = nodes1[3];
    nodes1[3]->next = nodes1[4];

    nodes1[0]->random = nullptr;
    nodes1[1]->random = nodes1[0];
    nodes1[2]->random = nodes1[4];
    nodes1[3]->random = nodes1[2];
    nodes1[4]->random = nodes1[0];

    // 测试原地复制解法
    Node* copied1 = copyRandomList(nodes1[0]);
    assert(validateCopy(nodes1[0], copied1));
    deleteList(copied1);

    // 重建原链表（因为原地复制会修改原链表结构，需要重建）
    nodes1.clear();
    for (int val : {7, 13, 11, 10, 1}) {
        nodes1.push_back(new Node(val));
    }
    nodes1[0]->next = nodes1[1];
    nodes1[1]->next = nodes1[2];
    nodes1[2]->next = nodes1[3];
    nodes1[3]->next = nodes1[4];
    nodes1[0]->random = nullptr;
    nodes1[1]->random = nodes1[0];
    nodes1[2]->random = nodes1[4];
    nodes1[3]->random = nodes1[2];
    nodes1[4]->random = nodes1[0];

    // 测试哈希表解法
    Node* copied2 = copyRandomListHash(nodes1[0]);
    assert(validateCopy(nodes1[0], copied2));
    deleteList(nodes1[0]);
    deleteList(copied2);

    // 测试用例 2: [[1,1],[2,1]]
    Node* n1 = new Node(1);
    Node* n2 = new Node(2);
    n1->next = n2;
    n1->random = n2;
    n2->random = n2;

    Node* copied3 = copyRandomList(n1);
    assert(validateCopy(n1, copied3));
    deleteList(n1);
    deleteList(copied3);

    // 测试用例 3: 空链表
    assert(copyRandomList(nullptr) == nullptr);
    assert(copyRandomListHash(nullptr) == nullptr);

    // 测试用例 4: 单节点，random 指向自己
    Node* single = new Node(1);
    single->random = single;
    Node* copiedSingle = copyRandomList(single);
    assert(copiedSingle != nullptr);
    assert(copiedSingle->val == 1);
    assert(copiedSingle->random == copiedSingle);
    assert(copiedSingle != single);
    delete single;
    delete copiedSingle;

    // 测试用例 5: 所有 random 都为 null
    Node* a = new Node(1);
    Node* b = new Node(2);
    Node* c = new Node(3);
    a->next = b;
    b->next = c;

    Node* copiedNull = copyRandomList(a);
    assert(validateCopy(a, copiedNull));
    deleteList(a);
    deleteList(copiedNull);

    std::cout << "copyRandomList: PASSED\n";
}

int main() {
    std::cout << "=== Copy Random List Solution Tests ===\n";

    testCopyRandomList();

    std::cout << "\nAll tests passed!\n";
    return 0;
}
