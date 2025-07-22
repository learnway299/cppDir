#include "sort/mergeSort/MergeSort.h"
#include "sort/other/StrCountSort.h"
#include "sort/quickSort/QuickSort.h"
#include "stack/IncrementalSequence.h"
#include "stack/Subsequence.h"
#include "stack/SubSet.h"
#include "stack/TreeNodeTraversal.h"
#include "thread/MessageQueue.h"

void solution() {
    SubSet::solution();
    QuickSort::solution();
    MergeSort::solution();
    Subsequence::solution();
    StrCountSort::solution();
    IncrementalSequence::solution();
    TreeNodeTraversal::solution();
    ProduceCustomer::solution();
}

int main() {
   solution();
    return 0;
}