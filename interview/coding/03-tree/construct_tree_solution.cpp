/**
 * @file construct_tree_solution.cpp
 * @brief 构造二叉树问题 - 解答文件
 *
 * 包含 construct_tree.cpp 中所有题目的解答和详细解析。
 */

#include <iostream>
#include <vector>
#include <queue>
#include <stack>
#include <unordered_map>
#include <string>

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
 * 解法: 递归 + 哈希表优化
 * 思路:
 *   - 前序遍历的第一个元素是根节点
 *   - 在中序遍历中找到根节点，左边是左子树，右边是右子树
 *   - 根据左子树的大小，在前序遍历中确定左右子树的范围
 *
 * 时间复杂度: O(n)
 * 空间复杂度: O(n)
 */
TreeNode* buildFromPreInHelper(std::vector<int>& preorder, int preStart, int preEnd,
                                std::vector<int>& inorder, int inStart, int inEnd,
                                std::unordered_map<int, int>& inorderMap) {
    if (preStart > preEnd || inStart > inEnd) {
        return nullptr;
    }

    // 前序遍历的第一个元素是根节点
    int rootVal = preorder[preStart];
    TreeNode* root = new TreeNode(rootVal);

    // 在中序遍历中找到根节点位置
    int rootIdx = inorderMap[rootVal];
    int leftSize = rootIdx - inStart;

    // 递归构造左右子树
    root->left = buildFromPreInHelper(preorder, preStart + 1, preStart + leftSize,
                                       inorder, inStart, rootIdx - 1, inorderMap);
    root->right = buildFromPreInHelper(preorder, preStart + leftSize + 1, preEnd,
                                        inorder, rootIdx + 1, inEnd, inorderMap);

    return root;
}

TreeNode* buildTreeFromPreIn(std::vector<int>& preorder, std::vector<int>& inorder) {
    if (preorder.empty()) return nullptr;

    // 构建中序遍历的值->索引映射
    std::unordered_map<int, int> inorderMap;
    for (int i = 0; i < static_cast<int>(inorder.size()); ++i) {
        inorderMap[inorder[i]] = i;
    }

    return buildFromPreInHelper(preorder, 0, preorder.size() - 1,
                                 inorder, 0, inorder.size() - 1, inorderMap);
}

/**
 * 题目2: 从中序与后序遍历序列构造二叉树
 *
 * 解法: 递归 + 哈希表
 * 思路:
 *   - 后序遍历的最后一个元素是根节点
 *   - 在中序遍历中找到根节点，划分左右子树
 *   - 从后往前处理后序数组（先处理右子树，再处理左子树）
 *
 * 时间复杂度: O(n)
 * 空间复杂度: O(n)
 */
TreeNode* buildFromInPostHelper(std::vector<int>& inorder, int inStart, int inEnd,
                                 std::vector<int>& postorder, int postStart, int postEnd,
                                 std::unordered_map<int, int>& inorderMap) {
    if (inStart > inEnd || postStart > postEnd) {
        return nullptr;
    }

    // 后序遍历的最后一个元素是根节点
    int rootVal = postorder[postEnd];
    TreeNode* root = new TreeNode(rootVal);

    // 在中序遍历中找到根节点位置
    int rootIdx = inorderMap[rootVal];
    int leftSize = rootIdx - inStart;

    // 递归构造左右子树
    root->left = buildFromInPostHelper(inorder, inStart, rootIdx - 1,
                                        postorder, postStart, postStart + leftSize - 1, inorderMap);
    root->right = buildFromInPostHelper(inorder, rootIdx + 1, inEnd,
                                         postorder, postStart + leftSize, postEnd - 1, inorderMap);

    return root;
}

TreeNode* buildTreeFromInPost(std::vector<int>& inorder, std::vector<int>& postorder) {
    if (inorder.empty()) return nullptr;

    std::unordered_map<int, int> inorderMap;
    for (int i = 0; i < static_cast<int>(inorder.size()); ++i) {
        inorderMap[inorder[i]] = i;
    }

    return buildFromInPostHelper(inorder, 0, inorder.size() - 1,
                                  postorder, 0, postorder.size() - 1, inorderMap);
}

/**
 * 题目3: 根据前序和后序遍历构造二叉树
 *
 * 解法: 递归
 * 思路:
 *   - 前序: 根 左子树 右子树
 *   - 后序: 左子树 右子树 根
 *   - 前序的第二个元素是左子树的根
 *   - 在后序中找到左子树根的位置，确定左子树的大小
 *
 * 时间复杂度: O(n)
 * 空间复杂度: O(n)
 */
TreeNode* buildFromPrePostHelper(std::vector<int>& preorder, int preStart, int preEnd,
                                  std::vector<int>& postorder, int postStart, int postEnd,
                                  std::unordered_map<int, int>& postMap) {
    if (preStart > preEnd) return nullptr;
    if (preStart == preEnd) return new TreeNode(preorder[preStart]);

    TreeNode* root = new TreeNode(preorder[preStart]);

    // 前序的第二个元素是左子树的根
    int leftRootVal = preorder[preStart + 1];
    int leftRootIdx = postMap[leftRootVal];
    int leftSize = leftRootIdx - postStart + 1;

    root->left = buildFromPrePostHelper(preorder, preStart + 1, preStart + leftSize,
                                         postorder, postStart, leftRootIdx, postMap);
    root->right = buildFromPrePostHelper(preorder, preStart + leftSize + 1, preEnd,
                                          postorder, leftRootIdx + 1, postEnd - 1, postMap);

    return root;
}

TreeNode* buildTreeFromPrePost(std::vector<int>& preorder, std::vector<int>& postorder) {
    if (preorder.empty()) return nullptr;

    std::unordered_map<int, int> postMap;
    for (int i = 0; i < static_cast<int>(postorder.size()); ++i) {
        postMap[postorder[i]] = i;
    }

    return buildFromPrePostHelper(preorder, 0, preorder.size() - 1,
                                   postorder, 0, postorder.size() - 1, postMap);
}

/**
 * 题目4: 最大二叉树
 *
 * 解法1: 递归
 * 时间复杂度: O(n^2) 最坏情况
 * 空间复杂度: O(n)
 */
TreeNode* constructMaxHelper(std::vector<int>& nums, int left, int right) {
    if (left > right) return nullptr;

    // 找到最大值的位置
    int maxIdx = left;
    for (int i = left + 1; i <= right; ++i) {
        if (nums[i] > nums[maxIdx]) {
            maxIdx = i;
        }
    }

    TreeNode* root = new TreeNode(nums[maxIdx]);
    root->left = constructMaxHelper(nums, left, maxIdx - 1);
    root->right = constructMaxHelper(nums, maxIdx + 1, right);

    return root;
}

TreeNode* constructMaximumBinaryTree(std::vector<int>& nums) {
    return constructMaxHelper(nums, 0, nums.size() - 1);
}

/**
 * 解法2: 单调栈（O(n) 解法）
 * 思路: 维护一个递减栈
 */
TreeNode* constructMaximumBinaryTreeStack(std::vector<int>& nums) {
    std::stack<TreeNode*> stk;

    for (int num : nums) {
        TreeNode* curr = new TreeNode(num);

        // 弹出所有比当前值小的节点，作为当前节点的左子树
        while (!stk.empty() && stk.top()->val < num) {
            curr->left = stk.top();
            stk.pop();
        }

        // 如果栈不空，当前节点成为栈顶节点的右子树
        if (!stk.empty()) {
            stk.top()->right = curr;
        }

        stk.push(curr);
    }

    // 找到栈底元素（根节点）
    TreeNode* root = nullptr;
    while (!stk.empty()) {
        root = stk.top();
        stk.pop();
    }

    return root;
}

/**
 * 题目5: 有序链表转换二叉搜索树
 *
 * 解法1: 快慢指针找中点
 * 时间复杂度: O(n log n)
 * 空间复杂度: O(log n)
 */
TreeNode* sortedListToBST(ListNode* head) {
    if (!head) return nullptr;
    if (!head->next) return new TreeNode(head->val);

    // 快慢指针找中点
    ListNode* slow = head;
    ListNode* fast = head;
    ListNode* prev = nullptr;

    while (fast && fast->next) {
        prev = slow;
        slow = slow->next;
        fast = fast->next->next;
    }

    // 断开左半部分
    if (prev) prev->next = nullptr;

    // slow 是中点
    TreeNode* root = new TreeNode(slow->val);

    // 递归构建左右子树
    root->left = sortedListToBST(head == slow ? nullptr : head);
    root->right = sortedListToBST(slow->next);

    return root;
}

/**
 * 解法2: 中序遍历模拟（O(n) 解法）
 * 思路: 利用 BST 的中序遍历是有序的特性
 */
ListNode* listCurr;

TreeNode* buildBSTFromList(int left, int right) {
    if (left > right) return nullptr;

    int mid = left + (right - left) / 2;

    // 先构建左子树
    TreeNode* leftChild = buildBSTFromList(left, mid - 1);

    // 构建当前节点
    TreeNode* root = new TreeNode(listCurr->val);
    listCurr = listCurr->next;

    root->left = leftChild;

    // 构建右子树
    root->right = buildBSTFromList(mid + 1, right);

    return root;
}

TreeNode* sortedListToBSTOptimal(ListNode* head) {
    if (!head) return nullptr;

    // 计算链表长度
    int len = 0;
    ListNode* curr = head;
    while (curr) {
        ++len;
        curr = curr->next;
    }

    listCurr = head;
    return buildBSTFromList(0, len - 1);
}

/**
 * 题目6: 有序数组转换为二叉搜索树
 *
 * 解法: 分治法
 * 时间复杂度: O(n)
 * 空间复杂度: O(log n)
 */
TreeNode* buildBSTFromArray(std::vector<int>& nums, int left, int right) {
    if (left > right) return nullptr;

    int mid = left + (right - left) / 2;

    TreeNode* root = new TreeNode(nums[mid]);
    root->left = buildBSTFromArray(nums, left, mid - 1);
    root->right = buildBSTFromArray(nums, mid + 1, right);

    return root;
}

TreeNode* sortedArrayToBST(std::vector<int>& nums) {
    return buildBSTFromArray(nums, 0, nums.size() - 1);
}

/**
 * 题目7: 将二叉搜索树转化为排序的双向链表
 *
 * 解法: 中序遍历
 * 时间复杂度: O(n)
 * 空间复杂度: O(h)
 */
TreeNode* listHead = nullptr;
TreeNode* listTail = nullptr;

void treeToListHelper(TreeNode* root) {
    if (!root) return;

    // 处理左子树
    treeToListHelper(root->left);

    // 处理当前节点
    if (listTail) {
        listTail->right = root;
        root->left = listTail;
    } else {
        listHead = root;
    }
    listTail = root;

    // 处理右子树
    treeToListHelper(root->right);
}

TreeNode* treeToDoublyList(TreeNode* root) {
    if (!root) return nullptr;

    listHead = nullptr;
    listTail = nullptr;

    treeToListHelper(root);

    // 连接首尾形成循环
    listHead->left = listTail;
    listTail->right = listHead;

    return listHead;
}

/**
 * 题目8: 从先序遍历还原二叉树
 *
 * 解法: 迭代 + 栈
 * 时间复杂度: O(n)
 * 空间复杂度: O(h)
 */
TreeNode* recoverFromPreorder(std::string traversal) {
    if (traversal.empty()) return nullptr;

    std::stack<TreeNode*> stk;
    int i = 0;
    int n = traversal.size();

    while (i < n) {
        // 计算当前深度（破折号数量）
        int depth = 0;
        while (i < n && traversal[i] == '-') {
            ++depth;
            ++i;
        }

        // 解析节点值
        int val = 0;
        while (i < n && traversal[i] != '-') {
            val = val * 10 + (traversal[i] - '0');
            ++i;
        }

        TreeNode* node = new TreeNode(val);

        // 调整栈使其大小等于当前深度
        while (static_cast<int>(stk.size()) > depth) {
            stk.pop();
        }

        // 连接到父节点
        if (!stk.empty()) {
            if (!stk.top()->left) {
                stk.top()->left = node;
            } else {
                stk.top()->right = node;
            }
        }

        stk.push(node);
    }

    // 返回根节点
    while (stk.size() > 1) {
        stk.pop();
    }

    return stk.top();
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

void preorder(TreeNode* root, std::vector<int>& result) {
    if (!root) return;
    result.push_back(root->val);
    preorder(root->left, result);
    preorder(root->right, result);
}

void inorder(TreeNode* root, std::vector<int>& result) {
    if (!root) return;
    inorder(root->left, result);
    result.push_back(root->val);
    inorder(root->right, result);
}

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

    std::vector<int> preResult, inResult;
    preorder(root, preResult);
    inorder(root, inResult);
    assert(preResult == pre);
    assert(inResult == in);

    deleteTree(root);

    // 空数组
    std::vector<int> empty;
    assert(buildTreeFromPreIn(empty, empty) == nullptr);

    std::cout << "buildTreeFromPreIn: PASSED\n";
}

void testBuildTreeFromInPost() {
    std::vector<int> in = {9, 3, 15, 20, 7};
    std::vector<int> post = {9, 15, 7, 20, 3};
    TreeNode* root = buildTreeFromInPost(in, post);

    std::vector<int> inResult, postResult;
    inorder(root, inResult);
    postorder(root, postResult);
    assert(inResult == in);
    assert(postResult == post);

    deleteTree(root);

    std::cout << "buildTreeFromInPost: PASSED\n";
}

void testBuildTreeFromPrePost() {
    std::vector<int> pre = {1, 2, 4, 5, 3, 6, 7};
    std::vector<int> post = {4, 5, 2, 6, 7, 3, 1};
    TreeNode* root = buildTreeFromPrePost(pre, post);

    std::vector<int> preResult, postResult;
    preorder(root, preResult);
    postorder(root, postResult);
    assert(preResult == pre);
    assert(postResult == post);

    deleteTree(root);

    std::cout << "buildTreeFromPrePost: PASSED\n";
}

void testConstructMaximumBinaryTree() {
    std::vector<int> nums = {3, 2, 1, 6, 0, 5};
    TreeNode* root = constructMaximumBinaryTree(nums);

    assert(root != nullptr && root->val == 6);
    assert(root->left->val == 3);
    assert(root->right->val == 5);

    deleteTree(root);

    // 测试单调栈解法
    root = constructMaximumBinaryTreeStack(nums);
    assert(root != nullptr && root->val == 6);
    deleteTree(root);

    std::cout << "constructMaximumBinaryTree: PASSED\n";
}

void testSortedArrayToBST() {
    std::vector<int> nums = {-10, -3, 0, 5, 9};
    TreeNode* root = sortedArrayToBST(nums);

    std::vector<int> inResult;
    inorder(root, inResult);
    assert(inResult == nums);

    deleteTree(root);

    std::cout << "sortedArrayToBST: PASSED\n";
}

void testSortedListToBST() {
    ListNode* head = createList({-10, -3, 0, 5, 9});
    TreeNode* root = sortedListToBST(head);

    std::vector<int> inResult;
    inorder(root, inResult);
    assert(inResult == std::vector<int>({-10, -3, 0, 5, 9}));

    deleteTree(root);

    std::cout << "sortedListToBST: PASSED\n";
}

void testTreeToDoublyList() {
    TreeNode* root = createTree({4, 2, 5, 1, 3});
    TreeNode* head = treeToDoublyList(root);

    // 验证双向链表
    assert(head->val == 1);
    assert(head->right->val == 2);
    assert(head->left->val == 5);  // 循环连接

    // 注意：转换后不能用 deleteTree，因为结构已改变

    std::cout << "treeToDoublyList: PASSED\n";
}

void testRecoverFromPreorder() {
    std::string traversal = "1-2--3--4-5--6--7";
    TreeNode* root = recoverFromPreorder(traversal);

    assert(root->val == 1);
    assert(root->left->val == 2);
    assert(root->right->val == 5);
    assert(root->left->left->val == 3);
    assert(root->left->right->val == 4);

    deleteTree(root);

    std::cout << "recoverFromPreorder: PASSED\n";
}

int main() {
    std::cout << "=== Construct Tree Solution Tests ===\n";

    testBuildTreeFromPreIn();
    testBuildTreeFromInPost();
    testBuildTreeFromPrePost();
    testConstructMaximumBinaryTree();
    testSortedArrayToBST();
    testSortedListToBST();
    testTreeToDoublyList();
    testRecoverFromPreorder();

    std::cout << "\nAll tests passed!\n";
    return 0;
}
