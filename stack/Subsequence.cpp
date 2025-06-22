//
// Created by learyuan on 25-6-22.
//

#include "Subsequence.h"
#include "../sort/util/Util.h"

namespace {
    void backTrack(std::vector<int> &nums, std::vector<std::vector<int> > &result,
                   std::vector<int> &current, std::vector<bool> &used) {
        if (current.size() == nums.size()) {
            result.push_back(current);
        } else {
            for (int i = 0; i < nums.size(); ++i) {
                if (used[nums[i]]) {
                    continue;
                }
                current.push_back(nums[i]);
                used[nums[i]] = true;
                backTrack(nums, result, current, used);
                current.pop_back();
                used[nums[i]] = false;
            }
        }
    }
};

void Subsequence::solution() {
    std::cout << "Subsequence::solution";
    std::vector<int> input{1, 3, 5, 4, 6};
    Util::prntVec(input);
    std::vector<std::vector<int> > result = getSubsequence(input);
    for (auto item : result) {
        Util::prntVec(item);
    }
}


std::vector<std::vector<int> > Subsequence::getSubsequence(std::vector<int> nums) {
    std::vector<std::vector<int> > result;
    std::vector<int> current;
    std::vector<bool> used(nums.size(), false);
    backTrack(nums, result, current, used);
    return result;
}
