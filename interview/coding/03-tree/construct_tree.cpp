/**
 * @file construct_tree.cpp
 * @brief 构造二叉树问题 - 题目文件
 *
 * 构造二叉树是面试高频题，需要理解遍历序列的特性来还原树的结构。
 */

#include <iostream>
#include <vector>
#include <queue>
#include <unordered_map>

struct TreeNode {
    int val;
    TreeNode* left;
    TreeNode* right;
    TreeNode() : val(0), left(nullptr), right(nullptr) {}
    TreeNode(int x) : val(x), left(nullptr), right(nullptr) {}
    TreeNode(int x, TreeNode* l, TreeNode* r) : val(x), left(l), right(r) {}
};

struct ListNode {
    int val;
    ListNode* next;
    ListNode() : val(0), next(nullptr) {}
    ListNode(int x) : val(x), next(nullptr) {}
    ListNode(int x, ListNode* n) : val(x), next(n) {}
};

/**
 * 题目1: 从前序与中序遍历序列构造二叉树
 *
 * 给定两个整数数组 preorder 和 inorder，其中 preorder 是二叉树的前序遍历，
 * inorder 是同一棵树的中序遍历，请构造二叉树并返回其根节点。
 *
 * 关键点：
 *   - 前序遍历的第一个元素是根节点
 *   - 在中序遍历中找到根节点，左边是左子树，右边是右子树
 *   - 递归构造左右子树
 *
 * 示例:
 *   输入: preorder = [3,9,20,15,7], inorder = [9,3,15,20,7]
 *   输出:
 *       3
 *      / \
 *     9  20
 *       /  \
 *      15   7
 *
 * @param preorder 前序遍历数组
 * @param inorder 中序遍历数组
 * @return 构造的二叉树根节点
 */
TreeNode* buildTreeFromPreIn(std::vector<int>& preorder, std::vector<int>& inorder) {
    // TODO: 实现你的解法
    return nullptr;
}

/**
 * 题目2: 从中序与后序遍历序列构造二叉树
 *
 * 给定两个整数数组 inorder 和 postorder，其中 inorder 是二叉树的中序遍历，
 * postorder 是同一棵树的后序遍历，请你构造并返回这棵二叉树。
 *
 * 关键点：
 *   - 后序遍历的最后一个元素是根节点
 *   - 在中序遍历中找到根节点位置，划分左右子树
 *   - 从后往前处理后序数组
 *
 * 示例:
 *   输入: inorder = [9,3,15,20,7], postorder = [9,15,7,20,3]
 *   输出:
 *       3
 *      / \
 *     9  20
 *       /  \
 *      15   7
 *
 * @param inorder 中序遍历数组
 * @param postorder 后序遍历数组
 * @return 构造的二叉树根节点
 */
TreeNode* buildTreeFromInPost(std::vector<int>& inorder, std::vector<int>& postorder) {
    // TODO: 实现你的解法
    return nullptr;
}

/**
 * 题目3: 根据前序和后序遍历构造二叉树
 *
 * 给定两个整数数组 preorder 和 postorder，其中 preorder 是一棵二叉树的前序遍历，
 * postorder 是同一棵树的后序遍历，请你构造并返回这棵二叉树。
 *
 * 注意：如果存在多个答案，您可以返回其中任何一个。
 *      （没有中序遍历，无法唯一确定树的结构）
 *
 * 关键点：
 *   - 前序遍历: 根 -> 左子树 -> 右子树
 *   - 后序遍历: 左子树 -> 右子树 -> 根
 *   - 前序的第二个元素是左子树的根（如果存在左子树）
 *   - 在后序中找到这个左子树根，可以确定左子树的范围
 *
 * 示例:
 *   输入: preorder = [1,2,4,5,3,6,7], postorder = [4,5,2,6,7,3,1]
 *   输出:
 *         1
 *        / \
 *       2   3
 *      / \ / \
 *     4  5 6  7
 *
 * @param preorder 前序遍历数组
 * @param postorder 后序遍历数组
 * @return 构造的二叉树根节点
 */
TreeNode* buildTreeFromPrePost(std::vector<int>& preorder, std::vector<int>& postorder) {
    // TODO: 实现你的解法
    return nullptr;
}

/**
 * 题目4: 最大二叉树
 *
 * 给定一个不重复的整数数组 nums。最大二叉树可以用下面的算法从 nums 递归地构建:
 *   1. 创建一个根节点，其值为 nums 中的最大值。
 *   2. 递归地在最大值左边的子数组上构建左子树。
 *   3. 递归地在最大值右边的子数组上构建右子树。
 *
 * 示例:
 *   输入: nums = [3,2,1,6,0,5]
 *   输出:
 *         6
 *        / \
 *       3   5
 *        \  /
 *         2 0
 *          \
 *           1
 *
 * @param nums 输入数组
 * @return 最大二叉树根节点
 */
TreeNode* constructMaximumBinaryTree(std::vector<int>& nums) {
    // TODO: 实现你的解法
    return nullptr;
}

/**
 * 题目5: 有序链表转换二叉搜索树
 *
 * 给定一个单链表的头节点 head，其中的元素按升序排序，
 * 将其转换为高度平衡的二叉搜索树。
 *
 * 高度平衡二叉树：每个节点的两个子树的深度差不超过 1。
 *
 * 示例:
 *   输入: head = [-10,-3,0,5,9]
 *   输出:
 *         0
 *        / \
 *      -3   9
 *      /   /
 *    -10  5
 *
 * @param head 有序链表头节点
 * @return 平衡 BST 根节点
 */
TreeNode* sortedListToBST(ListNode* head) {
    // TODO: 实现你的解法
    return nullptr;
}

/**
 * 题目6: 有序数组转换为二叉搜索树
 *
 * 给你一个整数数组 nums，其中元素已经按升序排列，
 * 请你将其转换为一棵高度平衡二叉搜索树。
 *
 * 示例:
 *   输入: nums = [-10,-3,0,5,9]
 *   输出:
 *         0
 *        / \
 *      -3   9
 *      /   /
 *    -10  5
 *
 * @param nums 有序数组
 * @return 平衡 BST 根节点
 */
TreeNode* sortedArrayToBST(std::vector<int>& nums) {
    // TODO: 实现你的解法
    return nullptr;
}

/**
 * 题目7: 将二叉搜索树转化为排序的双向链表
 *
 * 将一个二叉搜索树就地转换为一个已排序的双向循环链表。
 * 可以将左右指针作为双向链表的前驱后继指针。
 *
 * 对于双向循环链表：
 *   - 第一个节点的前驱是最后一个节点
 *   - 最后一个节点的后继是第一个节点
 *
 * 示例:
 *         4
 *        / \
 *       2   5
 *      / \
 *     1   3
 *
 *   输出: 1 <-> 2 <-> 3 <-> 4 <-> 5 (循环连接)
 *
 * @param root BST 根节点
 * @return 双向链表的头节点
 */
TreeNode* treeToDoublyList(TreeNode* root) {
    // TODO: 实现你的解法
    return nullptr;
}

/**
 * 题目8: 从先序遍历还原二叉树
 *
 * 我们从二叉树的根节点 root 开始进行深度优先搜索。
 * 在遍历中的每个节点处，我们输出 D 条短划线（其中 D 是该节点的深度），然后输出该节点的值。
 * 如果节点的深度为 D，则其直接子节点的深度为 D + 1。根节点的深度为 0。
 *
 * 如果节点只有一个子节点，那么保证该子节点为左子节点。
 *
 * 给出遍历输出 traversal，还原树并返回其根节点 root。
 *
 * 示例:
 *   输入: traversal = "1-2--3--4-5--6--7"
 *   输出:
 *         1
 *        / \
 *       2   5
 *      / \ / \
 *     3  4 6  7
 *
 *   输入: traversal = "1-2--3---4-5--6---7"
 *   输出:
 *         1
 *        / \
 *       2   5
 *      /   /
 *     3   6
 *    /   /
 *   4   7
 *
 * @param traversal 遍历字符串
 * @return 还原的二叉树根节点
 */
TreeNode* recoverFromPreorder(std::string traversal) {
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

ListNode* createList(const std::vector<int>& vals) {
    if (vals.empty()) return nullptr;
    ListNode* head = new ListNode(vals[0]);
    ListNode* curr = head;
    for (size_t i = 1; i < vals.size(); ++i) {
        curr->next = new ListNode(vals[i]);
        curr = curr->next;
    }
    return head;
}

void deleteList(ListNode* head) {
    while (head) {
        ListNode* temp = head;
        head = head->next;
        delete temp;
    }
}

// 前序遍历验证
void preorder(TreeNode* root, std::vector<int>& result) {
    if (!root) return;
    result.push_back(root->val);
    preorder(root->left, result);
    preorder(root->right, result);
}

// 中序遍历验证
void inorder(TreeNode* root, std::vector<int>& result) {
    if (!root) return;
    inorder(root->left, result);
    result.push_back(root->val);
    inorder(root->right, result);
}

// 后序遍历验证
void postorder(TreeNode* root, std::vector<int>& result) {
    if (!root) return;
    postorder(root->left, result);
    postorder(root->right, result);
    result.push_back(root->val);
}


// ==================== 测试代码 ====================
#include <cassert>

void testBuildTreeFromPreIn() {
    std::vector<int> pre = {3, 9, 20, 15, 7};
    std::vector<int> in = {9, 3, 15, 20, 7};
    TreeNode* root = buildTreeFromPreIn(pre, in);

    // 验证
    std::vector<int> preResult, inResult;
    preorder(root, preResult);
    inorder(root, inResult);
    assert(preResult == pre);
    assert(inResult == in);

    deleteTree(root);

    std::cout << "buildTreeFromPreIn: PASSED\n";
}

void testBuildTreeFromInPost() {
    std::vector<int> in = {9, 3, 15, 20, 7};
    std::vector<int> post = {9, 15, 7, 20, 3};
    TreeNode* root = buildTreeFromInPost(in, post);

    // 验证
    std::vector<int> inResult, postResult;
    inorder(root, inResult);
    postorder(root, postResult);
    assert(inResult == in);
    assert(postResult == post);

    deleteTree(root);

    std::cout << "buildTreeFromInPost: PASSED\n";
}

void testConstructMaximumBinaryTree() {
    std::vector<int> nums = {3, 2, 1, 6, 0, 5};
    TreeNode* root = constructMaximumBinaryTree(nums);

    assert(root != nullptr && root->val == 6);
    assert(root->left->val == 3);
    assert(root->right->val == 5);

    deleteTree(root);

    std::cout << "constructMaximumBinaryTree: PASSED\n";
}

void testSortedArrayToBST() {
    std::vector<int> nums = {-10, -3, 0, 5, 9};
    TreeNode* root = sortedArrayToBST(nums);

    // 验证中序遍历是有序的
    std::vector<int> inResult;
    inorder(root, inResult);
    assert(inResult == nums);

    deleteTree(root);

    std::cout << "sortedArrayToBST: PASSED\n";
}

void testSortedListToBST() {
    ListNode* head = createList({-10, -3, 0, 5, 9});
    TreeNode* root = sortedListToBST(head);

    // 验证中序遍历
    std::vector<int> inResult;
    inorder(root, inResult);
    assert(inResult == std::vector<int>({-10, -3, 0, 5, 9}));

    deleteTree(root);
    // 注意：head 在转换过程中可能已经被修改或保留，根据实现而定

    std::cout << "sortedListToBST: PASSED\n";
}

int main() {
    std::cout << "=== Construct Tree Tests ===\n";

    // 取消注释以运行测试（实现解法后）
    // testBuildTreeFromPreIn();
    // testBuildTreeFromInPost();
    // testConstructMaximumBinaryTree();
    // testSortedArrayToBST();
    // testSortedListToBST();

    std::cout << "All tests passed!\n";
    return 0;
}
