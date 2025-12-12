/**
 * @file lca_solution.cpp
 * @brief 最近公共祖先问题 - 解答文件
 *
 * 包含 lca.cpp 中所有题目的解答和详细解析。
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
 * 解法: 递归后序遍历
 * 思路:
 *   - 从根节点开始递归
 *   - 如果当前节点是 p 或 q，返回当前节点
 *   - 递归搜索左右子树
 *   - 如果左右子树都返回非空，说明 p、q 分别在两侧，当前节点就是 LCA
 *   - 如果只有一侧非空，返回那一侧的结果
 *
 * 时间复杂度: O(n)
 * 空间复杂度: O(h)
 */
TreeNode* lowestCommonAncestor(TreeNode* root, TreeNode* p, TreeNode* q) {
    // 基本情况：空节点或找到目标节点
    if (!root || root == p || root == q) {
        return root;
    }

    // 在左右子树中查找
    TreeNode* left = lowestCommonAncestor(root->left, p, q);
    TreeNode* right = lowestCommonAncestor(root->right, p, q);

    // 如果左右子树都找到了，说明当前节点是 LCA
    if (left && right) {
        return root;
    }

    // 否则返回非空的那个
    return left ? left : right;
}

/**
 * 解法2: 使用父指针记录路径
 * 思路: 记录每个节点的父节点，然后从 p 向上走记录祖先，再从 q 向上找第一个公共祖先
 */
TreeNode* lowestCommonAncestorWithMap(TreeNode* root, TreeNode* p, TreeNode* q) {
    std::unordered_map<TreeNode*, TreeNode*> parent;
    std::queue<TreeNode*> queue;

    parent[root] = nullptr;
    queue.push(root);

    // BFS 构建父指针映射
    while (!queue.empty() && (parent.find(p) == parent.end() || parent.find(q) == parent.end())) {
        TreeNode* node = queue.front();
        queue.pop();

        if (node->left) {
            parent[node->left] = node;
            queue.push(node->left);
        }
        if (node->right) {
            parent[node->right] = node;
            queue.push(node->right);
        }
    }

    // 记录 p 的所有祖先
    std::unordered_set<TreeNode*> ancestors;
    while (p) {
        ancestors.insert(p);
        p = parent[p];
    }

    // 找 q 的第一个在 p 祖先集合中的节点
    while (q) {
        if (ancestors.count(q)) {
            return q;
        }
        q = parent[q];
    }

    return nullptr;
}

/**
 * 题目2: 二叉搜索树的最近公共祖先
 *
 * 解法1: 递归
 * 利用 BST 性质进行剪枝
 *
 * 时间复杂度: O(h)
 * 空间复杂度: O(h)
 */
TreeNode* lowestCommonAncestorBST(TreeNode* root, TreeNode* p, TreeNode* q) {
    if (!root) return nullptr;

    // 两个节点都在左子树
    if (p->val < root->val && q->val < root->val) {
        return lowestCommonAncestorBST(root->left, p, q);
    }

    // 两个节点都在右子树
    if (p->val > root->val && q->val > root->val) {
        return lowestCommonAncestorBST(root->right, p, q);
    }

    // 分叉点，当前节点就是 LCA
    return root;
}

/**
 * 解法2: 迭代
 * 时间复杂度: O(h)
 * 空间复杂度: O(1)
 */
TreeNode* lowestCommonAncestorBSTIterative(TreeNode* root, TreeNode* p, TreeNode* q) {
    while (root) {
        if (p->val < root->val && q->val < root->val) {
            root = root->left;
        } else if (p->val > root->val && q->val > root->val) {
            root = root->right;
        } else {
            return root;
        }
    }
    return nullptr;
}

/**
 * 题目3: 有父指针的二叉树的最近公共祖先
 *
 * 解法1: 类似链表交点问题
 * 思路: 两个指针分别从 p 和 q 出发，到达 null 后切换到另一个起点
 *       最终会在 LCA 处相遇
 *
 * 时间复杂度: O(h)
 * 空间复杂度: O(1)
 */
Node* lowestCommonAncestorWithParent(Node* p, Node* q) {
    Node* a = p;
    Node* b = q;

    while (a != b) {
        a = a ? a->parent : q;
        b = b ? b->parent : p;
    }

    return a;
}

/**
 * 解法2: 使用集合记录祖先
 * 时间复杂度: O(h)
 * 空间复杂度: O(h)
 */
Node* lowestCommonAncestorWithParentSet(Node* p, Node* q) {
    std::unordered_set<Node*> ancestors;

    // 记录 p 的所有祖先
    Node* curr = p;
    while (curr) {
        ancestors.insert(curr);
        curr = curr->parent;
    }

    // 找 q 的第一个在集合中的祖先
    curr = q;
    while (curr) {
        if (ancestors.count(curr)) {
            return curr;
        }
        curr = curr->parent;
    }

    return nullptr;
}

/**
 * 题目4: 二叉树的最近公共祖先 II
 *
 * 解法: 后序遍历 + 标记是否找到
 * 与题目1的区别：需要确保 p 和 q 都存在于树中
 *
 * 时间复杂度: O(n)
 * 空间复杂度: O(h)
 */
bool foundP = false, foundQ = false;

TreeNode* lowestCommonAncestorIIHelper(TreeNode* root, TreeNode* p, TreeNode* q) {
    if (!root) return nullptr;

    TreeNode* left = lowestCommonAncestorIIHelper(root->left, p, q);
    TreeNode* right = lowestCommonAncestorIIHelper(root->right, p, q);

    if (root == p) {
        foundP = true;
        return root;
    }
    if (root == q) {
        foundQ = true;
        return root;
    }

    if (left && right) return root;
    return left ? left : right;
}

TreeNode* lowestCommonAncestorII(TreeNode* root, TreeNode* p, TreeNode* q) {
    foundP = false;
    foundQ = false;

    TreeNode* result = lowestCommonAncestorIIHelper(root, p, q);

    // 只有当 p 和 q 都被找到时才返回结果
    if (foundP && foundQ) {
        return result;
    }
    return nullptr;
}

/**
 * 题目5: 二叉树的最近公共祖先 III（多个节点）
 *
 * 解法: 迭代应用两两 LCA
 * 思路: LCA(a, b, c) = LCA(LCA(a, b), c)
 *
 * 时间复杂度: O(n * k)，k 为节点数
 * 空间复杂度: O(h)
 */
TreeNode* lowestCommonAncestorMultiple(TreeNode* root, std::vector<TreeNode*>& nodes) {
    if (nodes.empty()) return nullptr;

    TreeNode* result = nodes[0];
    for (size_t i = 1; i < nodes.size(); ++i) {
        result = lowestCommonAncestor(root, result, nodes[i]);
        if (result == root) {
            break;  // 已经是根节点，不可能有更高的 LCA
        }
    }

    return result;
}

/**
 * 解法2: 使用集合优化
 * 思路: 递归时检查当前节点是否是目标节点之一
 */
TreeNode* lowestCommonAncestorMultipleSet(TreeNode* root, std::unordered_set<TreeNode*>& nodeSet) {
    if (!root || nodeSet.count(root)) {
        return root;
    }

    TreeNode* left = lowestCommonAncestorMultipleSet(root->left, nodeSet);
    TreeNode* right = lowestCommonAncestorMultipleSet(root->right, nodeSet);

    if (left && right) return root;
    return left ? left : right;
}

/**
 * 题目6: 二叉树的最深叶节点的最近公共祖先
 *
 * 解法: DFS 返回深度和 LCA
 * 思路:
 *   - 递归返回两个信息：子树的最大深度和对应的 LCA
 *   - 如果左右子树深度相同，当前节点是 LCA
 *   - 否则返回更深那边的 LCA
 *
 * 时间复杂度: O(n)
 * 空间复杂度: O(h)
 */
std::pair<int, TreeNode*> lcaDeepestLeavesHelper(TreeNode* root) {
    if (!root) {
        return {0, nullptr};
    }

    auto [leftDepth, leftLCA] = lcaDeepestLeavesHelper(root->left);
    auto [rightDepth, rightLCA] = lcaDeepestLeavesHelper(root->right);

    if (leftDepth > rightDepth) {
        return {leftDepth + 1, leftLCA};
    } else if (rightDepth > leftDepth) {
        return {rightDepth + 1, rightLCA};
    } else {
        // 左右深度相同，当前节点是 LCA
        return {leftDepth + 1, root};
    }
}

TreeNode* lcaDeepestLeaves(TreeNode* root) {
    return lcaDeepestLeavesHelper(root).second;
}

/**
 * 解法2: 两遍遍历
 * 第一遍找最大深度，第二遍找最深叶节点的 LCA
 */
int maxDepth(TreeNode* root) {
    if (!root) return 0;
    return 1 + std::max(maxDepth(root->left), maxDepth(root->right));
}

TreeNode* lcaAtDepth(TreeNode* root, int currentDepth, int targetDepth) {
    if (!root) return nullptr;

    // 到达目标深度（叶子层）
    if (currentDepth == targetDepth) {
        return root;
    }

    TreeNode* left = lcaAtDepth(root->left, currentDepth + 1, targetDepth);
    TreeNode* right = lcaAtDepth(root->right, currentDepth + 1, targetDepth);

    if (left && right) return root;
    return left ? left : right;
}

TreeNode* lcaDeepestLeavesTwoPass(TreeNode* root) {
    int depth = maxDepth(root);
    return lcaAtDepth(root, 1, depth);
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

TreeNode* findNode(TreeNode* root, int val) {
    if (!root) return nullptr;
    if (root->val == val) return root;

    TreeNode* left = findNode(root->left, val);
    if (left) return left;

    return findNode(root->right, val);
}

// 创建带父指针的树
Node* createTreeWithParent(const std::vector<int>& vals) {
    if (vals.empty() || vals[0] == -1) return nullptr;

    Node* root = new Node(vals[0]);
    std::queue<Node*> q;
    q.push(root);

    size_t i = 1;
    while (!q.empty() && i < vals.size()) {
        Node* node = q.front();
        q.pop();

        if (i < vals.size() && vals[i] != -1) {
            node->left = new Node(vals[i]);
            node->left->parent = node;
            q.push(node->left);
        }
        ++i;

        if (i < vals.size() && vals[i] != -1) {
            node->right = new Node(vals[i]);
            node->right->parent = node;
            q.push(node->right);
        }
        ++i;
    }

    return root;
}

void deleteTreeWithParent(Node* root) {
    if (!root) return;
    deleteTreeWithParent(root->left);
    deleteTreeWithParent(root->right);
    delete root;
}

Node* findNodeWithParent(Node* root, int val) {
    if (!root) return nullptr;
    if (root->val == val) return root;

    Node* left = findNodeWithParent(root->left, val);
    if (left) return left;

    return findNodeWithParent(root->right, val);
}


// ==================== 测试代码 ====================
#include <cassert>

void testLowestCommonAncestor() {
    TreeNode* root = createTree({3, 5, 1, 6, 2, 0, 8, -1, -1, 7, 4});
    TreeNode* p = findNode(root, 5);
    TreeNode* q = findNode(root, 1);
    TreeNode* lca = lowestCommonAncestor(root, p, q);
    assert(lca != nullptr && lca->val == 3);

    p = findNode(root, 5);
    q = findNode(root, 4);
    lca = lowestCommonAncestor(root, p, q);
    assert(lca != nullptr && lca->val == 5);

    p = findNode(root, 7);
    q = findNode(root, 4);
    lca = lowestCommonAncestor(root, p, q);
    assert(lca != nullptr && lca->val == 2);

    // 测试使用 map 的解法
    p = findNode(root, 5);
    q = findNode(root, 1);
    lca = lowestCommonAncestorWithMap(root, p, q);
    assert(lca != nullptr && lca->val == 3);

    deleteTree(root);

    std::cout << "lowestCommonAncestor: PASSED\n";
}

void testLowestCommonAncestorBST() {
    TreeNode* root = createTree({6, 2, 8, 0, 4, 7, 9, -1, -1, 3, 5});
    TreeNode* p = findNode(root, 2);
    TreeNode* q = findNode(root, 8);
    TreeNode* lca = lowestCommonAncestorBST(root, p, q);
    assert(lca != nullptr && lca->val == 6);

    lca = lowestCommonAncestorBSTIterative(root, p, q);
    assert(lca != nullptr && lca->val == 6);

    p = findNode(root, 2);
    q = findNode(root, 4);
    lca = lowestCommonAncestorBST(root, p, q);
    assert(lca != nullptr && lca->val == 2);

    deleteTree(root);

    std::cout << "lowestCommonAncestorBST: PASSED\n";
}

void testLowestCommonAncestorWithParent() {
    Node* root = createTreeWithParent({3, 5, 1, 6, 2, 0, 8, -1, -1, 7, 4});
    Node* p = findNodeWithParent(root, 5);
    Node* q = findNodeWithParent(root, 1);
    Node* lca = lowestCommonAncestorWithParent(p, q);
    assert(lca != nullptr && lca->val == 3);

    p = findNodeWithParent(root, 5);
    q = findNodeWithParent(root, 4);
    lca = lowestCommonAncestorWithParent(p, q);
    assert(lca != nullptr && lca->val == 5);

    // 测试使用 set 的解法
    p = findNodeWithParent(root, 7);
    q = findNodeWithParent(root, 4);
    lca = lowestCommonAncestorWithParentSet(p, q);
    assert(lca != nullptr && lca->val == 2);

    deleteTreeWithParent(root);

    std::cout << "lowestCommonAncestorWithParent: PASSED\n";
}

void testLowestCommonAncestorMultiple() {
    TreeNode* root = createTree({3, 5, 1, 6, 2, 0, 8, -1, -1, 7, 4});

    std::vector<TreeNode*> nodes = {findNode(root, 4), findNode(root, 7)};
    TreeNode* lca = lowestCommonAncestorMultiple(root, nodes);
    assert(lca != nullptr && lca->val == 2);

    nodes = {findNode(root, 1)};
    lca = lowestCommonAncestorMultiple(root, nodes);
    assert(lca != nullptr && lca->val == 1);

    nodes = {findNode(root, 6), findNode(root, 4), findNode(root, 7)};
    lca = lowestCommonAncestorMultiple(root, nodes);
    assert(lca != nullptr && lca->val == 5);

    deleteTree(root);

    std::cout << "lowestCommonAncestorMultiple: PASSED\n";
}

void testLcaDeepestLeaves() {
    TreeNode* root = createTree({3, 5, 1, 6, 2, -1, -1, -1, -1, 7, 4});
    TreeNode* result = lcaDeepestLeaves(root);
    assert(result != nullptr && result->val == 2);

    result = lcaDeepestLeavesTwoPass(root);
    assert(result != nullptr && result->val == 2);
    deleteTree(root);

    root = createTree({1, 2, 3, 4, -1, 5});
    result = lcaDeepestLeaves(root);
    assert(result != nullptr && result->val == 1);
    deleteTree(root);

    // 单节点树
    root = new TreeNode(1);
    result = lcaDeepestLeaves(root);
    assert(result != nullptr && result->val == 1);
    delete root;

    std::cout << "lcaDeepestLeaves: PASSED\n";
}

int main() {
    std::cout << "=== LCA Problems Solution Tests ===\n";

    testLowestCommonAncestor();
    testLowestCommonAncestorBST();
    testLowestCommonAncestorWithParent();
    testLowestCommonAncestorMultiple();
    testLcaDeepestLeaves();

    std::cout << "\nAll tests passed!\n";
    return 0;
}
