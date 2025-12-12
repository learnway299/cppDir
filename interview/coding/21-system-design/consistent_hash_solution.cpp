/**
 * @file consistent_hash_solution.cpp
 * @brief 一致性哈希 - 解答
 */
#include <string>
#include <map>
#include <functional>
#include <iostream>
using namespace std;

class ConsistentHash {
    map<size_t, string> ring_;   // 哈希环
    int virtualNodes_;           // 每个物理节点的虚拟节点数
    hash<string> hasher_;

    size_t hashKey(const string& key) {
        return hasher_(key);
    }

public:
    ConsistentHash(int virtualNodes = 150) : virtualNodes_(virtualNodes) {}

    void addNode(const string& node) {
        for (int i = 0; i < virtualNodes_; ++i) {
            string vnode = node + "#" + to_string(i);
            size_t h = hashKey(vnode);
            ring_[h] = node;
        }
    }

    void removeNode(const string& node) {
        for (int i = 0; i < virtualNodes_; ++i) {
            string vnode = node + "#" + to_string(i);
            size_t h = hashKey(vnode);
            ring_.erase(h);
        }
    }

    string getNode(const string& key) {
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

int main() {
    ConsistentHash ch(3);  // 使用3个虚拟节点便于演示

    ch.addNode("server1");
    ch.addNode("server2");
    ch.addNode("server3");

    cout << "Ring size: " << ch.size() << "\n";  // 9

    // 测试key分配
    vector<string> keys = {"user1", "user2", "user3", "order1", "order2"};
    cout << "Key distribution:\n";
    for (const string& key : keys) {
        cout << "  " << key << " -> " << ch.getNode(key) << "\n";
    }

    // 删除一个节点
    cout << "\nAfter removing server2:\n";
    ch.removeNode("server2");
    for (const string& key : keys) {
        cout << "  " << key << " -> " << ch.getNode(key) << "\n";
    }

    return 0;
}
