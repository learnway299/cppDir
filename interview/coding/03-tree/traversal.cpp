/**
 * @file traversal.cpp
 * @brief 二叉树遍历问题 - 题目文件
 *
 * 树的遍历是最基础的树问题，需要掌握递归和迭代两种实现方式。
 */

#include <iostream>
#include <vector>
#include <stack>
#include <queue>

struct TreeNode {
    int val;
    TreeNode* left;
    TreeNode* right;
    TreeNode() : val(0), left(nullptr), right(nullptr) {}
    TreeNode(int x) : val(x), left(nullptr), right(nullptr) {}
    TreeNode(int x, TreeNode* l, TreeNode* r) : val(x), left(l), right(r) {}
};

/**
 * 题目1: 二叉树的前序遍历
 *
 * 给你二叉树的根节点 root，返回它节点值的前序遍历。
 * 前序遍历顺序：根 -> 左 -> 右
 *
 * 示例:
 *       1
 *        \
 *         2
 *        /
 *       3
 *   输入: root = [1,null,2,3]
 *   输出: [1,2,3]
 *
 * 进阶: 递归算法很简单，你可以通过迭代算法完成吗？
 *
 * @param root 根节点
 * @return 前序遍历结果
 */
std::vector<int> preorderTraversal(TreeNode* root) {
    // TODO: 实现你的解法
    return {};
}

/**
 * 题目2: 二叉树的中序遍历
 *
 * 给定一个二叉树的根节点 root，返回它的中序遍历。
 * 中序遍历顺序：左 -> 根 -> 右
 *
 * 示例:
 *       1
 *        \
 *         2
 *        /
 *       3
 *   输入: root = [1,null,2,3]
 *   输出: [1,3,2]
 *
 * @param root 根节点
 * @return 中序遍历结果
 */
std::vector<int> inorderTraversal(TreeNode* root) {
    // TODO: 实现你的解法
    return {};
}

/**
 * 题目3: 二叉树的后序遍历
 *
 * 给你一棵二叉树的根节点 root，返回其节点值的后序遍历。
 * 后序遍历顺序：左 -> 右 -> 根
 *
 * 示例:
 *       1
 *        \
 *         2
 *        /
 *       3
 *   输入: root = [1,null,2,3]
 *   输出: [3,2,1]
 *
 * @param root 根节点
 * @return 后序遍历结果
 */
std::vector<int> postorderTraversal(TreeNode* root) {
    // TODO: 实现你的解法
    return {};
}

/**
 * 题目4: 二叉树的层序遍历
 *
 * 给你二叉树的根节点 root，返回其节点值的层序遍历。
 * （即逐层地，从左到右访问所有节点）
 *
 * 示例:
 *       3
 *      / \
 *     9  20
 *       /  \
 *      15   7
 *   输入: root = [3,9,20,null,null,15,7]
 *   输出: [[3],[9,20],[15,7]]
 *
 * @param root 根节点
 * @return 层序遍历结果（每层一个数组）
 */
std::vector<std::vector<int>> levelOrder(TreeNode* root) {
    // TODO: 实现你的解法
    return {};
}

/**
 * 题目5: 二叉树的锯齿形层序遍历
 *
 * 给你二叉树的根节点 root，返回其节点值的锯齿形层序遍历。
 * （即先从左往右，再从右往左进行下一层遍历，以此类推，层与层之间交替进行）
 *
 * 示例:
 *       3
 *      / \
 *     9  20
 *       /  \
 *      15   7
 *   输入: root = [3,9,20,null,null,15,7]
 *   输出: [[3],[20,9],[15,7]]
 *
 * @param root 根节点
 * @return 锯齿形层序遍历结果
 */
std::vector<std::vector<int>> zigzagLevelOrder(TreeNode* root) {
    // TODO: 实现你的解法
    return {};
}

/**
 * 题目6: 二叉树的右视图
 *
 * 给定一个二叉树的根节点 root，想象自己站在它的右侧，
 * 按照从顶部到底部的顺序，返回从右侧所能看到的节点值。
 *
 * 示例:
 *       1
 *      / \
 *     2   3
 *      \   \
 *       5   4
 *   输入: root = [1,2,3,null,5,null,4]
 *   输出: [1,3,4]
 *
 * @param root 根节点
 * @return 右视图节点值
 */
std::vector<int> rightSideView(TreeNode* root) {
    // TODO: 实现你的解法
    return {};
}


// ==================== 辅助函数 ====================

// 从数组创建二叉树（层序，null 用 -1 表示）
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

// 释放二叉树内存
void deleteTree(TreeNode* root) {
    if (!root) return;
    deleteTree(root->left);
    deleteTree(root->right);
    delete root;
}

// 打印数组
template<typename T>
void printVector(const std::vector<T>& v) {
    std::cout << "[";
    for (size_t i = 0; i < v.size(); ++i) {
        std::cout << v[i];
        if (i < v.size() - 1) std::cout << ",";
    }
    std::cout << "]";
}


// ==================== 测试代码 ====================
#include <cassert>

void testPreorderTraversal() {
    //     1
    //      \
    //       2
    //      /
    //     3
    TreeNode* root = new TreeNode(1);
    root->right = new TreeNode(2);
    root->right->left = new TreeNode(3);

    std::vector<int> result = preorderTraversal(root);
    assert(result == std::vector<int>({1, 2, 3}));
    deleteTree(root);

    // 空树
    assert(preorderTraversal(nullptr).empty());

    std::cout << "preorderTraversal: PASSED\n";
}

void testInorderTraversal() {
    TreeNode* root = new TreeNode(1);
    root->right = new TreeNode(2);
    root->right->left = new TreeNode(3);

    std::vector<int> result = inorderTraversal(root);
    assert(result == std::vector<int>({1, 3, 2}));
    deleteTree(root);

    std::cout << "inorderTraversal: PASSED\n";
}

void testPostorderTraversal() {
    TreeNode* root = new TreeNode(1);
    root->right = new TreeNode(2);
    root->right->left = new TreeNode(3);

    std::vector<int> result = postorderTraversal(root);
    assert(result == std::vector<int>({3, 2, 1}));
    deleteTree(root);

    std::cout << "postorderTraversal: PASSED\n";
}

void testLevelOrder() {
    TreeNode* root = createTree({3, 9, 20, -1, -1, 15, 7});
    auto result = levelOrder(root);
    std::vector<std::vector<int>> expected = {{3}, {9, 20}, {15, 7}};
    assert(result == expected);
    deleteTree(root);

    std::cout << "levelOrder: PASSED\n";
}

void testZigzagLevelOrder() {
    TreeNode* root = createTree({3, 9, 20, -1, -1, 15, 7});
    auto result = zigzagLevelOrder(root);
    std::vector<std::vector<int>> expected = {{3}, {20, 9}, {15, 7}};
    assert(result == expected);
    deleteTree(root);

    std::cout << "zigzagLevelOrder: PASSED\n";
}

void testRightSideView() {
    TreeNode* root = createTree({1, 2, 3, -1, 5, -1, 4});
    auto result = rightSideView(root);
    assert(result == std::vector<int>({1, 3, 4}));
    deleteTree(root);

    std::cout << "rightSideView: PASSED\n";
}

int main() {
    std::cout << "=== Tree Traversal Tests ===\n";

    // 取消注释以运行测试（实现解法后）
    // testPreorderTraversal();
    // testInorderTraversal();
    // testPostorderTraversal();
    // testLevelOrder();
    // testZigzagLevelOrder();
    // testRightSideView();

    std::cout << "All tests passed!\n";
    return 0;
}
