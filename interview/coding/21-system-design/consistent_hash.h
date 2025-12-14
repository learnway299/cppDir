/**
 * @file consistent_hash.h
 * @brief 一致性哈希 - 面试题
 *
 * 题目：实现一致性哈希
 *
 * 要求：
 * 1. 实现 ConsistentHash 类，支持以下操作：
 *    - ConsistentHash(int virtualNodes): 构造函数，指定每个物理节点的虚拟节点数
 *    - void addNode(const std::string& node): 添加物理节点
 *    - void removeNode(const std::string& node): 删除物理节点
 *    - std::string getNode(const std::string& key): 根据 key 获取对应的节点
 *
 * 2. 实现虚拟节点机制：
 *    - 每个物理节点映射到多个虚拟节点
 *    - 虚拟节点均匀分布在哈希环上
 *    - 提高负载均衡性
 *
 * 3. 理解一致性哈希的特性：
 *    - 节点增删时，只影响相邻节点的数据迁移
 *    - 最小化数据重新分配
 *    - 使用哈希环结构
 *
 * 时间复杂度：
 * - addNode: O(V log N)，V 为虚拟节点数，N 为总节点数
 * - removeNode: O(V log N)
 * - getNode: O(log N)
 *
 * 空间复杂度：O(N * V)
 */

#ifndef CONSISTENT_HASH_H
#define CONSISTENT_HASH_H

#include <string>
#include <map>
#include <functional>

namespace ConsistentHashImpl {

class ConsistentHash {
private:
    std::map<size_t, std::string> ring_;   // 哈希环
    int virtualNodes_;                      // 每个物理节点的虚拟节点数
    std::hash<std::string> hasher_;

    size_t hashKey(const std::string& key);

public:
    ConsistentHash(int virtualNodes = 150);

    void addNode(const std::string& node);

    void removeNode(const std::string& node);

    std::string getNode(const std::string& key);

    size_t size() const;
};

void runTests();

} // namespace ConsistentHashImpl

#endif // CONSISTENT_HASH_H
