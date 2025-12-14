/**
 * @file serialize_tree_solution.cpp
 * @brief 二叉树序列化问题 - 参考答案
 */

#include "serialize_tree.h"
#include <iostream>
#include <cassert>
#include <sstream>
#include <unordered_map>
#include <climits>

namespace SerializeTree {

// ==================== 辅助函数 ====================

static void serializeHelper(TreeNode* root, std::string& result) {
    if (!root) { result += "null,"; return; }
    result += std::to_string(root->val) + ",";
    serializeHelper(root->left, result);
    serializeHelper(root->right, result);
}

static TreeNode* deserializeHelper(std::istringstream& iss) {
    std::string val;
    if (!std::getline(iss, val, ',')) return nullptr;
    if (val == "null") return nullptr;
    TreeNode* node = new TreeNode(std::stoi(val));
    node->left = deserializeHelper(iss);
    node->right = deserializeHelper(iss);
    return node;
}

static void serializeBSTHelper(TreeNode* root, std::string& result) {
    if (!root) return;
    result += std::to_string(root->val) + ",";
    serializeBSTHelper(root->left, result);
    serializeBSTHelper(root->right, result);
}

static TreeNode* deserializeBSTHelper(std::vector<int>& values, int& idx, int minVal, int maxVal) {
    if (idx >= static_cast<int>(values.size())) return nullptr;
    if (values[idx] < minVal || values[idx] > maxVal) return nullptr;
    TreeNode* node = new TreeNode(values[idx++]);
    node->left = deserializeBSTHelper(values, idx, minVal, node->val);
    node->right = deserializeBSTHelper(values, idx, node->val, maxVal);
    return node;
}

static std::string findDuplicateHelper(TreeNode* root, std::unordered_map<std::string, int>& count,
                                       std::vector<TreeNode*>& result) {
    if (!root) return "#";
    std::string serial = std::to_string(root->val) + "," +
                         findDuplicateHelper(root->left, count, result) + "," +
                         findDuplicateHelper(root->right, count, result);
    if (++count[serial] == 2) {
        result.push_back(root);
    }
    return serial;
}

static TreeNode* str2treeHelper(const std::string& s, int& idx) {
    if (idx >= static_cast<int>(s.size())) return nullptr;
    int sign = 1;
    if (s[idx] == '-') { sign = -1; ++idx; }
    int val = 0;
    while (idx < static_cast<int>(s.size()) && isdigit(s[idx])) {
        val = val * 10 + (s[idx] - '0');
        ++idx;
    }
    val *= sign;
    TreeNode* node = new TreeNode(val);
    if (idx < static_cast<int>(s.size()) && s[idx] == '(') {
        ++idx;
        node->left = str2treeHelper(s, idx);
        ++idx;
    }
    if (idx < static_cast<int>(s.size()) && s[idx] == '(') {
        ++idx;
        node->right = str2treeHelper(s, idx);
        ++idx;
    }
    return node;
}

static bool isSameTree(TreeNode* p, TreeNode* q) {
    if (!p && !q) return true;
    if (!p || !q) return false;
    return p->val == q->val &&
           isSameTree(p->left, q->left) &&
           isSameTree(p->right, q->right);
}

// ==================== 参考答案实现 ====================

// 题目1: 二叉树的序列化与反序列化 - DFS版本
std::string CodecSolution::serialize(TreeNode* root) {
    std::string result;
    serializeHelper(root, result);
    return result;
}

TreeNode* CodecSolution::deserialize(std::string data) {
    std::istringstream iss(data);
    return deserializeHelper(iss);
}

// 题目2: 二叉搜索树的序列化与反序列化
std::string CodecBSTSolution::serialize(TreeNode* root) {
    std::string result;
    serializeBSTHelper(root, result);
    if (!result.empty()) result.pop_back();
    return result;
}

TreeNode* CodecBSTSolution::deserialize(std::string data) {
    if (data.empty()) return nullptr;
    std::vector<int> values;
    std::stringstream ss(data);
    std::string item;
    while (std::getline(ss, item, ',')) {
        values.push_back(std::stoi(item));
    }
    int idx = 0;
    return deserializeBSTHelper(values, idx, INT_MIN, INT_MAX);
}

// 题目3: 寻找重复的子树
std::vector<TreeNode*> findDuplicateSubtreesSolution(TreeNode* root) {
    std::unordered_map<std::string, int> count;
    std::vector<TreeNode*> result;
    findDuplicateHelper(root, count, result);
    return result;
}

// 题目4: 验证二叉树的前序序列化
bool isValidSerializationSolution(std::string preorder) {
    int slots = 1;
    std::stringstream ss(preorder);
    std::string node;
    while (std::getline(ss, node, ',')) {
        --slots;
        if (slots < 0) return false;
        if (node != "#") slots += 2;
    }
    return slots == 0;
}

// 题目5: 从字符串生成二叉树
TreeNode* str2treeSolution(std::string s) {
    if (s.empty()) return nullptr;
    int idx = 0;
    return str2treeHelper(s, idx);
}

// 题目6: 根据二叉树创建字符串
std::string tree2strSolution(TreeNode* root) {
    if (!root) return "";
    std::string result = std::to_string(root->val);
    if (root->left || root->right) {
        result += "(" + tree2strSolution(root->left) + ")";
    }
    if (root->right) {
        result += "(" + tree2strSolution(root->right) + ")";
    }
    return result;
}

// ==================== 测试函数 ====================

void testSerializeTreeSolution() {
    std::cout << "=== Serialize Tree Tests (Solution) ===\n";

    // CodecSolution (DFS)
    CodecSolution codec;
    TreeNode* root = Util::createTree({1, 2, 3, -1, -1, 4, 5});
    std::string serialized = codec.serialize(root);
    TreeNode* deserialized = codec.deserialize(serialized);
    assert(isSameTree(root, deserialized));
    Util::deleteTree(root);
    Util::deleteTree(deserialized);
    std::cout << "  CodecSolution: PASSED\n";

    // CodecBSTSolution
    CodecBSTSolution codecBST;
    root = new TreeNode(5);
    root->left = new TreeNode(3);
    root->right = new TreeNode(7);
    root->left->left = new TreeNode(2);
    root->left->right = new TreeNode(4);
    serialized = codecBST.serialize(root);
    deserialized = codecBST.deserialize(serialized);
    assert(isSameTree(root, deserialized));
    Util::deleteTree(root);
    Util::deleteTree(deserialized);
    std::cout << "  CodecBSTSolution: PASSED\n";

    // isValidSerializationSolution
    assert(isValidSerializationSolution("9,3,4,#,#,1,#,#,2,#,6,#,#") == true);
    assert(isValidSerializationSolution("1,#") == false);
    assert(isValidSerializationSolution("#") == true);
    std::cout << "  isValidSerializationSolution: PASSED\n";

    // str2treeSolution
    root = str2treeSolution("4(2(3)(1))(6(5))");
    assert(root->val == 4);
    assert(root->left->val == 2);
    assert(root->right->val == 6);
    Util::deleteTree(root);
    assert(str2treeSolution("") == nullptr);
    std::cout << "  str2treeSolution: PASSED\n";

    // tree2strSolution
    root = Util::createTree({1, 2, 3, 4});
    std::string result = tree2strSolution(root);
    assert(result == "1(2(4))(3)");
    Util::deleteTree(root);
    root = Util::createTree({1, 2, 3, -1, 4});
    result = tree2strSolution(root);
    assert(result == "1(2()(4))(3)");
    Util::deleteTree(root);
    std::cout << "  tree2strSolution: PASSED\n";
}

} // namespace SerializeTree
