/**
 * @file copy_random_list.cpp
 * @brief 复制带随机指针的链表 - 题目文件
 *
 * 这是一道经典的链表深拷贝问题，涉及复杂链表结构的处理。
 */

#include <iostream>
#include <unordered_map>

// 带随机指针的链表节点
class Node {
public:
    int val;
    Node* next;
    Node* random;

    Node(int _val) : val(_val), next(nullptr), random(nullptr) {}
};

/**
 * 题目: 复制带随机指针的链表
 *
 * 给你一个长度为 n 的链表，每个节点包含一个额外增加的随机指针 random，
 * 该指针可以指向链表中的任何节点或空节点。
 *
 * 构造这个链表的深拷贝。深拷贝应该正好由 n 个全新节点组成，
 * 其中每个新节点的值都设为其对应的原节点的值。
 * 新节点的 next 指针和 random 指针也都应指向复制链表中的新节点，
 * 并使原链表和复制链表中的这些指针能够表示相同的链表状态。
 * 复制链表中的指针都不应指向原链表中的节点。
 *
 * 示例:
 *   输入: head = [[7,null],[13,0],[11,4],[10,2],[1,0]]
 *   （节点值为 7, 13, 11, 10, 1，random 指向的节点下标分别为 null, 0, 4, 2, 0）
 *   输出: 深拷贝后的新链表
 *
 *   输入: head = [[1,1],[2,1]]
 *   （两个节点，random 都指向第二个节点）
 *   输出: 深拷贝后的新链表
 *
 *   输入: head = [[3,null],[3,0],[3,null]]
 *   输出: 深拷贝后的新链表
 *
 * 提示:
 *   - 可以使用哈希表建立原节点到新节点的映射
 *   - 也可以使用原地复制的方法（O(1) 空间）
 *
 * @param head 原链表头节点
 * @return 深拷贝后的链表头节点
 */
Node* copyRandomList(Node* head) {
    // TODO: 实现你的解法
    return nullptr;
}


// ==================== 辅助函数 ====================

// 打印链表（用于调试）
void printList(Node* head) {
    std::cout << "[";
    Node* curr = head;
    while (curr) {
        std::cout << "[" << curr->val << ",";
        if (curr->random) {
            std::cout << curr->random->val;
        } else {
            std::cout << "null";
        }
        std::cout << "]";
        if (curr->next) std::cout << ",";
        curr = curr->next;
    }
    std::cout << "]\n";
}

// 释放链表内存
void deleteList(Node* head) {
    while (head) {
        Node* temp = head;
        head = head->next;
        delete temp;
    }
}

// 验证深拷贝是否正确
bool validateCopy(Node* original, Node* copied) {
    if (!original && !copied) return true;
    if (!original || !copied) return false;

    // 建立原节点到下标的映射
    std::unordered_map<Node*, int> originalIndex;
    Node* curr = original;
    int idx = 0;
    while (curr) {
        originalIndex[curr] = idx++;
        curr = curr->next;
    }

    // 建立拷贝节点到下标的映射
    std::unordered_map<Node*, int> copiedIndex;
    curr = copied;
    idx = 0;
    while (curr) {
        copiedIndex[curr] = idx++;
        curr = curr->next;
    }

    // 验证
    Node* o = original;
    Node* c = copied;
    while (o && c) {
        // 值相同
        if (o->val != c->val) return false;

        // 不能是同一个节点（必须是深拷贝）
        if (o == c) return false;

        // random 指向正确
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
#include <vector>

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

    Node* copied1 = copyRandomList(nodes1[0]);
    assert(validateCopy(nodes1[0], copied1));

    deleteList(nodes1[0]);
    deleteList(copied1);

    // 测试用例 2: [[1,1],[2,1]]
    Node* n1 = new Node(1);
    Node* n2 = new Node(2);
    n1->next = n2;
    n1->random = n2;
    n2->random = n2;

    Node* copied2 = copyRandomList(n1);
    assert(validateCopy(n1, copied2));

    deleteList(n1);
    deleteList(copied2);

    // 测试用例 3: 空链表
    assert(copyRandomList(nullptr) == nullptr);

    // 测试用例 4: 单节点，random 指向自己
    Node* single = new Node(1);
    single->random = single;
    Node* copiedSingle = copyRandomList(single);
    assert(copiedSingle != nullptr);
    assert(copiedSingle->val == 1);
    assert(copiedSingle->random == copiedSingle);
    assert(copiedSingle != single);  // 确保是新节点
    delete single;
    delete copiedSingle;

    std::cout << "copyRandomList: PASSED\n";
}

int main() {
    std::cout << "=== Copy Random List Tests ===\n";

    // 取消注释以运行测试（实现解法后）
    // testCopyRandomList();

    std::cout << "All tests passed!\n";
    return 0;
}
