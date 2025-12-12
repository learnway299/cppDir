/**
 * @file tree_properties.cpp
 * @brief 二叉树属性问题 - 题目文件
 *
 * 树的属性问题包括高度、深度、节点数、对称性等基本性质的求解。
 */

#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>

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
 * 给定一个二叉树，找出其最大深度。
 * 二叉树的深度为根节点到最远叶子节点的最长路径上的节点数。
 *
 * 示例:
 *       3
 *      / \
 *     9  20
 *       /  \
 *      15   7
 *   输入: root
 *   输出: 3
 *
 * @param root 根节点
 * @return 最大深度
 */
int maxDepth(TreeNode* root) {
    // TODO: 实现你的解法
    return 0;
}

/**
 * 题目2: 二叉树的最小深度
 *
 * 给定一个二叉树，找出其最小深度。
 * 最小深度是从根节点到最近叶子节点的最短路径上的节点数量。
 *
 * 注意：叶子节点是指没有子节点的节点。
 *
 * 示例:
 *       3
 *      / \
 *     9  20
 *       /  \
 *      15   7
 *   输入: root
 *   输出: 2
 *
 *       2
 *        \
 *         3
 *          \
 *           4
 *   输入: root
 *   输出: 4（不是 1，因为 2 不是叶子节点）
 *
 * @param root 根节点
 * @return 最小深度
 */
int minDepth(TreeNode* root) {
    // TODO: 实现你的解法
    return 0;
}

/**
 * 题目3: 平衡二叉树
 *
 * 给定一个二叉树，判断它是否是高度平衡的二叉树。
 * 一棵高度平衡二叉树定义为：一个二叉树每个节点的左右两个子树的高度差的绝对值不超过 1。
 *
 * 示例:
 *       3
 *      / \
 *     9  20
 *       /  \
 *      15   7
 *   输入: root
 *   输出: true
 *
 *         1
 *        / \
 *       2   2
 *      / \
 *     3   3
 *    / \
 *   4   4
 *   输入: root
 *   输出: false
 *
 * @param root 根节点
 * @return 是否平衡
 */
bool isBalanced(TreeNode* root) {
    // TODO: 实现你的解法
    return false;
}

/**
 * 题目4: 对称二叉树
 *
 * 给你一个二叉树的根节点 root，检查它是否轴对称。
 *
 * 示例:
 *       1
 *      / \
 *     2   2
 *    / \ / \
 *   3  4 4  3
 *   输入: root
 *   输出: true
 *
 *       1
 *      / \
 *     2   2
 *      \   \
 *       3   3
 *   输入: root
 *   输出: false
 *
 * @param root 根节点
 * @return 是否对称
 */
bool isSymmetric(TreeNode* root) {
    // TODO: 实现你的解法
    return false;
}

/**
 * 题目5: 相同的树
 *
 * 给你两棵二叉树的根节点 p 和 q，编写一个函数来检验这两棵树是否相同。
 * 如果两个树在结构上相同，并且节点具有相同的值，则认为它们是相同的。
 *
 * @param p 第一棵树的根节点
 * @param q 第二棵树的根节点
 * @return 是否相同
 */
bool isSameTree(TreeNode* p, TreeNode* q) {
    // TODO: 实现你的解法
    return false;
}

/**
 * 题目6: 另一棵树的子树
 *
 * 给你两棵二叉树 root 和 subRoot。
 * 检验 root 中是否包含和 subRoot 具有相同结构和节点值的子树。
 * 如果存在，返回 true；否则，返回 false。
 *
 * 示例:
 *   root:          subRoot:
 *       3             4
 *      / \           / \
 *     4   5         1   2
 *    / \
 *   1   2
 *   输出: true
 *
 * @param root 主树根节点
 * @param subRoot 子树根节点
 * @return 是否为子树
 */
bool isSubtree(TreeNode* root, TreeNode* subRoot) {
    // TODO: 实现你的解法
    return false;
}

/**
 * 题目7: 翻转二叉树
 *
 * 给你一棵二叉树的根节点 root，翻转这棵二叉树，并返回其根节点。
 *
 * 示例:
 *   输入:
 *       4
 *      / \
 *     2   7
 *    / \ / \
 *   1  3 6  9
 *
 *   输出:
 *       4
 *      / \
 *     7   2
 *    / \ / \
 *   9  6 3  1
 *
 * @param root 根节点
 * @return 翻转后的根节点
 */
TreeNode* invertTree(TreeNode* root) {
    // TODO: 实现你的解法
    return nullptr;
}

/**
 * 题目8: 合并二叉树
 *
 * 给你两棵二叉树 root1 和 root2。
 * 将这两棵树合并成一棵新的二叉树。合并规则：如果两个节点重叠，
 * 那么将这两个节点的值相加作为合并后节点的新值；否则，不为 null 的节点将直接作为新二叉树的节点。
 *
 * 示例:
 *   root1:        root2:
 *       1             2
 *      / \           / \
 *     3   2         1   3
 *    /               \   \
 *   5                 4   7
 *
 *   输出:
 *       3
 *      / \
 *     4   5
 *    / \   \
 *   5   4   7
 *
 * @param root1 第一棵树
 * @param root2 第二棵树
 * @return 合并后的树
 */
TreeNode* mergeTrees(TreeNode* root1, TreeNode* root2) {
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


// ==================== 测试代码 ====================
#include <cassert>

void testMaxDepth() {
    TreeNode* root = createTree({3, 9, 20, -1, -1, 15, 7});
    assert(maxDepth(root) == 3);
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
    deleteTree(root);

    // 单边树
    root = createTree({2, -1, 3, -1, -1, -1, 4});
    // 重建：2 -> 3 -> 4
    deleteTree(root);
    root = new TreeNode(2);
    root->right = new TreeNode(3);
    root->right->right = new TreeNode(4);
    assert(minDepth(root) == 3);
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

    std::cout << "isBalanced: PASSED\n";
}

void testIsSymmetric() {
    TreeNode* root = createTree({1, 2, 2, 3, 4, 4, 3});
    assert(isSymmetric(root) == true);
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

    std::cout << "isSameTree: PASSED\n";
}

void testIsSubtree() {
    TreeNode* root = createTree({3, 4, 5, 1, 2});
    TreeNode* subRoot = createTree({4, 1, 2});
    assert(isSubtree(root, subRoot) == true);
    deleteTree(root);
    deleteTree(subRoot);

    std::cout << "isSubtree: PASSED\n";
}

void testInvertTree() {
    TreeNode* root = createTree({4, 2, 7, 1, 3, 6, 9});
    TreeNode* inverted = invertTree(root);
    // 验证：根的左子树应该是原来的右子树
    assert(inverted->left->val == 7);
    assert(inverted->right->val == 2);
    deleteTree(inverted);

    std::cout << "invertTree: PASSED\n";
}

void testMergeTrees() {
    TreeNode* t1 = createTree({1, 3, 2, 5});
    TreeNode* t2 = createTree({2, 1, 3, -1, 4, -1, 7});
    TreeNode* merged = mergeTrees(t1, t2);
    assert(merged->val == 3);
    assert(merged->left->val == 4);
    deleteTree(merged);
    // t1 和 t2 已被修改或合并，不需要单独删除

    std::cout << "mergeTrees: PASSED\n";
}

int main() {
    std::cout << "=== Tree Properties Tests ===\n";

    // 取消注释以运行测试（实现解法后）
    // testMaxDepth();
    // testMinDepth();
    // testIsBalanced();
    // testIsSymmetric();
    // testIsSameTree();
    // testIsSubtree();
    // testInvertTree();
    // testMergeTrees();

    std::cout << "All tests passed!\n";
    return 0;
}
