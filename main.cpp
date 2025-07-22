#include "sort/other/StrCountSort.h"
#include "sort/quickSort/QuickSort.h"
#include "sort/mergeSort/MergeSort.h"
#include "stack/SubSet.h"
#include "stack/Subsequence.h"
#include "stack/TreeNodeTraversal.h"
#include "stack/IncrementalSequence.h"
#include "thread/MessageQueue.h"
#include "thread/AlternatePrint.h"

void solution() {
    SubSet::solution();
    QuickSort::solution();
    MergeSort::solution();
    Subsequence::solution();
    StrCountSort::solution();
    TreeNodeTraversal::solution();
    IncrementalSequence::solution();
    AlternatePrint::solution();
    ProduceCustomer::solution();
}

int main() {
   solution();
    return 0;
}