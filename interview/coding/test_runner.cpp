/**
 * @file test_runner.cpp
 * @brief 面试编程题测试入口 - 实现
 *
 * 编译命令 (需要 C++20 for coroutine):
 *   g++ -std=c++20 -o test_runner test_runner.cpp -pthread
 */

#include "test_runner.h"
#include <iostream>
#include <cstring>

// 01-basic
#include "01-basic/easy_problems.h"
#include "01-basic/fibonacci.h"

// 02-linked-list
#include "02-linked-list/reverse_list.h"
#include "02-linked-list/cycle_detection.h"
#include "02-linked-list/merge_lists.h"
#include "02-linked-list/two_pointer_problems.h"
#include "02-linked-list/copy_random_list.h"
#include "02-linked-list/lru_cache.h"
#include "02-linked-list/linked_list_sort.h"

// 03-tree
#include "03-tree/traversal.h"
#include "03-tree/tree_properties.h"
#include "03-tree/bst_operations.h"
#include "03-tree/lca.h"
#include "03-tree/path_problems.h"
#include "03-tree/construct_tree.h"
#include "03-tree/serialize_tree.h"

// 04-dp
#include "04-dp/coin_change.h"
#include "04-dp/knapsack.h"
#include "04-dp/linear_dp.h"
#include "04-dp/subsequence_dp.h"
#include "04-dp/interval_dp.h"

// 05-string
#include "05-string/palindrome.h"
#include "05-string/sliding_window.h"
#include "05-string/string_matching.h"
#include "05-string/string_operations.h"
#include "05-string/trie.h"

// 06-stack-queue
#include "06-stack-queue/monotonic_queue.h"
#include "06-stack-queue/monotonic_stack.h"
#include "06-stack-queue/stack_impl.h"

// 07-hash-design
#include "07-hash-design/hashmap_impl.h"
#include "07-hash-design/lru_cache.h"
#include "07-hash-design/lfu_cache.h"
#include "07-hash-design/random_set.h"
#include "07-hash-design/twitter_design.h"

// 08-sorting
#include "08-sorting/comparison_sorts.h"
#include "08-sorting/non_comparison_sorts.h"
#include "08-sorting/topk.h"
#include "08-sorting/merge_k_lists.h"

// 09-graph
#include "09-graph/traversal.h"
#include "09-graph/topological_sort.h"
#include "09-graph/shortest_path.h"
#include "09-graph/union_find.h"
#include "09-graph/mst.h"

// 10-smart-pointer
#include "10-smart-pointer/unique_ptr.h"
#include "10-smart-pointer/shared_ptr.h"
#include "10-smart-pointer/weak_ptr.h"
#include "10-smart-pointer/make_shared.h"

// 11-memory
#include "11-memory/memory_pool.h"
#include "11-memory/object_pool.h"
#include "11-memory/arena_allocator.h"
#include "11-memory/custom_allocator.h"

// 12-oop-design
#include "12-oop-design/singleton.h"
#include "12-oop-design/factory.h"
#include "12-oop-design/observer.h"
#include "12-oop-design/strategy.h"
#include "12-oop-design/pimpl.h"
#include "12-oop-design/crtp.h"
#include "12-oop-design/type_erasure.h"

// 13-thread
#include "13-thread/basic_sync.h"
#include "13-thread/barrier.h"
#include "13-thread/dining_philosophers.h"
#include "13-thread/print_alternately.h"
#include "13-thread/producer_consumer.h"
#include "13-thread/read_write_lock.h"
#include "13-thread/thread_pool.h"
#include "13-thread/thread_safe_queue.h"

// 14-lockfree
#include "14-lockfree/spinlock.h"
#include "14-lockfree/lockfree_stack.h"
#include "14-lockfree/spsc_queue.h"
#include "14-lockfree/mpmc_queue.h"
#include "14-lockfree/hazard_pointer.h"

// 15-template
#include "15-template/sfinae.h"
#include "15-template/type_traits.h"
#include "15-template/variadic_templates.h"
#include "15-template/type_erasure_impl.h"

// 16-coroutine
#include "16-coroutine/generator.h"
#include "16-coroutine/task.h"
#include "16-coroutine/scheduler.h"
#include "16-coroutine/coroutine_pool.h"
#include "16-coroutine/async_io.h"

// 17-backtracking
#include "17-backtracking/combination.h"
#include "17-backtracking/generate_parentheses.h"
#include "17-backtracking/n_queens.h"
#include "17-backtracking/palindrome_partition.h"
#include "17-backtracking/permutation.h"
#include "17-backtracking/sudoku.h"
#include "17-backtracking/word_search.h"

// 18-two-pointers
#include "18-two-pointers/two_sum_sorted.h"
#include "18-two-pointers/three_sum.h"
#include "18-two-pointers/four_sum.h"
#include "18-two-pointers/container_water.h"
#include "18-two-pointers/trapping_rain.h"
#include "18-two-pointers/remove_duplicates.h"
#include "18-two-pointers/move_zeroes.h"
#include "18-two-pointers/sort_colors.h"
#include "18-two-pointers/partition_list.h"

// 19-bit-manipulation
#include "19-bit-manipulation/single_number.h"
#include "19-bit-manipulation/power_of_two.h"
#include "19-bit-manipulation/reverse_bits.h"
#include "19-bit-manipulation/missing_number.h"
#include "19-bit-manipulation/bitwise_and_range.h"
#include "19-bit-manipulation/divide_without_operator.h"
#include "19-bit-manipulation/counting_bits.h"

// 20-heap
#include "20-heap/heap_impl.h"
#include "20-heap/kth_largest.h"
#include "20-heap/find_median.h"
#include "20-heap/top_k_frequent.h"
#include "20-heap/ugly_number.h"

// 21-system-design
#include "21-system-design/bloomfilter.h"
#include "21-system-design/consistent_hash.h"
#include "21-system-design/rate_limiter.h"
#include "21-system-design/skiplist.h"
#include "21-system-design/timer_wheel.h"

namespace interview {

void runAllTests() {
    std::cout << "========================================\n";
    std::cout << "       Interview Coding Tests          \n";
    std::cout << "========================================\n\n";

    // ========== 01-basic ==========
    std::cout << ">>> 01-basic <<<\n";
    EasyProblems::runTests();
    // Fibonacci::runTests();

    // ========== 02-linked-list ==========
    // std::cout << "\n>>> 02-linked-list <<<\n";
    // ReverseList::runTests();
    // CycleDetection::runTests();
    // MergeLists::runTests();
    // TwoPointerProblems::runTests();
    // CopyRandomList::runTests();
    // LRUCacheLinkedList::runTests();
    // LinkedListSort::runTests();

    // ========== 03-tree ==========
    // std::cout << "\n>>> 03-tree <<<\n";
    // TreeTraversal::runTests();
    // TreeProperties::runTests();
    // BSTOperations::runTests();
    // LCA::runTests();
    // PathProblems::runTests();
    // ConstructTree::runTests();
    // SerializeTree::runTests();

    // ========== 04-dp ==========
    // std::cout << "\n>>> 04-dp <<<\n";
    // CoinChange::runTests();
    // Knapsack::runTests();
    // LinearDP::runTests();
    // SubsequenceDP::runTests();
    // IntervalDP::runTests();

    // ========== 05-string ==========
    // std::cout << "\n>>> 05-string <<<\n";
    // PalindromeProblems::runTests();
    // SlidingWindowProblems::runTests();
    // StringMatching::runTests();
    // StringOperations::runTests();
    // TrieProblems::runTests();

    // ========== 06-stack-queue ==========
    // std::cout << "\n>>> 06-stack-queue <<<\n";
    // MonotonicQueueProblems::runTests();
    // MonotonicStackProblems::runTests();
    // StackImplProblems::runTests();

    // ========== 07-hash-design ==========
    // std::cout << "\n>>> 07-hash-design <<<\n";
    // HashMapImplProblems::runTests();
    // LRUCacheHash::runTests();
    // LFUCacheProblems::runTests();
    // RandomSetProblems::runTests();
    // TwitterDesign::runTests();

    // ========== 08-sorting ==========
    // std::cout << "\n>>> 08-sorting <<<\n";
    // ComparisonSorts::runTests();
    // NonComparisonSorts::runTests();
    // TopK::runTests();
    // MergeKLists::runTests();

    // ========== 09-graph ==========
    // std::cout << "\n>>> 09-graph <<<\n";
    // GraphTraversal::runTests();
    // TopologicalSort::runTests();
    // ShortestPath::runTests();
    // UnionFindProblems::runTests();
    // MST::runTests();

    // ========== 10-smart-pointer ==========
    // std::cout << "\n>>> 10-smart-pointer <<<\n";
    // SmartPointer::runTests();
    // SharedPtrImpl::runTests();
    // WeakPtrImpl::runTests();
    // MakeSharedImpl::runTests();

    // ========== 11-memory ==========
    // std::cout << "\n>>> 11-memory <<<\n";
    // MemoryPoolImpl::runTests();
    // ObjectPoolImpl::runTests();
    // ArenaAllocatorImpl::runTests();
    // CustomAllocatorImpl::runTests();

    // ========== 12-oop-design ==========
    // std::cout << "\n>>> 12-oop-design <<<\n";
    // Singleton::runTests();
    // FactoryImpl::runTests();
    // ObserverImpl::runTests();
    // StrategyImpl::runTests();
    // PimplImpl::runTests();
    // CRTPImpl::runTests();
    // TypeErasureImpl::runTests();

    // ========== 13-thread ==========
    // std::cout << "\n>>> 13-thread <<<\n";
    // BasicSync::runTests();
    // Barrier::runTests();
    // DiningPhilosophers::runTests();
    // PrintAlternately::runTests();
    // ProducerConsumer::runTests();
    // ReadWriteLock::runTests();
    // ThreadPool::runTests();
    // ThreadSafeQueue::runTests();

    // ========== 14-lockfree ==========
    // std::cout << "\n>>> 14-lockfree <<<\n";
    // Spinlock::runTests();
    // LockFreeStack::runTests();
    // SPSCQueue::runTests();
    // MPMCQueue::runTests();
    // HazardPointer::runTests();

    // ========== 15-template ==========
    // std::cout << "\n>>> 15-template <<<\n";
    // SfinaeImpl::runTests();
    // TypeTraitsImpl::runTests();
    // VariadicTemplatesImpl::runTests();
    // TypeErasureTemplateImpl::runTests();

    // ========== 16-coroutine ==========
    // std::cout << "\n>>> 16-coroutine <<<\n";
    // GeneratorImpl::runTests();
    // TaskImpl::runTests();
    // SchedulerImpl::runTests();
    // CoroutinePoolImpl::runTests();
    // AsyncIOImpl::runTests();

    // ========== 17-backtracking ==========
    // std::cout << "\n>>> 17-backtracking <<<\n";
    // Combination::runTests();
    // GenerateParentheses::runTests();
    // NQueens::runTests();
    // PalindromePartition::runTests();
    // Permutation::runTests();
    // Sudoku::runTests();
    // WordSearch::runTests();

    // ========== 18-two-pointers ==========
    // std::cout << "\n>>> 18-two-pointers <<<\n";
    // TwoSumSortedImpl::runTests();
    // ThreeSumImpl::runTests();
    // FourSumImpl::runTests();
    // ContainerWaterImpl::runTests();
    // TrappingRainImpl::runTests();
    // RemoveDuplicatesImpl::runTests();
    // MoveZeroesImpl::runTests();
    // SortColorsImpl::runTests();
    // PartitionListImpl::runTests();

    // ========== 19-bit-manipulation ==========
    // std::cout << "\n>>> 19-bit-manipulation <<<\n";
    // SingleNumberImpl::runTests();
    // PowerOfTwoImpl::runTests();
    // ReverseBitsImpl::runTests();
    // MissingNumberImpl::runTests();
    // BitwiseAndRangeImpl::runTests();
    // DivideWithoutOperatorImpl::runTests();
    // CountingBitsImpl::runTests();

    // ========== 20-heap ==========
    // std::cout << "\n>>> 20-heap <<<\n";
    // HeapImplImpl::runTests();
    // KthLargestImpl::runTests();
    // FindMedianImpl::runTests();
    // TopKFrequentImpl::runTests();
    // UglyNumberImpl::runTests();

    // ========== 21-system-design ==========
    // std::cout << "\n>>> 21-system-design <<<\n";
    // BloomFilterImpl::runTests();
    // ConsistentHashImpl::runTests();
    // RateLimiterImpl::runTests();
    // SkiplistImpl::runTests();
    // TimerWheelImpl::runTests();

    std::cout << "\n========================================\n";
    std::cout << "       All Tests Completed!            \n";
    std::cout << "========================================\n";
}

// void runCategoryTests(const char* category) {
//     if (std::strcmp(category, "01-basic") == 0) {
//         std::cout << ">>> 01-basic <<<\n";
//         EasyProblems::runTests();
//         Fibonacci::runTests();
//     }
//     else if (std::strcmp(category, "02-linked-list") == 0) {
//         std::cout << ">>> 02-linked-list <<<\n";
//         ReverseList::runTests();
//         CycleDetection::runTests();
//         MergeLists::runTests();
//         TwoPointerProblems::runTests();
//         CopyRandomList::runTests();
//         LRUCacheLinkedList::runTests();
//         LinkedListSort::runTests();
//     }
//     else if (std::strcmp(category, "03-tree") == 0) {
//         std::cout << ">>> 03-tree <<<\n";
//         TreeTraversal::runTests();
//         TreeProperties::runTests();
//         BSTOperations::runTests();
//         LCA::runTests();
//         PathProblems::runTests();
//         ConstructTree::runTests();
//         SerializeTree::runTests();
//     }
//     else if (std::strcmp(category, "04-dp") == 0) {
//         std::cout << ">>> 04-dp <<<\n";
//         CoinChange::runTests();
//         Knapsack::runTests();
//         LinearDP::runTests();
//         SubsequenceDP::runTests();
//         IntervalDP::runTests();
//     }
//     else if (std::strcmp(category, "05-string") == 0) {
//         std::cout << ">>> 05-string <<<\n";
//         PalindromeProblems::runTests();
//         SlidingWindowProblems::runTests();
//         StringMatching::runTests();
//         StringOperations::runTests();
//         TrieProblems::runTests();
//     }
//     else if (std::strcmp(category, "06-stack-queue") == 0) {
//         std::cout << ">>> 06-stack-queue <<<\n";
//         MonotonicQueueProblems::runTests();
//         MonotonicStackProblems::runTests();
//         StackImplProblems::runTests();
//     }
//     else if (std::strcmp(category, "07-hash-design") == 0) {
//         std::cout << ">>> 07-hash-design <<<\n";
//         HashMapImplProblems::runTests();
//         LRUCacheHash::runTests();
//         LFUCacheProblems::runTests();
//         RandomSetProblems::runTests();
//         TwitterDesign::runTests();
//     }
//     else if (std::strcmp(category, "08-sorting") == 0) {
//         std::cout << ">>> 08-sorting <<<\n";
//         ComparisonSorts::runTests();
//         NonComparisonSorts::runTests();
//         TopK::runTests();
//         MergeKLists::runTests();
//     }
//     else if (std::strcmp(category, "09-graph") == 0) {
//         std::cout << ">>> 09-graph <<<\n";
//         GraphTraversal::runTests();
//         TopologicalSort::runTests();
//         ShortestPath::runTests();
//         UnionFindProblems::runTests();
//         MST::runTests();
//     }
//     else if (std::strcmp(category, "10-smart-pointer") == 0) {
//         std::cout << ">>> 10-smart-pointer <<<\n";
//         SmartPointer::runTests();
//         SharedPtrImpl::runTests();
//         WeakPtrImpl::runTests();
//         MakeSharedImpl::runTests();
//     }
//     else if (std::strcmp(category, "11-memory") == 0) {
//         std::cout << ">>> 11-memory <<<\n";
//         MemoryPoolImpl::runTests();
//         ObjectPoolImpl::runTests();
//         ArenaAllocatorImpl::runTests();
//         CustomAllocatorImpl::runTests();
//     }
//     else if (std::strcmp(category, "12-oop-design") == 0) {
//         std::cout << ">>> 12-oop-design <<<\n";
//         Singleton::runTests();
//         FactoryImpl::runTests();
//         ObserverImpl::runTests();
//         StrategyImpl::runTests();
//         PimplImpl::runTests();
//         CRTPImpl::runTests();
//         TypeErasureImpl::runTests();
//     }
//     else if (std::strcmp(category, "13-thread") == 0) {
//         std::cout << ">>> 13-thread <<<\n";
//         BasicSync::runTests();
//         // Barrier::runTests();
//         // DiningPhilosophers::runTests();
//         PrintAlternately::runTests();
//         // ProducerConsumer::runTests();
//         ReadWriteLock::runTests();
//         ThreadPool::runTests();
//         ThreadSafeQueue::runTests();
//     }
//     else if (std::strcmp(category, "14-lockfree") == 0) {
//         std::cout << ">>> 14-lockfree <<<\n";
//         Spinlock::runTests();
//         LockFreeStack::runTests();
//         SPSCQueue::runTests();
//         MPMCQueue::runTests();
//         HazardPointer::runTests();
//     }
//     else if (std::strcmp(category, "15-template") == 0) {
//         std::cout << ">>> 15-template <<<\n";
//         SfinaeImpl::runTests();
//         TypeTraitsImpl::runTests();
//         VariadicTemplatesImpl::runTests();
//         TypeErasureTemplateImpl::runTests();
//     }
//     else if (std::strcmp(category, "16-coroutine") == 0) {
//         std::cout << ">>> 16-coroutine <<<\n";
//         GeneratorImpl::runTests();
//         TaskImpl::runTests();
//         SchedulerImpl::runTests();
//         CoroutinePoolImpl::runTests();
//         AsyncIOImpl::runTests();
//     }
//     else if (std::strcmp(category, "17-backtracking") == 0) {
//         std::cout << ">>> 17-backtracking <<<\n";
//         Combination::runTests();
//         GenerateParentheses::runTests();
//         NQueens::runTests();
//         PalindromePartition::runTests();
//         Permutation::runTests();
//         Sudoku::runTests();
//         WordSearch::runTests();
//     }
//     else if (std::strcmp(category, "18-two-pointers") == 0) {
//         std::cout << ">>> 18-two-pointers <<<\n";
//         TwoSumSortedImpl::runTests();
//         ThreeSumImpl::runTests();
//         FourSumImpl::runTests();
//         ContainerWaterImpl::runTests();
//         TrappingRainImpl::runTests();
//         RemoveDuplicatesImpl::runTests();
//         MoveZeroesImpl::runTests();
//         SortColorsImpl::runTests();
//         PartitionListImpl::runTests();
//     }
//     else if (std::strcmp(category, "19-bit-manipulation") == 0) {
//         std::cout << ">>> 19-bit-manipulation <<<\n";
//         SingleNumberImpl::runTests();
//         PowerOfTwoImpl::runTests();
//         ReverseBitsImpl::runTests();
//         MissingNumberImpl::runTests();
//         BitwiseAndRangeImpl::runTests();
//         DivideWithoutOperatorImpl::runTests();
//         CountingBitsImpl::runTests();
//     }
//     else if (std::strcmp(category, "20-heap") == 0) {
//         std::cout << ">>> 20-heap <<<\n";
//         HeapImplImpl::runTests();
//         KthLargestImpl::runTests();
//         FindMedianImpl::runTests();
//         TopKFrequentImpl::runTests();
//         UglyNumberImpl::runTests();
//     }
//     else if (std::strcmp(category, "21-system-design") == 0) {
//         std::cout << ">>> 21-system-design <<<\n";
//         BloomFilterImpl::runTests();
//         ConsistentHashImpl::runTests();
//         RateLimiterImpl::runTests();
//         SkiplistImpl::runTests();
//         TimerWheelImpl::runTests();
//     }
//     else {
//         std::cout << "Unknown category: " << category << "\n";
//         std::cout << "Available categories: 01-basic, 02-linked-list, 03-tree, 04-dp, 05-string,\n";
//         std::cout << "  06-stack-queue, 07-hash-design, 08-sorting, 09-graph, 10-smart-pointer,\n";
//         std::cout << "  11-memory, 12-oop-design, 13-thread, 14-lockfree, 15-template,\n";
//         std::cout << "  16-coroutine, 17-backtracking, 18-two-pointers, 19-bit-manipulation,\n";
//         std::cout << "  20-heap, 21-system-design\n";
//     }
// }

} // namespace interview
