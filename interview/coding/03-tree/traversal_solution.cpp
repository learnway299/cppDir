/**
 * @file traversal_solution.cpp
 * @brief 二叉树遍历问题 - 解答文件
 *
 * 包含 traversal.cpp 中所有题目的解答和详细解析。
 */

#include <iostream>
#include <vector>
#include <stack>
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
 * 题目1: 二叉树的前序遍历
 *
 * 解法1: 递归
 * 时间复杂度: O(n)
 * 空间复杂度: O(n) 递归栈
 */
void preorderHelper(TreeNode* root, std::vector<int>& result) {
    if (!root) return;
    result.push_back(root->val);      // 根
    preorderHelper(root->left, result);  // 左
    preorderHelper(root->right, result); // 右
}

std::vector<int> preorderTraversalRecursive(TreeNode* root) {
    std::vector<int> result;
    preorderHelper(root, result);
    return result;
}

/**
 * 解法2: 迭代（使用栈）
 * 思路: 模拟递归，先压右子树再压左子树
 */
std::vector<int> preorderTraversal(TreeNode* root) {
    std::vector<int> result;
    if (!root) return result;

    std::stack<TreeNode*> stk;
    stk.push(root);

    while (!stk.empty()) {
        TreeNode* node = stk.top();
        stk.pop();
        result.push_back(node->val);

        // 先压右，后压左（这样左先出栈）
        if (node->right) stk.push(node->right);
        if (node->left) stk.push(node->left);
    }

    return result;
}

/**
 * 题目2: 二叉树的中序遍历
 *
 * 解法1: 递归
 */
void inorderHelper(TreeNode* root, std::vector<int>& result) {
    if (!root) return;
    inorderHelper(root->left, result);   // 左
    result.push_back(root->val);         // 根
    inorderHelper(root->right, result);  // 右
}

std::vector<int> inorderTraversalRecursive(TreeNode* root) {
    std::vector<int> result;
    inorderHelper(root, result);
    return result;
}

/**
 * 解法2: 迭代
 * 思路: 一直往左走到底，然后处理当前节点，再转向右子树
 */
std::vector<int> inorderTraversal(TreeNode* root) {
    std::vector<int> result;
    std::stack<TreeNode*> stk;
    TreeNode* curr = root;

    while (curr || !stk.empty()) {
        // 一直往左走到底
        while (curr) {
            stk.push(curr);
            curr = curr->left;
        }

        // 处理当前节点
        curr = stk.top();
        stk.pop();
        result.push_back(curr->val);

        // 转向右子树
        curr = curr->right;
    }

    return result;
}

/**
 * 题目3: 二叉树的后序遍历
 *
 * 解法1: 递归
 */
void postorderHelper(TreeNode* root, std::vector<int>& result) {
    if (!root) return;
    postorderHelper(root->left, result);  // 左
    postorderHelper(root->right, result); // 右
    result.push_back(root->val);          // 根
}

std::vector<int> postorderTraversalRecursive(TreeNode* root) {
    std::vector<int> result;
    postorderHelper(root, result);
    return result;
}

/**
 * 解法2: 迭代（反转前序变体）
 * 思路: 前序是 根->左->右，改为 根->右->左，然后反转得到 左->右->根
 */
std::vector<int> postorderTraversal(TreeNode* root) {
    std::vector<int> result;
    if (!root) return result;

    std::stack<TreeNode*> stk;
    stk.push(root);

    while (!stk.empty()) {
        TreeNode* node = stk.top();
        stk.pop();
        result.push_back(node->val);

        // 先压左，后压右（与前序相反）
        if (node->left) stk.push(node->left);
        if (node->right) stk.push(node->right);
    }

    // 反转结果
    std::reverse(result.begin(), result.end());
    return result;
}

/**
 * 解法3: 迭代（标准后序，不反转）
 * 思路: 使用标记法，记录节点是否已经访问过右子树
 */
std::vector<int> postorderTraversalV2(TreeNode* root) {
    std::vector<int> result;
    std::stack<TreeNode*> stk;
    TreeNode* curr = root;
    TreeNode* lastVisited = nullptr;

    while (curr || !stk.empty()) {
        // 一直往左走
        while (curr) {
            stk.push(curr);
            curr = curr->left;
        }

        TreeNode* top = stk.top();

        // 如果右子树存在且未访问过，先处理右子树
        if (top->right && top->right != lastVisited) {
            curr = top->right;
        } else {
            // 处理当前节点
            result.push_back(top->val);
            lastVisited = top;
            stk.pop();
        }
    }

    return result;
}

/**
 * 题目4: 二叉树的层序遍历
 *
 * 解法: BFS + 队列
 * 时间复杂度: O(n)
 * 空间复杂度: O(n)
 */
std::vector<std::vector<int>> levelOrder(TreeNode* root) {
    std::vector<std::vector<int>> result;
    if (!root) return result;

    std::queue<TreeNode*> q;
    q.push(root);

    while (!q.empty()) {
        int levelSize = q.size();
        std::vector<int> level;

        for (int i = 0; i < levelSize; ++i) {
            TreeNode* node = q.front();
            q.pop();
            level.push_back(node->val);

            if (node->left) q.push(node->left);
            if (node->right) q.push(node->right);
        }

        result.push_back(level);
    }

    return result;
}

// DFS 解法
void levelOrderDFS(TreeNode* root, int depth, std::vector<std::vector<int>>& result) {
    if (!root) return;

    if (depth >= static_cast<int>(result.size())) {
        result.push_back({});
    }

    result[depth].push_back(root->val);
    levelOrderDFS(root->left, depth + 1, result);
    levelOrderDFS(root->right, depth + 1, result);
}

std::vector<std::vector<int>> levelOrderDFSWrapper(TreeNode* root) {
    std::vector<std::vector<int>> result;
    levelOrderDFS(root, 0, result);
    return result;
}

/**
 * 题目5: 二叉树的锯齿形层序遍历
 *
 * 解法: BFS，奇数层反转
 */
std::vector<std::vector<int>> zigzagLevelOrder(TreeNode* root) {
    std::vector<std::vector<int>> result;
    if (!root) return result;

    std::queue<TreeNode*> q;
    q.push(root);
    bool leftToRight = true;

    while (!q.empty()) {
        int levelSize = q.size();
        std::vector<int> level(levelSize);

        for (int i = 0; i < levelSize; ++i) {
            TreeNode* node = q.front();
            q.pop();

            // 根据方向决定放入位置
            int idx = leftToRight ? i : levelSize - 1 - i;
            level[idx] = node->val;

            if (node->left) q.push(node->left);
            if (node->right) q.push(node->right);
        }

        result.push_back(level);
        leftToRight = !leftToRight;
    }

    return result;
}

/**
 * 题目6: 二叉树的右视图
 *
 * 解法1: BFS，取每层最后一个节点
 */
std::vector<int> rightSideView(TreeNode* root) {
    std::vector<int> result;
    if (!root) return result;

    std::queue<TreeNode*> q;
    q.push(root);

    while (!q.empty()) {
        int levelSize = q.size();

        for (int i = 0; i < levelSize; ++i) {
            TreeNode* node = q.front();
            q.pop();

            // 只保留每层最后一个节点
            if (i == levelSize - 1) {
                result.push_back(node->val);
            }

            if (node->left) q.push(node->left);
            if (node->right) q.push(node->right);
        }
    }

    return result;
}

/**
 * 解法2: DFS，优先访问右子树
 * 思路: 每层第一个访问到的节点就是右视图节点
 */
void rightSideViewDFS(TreeNode* root, int depth, std::vector<int>& result) {
    if (!root) return;

    // 当前深度第一次访问
    if (depth == static_cast<int>(result.size())) {
        result.push_back(root->val);
    }

    // 先访问右子树
    rightSideViewDFS(root->right, depth + 1, result);
    rightSideViewDFS(root->left, depth + 1, result);
}

std::vector<int> rightSideViewDFSWrapper(TreeNode* root) {
    std::vector<int> result;
    rightSideViewDFS(root, 0, result);
    return result;
}


// ==================== Morris 遍历（进阶，O(1) 空间）====================

/**
 * Morris 中序遍历
 * 思路: 利用叶子节点的空指针建立临时链接
 * 时间复杂度: O(n)
 * 空间复杂度: O(1)
 */
std::vector<int> inorderMorris(TreeNode* root) {
    std::vector<int> result;
    TreeNode* curr = root;

    while (curr) {
        if (!curr->left) {
            // 没有左子树，访问当前节点，移动到右子树
            result.push_back(curr->val);
            curr = curr->right;
        } else {
            // 找到左子树的最右节点（前驱）
            TreeNode* pred = curr->left;
            while (pred->right && pred->right != curr) {
                pred = pred->right;
            }

            if (!pred->right) {
                // 建立链接
                pred->right = curr;
                curr = curr->left;
            } else {
                // 恢复树结构，访问当前节点
                pred->right = nullptr;
                result.push_back(curr->val);
                curr = curr->right;
            }
        }
    }

    return result;
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

void testPreorderTraversal() {
    TreeNode* root = new TreeNode(1);
    root->right = new TreeNode(2);
    root->right->left = new TreeNode(3);

    assert(preorderTraversal(root) == std::vector<int>({1, 2, 3}));
    assert(preorderTraversalRecursive(root) == std::vector<int>({1, 2, 3}));
    deleteTree(root);

    assert(preorderTraversal(nullptr).empty());

    std::cout << "preorderTraversal: PASSED\n";
}

void testInorderTraversal() {
    TreeNode* root = new TreeNode(1);
    root->right = new TreeNode(2);
    root->right->left = new TreeNode(3);

    assert(inorderTraversal(root) == std::vector<int>({1, 3, 2}));
    assert(inorderTraversalRecursive(root) == std::vector<int>({1, 3, 2}));
    assert(inorderMorris(root) == std::vector<int>({1, 3, 2}));
    deleteTree(root);

    std::cout << "inorderTraversal: PASSED\n";
}

void testPostorderTraversal() {
    TreeNode* root = new TreeNode(1);
    root->right = new TreeNode(2);
    root->right->left = new TreeNode(3);

    assert(postorderTraversal(root) == std::vector<int>({3, 2, 1}));
    assert(postorderTraversalRecursive(root) == std::vector<int>({3, 2, 1}));
    assert(postorderTraversalV2(root) == std::vector<int>({3, 2, 1}));
    deleteTree(root);

    std::cout << "postorderTraversal: PASSED\n";
}

void testLevelOrder() {
    TreeNode* root = createTree({3, 9, 20, -1, -1, 15, 7});
    std::vector<std::vector<int>> expected = {{3}, {9, 20}, {15, 7}};
    assert(levelOrder(root) == expected);
    assert(levelOrderDFSWrapper(root) == expected);
    deleteTree(root);

    assert(levelOrder(nullptr).empty());

    std::cout << "levelOrder: PASSED\n";
}

void testZigzagLevelOrder() {
    TreeNode* root = createTree({3, 9, 20, -1, -1, 15, 7});
    std::vector<std::vector<int>> expected = {{3}, {20, 9}, {15, 7}};
    assert(zigzagLevelOrder(root) == expected);
    deleteTree(root);

    std::cout << "zigzagLevelOrder: PASSED\n";
}

void testRightSideView() {
    TreeNode* root = createTree({1, 2, 3, -1, 5, -1, 4});
    assert(rightSideView(root) == std::vector<int>({1, 3, 4}));
    assert(rightSideViewDFSWrapper(root) == std::vector<int>({1, 3, 4}));
    deleteTree(root);

    // 左倾树
    root = createTree({1, 2, -1, 3});
    assert(rightSideView(root) == std::vector<int>({1, 2, 3}));
    deleteTree(root);

    std::cout << "rightSideView: PASSED\n";
}

int main() {
    std::cout << "=== Tree Traversal Solution Tests ===\n";

    testPreorderTraversal();
    testInorderTraversal();
    testPostorderTraversal();
    testLevelOrder();
    testZigzagLevelOrder();
    testRightSideView();

    std::cout << "\nAll tests passed!\n";
    return 0;
}
