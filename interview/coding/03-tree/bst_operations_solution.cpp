/**
 * @file bst_operations_solution.cpp
 * @brief 二叉搜索树操作 - 解答文件
 *
 * 包含 bst_operations.cpp 中所有题目的解答和详细解析。
 */

#include <iostream>
#include <vector>
#include <queue>
#include <stack>
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
 * 解法1: 递归（范围验证）
 * 思路: 每个节点的值必须在 (min, max) 范围内
 * 时间复杂度: O(n)
 * 空间复杂度: O(h)
 */
bool isValidBSTHelper(TreeNode* root, long long minVal, long long maxVal) {
    if (!root) return true;

    if (root->val <= minVal || root->val >= maxVal) {
        return false;
    }

    return isValidBSTHelper(root->left, minVal, root->val) &&
           isValidBSTHelper(root->right, root->val, maxVal);
}

bool isValidBST(TreeNode* root) {
    return isValidBSTHelper(root, LLONG_MIN, LLONG_MAX);
}

/**
 * 解法2: 中序遍历（迭代）
 * 思路: BST 的中序遍历是严格递增的
 */
bool isValidBSTInorder(TreeNode* root) {
    std::stack<TreeNode*> stk;
    TreeNode* curr = root;
    TreeNode* prev = nullptr;

    while (curr || !stk.empty()) {
        while (curr) {
            stk.push(curr);
            curr = curr->left;
        }

        curr = stk.top();
        stk.pop();

        // 检查是否递增
        if (prev && curr->val <= prev->val) {
            return false;
        }
        prev = curr;

        curr = curr->right;
    }

    return true;
}

/**
 * 题目2: 二叉搜索树中的搜索
 *
 * 解法1: 递归
 * 时间复杂度: O(h)
 * 空间复杂度: O(h)
 */
TreeNode* searchBST(TreeNode* root, int val) {
    if (!root) return nullptr;

    if (val == root->val) {
        return root;
    } else if (val < root->val) {
        return searchBST(root->left, val);
    } else {
        return searchBST(root->right, val);
    }
}

/**
 * 解法2: 迭代
 * 时间复杂度: O(h)
 * 空间复杂度: O(1)
 */
TreeNode* searchBSTIterative(TreeNode* root, int val) {
    while (root) {
        if (val == root->val) {
            return root;
        } else if (val < root->val) {
            root = root->left;
        } else {
            root = root->right;
        }
    }
    return nullptr;
}

/**
 * 题目3: 二叉搜索树中的插入操作
 *
 * 解法1: 递归
 * 时间复杂度: O(h)
 * 空间复杂度: O(h)
 */
TreeNode* insertIntoBST(TreeNode* root, int val) {
    if (!root) {
        return new TreeNode(val);
    }

    if (val < root->val) {
        root->left = insertIntoBST(root->left, val);
    } else {
        root->right = insertIntoBST(root->right, val);
    }

    return root;
}

/**
 * 解法2: 迭代
 * 时间复杂度: O(h)
 * 空间复杂度: O(1)
 */
TreeNode* insertIntoBSTIterative(TreeNode* root, int val) {
    TreeNode* newNode = new TreeNode(val);

    if (!root) return newNode;

    TreeNode* curr = root;
    TreeNode* parent = nullptr;

    while (curr) {
        parent = curr;
        if (val < curr->val) {
            curr = curr->left;
        } else {
            curr = curr->right;
        }
    }

    if (val < parent->val) {
        parent->left = newNode;
    } else {
        parent->right = newNode;
    }

    return root;
}

/**
 * 题目4: 删除二叉搜索树中的节点
 *
 * 解法: 递归
 * 删除分三种情况:
 *   1. 叶子节点: 直接删除
 *   2. 只有一个子树: 用子树替代
 *   3. 有两个子树: 用后继节点（右子树最小）替代
 *
 * 时间复杂度: O(h)
 * 空间复杂度: O(h)
 */
// 找到最小节点（最左节点）
TreeNode* findMin(TreeNode* root) {
    while (root->left) {
        root = root->left;
    }
    return root;
}

TreeNode* deleteNode(TreeNode* root, int key) {
    if (!root) return nullptr;

    if (key < root->val) {
        // 在左子树中删除
        root->left = deleteNode(root->left, key);
    } else if (key > root->val) {
        // 在右子树中删除
        root->right = deleteNode(root->right, key);
    } else {
        // 找到要删除的节点

        // 情况1: 叶子节点
        if (!root->left && !root->right) {
            delete root;
            return nullptr;
        }

        // 情况2: 只有一个子树
        if (!root->left) {
            TreeNode* rightChild = root->right;
            delete root;
            return rightChild;
        }
        if (!root->right) {
            TreeNode* leftChild = root->left;
            delete root;
            return leftChild;
        }

        // 情况3: 有两个子树
        // 找到右子树的最小节点（后继）
        TreeNode* successor = findMin(root->right);
        // 用后继的值替换当前节点
        root->val = successor->val;
        // 删除后继节点
        root->right = deleteNode(root->right, successor->val);
    }

    return root;
}

/**
 * 删除节点的迭代解法
 */
TreeNode* deleteNodeIterative(TreeNode* root, int key) {
    TreeNode* parent = nullptr;
    TreeNode* curr = root;

    // 找到要删除的节点
    while (curr && curr->val != key) {
        parent = curr;
        if (key < curr->val) {
            curr = curr->left;
        } else {
            curr = curr->right;
        }
    }

    if (!curr) return root;  // 未找到

    // 删除操作的辅助函数
    auto deleteAndReplace = [&](TreeNode* replacement) {
        if (!parent) {
            root = replacement;
        } else if (parent->left == curr) {
            parent->left = replacement;
        } else {
            parent->right = replacement;
        }
    };

    // 情况1和2: 最多一个子树
    if (!curr->left) {
        deleteAndReplace(curr->right);
        delete curr;
    } else if (!curr->right) {
        deleteAndReplace(curr->left);
        delete curr;
    } else {
        // 情况3: 两个子树，找后继
        TreeNode* succParent = curr;
        TreeNode* succ = curr->right;
        while (succ->left) {
            succParent = succ;
            succ = succ->left;
        }

        curr->val = succ->val;

        if (succParent->left == succ) {
            succParent->left = succ->right;
        } else {
            succParent->right = succ->right;
        }
        delete succ;
    }

    return root;
}

/**
 * 题目5: 二叉搜索树的最近公共祖先
 *
 * 解法1: 递归
 * 利用 BST 性质：
 *   - 如果 p, q 都小于 root，LCA 在左子树
 *   - 如果 p, q 都大于 root，LCA 在右子树
 *   - 否则 root 就是 LCA
 *
 * 时间复杂度: O(h)
 * 空间复杂度: O(h)
 */
TreeNode* lowestCommonAncestorBST(TreeNode* root, TreeNode* p, TreeNode* q) {
    if (!root) return nullptr;

    if (p->val < root->val && q->val < root->val) {
        return lowestCommonAncestorBST(root->left, p, q);
    }
    if (p->val > root->val && q->val > root->val) {
        return lowestCommonAncestorBST(root->right, p, q);
    }

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
 * 题目6: 二叉搜索树中第 K 小的元素
 *
 * 解法1: 中序遍历（递归）
 * 思路: BST 中序遍历是有序的，找第 k 个
 * 时间复杂度: O(n)
 * 空间复杂度: O(h)
 */
void kthSmallestHelper(TreeNode* root, int& k, int& result) {
    if (!root || k <= 0) return;

    kthSmallestHelper(root->left, k, result);

    --k;
    if (k == 0) {
        result = root->val;
        return;
    }

    kthSmallestHelper(root->right, k, result);
}

int kthSmallest(TreeNode* root, int k) {
    int result = 0;
    kthSmallestHelper(root, k, result);
    return result;
}

/**
 * 解法2: 中序遍历（迭代）
 */
int kthSmallestIterative(TreeNode* root, int k) {
    std::stack<TreeNode*> stk;
    TreeNode* curr = root;

    while (curr || !stk.empty()) {
        while (curr) {
            stk.push(curr);
            curr = curr->left;
        }

        curr = stk.top();
        stk.pop();

        --k;
        if (k == 0) {
            return curr->val;
        }

        curr = curr->right;
    }

    return -1;  // 不会到达这里
}

/**
 * 题目7: 将有序数组转换为二叉搜索树
 *
 * 解法: 分治法（递归）
 * 思路: 取中间元素作为根，递归构建左右子树
 * 时间复杂度: O(n)
 * 空间复杂度: O(log n) 递归栈
 */
TreeNode* sortedArrayToBSTHelper(std::vector<int>& nums, int left, int right) {
    if (left > right) return nullptr;

    // 取中间元素作为根（偏左）
    int mid = left + (right - left) / 2;

    TreeNode* root = new TreeNode(nums[mid]);
    root->left = sortedArrayToBSTHelper(nums, left, mid - 1);
    root->right = sortedArrayToBSTHelper(nums, mid + 1, right);

    return root;
}

TreeNode* sortedArrayToBST(std::vector<int>& nums) {
    return sortedArrayToBSTHelper(nums, 0, static_cast<int>(nums.size()) - 1);
}

/**
 * 题目8: 二叉搜索树的范围和
 *
 * 解法1: DFS 递归（利用 BST 剪枝）
 * 时间复杂度: O(n)
 * 空间复杂度: O(h)
 */
int rangeSumBST(TreeNode* root, int low, int high) {
    if (!root) return 0;

    // 利用 BST 性质剪枝
    if (root->val < low) {
        // 当前节点和左子树都小于 low，只需要搜索右子树
        return rangeSumBST(root->right, low, high);
    }
    if (root->val > high) {
        // 当前节点和右子树都大于 high，只需要搜索左子树
        return rangeSumBST(root->left, low, high);
    }

    // root->val 在 [low, high] 范围内
    return root->val +
           rangeSumBST(root->left, low, high) +
           rangeSumBST(root->right, low, high);
}

/**
 * 解法2: 迭代（BFS）
 */
int rangeSumBSTIterative(TreeNode* root, int low, int high) {
    if (!root) return 0;

    int sum = 0;
    std::queue<TreeNode*> q;
    q.push(root);

    while (!q.empty()) {
        TreeNode* node = q.front();
        q.pop();

        if (!node) continue;

        if (node->val >= low && node->val <= high) {
            sum += node->val;
        }

        // 剪枝：只在需要时加入子节点
        if (node->val > low) {
            q.push(node->left);
        }
        if (node->val < high) {
            q.push(node->right);
        }
    }

    return sum;
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
    assert(isValidBSTInorder(root) == true);
    deleteTree(root);

    root = createTree({5, 1, 4, -1, -1, 3, 6});
    assert(isValidBST(root) == false);
    assert(isValidBSTInorder(root) == false);
    deleteTree(root);

    // 边界情况：单节点
    root = new TreeNode(1);
    assert(isValidBST(root) == true);
    delete root;

    std::cout << "isValidBST: PASSED\n";
}

void testSearchBST() {
    TreeNode* root = createTree({4, 2, 7, 1, 3});
    TreeNode* result = searchBST(root, 2);
    assert(result != nullptr && result->val == 2);

    result = searchBSTIterative(root, 2);
    assert(result != nullptr && result->val == 2);

    result = searchBST(root, 5);
    assert(result == nullptr);

    result = searchBSTIterative(root, 5);
    assert(result == nullptr);

    deleteTree(root);

    std::cout << "searchBST: PASSED\n";
}

void testInsertIntoBST() {
    TreeNode* root = createTree({4, 2, 7, 1, 3});
    root = insertIntoBST(root, 5);

    std::vector<int> inorderResult;
    inorder(root, inorderResult);
    assert(inorderResult == std::vector<int>({1, 2, 3, 4, 5, 7}));
    deleteTree(root);

    // 测试迭代版本
    root = createTree({4, 2, 7, 1, 3});
    root = insertIntoBSTIterative(root, 5);
    inorderResult.clear();
    inorder(root, inorderResult);
    assert(inorderResult == std::vector<int>({1, 2, 3, 4, 5, 7}));
    deleteTree(root);

    // 插入到空树
    root = insertIntoBST(nullptr, 5);
    assert(root != nullptr && root->val == 5);
    delete root;

    std::cout << "insertIntoBST: PASSED\n";
}

void testDeleteNode() {
    // 删除有两个子树的节点
    TreeNode* root = createTree({5, 3, 6, 2, 4, -1, 7});
    root = deleteNode(root, 3);
    std::vector<int> inorderResult;
    inorder(root, inorderResult);
    assert(inorderResult == std::vector<int>({2, 4, 5, 6, 7}));
    deleteTree(root);

    // 删除叶子节点
    root = createTree({5, 3, 6, 2, 4, -1, 7});
    root = deleteNode(root, 2);
    inorderResult.clear();
    inorder(root, inorderResult);
    assert(inorderResult == std::vector<int>({3, 4, 5, 6, 7}));
    deleteTree(root);

    // 删除根节点
    root = createTree({5, 3, 6, 2, 4, -1, 7});
    root = deleteNode(root, 5);
    inorderResult.clear();
    inorder(root, inorderResult);
    assert(inorderResult == std::vector<int>({2, 3, 4, 6, 7}));
    deleteTree(root);

    std::cout << "deleteNode: PASSED\n";
}

void testLowestCommonAncestorBST() {
    TreeNode* root = createTree({6, 2, 8, 0, 4, 7, 9, -1, -1, 3, 5});
    TreeNode* p = root->left;        // 2
    TreeNode* q = root->right;       // 8
    TreeNode* lca = lowestCommonAncestorBST(root, p, q);
    assert(lca == root);  // 6

    lca = lowestCommonAncestorBSTIterative(root, p, q);
    assert(lca == root);

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

    assert(kthSmallestIterative(root, 1) == 1);
    assert(kthSmallestIterative(root, 2) == 2);
    assert(kthSmallestIterative(root, 3) == 3);

    deleteTree(root);

    root = createTree({5, 3, 6, 2, 4, -1, -1, 1});
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

    // 空数组
    nums = {};
    root = sortedArrayToBST(nums);
    assert(root == nullptr);

    // 单元素
    nums = {1};
    root = sortedArrayToBST(nums);
    assert(root != nullptr && root->val == 1);
    delete root;

    std::cout << "sortedArrayToBST: PASSED\n";
}

void testRangeSumBST() {
    TreeNode* root = createTree({10, 5, 15, 3, 7, -1, 18});
    assert(rangeSumBST(root, 7, 15) == 32);
    assert(rangeSumBSTIterative(root, 7, 15) == 32);
    deleteTree(root);

    root = createTree({10, 5, 15, 3, 7, 13, 18, 1, -1, 6});
    assert(rangeSumBST(root, 6, 10) == 23);  // 6 + 7 + 10
    deleteTree(root);

    std::cout << "rangeSumBST: PASSED\n";
}

int main() {
    std::cout << "=== BST Operations Solution Tests ===\n";

    testIsValidBST();
    testSearchBST();
    testInsertIntoBST();
    testDeleteNode();
    testLowestCommonAncestorBST();
    testKthSmallest();
    testSortedArrayToBST();
    testRangeSumBST();

    std::cout << "\nAll tests passed!\n";
    return 0;
}
