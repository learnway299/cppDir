/**
 * @file serialize_tree.h
 * @brief 二叉树序列化问题 - 题目说明与声明
 *
 * 序列化是将数据结构转换为可存储或传输的格式的过程。
 * 反序列化是从序列化的格式中恢复数据结构的过程。
 */

#ifndef SERIALIZE_TREE_H
#define SERIALIZE_TREE_H

#include "../../util/Util.h"
#include <vector>
#include <string>

namespace SerializeTree {

// ==================== 题目声明 ====================

/**
 * 题目1: 二叉树的序列化与反序列化
 *
 * 设计一个算法来实现二叉树的序列化与反序列化。
 * 常见方案：BFS 层序、DFS 前序/后序。
 */
class Codec {
public:
    std::string serialize(TreeNode* root);
    TreeNode* deserialize(std::string data);
};

class CodecSolution {
public:
    std::string serialize(TreeNode* root);
    TreeNode* deserialize(std::string data);
};

/**
 * 题目2: 二叉搜索树的序列化与反序列化
 *
 * 利用 BST 的性质，可以不存储 null 节点，
 * 因为 BST 的前序遍历可以唯一确定树的结构。
 */
class CodecBST {
public:
    std::string serialize(TreeNode* root);
    TreeNode* deserialize(std::string data);
};

class CodecBSTSolution {
public:
    std::string serialize(TreeNode* root);
    TreeNode* deserialize(std::string data);
};

/**
 * 题目3: 寻找重复的子树
 *
 * 通过序列化每个子树，用哈希表记录出现次数。
 */
std::vector<TreeNode*> findDuplicateSubtrees(TreeNode* root);
std::vector<TreeNode*> findDuplicateSubtreesSolution(TreeNode* root);

/**
 * 题目4: 验证二叉树的前序序列化
 *
 * 使用入度出度或槽位的概念来验证。
 * 示例: "9,3,4,#,#,1,#,#,2,#,6,#,#" -> true
 */
bool isValidSerialization(std::string preorder);
bool isValidSerializationSolution(std::string preorder);

/**
 * 题目5: 从字符串生成二叉树
 *
 * 输入: "4(2(3)(1))(6(5))"，括号表示子节点。
 */
TreeNode* str2tree(std::string s);
TreeNode* str2treeSolution(std::string s);

/**
 * 题目6: 根据二叉树创建字符串
 *
 * 使用括号表示的前序遍历，省略不必要的空括号。
 * 示例: "1(2(4))(3)"
 */
std::string tree2str(TreeNode* root);
std::string tree2strSolution(TreeNode* root);

// ==================== 测试函数声明 ====================

void testSerializeTree();          // 测试面试者实现
void testSerializeTreeSolution();  // 测试解答实现

// runTests 在头文件 inline 定义，调用两个测试函数
inline void runTests() {
    testSerializeTree();
    testSerializeTreeSolution();
}

} // namespace SerializeTree

#endif // SERIALIZE_TREE_H
