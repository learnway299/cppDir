/**
 * @file consistent_hash.cpp
 * @brief 一致性哈希 - 面试题实现
 */

#include "consistent_hash.h"

namespace ConsistentHashImpl {

size_t ConsistentHash::hashKey(const std::string& key) {
    // TODO: 实现哈希函数
    return 0;
}

ConsistentHash::ConsistentHash(int virtualNodes) : virtualNodes_(virtualNodes) {
    // TODO: 初始化成员变量（已在初始化列表中完成）
}

void ConsistentHash::addNode(const std::string& node) {
    // TODO: 实现添加节点
    // 提示：为每个物理节点创建 virtualNodes_ 个虚拟节点
    // 虚拟节点名称可以是 node + "#" + std::to_string(i)
    // 将虚拟节点的哈希值作为 key，物理节点名称作为 value 存入 ring_
}

void ConsistentHash::removeNode(const std::string& node) {
    // TODO: 实现删除节点
    // 提示：删除该物理节点对应的所有虚拟节点
}

std::string ConsistentHash::getNode(const std::string& key) {
    // TODO: 实现根据 key 获取节点
    // 提示：
    // 1. 如果 ring_ 为空，返回空字符串
    // 2. 计算 key 的哈希值
    // 3. 使用 lower_bound 找到哈希环上第一个大于等于该哈希值的虚拟节点
    // 4. 如果找不到（到达环的末尾），返回环的起点（环形结构）
    // 5. 返回虚拟节点对应的物理节点名称
    return "";
}

size_t ConsistentHash::size() const {
    return ring_.size();
}

} // namespace ConsistentHashImpl
