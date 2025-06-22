//
// Created by learyuan on 2025/6/22.
//

#include "SubSet.h"
#include "../sort/util/Util.h"

namespace {
    void backTrack(std::vector<int> &nums, std::vector<std::vector<int> > &result,
                   std::vector<int> &current, std::vector<bool> &used, int start) {
        result.push_back(current);

        for (int i = start; i < nums.size(); ++i) {
            if (used[nums[i]]) {
                continue;
            }
            current.push_back(nums[i]);
            used[nums[i]] = true;
            backTrack(nums, result, current, used, i + 1);
            current.pop_back();
            used[nums[i]] = false;
        }
    }
};

namespace SubSet {
    void solution() {
        std::cout << "SubSet::solution";
        std::vector<int> input{1, 2, 3};
        Util::prntVec(input);
        std::vector<std::vector<int> > result = getSubset(input);
        for (auto item: result) {
            Util::prntVec(item);
        }
    }

    std::vector<std::vector<int> > getSubset(std::vector<int> nums) {
        std::vector<std::vector<int> > result;
        std::vector<int> current;
        std::vector<bool> used(nums.size(), false);
        backTrack(nums, result, current, used, 0);
        return result;
    }
}
