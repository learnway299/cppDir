/**
 * @file Util.cpp
 * @brief 公共工具类 - 工具函数实现
 */

#include "Util.h"
#include <queue>
#include <climits>

namespace Util {

// ==================== 打印函数实现 ====================

void printVector(const std::vector<int>& vec) {
    std::cout << "[";
    for (size_t i = 0; i < vec.size(); ++i) {
        std::cout << vec[i];
        if (i < vec.size() - 1) std::cout << ", ";
    }
    std::cout << "]" << std::endl;
}

void printMatrix(const std::vector<std::vector<int>>& matrix) {
    std::cout << "[" << std::endl;
    for (const auto& row : matrix) {
        std::cout << "  [";
        for (size_t i = 0; i < row.size(); ++i) {
            std::cout << row[i];
            if (i < row.size() - 1) std::cout << ", ";
        }
        std::cout << "]" << std::endl;
    }
    std::cout << "]" << std::endl;
}

void printStrings(const std::vector<std::string>& strs) {
    std::cout << "[";
    for (size_t i = 0; i < strs.size(); ++i) {
        std::cout << "\"" << strs[i] << "\"";
        if (i < strs.size() - 1) std::cout << ", ";
    }
    std::cout << "]" << std::endl;
}

void printList(ListNode* head) {
    std::cout << "[";
    while (head) {
        std::cout << head->val;
        if (head->next) std::cout << " -> ";
        head = head->next;
    }
    std::cout << "]" << std::endl;
}

void printTree(TreeNode* root) {
    if (!root) {
        std::cout << "[]" << std::endl;
        return;
    }

    std::vector<std::string> result;
    std::queue<TreeNode*> q;
    q.push(root);

    while (!q.empty()) {
        TreeNode* node = q.front();
        q.pop();

        if (node) {
            result.push_back(std::to_string(node->val));
            q.push(node->left);
            q.push(node->right);
        } else {
            result.push_back("null");
        }
    }

    // 移除尾部的 null
    while (!result.empty() && result.back() == "null") {
        result.pop_back();
    }

    std::cout << "[";
    for (size_t i = 0; i < result.size(); ++i) {
        std::cout << result[i];
        if (i < result.size() - 1) std::cout << ", ";
    }
    std::cout << "]" << std::endl;
}

// ==================== 链表工具实现 ====================

ListNode* createList(const std::vector<int>& values) {
    if (values.empty()) return nullptr;

    ListNode dummy(0);
    ListNode* current = &dummy;

    for (int val : values) {
        current->next = new ListNode(val);
        current = current->next;
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

std::vector<int> listToVector(ListNode* head) {
    std::vector<int> result;
    while (head) {
        result.push_back(head->val);
        head = head->next;
    }
    return result;
}

bool compareLists(ListNode* l1, ListNode* l2) {
    while (l1 && l2) {
        if (l1->val != l2->val) return false;
        l1 = l1->next;
        l2 = l2->next;
    }
    return l1 == nullptr && l2 == nullptr;
}

// ==================== 二叉树工具实现 ====================

TreeNode* createTree(const std::vector<int>& values, int nullValue) {
    if (values.empty() || values[0] == nullValue) return nullptr;

    TreeNode* root = new TreeNode(values[0]);
    std::queue<TreeNode*> q;
    q.push(root);

    size_t i = 1;
    while (!q.empty() && i < values.size()) {
        TreeNode* node = q.front();
        q.pop();

        // 左子节点
        if (i < values.size()) {
            if (values[i] != nullValue) {
                node->left = new TreeNode(values[i]);
                q.push(node->left);
            }
            ++i;
        }

        // 右子节点
        if (i < values.size()) {
            if (values[i] != nullValue) {
                node->right = new TreeNode(values[i]);
                q.push(node->right);
            }
            ++i;
        }
    }

    return root;
}

void deleteTree(TreeNode* root) {
    if (!root) return;
    deleteTree(root->left);
    deleteTree(root->right);
    delete root;
}

std::vector<int> treeToVector(TreeNode* root, int nullValue) {
    std::vector<int> result;
    if (!root) return result;

    std::queue<TreeNode*> q;
    q.push(root);

    while (!q.empty()) {
        TreeNode* node = q.front();
        q.pop();

        if (node) {
            result.push_back(node->val);
            q.push(node->left);
            q.push(node->right);
        } else {
            result.push_back(nullValue);
        }
    }

    // 移除尾部的 null 值
    while (!result.empty() && result.back() == nullValue) {
        result.pop_back();
    }

    return result;
}

TreeNode* findNode(TreeNode* root, int val) {
    if (!root) return nullptr;
    if (root->val == val) return root;
    TreeNode* left = findNode(root->left, val);
    if (left) return left;
    return findNode(root->right, val);
}

void inorder(TreeNode* root, std::vector<int>& result) {
    if (!root) return;
    inorder(root->left, result);
    result.push_back(root->val);
    inorder(root->right, result);
}

// ==================== 带父指针的二叉树工具实现 ====================

ParentNode* createTreeWithParent(const std::vector<int>& values, int nullValue) {
    if (values.empty() || values[0] == nullValue) return nullptr;

    ParentNode* root = new ParentNode(values[0]);
    std::queue<ParentNode*> q;
    q.push(root);

    size_t i = 1;
    while (!q.empty() && i < values.size()) {
        ParentNode* node = q.front();
        q.pop();

        if (i < values.size() && values[i] != nullValue) {
            node->left = new ParentNode(values[i]);
            node->left->parent = node;
            q.push(node->left);
        }
        ++i;

        if (i < values.size() && values[i] != nullValue) {
            node->right = new ParentNode(values[i]);
            node->right->parent = node;
            q.push(node->right);
        }
        ++i;
    }

    return root;
}

void deleteTreeWithParent(ParentNode* root) {
    if (!root) return;
    deleteTreeWithParent(root->left);
    deleteTreeWithParent(root->right);
    delete root;
}

ParentNode* findNodeWithParent(ParentNode* root, int val) {
    if (!root) return nullptr;
    if (root->val == val) return root;
    ParentNode* left = findNodeWithParent(root->left, val);
    if (left) return left;
    return findNodeWithParent(root->right, val);
}

// ==================== 其他工具实现 ====================

void swap(int& a, int& b) noexcept {
    if (&a != &b) {
        a = a ^ b;
        b = a ^ b;
        a = a ^ b;
    }
}

} // namespace Util
