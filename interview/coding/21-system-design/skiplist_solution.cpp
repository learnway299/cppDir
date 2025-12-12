/**
 * @file skiplist_solution.cpp
 * @brief 跳表 - 解答
 */
#include <vector>
#include <random>
#include <iostream>
#include <climits>
using namespace std;

class Skiplist {
    static const int MAX_LEVEL = 16;

    struct Node {
        int val;
        vector<Node*> forward;
        Node(int v, int level) : val(v), forward(level, nullptr) {}
    };

    Node* head_;
    int level_;
    mt19937 rng_;

    int randomLevel() {
        int lvl = 1;
        while (rng_() % 2 && lvl < MAX_LEVEL) lvl++;
        return lvl;
    }

public:
    Skiplist() : level_(1), rng_(random_device{}()) {
        head_ = new Node(INT_MIN, MAX_LEVEL);
    }

    ~Skiplist() {
        Node* curr = head_;
        while (curr) {
            Node* next = curr->forward[0];
            delete curr;
            curr = next;
        }
    }

    bool search(int target) {
        Node* curr = head_;
        for (int i = level_ - 1; i >= 0; --i) {
            while (curr->forward[i] && curr->forward[i]->val < target) {
                curr = curr->forward[i];
            }
        }
        curr = curr->forward[0];
        return curr && curr->val == target;
    }

    void add(int num) {
        vector<Node*> update(MAX_LEVEL, head_);
        Node* curr = head_;

        for (int i = level_ - 1; i >= 0; --i) {
            while (curr->forward[i] && curr->forward[i]->val < num) {
                curr = curr->forward[i];
            }
            update[i] = curr;
        }

        int newLevel = randomLevel();
        if (newLevel > level_) {
            level_ = newLevel;
        }

        Node* newNode = new Node(num, newLevel);
        for (int i = 0; i < newLevel; ++i) {
            newNode->forward[i] = update[i]->forward[i];
            update[i]->forward[i] = newNode;
        }
    }

    bool erase(int num) {
        vector<Node*> update(MAX_LEVEL);
        Node* curr = head_;

        for (int i = level_ - 1; i >= 0; --i) {
            while (curr->forward[i] && curr->forward[i]->val < num) {
                curr = curr->forward[i];
            }
            update[i] = curr;
        }

        curr = curr->forward[0];
        if (!curr || curr->val != num) return false;

        for (int i = 0; i < level_; ++i) {
            if (update[i]->forward[i] != curr) break;
            update[i]->forward[i] = curr->forward[i];
        }

        delete curr;

        while (level_ > 1 && !head_->forward[level_ - 1]) {
            level_--;
        }

        return true;
    }

    void print() {
        cout << "Skiplist (level " << level_ << "):\n";
        for (int i = level_ - 1; i >= 0; --i) {
            cout << "Level " << i << ": ";
            Node* curr = head_->forward[i];
            while (curr) {
                cout << curr->val << " -> ";
                curr = curr->forward[i];
            }
            cout << "null\n";
        }
    }
};

int main() {
    Skiplist sl;

    // 添加元素
    sl.add(1);
    sl.add(2);
    sl.add(3);
    sl.add(5);
    sl.add(8);

    sl.print();

    // 查找
    cout << "\nsearch(2): " << sl.search(2) << "\n";  // 1
    cout << "search(4): " << sl.search(4) << "\n";    // 0

    // 删除
    cout << "erase(2): " << sl.erase(2) << "\n";      // 1
    cout << "search(2): " << sl.search(2) << "\n";    // 0

    sl.print();

    return 0;
}
