#include "sort/other/StrCountSort.h"
#include "sort/quickSort/QuickSort.h"
#include "sort/mergeSort/MergeSort.h"
#include "stack/SubSet.h"
#include "stack/Subsequence.h"
#include "stack/TreeNodeTraversal.h"
#include "stack/IncrementalSequence.h"
#include "thread/MessageQueue.h"
#include "thread/AlternatePrint.h"

// 面试编程题测试
#include "interview/coding/test_runner.h"

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
    // solution();

    // 运行面试编程题测试
    interview::runAllTests();

    return 0;
}