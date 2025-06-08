//
// Created by learyuan on 25-6-8.
//

#ifndef QUICKSORT_H
#define QUICKSORT_H
#include <vector>

namespace QuickSort {
    int partition(std::vector<int>& input, const int left, const int right);
    void quickSort(std::vector<int>& input, int left, int right);
    void solution();
} // QuickSort

#endif //QUICKSORT_H
