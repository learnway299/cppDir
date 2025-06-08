//
// Created by learyuan on 25-6-8.
//

#include "MergeSort.h"
#include "../util/Util.h"

namespace MergeSort {
    void solution() {
        std::cout <<"MergeSort::solution" <<std::endl;

        std::vector<int> vecN = {1, 3, 5, 7, 9};
        std::vector<int> vecM = {2, 3, 4, 6, 9};
        Util::prntVec(vecN);
        Util::prntVec(vecM);
        // 调整 vecN 的大小以容纳合并后的结果
        vecN.reserve(vecN.size() + vecM.size());

        mergeSortedVectors(vecM, vecN);
        Util::prntVec(vecN);
    }

    void mergeSortedVectors(std::vector<int>& vecM, std::vector<int>& vecN) {
        vecN.resize(vecN.size() + vecM.size());
        int nIndex = vecN.size() - vecM.size() - 1;
        int mIndex = vecM.size() - 1;
        int curIndex = vecN.size() - 1;
        while (mIndex >= 0 && nIndex >= 0) {
            if (vecM[mIndex] > vecN[nIndex]) {
                vecN[curIndex--] = vecM[mIndex--];
            } else {
                vecN[curIndex--] = vecN[nIndex--];
            }
        }
        while (mIndex >= 0) {
            vecN[curIndex--] = vecM[mIndex--];
        }
    }
} // MergeSort