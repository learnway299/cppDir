//
// Created by learyuan on 25-6-15.
//

#include "IncrementalSequence.h"
#include <algorithm>
#include <stack>
#include "../sort/util/Util.h"

namespace IncrementalSequence {
    void solution() {
        std::cout << "IncrementalSequence::solution" << std::endl;
        std::vector<int> temp{1,2,3,5,8,6,7};
        Util::prntVec(temp);
        std::vector<int> result = incrementalSequence(temp);
        Util::prntVec(result);
    }
    std::vector<int> incrementalSequence(const std::vector<int> &input) {
        stack<int> cache;
        for (auto num : input) {
            if (cache.empty()) {
                cache.push(num);
            }
            else {
                while (!cache.empty() && cache.top() > num) {
                    cache.pop();
                }
                cache.push(num);
            }
        }
        std::vector<int> result;
        while (!cache.empty()) {
            result.push_back(cache.top());
            cache.pop();
        }
        std::reverse(result.begin(), result.end());
        return result;
    }
} // IncrementalSequence