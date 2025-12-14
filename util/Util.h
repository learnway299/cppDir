/**
 * @file Util.h
 * @brief 公共工具类 - 数据结构定义和工具函数声明
 *
 * 包含面试编程题常用的数据结构和工具函数。
 */

#ifndef INTERVIEW_UTIL_H
#define INTERVIEW_UTIL_H

#include <vector>
#include <string>
#include <iostream>
#include <queue>
#include <sstream>

// ==================== 数据结构定义 ====================

/**
 * 单链表节点
 */
struct ListNode {
    int val;
    ListNode* next;

    ListNode() : val(0), next(nullptr) {}
    ListNode(int x) : val(x), next(nullptr) {}
    ListNode(int x, ListNode* next) : val(x), next(next) {}
};

/**
 * 带随机指针的链表节点
 */
struct RandomListNode {
    int val;
    RandomListNode* next;
    RandomListNode* random;

    RandomListNode(int x) : val(x), next(nullptr), random(nullptr) {}
};

/**
 * 二叉树节点
 */
struct TreeNode {
    int val;
    TreeNode* left;
    TreeNode* right;

    TreeNode() : val(0), left(nullptr), right(nullptr) {}
    TreeNode(int x) : val(x), left(nullptr), right(nullptr) {}
    TreeNode(int x, TreeNode* left, TreeNode* right) : val(x), left(left), right(right) {}
};

/**
 * N 叉树节点
 */
struct NaryTreeNode {
    int val;
    std::vector<NaryTreeNode*> children;

    NaryTreeNode() : val(0) {}
    NaryTreeNode(int x) : val(x) {}
};

/**
 * 带父指针的二叉树节点
 */
struct ParentNode {
    int val;
    ParentNode* left;
    ParentNode* right;
    ParentNode* parent;

    ParentNode() : val(0), left(nullptr), right(nullptr), parent(nullptr) {}
    ParentNode(int x) : val(x), left(nullptr), right(nullptr), parent(nullptr) {}
};

// ==================== 工具函数命名空间 ====================

namespace Util {

// ---------- 打印函数 ----------

/**
 * 打印 vector<int>
 */
void printVector(const std::vector<int>& vec);

/**
 * 打印 vector<vector<int>>
 */
void printMatrix(const std::vector<std::vector<int>>& matrix);

/**
 * 打印 vector<string>
 */
void printStrings(const std::vector<std::string>& strs);

/**
 * 打印链表
 */
void printList(ListNode* head);

/**
 * 打印二叉树（层序遍历格式）
 */
void printTree(TreeNode* root);

// ---------- 链表工具 ----------

/**
 * 从 vector 创建链表
 */
ListNode* createList(const std::vector<int>& values);

/**
 * 删除链表，释放内存
 */
void deleteList(ListNode* head);

/**
 * 链表转 vector
 */
std::vector<int> listToVector(ListNode* head);

/**
 * 比较两个链表是否相等
 */
bool compareLists(ListNode* l1, ListNode* l2);

// ---------- 二叉树工具 ----------

/**
 * 从层序遍历数组创建二叉树（-1 表示 null）
 */
TreeNode* createTree(const std::vector<int>& values, int nullValue = -1);

/**
 * 删除二叉树，释放内存
 */
void deleteTree(TreeNode* root);

/**
 * 二叉树转层序遍历数组
 */
std::vector<int> treeToVector(TreeNode* root, int nullValue = -1);

/**
 * 在树中查找值为 val 的节点
 */
TreeNode* findNode(TreeNode* root, int val);

/**
 * 中序遍历（结果存入 vector）
 */
void inorder(TreeNode* root, std::vector<int>& result);

// ---------- 带父指针的二叉树工具 ----------

/**
 * 创建带父指针的二叉树
 */
ParentNode* createTreeWithParent(const std::vector<int>& values, int nullValue = -1);

/**
 * 删除带父指针的二叉树
 */
void deleteTreeWithParent(ParentNode* root);

/**
 * 在带父指针的树中查找节点
 */
ParentNode* findNodeWithParent(ParentNode* root, int val);

// ---------- 其他工具 ----------

/**
 * 交换两个整数
 */
void swap(int& a, int& b) noexcept;

} // namespace Util

#endif // INTERVIEW_UTIL_H
