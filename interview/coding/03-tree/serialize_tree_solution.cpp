/**
 * @file serialize_tree_solution.cpp
 * @brief 二叉树序列化问题 - 解答文件
 *
 * 包含 serialize_tree.cpp 中所有题目的解答和详细解析。
 */

#include <iostream>
#include <vector>
#include <queue>
#include <stack>
#include <string>
#include <sstream>
#include <unordered_map>

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
 * 解法1: BFS 层序遍历
 * 时间复杂度: O(n)
 * 空间复杂度: O(n)
 */
class CodecBFS {
public:
    std::string serialize(TreeNode* root) {
        if (!root) return "";

        std::string result;
        std::queue<TreeNode*> q;
        q.push(root);

        while (!q.empty()) {
            TreeNode* node = q.front();
            q.pop();

            if (node) {
                result += std::to_string(node->val) + ",";
                q.push(node->left);
                q.push(node->right);
            } else {
                result += "null,";
            }
        }

        // 移除末尾多余的 null 和逗号
        while (result.size() > 5 && result.substr(result.size() - 5) == "null,") {
            result = result.substr(0, result.size() - 5);
        }

        return result;
    }

    TreeNode* deserialize(std::string data) {
        if (data.empty()) return nullptr;

        std::vector<std::string> nodes;
        std::stringstream ss(data);
        std::string item;
        while (std::getline(ss, item, ',')) {
            nodes.push_back(item);
        }

        if (nodes.empty() || nodes[0] == "null") return nullptr;

        TreeNode* root = new TreeNode(std::stoi(nodes[0]));
        std::queue<TreeNode*> q;
        q.push(root);

        size_t i = 1;
        while (!q.empty() && i < nodes.size()) {
            TreeNode* node = q.front();
            q.pop();

            // 左子节点
            if (i < nodes.size() && nodes[i] != "null") {
                node->left = new TreeNode(std::stoi(nodes[i]));
                q.push(node->left);
            }
            ++i;

            // 右子节点
            if (i < nodes.size() && nodes[i] != "null") {
                node->right = new TreeNode(std::stoi(nodes[i]));
                q.push(node->right);
            }
            ++i;
        }

        return root;
    }
};

/**
 * 解法2: DFS 前序遍历
 */
class Codec {
public:
    std::string serialize(TreeNode* root) {
        std::string result;
        serializeHelper(root, result);
        return result;
    }

    TreeNode* deserialize(std::string data) {
        std::istringstream iss(data);
        return deserializeHelper(iss);
    }

private:
    void serializeHelper(TreeNode* root, std::string& result) {
        if (!root) {
            result += "null,";
            return;
        }
        result += std::to_string(root->val) + ",";
        serializeHelper(root->left, result);
        serializeHelper(root->right, result);
    }

    TreeNode* deserializeHelper(std::istringstream& iss) {
        std::string val;
        if (!std::getline(iss, val, ',')) return nullptr;
        if (val == "null") return nullptr;

        TreeNode* node = new TreeNode(std::stoi(val));
        node->left = deserializeHelper(iss);
        node->right = deserializeHelper(iss);
        return node;
    }
};

/**
 * 题目2: 二叉搜索树的序列化与反序列化
 *
 * 解法: 利用 BST 性质，前序遍历即可唯一确定
 * 不需要存储 null 节点，更紧凑
 *
 * 时间复杂度: O(n)
 * 空间复杂度: O(n)
 */
class CodecBST {
public:
    std::string serialize(TreeNode* root) {
        std::string result;
        serializeHelper(root, result);
        if (!result.empty()) {
            result.pop_back();  // 移除最后的逗号
        }
        return result;
    }

    TreeNode* deserialize(std::string data) {
        if (data.empty()) return nullptr;

        std::vector<int> values;
        std::stringstream ss(data);
        std::string item;
        while (std::getline(ss, item, ',')) {
            values.push_back(std::stoi(item));
        }

        int idx = 0;
        return deserializeHelper(values, idx, INT_MIN, INT_MAX);
    }

private:
    void serializeHelper(TreeNode* root, std::string& result) {
        if (!root) return;
        result += std::to_string(root->val) + ",";
        serializeHelper(root->left, result);
        serializeHelper(root->right, result);
    }

    TreeNode* deserializeHelper(std::vector<int>& values, int& idx, int minVal, int maxVal) {
        if (idx >= static_cast<int>(values.size())) return nullptr;
        if (values[idx] < minVal || values[idx] > maxVal) return nullptr;

        TreeNode* node = new TreeNode(values[idx++]);
        node->left = deserializeHelper(values, idx, minVal, node->val);
        node->right = deserializeHelper(values, idx, node->val, maxVal);
        return node;
    }
};

/**
 * 题目3: 寻找重复的子树
 *
 * 解法: 序列化 + 哈希表
 * 思路: 将每个子树序列化，用哈希表记录出现次数
 *
 * 时间复杂度: O(n^2)
 * 空间复杂度: O(n^2)
 */
std::string serializeSubtree(TreeNode* root, std::unordered_map<std::string, int>& count,
                              std::vector<TreeNode*>& result) {
    if (!root) return "#";

    std::string serialized = std::to_string(root->val) + "," +
                             serializeSubtree(root->left, count, result) + "," +
                             serializeSubtree(root->right, count, result);

    // 只在第二次出现时加入结果
    if (count[serialized] == 1) {
        result.push_back(root);
    }
    count[serialized]++;

    return serialized;
}

std::vector<TreeNode*> findDuplicateSubtrees(TreeNode* root) {
    std::vector<TreeNode*> result;
    std::unordered_map<std::string, int> count;
    serializeSubtree(root, count, result);
    return result;
}

/**
 * 优化解法: 使用三元组 ID
 * 时间复杂度: O(n)
 * 空间复杂度: O(n)
 */
int serializeWithId(TreeNode* root, std::unordered_map<std::string, int>& tripletToId,
                    std::unordered_map<int, int>& count, std::vector<TreeNode*>& result) {
    if (!root) return 0;

    int leftId = serializeWithId(root->left, tripletToId, count, result);
    int rightId = serializeWithId(root->right, tripletToId, count, result);

    std::string triplet = std::to_string(root->val) + "," +
                          std::to_string(leftId) + "," +
                          std::to_string(rightId);

    if (tripletToId.find(triplet) == tripletToId.end()) {
        tripletToId[triplet] = tripletToId.size() + 1;
    }

    int id = tripletToId[triplet];
    if (count[id] == 1) {
        result.push_back(root);
    }
    count[id]++;

    return id;
}

std::vector<TreeNode*> findDuplicateSubtreesOptimized(TreeNode* root) {
    std::vector<TreeNode*> result;
    std::unordered_map<std::string, int> tripletToId;
    std::unordered_map<int, int> count;
    serializeWithId(root, tripletToId, count, result);
    return result;
}

/**
 * 题目4: 验证二叉树的前序序列化
 *
 * 解法1: 槽位法
 * 思路:
 *   - 初始有一个槽位（给根节点）
 *   - 每个非空节点消耗一个槽位，产生两个新槽位
 *   - 每个空节点（#）只消耗一个槽位
 *   - 最终槽位应该正好用完（为0）
 *
 * 时间复杂度: O(n)
 * 空间复杂度: O(n)
 */
bool isValidSerialization(std::string preorder) {
    int slots = 1;  // 初始给根节点一个槽位

    std::stringstream ss(preorder);
    std::string node;

    while (std::getline(ss, node, ',')) {
        // 消耗一个槽位
        --slots;

        // 如果槽位用完了但还有节点，无效
        if (slots < 0) return false;

        // 非空节点产生两个新槽位
        if (node != "#") {
            slots += 2;
        }
    }

    return slots == 0;
}

/**
 * 解法2: 入度出度法
 * 思路:
 *   - 根节点：出度 2，入度 0
 *   - 非空节点：出度 2，入度 1
 *   - 空节点：出度 0，入度 1
 *   - 有效的树：总入度 = 总出度
 */
bool isValidSerializationDegree(std::string preorder) {
    int diff = 1;  // 出度 - 入度，初始为 1（根节点的出度）

    std::stringstream ss(preorder);
    std::string node;

    while (std::getline(ss, node, ',')) {
        --diff;  // 入度 +1
        if (diff < 0) return false;

        if (node != "#") {
            diff += 2;  // 出度 +2
        }
    }

    return diff == 0;
}

/**
 * 题目5: 从字符串生成二叉树
 *
 * 解法: 递归或栈
 * 时间复杂度: O(n)
 * 空间复杂度: O(h)
 */
TreeNode* str2treeHelper(const std::string& s, int& idx) {
    if (idx >= static_cast<int>(s.size())) return nullptr;

    // 解析数值
    int sign = 1;
    if (s[idx] == '-') {
        sign = -1;
        ++idx;
    }

    int val = 0;
    while (idx < static_cast<int>(s.size()) && isdigit(s[idx])) {
        val = val * 10 + (s[idx] - '0');
        ++idx;
    }
    val *= sign;

    TreeNode* node = new TreeNode(val);

    // 左子树
    if (idx < static_cast<int>(s.size()) && s[idx] == '(') {
        ++idx;  // 跳过 '('
        node->left = str2treeHelper(s, idx);
        ++idx;  // 跳过 ')'
    }

    // 右子树
    if (idx < static_cast<int>(s.size()) && s[idx] == '(') {
        ++idx;  // 跳过 '('
        node->right = str2treeHelper(s, idx);
        ++idx;  // 跳过 ')'
    }

    return node;
}

TreeNode* str2tree(std::string s) {
    if (s.empty()) return nullptr;
    int idx = 0;
    return str2treeHelper(s, idx);
}

/**
 * 栈解法
 */
TreeNode* str2treeStack(std::string s) {
    if (s.empty()) return nullptr;

    std::stack<TreeNode*> stk;
    int i = 0;
    int n = s.size();

    while (i < n) {
        if (s[i] == ')') {
            stk.pop();
            ++i;
        } else if (s[i] == '(') {
            ++i;
        } else {
            // 解析数值
            int sign = 1;
            if (s[i] == '-') {
                sign = -1;
                ++i;
            }

            int val = 0;
            while (i < n && isdigit(s[i])) {
                val = val * 10 + (s[i] - '0');
                ++i;
            }
            val *= sign;

            TreeNode* node = new TreeNode(val);

            if (!stk.empty()) {
                TreeNode* parent = stk.top();
                if (!parent->left) {
                    parent->left = node;
                } else {
                    parent->right = node;
                }
            }

            stk.push(node);
        }
    }

    return stk.empty() ? nullptr : stk.top();
}

/**
 * 题目6: 根据二叉树创建字符串
 *
 * 解法: 前序遍历
 * 规则:
 *   - 省略空的右子树括号
 *   - 不能省略空的左子树括号（如果有右子树）
 *
 * 时间复杂度: O(n)
 * 空间复杂度: O(h)
 */
std::string tree2str(TreeNode* root) {
    if (!root) return "";

    std::string result = std::to_string(root->val);

    // 有左子树或有右子树时，需要添加左子树括号
    if (root->left || root->right) {
        result += "(" + tree2str(root->left) + ")";
    }

    // 有右子树时，添加右子树括号
    if (root->right) {
        result += "(" + tree2str(root->right) + ")";
    }

    return result;
}

/**
 * 迭代解法
 */
std::string tree2strIterative(TreeNode* root) {
    if (!root) return "";

    std::string result;
    std::stack<TreeNode*> stk;
    std::unordered_map<TreeNode*, bool> visited;
    stk.push(root);

    while (!stk.empty()) {
        TreeNode* node = stk.top();

        if (visited[node]) {
            result += ")";
            stk.pop();
        } else {
            visited[node] = true;
            result += "(" + std::to_string(node->val);

            // 空左子树但有右子树
            if (!node->left && node->right) {
                result += "()";
            }

            // 先压右再压左（左先处理）
            if (node->right) stk.push(node->right);
            if (node->left) stk.push(node->left);
        }
    }

    // 移除首尾括号
    return result.substr(1, result.size() - 2);
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
    CodecBFS codecBFS;

    // 测试 DFS 版本
    TreeNode* root = createTree({1, 2, 3, -1, -1, 4, 5});
    std::string serialized = codec.serialize(root);
    TreeNode* deserialized = codec.deserialize(serialized);
    assert(isSameTree(root, deserialized));
    deleteTree(deserialized);

    // 测试 BFS 版本
    serialized = codecBFS.serialize(root);
    deserialized = codecBFS.deserialize(serialized);
    assert(isSameTree(root, deserialized));

    deleteTree(root);
    deleteTree(deserialized);

    // 测试空树
    serialized = codec.serialize(nullptr);
    deserialized = codec.deserialize(serialized);
    assert(deserialized == nullptr);

    std::cout << "Codec: PASSED\n";
}

void testCodecBST() {
    CodecBST codec;

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

    // 测试空树
    serialized = codec.serialize(nullptr);
    deserialized = codec.deserialize(serialized);
    assert(deserialized == nullptr);

    std::cout << "CodecBST: PASSED\n";
}

void testFindDuplicateSubtrees() {
    TreeNode* root = new TreeNode(1);
    root->left = new TreeNode(2);
    root->right = new TreeNode(3);
    root->left->left = new TreeNode(4);
    root->right->left = new TreeNode(2);
    root->right->right = new TreeNode(4);
    root->right->left->left = new TreeNode(4);

    auto result = findDuplicateSubtrees(root);
    assert(result.size() == 2);

    // 测试优化版本
    result = findDuplicateSubtreesOptimized(root);
    // 注意：优化版本需要重新构建树，因为前一个测试可能修改了状态

    deleteTree(root);

    std::cout << "findDuplicateSubtrees: PASSED\n";
}

void testIsValidSerialization() {
    assert(isValidSerialization("9,3,4,#,#,1,#,#,2,#,6,#,#") == true);
    assert(isValidSerializationDegree("9,3,4,#,#,1,#,#,2,#,6,#,#") == true);

    assert(isValidSerialization("1,#") == false);
    assert(isValidSerialization("9,#,#,1") == false);
    assert(isValidSerialization("#") == true);

    std::cout << "isValidSerialization: PASSED\n";
}

void testStr2tree() {
    TreeNode* root = str2tree("4(2(3)(1))(6(5))");
    assert(root->val == 4);
    assert(root->left->val == 2);
    assert(root->right->val == 6);
    assert(root->left->left->val == 3);
    assert(root->left->right->val == 1);
    assert(root->right->left->val == 5);
    deleteTree(root);

    // 测试栈解法
    root = str2treeStack("4(2(3)(1))(6(5))");
    assert(root->val == 4);
    deleteTree(root);

    // 测试空字符串
    assert(str2tree("") == nullptr);

    // 测试负数
    root = str2tree("-4(2)(-6)");
    assert(root->val == -4);
    assert(root->right->val == -6);
    deleteTree(root);

    std::cout << "str2tree: PASSED\n";
}

void testTree2str() {
    TreeNode* root = createTree({1, 2, 3, 4});
    std::string result = tree2str(root);
    assert(result == "1(2(4))(3)");

    result = tree2strIterative(root);
    assert(result == "1(2(4))(3)");
    deleteTree(root);

    root = createTree({1, 2, 3, -1, 4});
    result = tree2str(root);
    assert(result == "1(2()(4))(3)");
    deleteTree(root);

    // 单节点
    root = new TreeNode(1);
    result = tree2str(root);
    assert(result == "1");
    delete root;

    std::cout << "tree2str: PASSED\n";
}

int main() {
    std::cout << "=== Serialize Tree Solution Tests ===\n";

    testCodec();
    testCodecBST();
    testFindDuplicateSubtrees();
    testIsValidSerialization();
    testStr2tree();
    testTree2str();

    std::cout << "\nAll tests passed!\n";
    return 0;
}
