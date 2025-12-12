/**
 * @file serialize_tree.cpp
 * @brief 二叉树序列化问题 - 题目文件
 *
 * 序列化是将数据结构转换为可存储或传输的格式的过程。
 * 反序列化是从序列化的格式中恢复数据结构的过程。
 */

#include <iostream>
#include <vector>
#include <queue>
#include <string>
#include <sstream>

struct TreeNode {
    int val;
    TreeNode* left;
    TreeNode* right;
    TreeNode() : val(0), left(nullptr), right(nullptr) {}
    TreeNode(int x) : val(x), left(nullptr), right(nullptr) {}
    TreeNode(int x, TreeNode* l, TreeNode* r) : val(x), left(l), right(r) {}
};

/**
 * 题目1: 二叉树的序列化与反序列化
 *
 * 设计一个算法来实现二叉树的序列化与反序列化。
 * 这里不限定你的序列化/反序列化算法执行逻辑，
 * 你只需要保证一个二叉树可以被序列化为一个字符串并且将这个字符串反序列化为原始的树结构。
 *
 * 序列化示例:
 *       1
 *      / \
 *     2   3
 *        / \
 *       4   5
 *   可序列化为: "1,2,3,null,null,4,5" (层序)
 *   或者: "1,2,null,null,3,4,null,null,5,null,null" (前序)
 *
 * 提示:
 *   - 可以使用任何序列化格式，只要能正确还原即可
 *   - 常见方案：BFS 层序、DFS 前序/后序
 */
class Codec {
public:
    /**
     * 将二叉树序列化为字符串
     * @param root 根节点
     * @return 序列化字符串
     */
    std::string serialize(TreeNode* root) {
        // TODO: 实现你的解法
        return "";
    }

    /**
     * 将字符串反序列化为二叉树
     * @param data 序列化字符串
     * @return 根节点
     */
    TreeNode* deserialize(std::string data) {
        // TODO: 实现你的解法
        return nullptr;
    }
};

/**
 * 题目2: 二叉搜索树的序列化与反序列化
 *
 * 设计一个算法来序列化和反序列化二叉搜索树。
 * 对序列化/反序列化算法的工作方式没有限制。
 * 您只需确保二叉搜索树可以序列化为字符串，
 * 并且可以将该字符串反序列化为最初的二叉搜索树。
 *
 * 编码的字符串应尽可能紧凑。
 *
 * 提示：利用 BST 的性质，可以不存储 null 节点
 *       因为 BST 的前序遍历可以唯一确定树的结构
 */
class CodecBST {
public:
    /**
     * 将 BST 序列化为字符串
     * @param root 根节点
     * @return 序列化字符串
     */
    std::string serialize(TreeNode* root) {
        // TODO: 实现你的解法
        return "";
    }

    /**
     * 将字符串反序列化为 BST
     * @param data 序列化字符串
     * @return 根节点
     */
    TreeNode* deserialize(std::string data) {
        // TODO: 实现你的解法
        return nullptr;
    }
};

/**
 * 题目3: 寻找重复的子树
 *
 * 给你一棵二叉树的根节点 root，返回所有重复的子树。
 * 对于同一类的重复子树，你只需要返回其中任意一棵的根结点即可。
 *
 * 如果两棵树具有相同的结构和相同的结点值，则认为它们是重复的。
 *
 * 示例:
 *       1
 *      / \
 *     2   3
 *    /   / \
 *   4   2   4
 *      /
 *     4
 *   输出: [[2,4],[4]]
 *   解释: 子树 [4] 出现了 3 次，子树 [2,4] 出现了 2 次
 *
 * 思路提示：
 *   - 序列化每个子树
 *   - 用哈希表记录每种序列化结果出现的次数
 *
 * @param root 根节点
 * @return 重复子树的根节点列表
 */
std::vector<TreeNode*> findDuplicateSubtrees(TreeNode* root) {
    // TODO: 实现你的解法
    return {};
}

/**
 * 题目4: 验证二叉树的前序序列化
 *
 * 序列化二叉树的一种方法是使用前序遍历。
 * 当我们遇到一个非空节点时，我们可以记录下这个节点的值。
 * 如果它是一个空节点，我们可以使用一个标记值记录，例如 #。
 *
 *      _9_
 *     /   \
 *    3     2
 *   / \   / \
 *  4   1  #  6
 * / \ / \   / \
 * # # # #   # #
 *
 * 例如，上面的二叉树可以被序列化为字符串 "9,3,4,#,#,1,#,#,2,#,6,#,#"
 *
 * 给定一串以逗号分隔的序列，验证它是否是正确的二叉树的前序序列化。
 * 不要重建树，而是利用特性来验证。
 *
 * 提示：
 *   - 可以使用入度出度的方法
 *   - 或者使用槽位的概念
 *
 * @param preorder 前序序列化字符串
 * @return 是否有效
 */
bool isValidSerialization(std::string preorder) {
    // TODO: 实现你的解法
    return false;
}

/**
 * 题目5: 从字符串生成二叉树
 *
 * 你需要从一个包含括号和整数的字符串构建一棵二叉树。
 *
 * 输入字符串只包含 '(', ')', '-' 和 '0' ~ '9'。
 *
 * 一对括号内的整数值表示子节点：
 *   - 如果存在左子节点，则第一对括号表示左子节点
 *   - 如果存在右子节点，则第二对括号表示右子节点
 *
 * 示例:
 *   输入: "4(2(3)(1))(6(5))"
 *   输出:
 *       4
 *      / \
 *     2   6
 *    / \ /
 *   3  1 5
 *
 *   输入: "4(2(3)(1))(6(5)(7))"
 *   输出:
 *       4
 *      / \
 *     2   6
 *    / \ / \
 *   3  1 5  7
 *
 * @param s 输入字符串
 * @return 根节点
 */
TreeNode* str2tree(std::string s) {
    // TODO: 实现你的解法
    return nullptr;
}

/**
 * 题目6: 根据二叉树创建字符串
 *
 * 给你二叉树的根节点 root，请你采用前序遍历的方式，
 * 将二叉树转化为一个由括号和整数组成的字符串，返回构造出的字符串。
 *
 * 空节点使用一对空括号对 "()" 表示，转化后需要省略所有不影响字符串与原始二叉树之间的一对一映射关系的空括号对。
 *
 * 示例:
 *       1
 *      / \
 *     2   3
 *    /
 *   4
 *   输出: "1(2(4))(3)"
 *   解释: 原本为 "1(2(4)())(3()())"，省略不必要的空括号对后得到 "1(2(4))(3)"
 *
 *       1
 *      / \
 *     2   3
 *      \
 *       4
 *   输出: "1(2()(4))(3)"
 *   解释: 类似第一个示例，但是无法省略第一对括号来创建二叉树与字符串的一一映射，
 *         如果省略将得到 "1(2(4))(3)"，这与另一棵二叉树结构不同
 *
 * @param root 根节点
 * @return 括号字符串
 */
std::string tree2str(TreeNode* root) {
    // TODO: 实现你的解法
    return "";
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

// 比较两棵树是否相同
bool isSameTree(TreeNode* p, TreeNode* q) {
    if (!p && !q) return true;
    if (!p || !q) return false;
    return p->val == q->val &&
           isSameTree(p->left, q->left) &&
           isSameTree(p->right, q->right);
}


// ==================== 测试代码 ====================
#include <cassert>

void testCodec() {
    Codec codec;

    // 测试普通树
    TreeNode* root = createTree({1, 2, 3, -1, -1, 4, 5});
    std::string serialized = codec.serialize(root);
    TreeNode* deserialized = codec.deserialize(serialized);
    assert(isSameTree(root, deserialized));
    deleteTree(root);
    deleteTree(deserialized);

    // 测试空树
    serialized = codec.serialize(nullptr);
    deserialized = codec.deserialize(serialized);
    assert(deserialized == nullptr);

    // 测试单节点
    root = new TreeNode(1);
    serialized = codec.serialize(root);
    deserialized = codec.deserialize(serialized);
    assert(isSameTree(root, deserialized));
    delete root;
    deleteTree(deserialized);

    std::cout << "Codec: PASSED\n";
}

void testCodecBST() {
    CodecBST codec;

    // 构建 BST
    TreeNode* root = new TreeNode(5);
    root->left = new TreeNode(3);
    root->right = new TreeNode(7);
    root->left->left = new TreeNode(2);
    root->left->right = new TreeNode(4);

    std::string serialized = codec.serialize(root);
    TreeNode* deserialized = codec.deserialize(serialized);
    assert(isSameTree(root, deserialized));
    deleteTree(root);
    deleteTree(deserialized);

    std::cout << "CodecBST: PASSED\n";
}

void testFindDuplicateSubtrees() {
    //     1
    //    / \
    //   2   3
    //  /   / \
    // 4   2   4
    //    /
    //   4
    TreeNode* root = new TreeNode(1);
    root->left = new TreeNode(2);
    root->right = new TreeNode(3);
    root->left->left = new TreeNode(4);
    root->right->left = new TreeNode(2);
    root->right->right = new TreeNode(4);
    root->right->left->left = new TreeNode(4);

    auto result = findDuplicateSubtrees(root);
    // 应该找到 [4] 和 [2,4] 两种重复子树
    assert(result.size() == 2);

    deleteTree(root);

    std::cout << "findDuplicateSubtrees: PASSED\n";
}

void testIsValidSerialization() {
    assert(isValidSerialization("9,3,4,#,#,1,#,#,2,#,6,#,#") == true);
    assert(isValidSerialization("1,#") == false);
    assert(isValidSerialization("9,#,#,1") == false);
    assert(isValidSerialization("#") == true);

    std::cout << "isValidSerialization: PASSED\n";
}

void testTree2str() {
    TreeNode* root = createTree({1, 2, 3, 4});
    std::string result = tree2str(root);
    assert(result == "1(2(4))(3)");
    deleteTree(root);

    root = createTree({1, 2, 3, -1, 4});
    result = tree2str(root);
    assert(result == "1(2()(4))(3)");
    deleteTree(root);

    std::cout << "tree2str: PASSED\n";
}

int main() {
    std::cout << "=== Serialize Tree Tests ===\n";

    // 取消注释以运行测试（实现解法后）
    // testCodec();
    // testCodecBST();
    // testFindDuplicateSubtrees();
    // testIsValidSerialization();
    // testTree2str();

    std::cout << "All tests passed!\n";
    return 0;
}
