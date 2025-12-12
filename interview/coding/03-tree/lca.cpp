/**
 * @file lca.cpp
 * @brief 最近公共祖先问题 - 题目文件
 *
 * LCA (Lowest Common Ancestor) 是树问题中的经典问题，
 * 在普通二叉树和二叉搜索树中有不同的解法。
 */

#include <iostream>
#include <vector>
#include <queue>
#include <unordered_set>
#include <unordered_map>

struct TreeNode {
    int val;
    TreeNode* left;
    TreeNode* right;
    TreeNode() : val(0), left(nullptr), right(nullptr) {}
    TreeNode(int x) : val(x), left(nullptr), right(nullptr) {}
    TreeNode(int x, TreeNode* l, TreeNode* r) : val(x), left(l), right(r) {}
};

// 带父指针的节点（用于题目3）
struct Node {
    int val;
    Node* left;
    Node* right;
    Node* parent;
    Node() : val(0), left(nullptr), right(nullptr), parent(nullptr) {}
    Node(int x) : val(x), left(nullptr), right(nullptr), parent(nullptr) {}
};

/**
 * 题目1: 二叉树的最近公共祖先
 *
 * 给定一个二叉树，找到该树中两个指定节点的最近公共祖先。
 *
 * 最近公共祖先的定义为：对于有根树 T 的两个节点 p、q，
 * 最近公共祖先表示为一个节点 x，满足 x 是 p、q 的祖先且 x 的深度尽可能大
 * （一个节点也可以是它自己的祖先）。
 *
 * 示例:
 *           3
 *          / \
 *         5   1
 *        / \ / \
 *       6  2 0  8
 *         / \
 *        7   4
 *
 *   输入: root = [3,5,1,6,2,0,8,null,null,7,4], p = 5, q = 1
 *   输出: 3
 *   解释: 节点 5 和节点 1 的最近公共祖先是节点 3。
 *
 *   输入: root = [3,5,1,6,2,0,8,null,null,7,4], p = 5, q = 4
 *   输出: 5
 *   解释: 节点 5 和节点 4 的最近公共祖先是节点 5。因为根据定义最近公共祖先节点可以为节点本身。
 *
 * 说明:
 *   - 所有节点的值都是唯一的
 *   - p、q 为不同节点且均存在于给定的二叉树中
 *
 * @param root 根节点
 * @param p 节点 p
 * @param q 节点 q
 * @return 最近公共祖先
 */
TreeNode* lowestCommonAncestor(TreeNode* root, TreeNode* p, TreeNode* q) {
    // TODO: 实现你的解法
    return nullptr;
}

/**
 * 题目2: 二叉搜索树的最近公共祖先
 *
 * 给定一个二叉搜索树，找到该树中两个指定节点的最近公共祖先。
 *
 * BST 的性质可以简化查找：
 *   - 如果 p, q 都小于 root，则 LCA 在左子树
 *   - 如果 p, q 都大于 root，则 LCA 在右子树
 *   - 否则 root 就是 LCA（分叉点）
 *
 * 示例:
 *           6
 *          / \
 *         2   8
 *        / \ / \
 *       0  4 7  9
 *         / \
 *        3   5
 *
 *   输入: root = [6,2,8,0,4,7,9,null,null,3,5], p = 2, q = 8
 *   输出: 6
 *
 *   输入: root = [6,2,8,0,4,7,9,null,null,3,5], p = 2, q = 4
 *   输出: 2
 *
 * @param root 根节点
 * @param p 节点 p
 * @param q 节点 q
 * @return 最近公共祖先
 */
TreeNode* lowestCommonAncestorBST(TreeNode* root, TreeNode* p, TreeNode* q) {
    // TODO: 实现你的解法
    return nullptr;
}

/**
 * 题目3: 有父指针的二叉树的最近公共祖先
 *
 * 给定一棵二叉树中的两个节点 p 和 q，返回它们的最近公共祖先节点。
 * 每个节点都包含其父节点的引用。
 *
 * 思路：类似于两个链表找交点问题
 *
 * 示例:
 *           3
 *          / \
 *         5   1
 *        / \ / \
 *       6  2 0  8
 *         / \
 *        7   4
 *
 *   输入: p = 5, q = 1
 *   输出: 3
 *
 * @param p 节点 p
 * @param q 节点 q
 * @return 最近公共祖先
 */
Node* lowestCommonAncestorWithParent(Node* p, Node* q) {
    // TODO: 实现你的解法
    return nullptr;
}

/**
 * 题目4: 二叉树的最近公共祖先 II
 *
 * 给定一棵二叉树的根节点 root，返回给定节点 p 和 q 的最近公共祖先（LCA）节点。
 * 如果 p 或 q 之一不存在于该二叉树中，返回 null。
 *
 * 与题目1的区别：p 或 q 可能不在树中
 *
 * 示例:
 *           3
 *          / \
 *         5   1
 *        / \ / \
 *       6  2 0  8
 *         / \
 *        7   4
 *
 *   输入: root = [3,5,1,6,2,0,8,null,null,7,4], p = 5, q = 10
 *   输出: null
 *   解释: 节点 10 不在树中，所以返回 null。
 *
 * @param root 根节点
 * @param p 节点 p
 * @param q 节点 q
 * @return 最近公共祖先（如果 p、q 都存在）
 */
TreeNode* lowestCommonAncestorII(TreeNode* root, TreeNode* p, TreeNode* q) {
    // TODO: 实现你的解法
    return nullptr;
}

/**
 * 题目5: 二叉树的最近公共祖先 III（多个节点）
 *
 * 给定一棵二叉树的根节点 root 和一个节点数组 nodes，
 * 返回 nodes 中所有节点的最近公共祖先。
 *
 * 示例:
 *           3
 *          / \
 *         5   1
 *        / \ / \
 *       6  2 0  8
 *         / \
 *        7   4
 *
 *   输入: root = [3,5,1,6,2,0,8,null,null,7,4], nodes = [4,7]
 *   输出: 2
 *
 *   输入: root = [3,5,1,6,2,0,8,null,null,7,4], nodes = [1]
 *   输出: 1
 *   解释: 单个节点的最近公共祖先是节点本身。
 *
 * @param root 根节点
 * @param nodes 节点数组
 * @return 最近公共祖先
 */
TreeNode* lowestCommonAncestorMultiple(TreeNode* root, std::vector<TreeNode*>& nodes) {
    // TODO: 实现你的解法
    return nullptr;
}

/**
 * 题目6: 二叉树的最深叶节点的最近公共祖先
 *
 * 给你一个有根节点 root 的二叉树，返回它最深的叶节点的最近公共祖先。
 *
 * 提示：
 *   - 如果某一个深度的节点只有一个，那么它就是自己的 LCA
 *   - 如果最深处有多个节点，返回它们的 LCA
 *
 * 示例:
 *           3
 *          / \
 *         5   1
 *        / \
 *       6   2
 *          / \
 *         7   4
 *
 *   输入: root = [3,5,1,6,2,null,null,null,null,7,4]
 *   输出: 2
 *   解释: 最深的叶节点是 7 和 4，它们的 LCA 是 2。
 *
 *           1
 *          / \
 *         2   3
 *        /   /
 *       4   5
 *
 *   输入: root = [1,2,3,4,null,5]
 *   输出: 1
 *   解释: 最深的叶节点是 4 和 5，它们的 LCA 是 1。
 *
 * @param root 根节点
 * @return 最深叶节点的最近公共祖先
 */
TreeNode* lcaDeepestLeaves(TreeNode* root) {
    // TODO: 实现你的解法
    return nullptr;
}


// ==================== 辅助函数 ====================

TreeNode* createTree(const std::vector<int>& vals) {
    if (vals.empty() || vals[0] == -1) return nullptr;

    TreeNode* root = new TreeNode(vals[0]);
    std::queue<TreeNode*> q;
    q.push(root);

    size_t i = 1;
    while (!q.empty() && i < vals.size()) {
        TreeNode* node = q.front();
        q.pop();

        if (i < vals.size() && vals[i] != -1) {
            node->left = new TreeNode(vals[i]);
            q.push(node->left);
        }
        ++i;

        if (i < vals.size() && vals[i] != -1) {
            node->right = new TreeNode(vals[i]);
            q.push(node->right);
        }
        ++i;
    }

    return root;
}

void deleteTree(TreeNode* root) {
    if (!root) return;
    deleteTree(root->left);
    deleteTree(root->right);
    delete root;
}

// 在树中查找值为 val 的节点
TreeNode* findNode(TreeNode* root, int val) {
    if (!root) return nullptr;
    if (root->val == val) return root;

    TreeNode* left = findNode(root->left, val);
    if (left) return left;

    return findNode(root->right, val);
}


// ==================== 测试代码 ====================
#include <cassert>

void testLowestCommonAncestor() {
    //        3
    //       / \
    //      5   1
    //     / \ / \
    //    6  2 0  8
    //      / \
    //     7   4
    TreeNode* root = createTree({3, 5, 1, 6, 2, 0, 8, -1, -1, 7, 4});
    TreeNode* p = findNode(root, 5);
    TreeNode* q = findNode(root, 1);
    TreeNode* lca = lowestCommonAncestor(root, p, q);
    assert(lca != nullptr && lca->val == 3);

    p = findNode(root, 5);
    q = findNode(root, 4);
    lca = lowestCommonAncestor(root, p, q);
    assert(lca != nullptr && lca->val == 5);

    deleteTree(root);

    std::cout << "lowestCommonAncestor: PASSED\n";
}

void testLowestCommonAncestorBST() {
    //        6
    //       / \
    //      2   8
    //     / \ / \
    //    0  4 7  9
    //      / \
    //     3   5
    TreeNode* root = createTree({6, 2, 8, 0, 4, 7, 9, -1, -1, 3, 5});
    TreeNode* p = findNode(root, 2);
    TreeNode* q = findNode(root, 8);
    TreeNode* lca = lowestCommonAncestorBST(root, p, q);
    assert(lca != nullptr && lca->val == 6);

    p = findNode(root, 2);
    q = findNode(root, 4);
    lca = lowestCommonAncestorBST(root, p, q);
    assert(lca != nullptr && lca->val == 2);

    deleteTree(root);

    std::cout << "lowestCommonAncestorBST: PASSED\n";
}

void testLcaDeepestLeaves() {
    //        3
    //       / \
    //      5   1
    //     / \
    //    6   2
    //       / \
    //      7   4
    TreeNode* root = createTree({3, 5, 1, 6, 2, -1, -1, -1, -1, 7, 4});
    TreeNode* result = lcaDeepestLeaves(root);
    assert(result != nullptr && result->val == 2);
    deleteTree(root);

    //      1
    //     / \
    //    2   3
    //   /   /
    //  4   5
    root = createTree({1, 2, 3, 4, -1, 5});
    result = lcaDeepestLeaves(root);
    assert(result != nullptr && result->val == 1);
    deleteTree(root);

    std::cout << "lcaDeepestLeaves: PASSED\n";
}

int main() {
    std::cout << "=== LCA Problems Tests ===\n";

    // 取消注释以运行测试（实现解法后）
    // testLowestCommonAncestor();
    // testLowestCommonAncestorBST();
    // testLcaDeepestLeaves();

    std::cout << "All tests passed!\n";
    return 0;
}
