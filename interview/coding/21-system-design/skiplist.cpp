/**
 * @file skiplist.cpp
 * @brief 跳表 - 面试题实现
 */

#include "skiplist.h"
#include <climits>

namespace SkiplistImpl {

Skiplist::Node::Node(int v, int level) : val(v), forward(level, nullptr) {}

Skiplist::Skiplist() : level_(1), rng_(std::random_device{}()) {
    head_ = new Node(INT_MIN, MAX_LEVEL);
}

Skiplist::~Skiplist() {
    // TODO: 实现析构函数，释放所有节点
    // 提示：从 head_ 开始，沿着第 0 层遍历删除所有节点
}

int Skiplist::randomLevel() {
    // TODO: 实现随机层数生成
    // 提示：从 1 开始，每次 50% 概率增加一层，最多到 MAX_LEVEL
    return 1;
}

bool Skiplist::search(int target) {
    // TODO: 实现查找操作
    // 提示：
    // 1. 从最高层开始向下遍历
    // 2. 在每一层，向右移动直到下一个节点的值 >= target
    // 3. 然后下降到下一层继续
    // 4. 最后检查第 0 层的下一个节点是否等于 target
    return false;
}

void Skiplist::add(int num) {
    // TODO: 实现添加操作
    // 提示：
    // 1. 创建 update 数组记录每一层需要更新的节点
    // 2. 从最高层向下查找插入位置，记录路径到 update
    // 3. 随机生成新节点的层数
    // 4. 如果新层数大于当前最大层数，更新 level_
    // 5. 创建新节点并更新各层指针
}

bool Skiplist::erase(int num) {
    // TODO: 实现删除操作
    // 提示：
    // 1. 创建 update 数组记录每一层需要更新的节点
    // 2. 从最高层向下查找要删除的节点
    // 3. 如果找不到，返回 false
    // 4. 更新各层指针，跳过要删除的节点
    // 5. 删除节点
    // 6. 如果删除后某些高层为空，减少 level_
    return false;
}

void Skiplist::print() {
    // TODO: 实现打印跳表结构（用于调试）
}

} // namespace SkiplistImpl
