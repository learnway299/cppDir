/**
 * @file random_set_solution.cpp
 * @brief O(1) 随机集合实现 - 参考答案
 */

#include "random_set.h"
#include <iostream>
#include <cassert>
#include <set>
#include <algorithm>

namespace RandomSetProblems {

// ==================== 参考答案实现 ====================

// RandomizedSetSolution 实现
RandomizedSetSolution::RandomizedSetSolution() : rng_(std::random_device{}()) {}

bool RandomizedSetSolution::insert(int val) {
    if (valToIndex_.count(val)) {
        return false;
    }

    valToIndex_[val] = static_cast<int>(nums_.size());
    nums_.push_back(val);
    return true;
}

bool RandomizedSetSolution::remove(int val) {
    auto it = valToIndex_.find(val);
    if (it == valToIndex_.end()) {
        return false;
    }

    int index = it->second;
    int lastVal = nums_.back();

    nums_[index] = lastVal;
    valToIndex_[lastVal] = index;

    nums_.pop_back();
    valToIndex_.erase(val);

    return true;
}

int RandomizedSetSolution::getRandom() {
    std::uniform_int_distribution<int> dist(0, static_cast<int>(nums_.size()) - 1);
    return nums_[dist(rng_)];
}

// RandomizedCollectionSolution 实现
RandomizedCollectionSolution::RandomizedCollectionSolution() : rng_(std::random_device{}()) {}

bool RandomizedCollectionSolution::insert(int val) {
    bool notExist = !valToIndices_.count(val) || valToIndices_[val].empty();

    valToIndices_[val].insert(static_cast<int>(nums_.size()));
    nums_.push_back(val);

    return notExist;
}

bool RandomizedCollectionSolution::remove(int val) {
    if (!valToIndices_.count(val) || valToIndices_[val].empty()) {
        return false;
    }

    int removeIdx = *valToIndices_[val].begin();
    int lastVal = nums_.back();
    int lastIdx = static_cast<int>(nums_.size()) - 1;

    nums_[removeIdx] = lastVal;

    valToIndices_[val].erase(removeIdx);
    valToIndices_[lastVal].erase(lastIdx);

    if (removeIdx != lastIdx) {
        valToIndices_[lastVal].insert(removeIdx);
    }

    nums_.pop_back();

    return true;
}

int RandomizedCollectionSolution::getRandom() {
    std::uniform_int_distribution<int> dist(0, static_cast<int>(nums_.size()) - 1);
    return nums_[dist(rng_)];
}

// BlacklistRandomSolution 实现
BlacklistRandomSolution::BlacklistRandomSolution(int n, std::vector<int>& blacklist) : rng_(std::random_device{}()) {
    whitelistSize_ = n - static_cast<int>(blacklist.size());

    std::unordered_set<int> blackSet(blacklist.begin(), blacklist.end());

    std::vector<int> whitesInTail;
    for (int i = whitelistSize_; i < n; ++i) {
        if (!blackSet.count(i)) {
            whitesInTail.push_back(i);
        }
    }

    int idx = 0;
    for (int b : blacklist) {
        if (b < whitelistSize_) {
            blackToWhite_[b] = whitesInTail[idx++];
        }
    }
}

int BlacklistRandomSolution::pick() {
    std::uniform_int_distribution<int> dist(0, whitelistSize_ - 1);
    int r = dist(rng_);

    auto it = blackToWhite_.find(r);
    if (it != blackToWhite_.end()) {
        return it->second;
    }
    return r;
}

// WeightedRandomSolution 实现
WeightedRandomSolution::WeightedRandomSolution(std::vector<int>& w) : rng_(std::random_device{}()) {
    prefixSum_.resize(w.size());
    prefixSum_[0] = w[0];
    for (size_t i = 1; i < w.size(); ++i) {
        prefixSum_[i] = prefixSum_[i - 1] + w[i];
    }
}

int WeightedRandomSolution::pickIndex() {
    std::uniform_int_distribution<int> dist(1, prefixSum_.back());
    int target = dist(rng_);

    return static_cast<int>(std::lower_bound(prefixSum_.begin(), prefixSum_.end(), target) - prefixSum_.begin());
}

// ReservoirSamplingSolution 实现
ReservoirSamplingSolution::ReservoirSamplingSolution(ListNode* head) : head_(head), rng_(std::random_device{}()) {}

int ReservoirSamplingSolution::getRandom() {
    int result = head_->val;
    ListNode* curr = head_->next;
    int i = 2;

    while (curr) {
        std::uniform_int_distribution<int> dist(1, i);
        if (dist(rng_) == 1) {
            result = curr->val;
        }
        curr = curr->next;
        ++i;
    }

    return result;
}

std::vector<int> ReservoirSamplingSolution::getKRandom(int k) {
    std::vector<int> reservoir;
    ListNode* curr = head_;

    for (int i = 0; i < k && curr; ++i) {
        reservoir.push_back(curr->val);
        curr = curr->next;
    }

    int i = k + 1;
    while (curr) {
        std::uniform_int_distribution<int> dist(1, i);
        int j = dist(rng_);
        if (j <= k) {
            reservoir[j - 1] = curr->val;
        }
        curr = curr->next;
        ++i;
    }

    return reservoir;
}

// ShuffleArraySolution 实现
ShuffleArraySolution::ShuffleArraySolution(std::vector<int>& nums)
    : original_(nums), shuffled_(nums), rng_(std::random_device{}()) {}

std::vector<int> ShuffleArraySolution::reset() {
    shuffled_ = original_;
    return shuffled_;
}

std::vector<int> ShuffleArraySolution::shuffle() {
    int n = static_cast<int>(shuffled_.size());
    for (int i = n - 1; i > 0; --i) {
        std::uniform_int_distribution<int> dist(0, i);
        int j = dist(rng_);
        std::swap(shuffled_[i], shuffled_[j]);
    }
    return shuffled_;
}

// ==================== 测试函数 ====================

void testRandomSetSolution() {
    std::cout << "=== Random Set Tests (Solution) ===\n";

    // RandomizedSetSolution 测试
    RandomizedSetSolution randomizedSet;
    assert(randomizedSet.insert(1) == true);
    assert(randomizedSet.remove(2) == false);
    assert(randomizedSet.insert(2) == true);
    std::set<int> results;
    for (int i = 0; i < 100; ++i) {
        results.insert(randomizedSet.getRandom());
    }
    assert(results.count(1) && results.count(2));
    assert(randomizedSet.remove(1) == true);
    assert(randomizedSet.insert(2) == false);
    assert(randomizedSet.getRandom() == 2);
    std::cout << "  RandomizedSetSolution: PASSED\n";

    // RandomizedCollectionSolution 测试
    RandomizedCollectionSolution collection;
    assert(collection.insert(1) == true);
    assert(collection.insert(1) == false);
    assert(collection.insert(2) == true);
    int count1 = 0, count2 = 0;
    for (int i = 0; i < 3000; ++i) {
        int r = collection.getRandom();
        if (r == 1) ++count1;
        else if (r == 2) ++count2;
    }
    assert(count1 > count2);
    assert(collection.remove(1) == true);
    std::cout << "  RandomizedCollectionSolution: PASSED\n";

    // BlacklistRandomSolution 测试
    std::vector<int> blacklist = {2, 3, 5};
    BlacklistRandomSolution blRandom(7, blacklist);
    std::set<int> blResults;
    for (int i = 0; i < 100; ++i) {
        int r = blRandom.pick();
        blResults.insert(r);
        assert(r != 2 && r != 3 && r != 5);
    }
    assert(blResults.size() == 4);
    std::cout << "  BlacklistRandomSolution: PASSED\n";

    // WeightedRandomSolution 测试
    std::vector<int> weights = {1, 3};
    WeightedRandomSolution wr(weights);
    int cnt0 = 0, cnt1 = 0;
    for (int i = 0; i < 4000; ++i) {
        if (wr.pickIndex() == 0) ++cnt0;
        else ++cnt1;
    }
    assert(cnt1 > cnt0 * 2);
    std::cout << "  WeightedRandomSolution: PASSED\n";

    // ReservoirSamplingSolution 测试
    ListNode* head = new ListNode(1);
    head->next = new ListNode(2);
    head->next->next = new ListNode(3);
    ReservoirSamplingSolution rs(head);
    std::set<int> rsResults;
    for (int i = 0; i < 100; ++i) {
        rsResults.insert(rs.getRandom());
    }
    assert(rsResults.size() == 3);
    while (head) {
        ListNode* next = head->next;
        delete head;
        head = next;
    }
    std::cout << "  ReservoirSamplingSolution: PASSED\n";

    // ShuffleArraySolution 测试
    std::vector<int> nums = {1, 2, 3, 4, 5};
    ShuffleArraySolution sa(nums);
    auto resetVec = sa.reset();
    assert(resetVec == nums);
    bool different = false;
    for (int i = 0; i < 10; ++i) {
        auto shuffled = sa.shuffle();
        if (shuffled != nums) {
            different = true;
            break;
        }
    }
    assert(different);
    std::cout << "  ShuffleArraySolution: PASSED\n";
}

} // namespace RandomSetProblems
