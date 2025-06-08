//
// Created by learyuan on 25-6-8.
//

#include "QuickSort.h"
#include "../util/Util.h"

namespace QuickSort {
    int partition(std::vector<int>& input, const int left, const int right)
    {
        const int pivot = input[right];
        int curIndex = left - 1;
        for (int i = left; i < right; i++)
        {
            if (input[i] <= pivot) {
                curIndex++;
                Util::swap(input[i], input[curIndex]);
            }
        }
        Util::swap(input[curIndex + 1], input[right]);
        return curIndex + 1;
    }

    void quickSort(std::vector<int> &input, int left, int right) {
        if (left < right) {
            int pivot = partition(input, left, right);
            quickSort(input, left, pivot - 1);
            quickSort(input, pivot + 1, right);
        }
    }

    void solution() {
        std::cout <<"QuickSort::solution" <<std::endl;
        std::vector<int> temp = {4, 2, 23, 14, 5, 16, 7, 28, 19, 10};
        Util::prntVec(temp);
        quickSort(temp, 0, temp.size() - 1);
        Util::prntVec(temp);
    }
} // QuickSort