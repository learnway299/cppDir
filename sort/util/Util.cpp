//
// Created by learyuan on 25-6-8.
//

#include "Util.h"

namespace Util {
    void prntVec(const std::vector<int> &input) {
        for (const auto& item : input) {
            std::cout << item << " ";
        }
        std::cout << std::endl;
    }

    void swap(int &a, int &b) noexcept {
        if (&a != &b) {
            a = a ^ b;
            b = a ^ b;
            a = a ^ b;
        }
    }
} // Util