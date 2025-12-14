/**
 * @file consistent_hash_solution.cpp
 * @brief 一致性哈希 - 参考解答
 */

#include "consistent_hash.h"
#include <iostream>
#include <vector>

namespace ConsistentHashImpl {
namespace Solution {

class ConsistentHash {
private:
    std::map<size_t, std::string> ring_;   // 哈希环
    int virtualNodes_;                      // 每个物理节点的虚拟节点数
    std::hash<std::string> hasher_;

    size_t hashKey(const std::string& key) {
        return hasher_(key);
    }

public:
    ConsistentHash(int virtualNodes = 150) : virtualNodes_(virtualNodes) {}

    void addNode(const std::string& node) {
        for (int i = 0; i < virtualNodes_; ++i) {
            std::string vnode = node + "#" + std::to_string(i);
            size_t h = hashKey(vnode);
            ring_[h] = node;
        }
    }

    void removeNode(const std::string& node) {
        for (int i = 0; i < virtualNodes_; ++i) {
            std::string vnode = node + "#" + std::to_string(i);
            size_t h = hashKey(vnode);
            ring_.erase(h);
        }
    }

    std::string getNode(const std::string& key) {
        if (ring_.empty()) return "";

        size_t h = hashKey(key);
        auto it = ring_.lower_bound(h);
        if (it == ring_.end()) {
            it = ring_.begin();  // 环形，回到起点
        }
        return it->second;
    }

    size_t size() const { return ring_.size(); }
};

void runSolutionTests() {
    std::cout << "=== Consistent Hash Solution ===" << std::endl;

    ConsistentHash ch(3);  // 使用3个虚拟节点便于演示

    ch.addNode("server1");
    ch.addNode("server2");
    ch.addNode("server3");

    std::cout << "Ring size: " << ch.size() << std::endl;  // 9

    // 测试key分配
    std::vector<std::string> keys = {"user1", "user2", "user3", "order1", "order2"};
    std::cout << "Key distribution:" << std::endl;
    for (const std::string& key : keys) {
        std::cout << "  " << key << " -> " << ch.getNode(key) << std::endl;
    }

    // 删除一个节点
    std::cout << "\nAfter removing server2:" << std::endl;
    ch.removeNode("server2");
    for (const std::string& key : keys) {
        std::cout << "  " << key << " -> " << ch.getNode(key) << std::endl;
    }

    std::cout << "\nConsistent hash tests completed!" << std::endl;
}

} // namespace Solution

void runTests() {
    Solution::runSolutionTests();
}

} // namespace ConsistentHashImpl
