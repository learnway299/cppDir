#include "sort/mergeSort/MergeSort.h"
#include "sort/other/StrCountSort.h"
#include "sort/quickSort/QuickSort.h"
#include "stack/IncrementalSequence.h"

void solution() {
    QuickSort::solution();
    MergeSort::solution();
    StrCountSort::solution();
    IncrementalSequence::solution();
}

int main() {
   solution();
    return 0;
}