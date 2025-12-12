/**
 * @file bst_operations.cpp
 * @brief 二叉搜索树操作 - 题目文件
 *
 * BST 的核心性质：左子树所有节点 < 根 < 右子树所有节点
 * 中序遍历结果是有序的
 */

#include <iostream>
#include <vector>
#include <queue>
#include <climits>

struct TreeNode {
    int val;
    TreeNode* left;
    TreeNode* right;
    TreeNode() : val(0), left(nullptr), right(nullptr) {}
    TreeNode(int x) : val(x), left(nullptr), right(nullptr) {}
    TreeNode(int x, TreeNode* l, TreeNode* r) : val(x), left(l), right(r) {}
};

/**
 * 题目1: 验证二叉搜索树
 *
 * 给你一个二叉树的根节点 root，判断其是否是一个有效的二叉搜索树。
 *
 * 有��� BST 定义如下：
 *   - 节点的左子树只包含小于当前节点的数
 *   - 节点的右子树只包含大于当前节点的数
 *   - 所有左子树和右子树自身必须也是二叉搜索树
 *
 * 示例:
 *       2
 *      / \
 *     1   3
 *   输出: true
 *
 *       5
 *      / \
 *     1   4
 *        / \
 *       3   6
 *   输出: false（4 的左子树 3 小于 5）
 *
 * @param root 根节点
 * @return 是否为有效 BST
 */
bool isValidBST(TreeNode* root) {
    // TODO: 实现你的解法
    return false;
}

/**
 * 题目2: 二叉搜索树中的搜索
 *
 * 给定二叉搜索树的根节点 root 和一个值 val。
 * 你需要在 BST 中找到节点值等于 val 的节点。
 * 返回以该节点为根的子树。如果节点不存在，则返回 null。
 *
 * @param root 根节点
 * @param val 要搜索的值
 * @return 找到的节点（或 null）
 */
TreeNode* searchBST(TreeNode* root, int val) {
    // TODO: 实现你的解法
    return nullptr;
}

/**
 * 题目3: 二叉搜索树中的插入操作
 *
 * 给定二叉搜索树的根节点 root 和要插入树中的值 val，
 * 将值插入二叉搜索树。返回插入后二叉搜索树的根节点。
 *
 * 保证原始二叉搜索树中不存在新值。
 *
 * @param root 根节点
 * @param val 要插入的值
 * @return 插入后的根节点
 */
TreeNode* insertIntoBST(TreeNode* root, int val) {
    // TODO: 实现你的解法
    return nullptr;
}

/**
 * 题目4: 删除二叉搜索树中的节点
 *
 * 给定一个二叉搜索树的根节点 root 和一个值 key，
 * 删除二叉搜索树中的 key 对应的节点，并保证二叉搜索树的性质不变。
 * 返回二叉搜索树（有可能被更新）的根节点的引用。
 *
 * 删除分为三种情况:
 *   1. 节点是叶子节点，直接删除
 *   2. 节点只有一个子树，用子树替代
 *   3. 节点有两个子树，用后继（或前驱）替代
 *
 * @param root 根节点
 * @param key 要删除的值
 * @return 删除后的根节点
 */
TreeNode* deleteNode(TreeNode* root, int key) {
    // TODO: 实现你的解法
    return nullptr;
}

/**
 * 题目5: 二叉搜索树的最近公共祖先
 *
 * 给定一个二叉搜索树, 找到该树中两个指定节点的最近公共祖先。
 *
 * BST 的特性可以简化查找：
 *   - 如果 p, q 都小于 root，则 LCA 在左子树
 *   - 如果 p, q 都大于 root，则 LCA 在右子树
 *   - 否则 root 就是 LCA
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
 * 题目6: 二叉搜索树中第 K 小的元素
 *
 * 给定一个二叉搜索树的根节点 root，和一个整数 k，
 * 请你设计一个算法查找其中第 k 个最小元素（从 1 开始计数）。
 *
 * 示例:
 *       3
 *      / \
 *     1   4
 *      \
 *       2
 *   输入: k = 1
 *   输出: 1
 *
 * @param root 根节点
 * @param k 第 k 小
 * @return 第 k 小的值
 */
int kthSmallest(TreeNode* root, int k) {
    // TODO: 实现你的解法
    return 0;
}

/**
 * 题目7: 将有序数组转换为二叉搜索树
 *
 * 给你一个整数数组 nums，其中元素已经按升序排列，
 * 请你将其转换为一棵高度平衡二叉搜索树。
 *
 * 高度平衡二叉树：每个节点的两个子树的深度相差不超过 1。
 *
 * 示例:
 *   输入: nums = [-10,-3,0,5,9]
 *   输出: [0,-3,9,-10,null,5] 或其他有效答案
 *
 * @param nums 有序数组
 * @return BST 根节点
 */
TreeNode* sortedArrayToBST(std::vector<int>& nums) {
    // TODO: 实现你的解法
    return nullptr;
}

/**
 * 题目8: 二叉搜索树的范围和
 *
 * 给定二叉搜索树的根结点 root，返回值位于范围 [low, high] 之间的所有结点的值的和。
 *
 * 示例:
 *       10
 *      /  \
 *     5   15
 *    / \    \
 *   3   7   18
 *   输入: low = 7, high = 15
 *   输出: 32 (7 + 10 + 15)
 *
 * @param root 根节点
 * @param low 下界
 * @param high 上界
 * @return 范围内的和
 */
int rangeSumBST(TreeNode* root, int low, int high) {
    // TODO: 实现你的解法
    return 0;
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

// 中序遍历（用于验证 BST）
void inorder(TreeNode* root, std::vector<int>& result) {
    if (!root) return;
    inorder(root->left, result);
    result.push_back(root->val);
    inorder(root->right, result);
}


// ==================== 测试代码 ====================
#include <cassert>

void testIsValidBST() {
    TreeNode* root = createTree({2, 1, 3});
    assert(isValidBST(root) == true);
    deleteTree(root);

    root = createTree({5, 1, 4, -1, -1, 3, 6});
    assert(isValidBST(root) == false);
    deleteTree(root);

    std::cout << "isValidBST: PASSED\n";
}

void testSearchBST() {
    TreeNode* root = createTree({4, 2, 7, 1, 3});
    TreeNode* result = searchBST(root, 2);
    assert(result != nullptr && result->val == 2);

    result = searchBST(root, 5);
    assert(result == nullptr);
    deleteTree(root);

    std::cout << "searchBST: PASSED\n";
}

void testInsertIntoBST() {
    TreeNode* root = createTree({4, 2, 7, 1, 3});
    root = insertIntoBST(root, 5);
    // 验证 5 被正确插入
    std::vector<int> inorderResult;
    inorder(root, inorderResult);
    assert(inorderResult == std::vector<int>({1, 2, 3, 4, 5, 7}));
    deleteTree(root);

    std::cout << "insertIntoBST: PASSED\n";
}

void testDeleteNode() {
    TreeNode* root = createTree({5, 3, 6, 2, 4, -1, 7});
    root = deleteNode(root, 3);
    std::vector<int> inorderResult;
    inorder(root, inorderResult);
    // 3 被删除，顺序应该是 2,4,5,6,7
    assert(inorderResult == std::vector<int>({2, 4, 5, 6, 7}));
    deleteTree(root);

    std::cout << "deleteNode: PASSED\n";
}

void testLowestCommonAncestorBST() {
    TreeNode* root = createTree({6, 2, 8, 0, 4, 7, 9, -1, -1, 3, 5});
    TreeNode* p = root->left;        // 2
    TreeNode* q = root->right;       // 8
    TreeNode* lca = lowestCommonAncestorBST(root, p, q);
    assert(lca == root);  // 6

    p = root->left;                  // 2
    q = root->left->right;           // 4
    lca = lowestCommonAncestorBST(root, p, q);
    assert(lca == root->left);  // 2

    deleteTree(root);

    std::cout << "lowestCommonAncestorBST: PASSED\n";
}

void testKthSmallest() {
    TreeNode* root = createTree({3, 1, 4, -1, 2});
    assert(kthSmallest(root, 1) == 1);
    assert(kthSmallest(root, 2) == 2);
    assert(kthSmallest(root, 3) == 3);
    deleteTree(root);

    std::cout << "kthSmallest: PASSED\n";
}

void testSortedArrayToBST() {
    std::vector<int> nums = {-10, -3, 0, 5, 9};
    TreeNode* root = sortedArrayToBST(nums);
    // 验证是有效 BST
    assert(isValidBST(root) == true);
    // 验证中序遍历结果正确
    std::vector<int> inorderResult;
    inorder(root, inorderResult);
    assert(inorderResult == nums);
    deleteTree(root);

    std::cout << "sortedArrayToBST: PASSED\n";
}

void testRangeSumBST() {
    TreeNode* root = createTree({10, 5, 15, 3, 7, -1, 18});
    assert(rangeSumBST(root, 7, 15) == 32);
    deleteTree(root);

    std::cout << "rangeSumBST: PASSED\n";
}

int main() {
    std::cout << "=== BST Operations Tests ===\n";

    // 取消注释以运行测试（实现解法后）
    // testIsValidBST();
    // testSearchBST();
    // testInsertIntoBST();
    // testDeleteNode();
    // testLowestCommonAncestorBST();
    // testKthSmallest();
    // testSortedArrayToBST();
    // testRangeSumBST();

    std::cout << "All tests passed!\n";
    return 0;
}
