/**
 * @file tree_properties_solution.cpp
 * @brief 二叉树属性问题 - 解答文件
 *
 * 包含 tree_properties.cpp 中所有题目的解答和详细解析。
 */

#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
#include <cmath>

struct TreeNode {
    int val;
    TreeNode* left;
    TreeNode* right;
    TreeNode() : val(0), left(nullptr), right(nullptr) {}
    TreeNode(int x) : val(x), left(nullptr), right(nullptr) {}
    TreeNode(int x, TreeNode* l, TreeNode* r) : val(x), left(l), right(r) {}
};

/**
 * 题目1: 二叉树的最大深度
 *
 * 解法1: DFS 递归
 * 时间复杂度: O(n)
 * 空间复杂度: O(h)，h 为树高
 */
int maxDepth(TreeNode* root) {
    if (!root) return 0;
    return 1 + std::max(maxDepth(root->left), maxDepth(root->right));
}

// 解法2: BFS 迭代
int maxDepthBFS(TreeNode* root) {
    if (!root) return 0;

    std::queue<TreeNode*> q;
    q.push(root);
    int depth = 0;

    while (!q.empty()) {
        int size = q.size();
        ++depth;
        for (int i = 0; i < size; ++i) {
            TreeNode* node = q.front();
            q.pop();
            if (node->left) q.push(node->left);
            if (node->right) q.push(node->right);
        }
    }

    return depth;
}

/**
 * 题目2: 二叉树的最小深度
 *
 * 解法1: DFS 递归
 * 注意: 需要特判只有一边子树的情况
 */
int minDepth(TreeNode* root) {
    if (!root) return 0;

    // 只有右子树
    if (!root->left) return 1 + minDepth(root->right);
    // 只有左子树
    if (!root->right) return 1 + minDepth(root->left);

    // 两边都有
    return 1 + std::min(minDepth(root->left), minDepth(root->right));
}

// 解法2: BFS（更高效，找到第一个叶子即可返回）
int minDepthBFS(TreeNode* root) {
    if (!root) return 0;

    std::queue<TreeNode*> q;
    q.push(root);
    int depth = 0;

    while (!q.empty()) {
        int size = q.size();
        ++depth;
        for (int i = 0; i < size; ++i) {
            TreeNode* node = q.front();
            q.pop();

            // 找到叶子节点，直接返回
            if (!node->left && !node->right) {
                return depth;
            }

            if (node->left) q.push(node->left);
            if (node->right) q.push(node->right);
        }
    }

    return depth;
}

/**
 * 题目3: 平衡二叉树
 *
 * 解法1: 自顶向下（简单但低效）
 * 时间复杂度: O(n^2)
 */
int height(TreeNode* root) {
    if (!root) return 0;
    return 1 + std::max(height(root->left), height(root->right));
}

bool isBalancedTopDown(TreeNode* root) {
    if (!root) return true;

    int leftH = height(root->left);
    int rightH = height(root->right);

    return std::abs(leftH - rightH) <= 1 &&
           isBalancedTopDown(root->left) &&
           isBalancedTopDown(root->right);
}

/**
 * 解法2: 自底向上（推荐，O(n)）
 * 思路: 在计算高度的同时检查平衡性，返回 -1 表示不平衡
 */
int checkHeight(TreeNode* root) {
    if (!root) return 0;

    int leftH = checkHeight(root->left);
    if (leftH == -1) return -1;

    int rightH = checkHeight(root->right);
    if (rightH == -1) return -1;

    if (std::abs(leftH - rightH) > 1) return -1;

    return 1 + std::max(leftH, rightH);
}

bool isBalanced(TreeNode* root) {
    return checkHeight(root) != -1;
}

/**
 * 题目4: 对称二叉树
 *
 * 解法1: 递归
 * 思路: 比较左子树和右子树是否镜像
 */
bool isMirror(TreeNode* left, TreeNode* right) {
    if (!left && !right) return true;
    if (!left || !right) return false;

    return left->val == right->val &&
           isMirror(left->left, right->right) &&
           isMirror(left->right, right->left);
}

bool isSymmetric(TreeNode* root) {
    if (!root) return true;
    return isMirror(root->left, root->right);
}

// 解法2: 迭代（使用队列）
bool isSymmetricIterative(TreeNode* root) {
    if (!root) return true;

    std::queue<TreeNode*> q;
    q.push(root->left);
    q.push(root->right);

    while (!q.empty()) {
        TreeNode* left = q.front(); q.pop();
        TreeNode* right = q.front(); q.pop();

        if (!left && !right) continue;
        if (!left || !right) return false;
        if (left->val != right->val) return false;

        q.push(left->left);
        q.push(right->right);
        q.push(left->right);
        q.push(right->left);
    }

    return true;
}

/**
 * 题目5: 相同的树
 *
 * 解法: 递归比较
 */
bool isSameTree(TreeNode* p, TreeNode* q) {
    if (!p && !q) return true;
    if (!p || !q) return false;

    return p->val == q->val &&
           isSameTree(p->left, q->left) &&
           isSameTree(p->right, q->right);
}

/**
 * 题目6: 另一棵树的子树
 *
 * 解法: 遍历主树，对每个节点检查是否与子树相同
 * 时间复杂度: O(m * n)
 */
bool isSubtree(TreeNode* root, TreeNode* subRoot) {
    if (!root) return false;
    if (isSameTree(root, subRoot)) return true;
    return isSubtree(root->left, subRoot) || isSubtree(root->right, subRoot);
}

/**
 * 题目7: 翻转二叉树
 *
 * 解法1: 递归
 */
TreeNode* invertTree(TreeNode* root) {
    if (!root) return nullptr;

    // 交换左右子树
    std::swap(root->left, root->right);

    // 递归翻转子树
    invertTree(root->left);
    invertTree(root->right);

    return root;
}

// 解法2: 迭代（BFS）
TreeNode* invertTreeBFS(TreeNode* root) {
    if (!root) return nullptr;

    std::queue<TreeNode*> q;
    q.push(root);

    while (!q.empty()) {
        TreeNode* node = q.front();
        q.pop();

        std::swap(node->left, node->right);

        if (node->left) q.push(node->left);
        if (node->right) q.push(node->right);
    }

    return root;
}

/**
 * 题目8: 合并二叉树
 *
 * 解法1: 递归
 * 思路: 同时遍历两棵树，合并节点值
 */
TreeNode* mergeTrees(TreeNode* root1, TreeNode* root2) {
    if (!root1) return root2;
    if (!root2) return root1;

    // 合并当前节点
    root1->val += root2->val;

    // 递归合并子树
    root1->left = mergeTrees(root1->left, root2->left);
    root1->right = mergeTrees(root1->right, root2->right);

    return root1;
}

// 解法2: 迭代（BFS）
TreeNode* mergeTreesBFS(TreeNode* root1, TreeNode* root2) {
    if (!root1) return root2;
    if (!root2) return root1;

    std::queue<TreeNode*> q1, q2;
    q1.push(root1);
    q2.push(root2);

    while (!q1.empty()) {
        TreeNode* n1 = q1.front(); q1.pop();
        TreeNode* n2 = q2.front(); q2.pop();

        n1->val += n2->val;

        // 处理左子树
        if (n1->left && n2->left) {
            q1.push(n1->left);
            q2.push(n2->left);
        } else if (!n1->left) {
            n1->left = n2->left;
        }

        // 处理右子树
        if (n1->right && n2->right) {
            q1.push(n1->right);
            q2.push(n2->right);
        } else if (!n1->right) {
            n1->right = n2->right;
        }
    }

    return root1;
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


// ==================== 测试代码 ====================
#include <cassert>

void testMaxDepth() {
    TreeNode* root = createTree({3, 9, 20, -1, -1, 15, 7});
    assert(maxDepth(root) == 3);
    assert(maxDepthBFS(root) == 3);
    deleteTree(root);

    assert(maxDepth(nullptr) == 0);

    root = new TreeNode(1);
    assert(maxDepth(root) == 1);
    delete root;

    std::cout << "maxDepth: PASSED\n";
}

void testMinDepth() {
    TreeNode* root = createTree({3, 9, 20, -1, -1, 15, 7});
    assert(minDepth(root) == 2);
    assert(minDepthBFS(root) == 2);
    deleteTree(root);

    // 单边树
    root = new TreeNode(2);
    root->right = new TreeNode(3);
    root->right->right = new TreeNode(4);
    assert(minDepth(root) == 3);
    assert(minDepthBFS(root) == 3);
    deleteTree(root);

    std::cout << "minDepth: PASSED\n";
}

void testIsBalanced() {
    TreeNode* root = createTree({3, 9, 20, -1, -1, 15, 7});
    assert(isBalanced(root) == true);
    deleteTree(root);

    root = createTree({1, 2, 2, 3, 3, -1, -1, 4, 4});
    assert(isBalanced(root) == false);
    deleteTree(root);

    assert(isBalanced(nullptr) == true);

    std::cout << "isBalanced: PASSED\n";
}

void testIsSymmetric() {
    TreeNode* root = createTree({1, 2, 2, 3, 4, 4, 3});
    assert(isSymmetric(root) == true);
    assert(isSymmetricIterative(root) == true);
    deleteTree(root);

    root = createTree({1, 2, 2, -1, 3, -1, 3});
    assert(isSymmetric(root) == false);
    deleteTree(root);

    std::cout << "isSymmetric: PASSED\n";
}

void testIsSameTree() {
    TreeNode* p = createTree({1, 2, 3});
    TreeNode* q = createTree({1, 2, 3});
    assert(isSameTree(p, q) == true);
    deleteTree(p);
    deleteTree(q);

    p = createTree({1, 2});
    q = createTree({1, -1, 2});
    assert(isSameTree(p, q) == false);
    deleteTree(p);
    deleteTree(q);

    assert(isSameTree(nullptr, nullptr) == true);

    std::cout << "isSameTree: PASSED\n";
}

void testIsSubtree() {
    TreeNode* root = createTree({3, 4, 5, 1, 2});
    TreeNode* subRoot = createTree({4, 1, 2});
    assert(isSubtree(root, subRoot) == true);
    deleteTree(root);
    deleteTree(subRoot);

    root = createTree({3, 4, 5, 1, 2, -1, -1, -1, -1, 0});
    subRoot = createTree({4, 1, 2});
    assert(isSubtree(root, subRoot) == false);
    deleteTree(root);
    deleteTree(subRoot);

    std::cout << "isSubtree: PASSED\n";
}

void testInvertTree() {
    TreeNode* root = createTree({4, 2, 7, 1, 3, 6, 9});
    TreeNode* inverted = invertTree(root);
    assert(inverted->left->val == 7);
    assert(inverted->right->val == 2);
    assert(inverted->left->left->val == 9);
    assert(inverted->right->right->val == 1);
    deleteTree(inverted);

    assert(invertTree(nullptr) == nullptr);

    std::cout << "invertTree: PASSED\n";
}

void testMergeTrees() {
    TreeNode* t1 = createTree({1, 3, 2, 5});
    TreeNode* t2 = createTree({2, 1, 3, -1, 4, -1, 7});
    TreeNode* merged = mergeTrees(t1, t2);
    assert(merged->val == 3);
    assert(merged->left->val == 4);
    assert(merged->right->val == 5);
    deleteTree(merged);

    // 一棵为空
    t1 = createTree({1});
    assert(mergeTrees(t1, nullptr)->val == 1);
    deleteTree(t1);

    std::cout << "mergeTrees: PASSED\n";
}

int main() {
    std::cout << "=== Tree Properties Solution Tests ===\n";

    testMaxDepth();
    testMinDepth();
    testIsBalanced();
    testIsSymmetric();
    testIsSameTree();
    testIsSubtree();
    testInvertTree();
    testMergeTrees();

    std::cout << "\nAll tests passed!\n";
    return 0;
}
