#Created by learyuan on 25-6-8.
cmake_minimum_required(VERSION 3.31)
project(project1)

set(CMAKE_CXX_STANDARD 20)

# 原有源文件
set(ORIGINAL_SOURCES
        sort/util/Util.cpp
        sort/util/Util.h
        sort/quickSort/QuickSort.cpp
        sort/quickSort/QuickSort.h
        sort/mergeSort/MergeSort.cpp
        sort/mergeSort/MergeSort.h
        sort/other/StrCountSort.cpp
        sort/other/StrCountSort.h
        stack/IncrementalSequence.cpp
        stack/IncrementalSequence.h
        stack/Subsequence.cpp
        stack/Subsequence.h
        stack/SubSet.cpp
        stack/SubSet.h
        stack/TreeNodeTraversal.cpp
        stack/TreeNodeTraversal.h
        thread/MessageQueue.cpp
        thread/MessageQueue.h
        thread/AlternatePrint.cpp
        thread/AlternatePrint.h
)

# Interview Coding 测试入口
set(INTERVIEW_TEST_RUNNER
        interview/coding/test_runner.cpp
)

# 01-basic
set(INTERVIEW_01_BASIC
        interview/coding/01-basic/easy_problems.cpp
        interview/coding/01-basic/easy_problems_solution.cpp
        interview/coding/01-basic/fibonacci.cpp
        interview/coding/01-basic/fibonacci_solution.cpp
)

# 02-linked-list
set(INTERVIEW_02_LINKED_LIST
        interview/coding/02-linked-list/reverse_list.cpp
        interview/coding/02-linked-list/reverse_list_solution.cpp
        interview/coding/02-linked-list/cycle_detection.cpp
        interview/coding/02-linked-list/cycle_detection_solution.cpp
        interview/coding/02-linked-list/merge_lists.cpp
        interview/coding/02-linked-list/merge_lists_solution.cpp
        interview/coding/02-linked-list/two_pointer_problems.cpp
        interview/coding/02-linked-list/two_pointer_problems_solution.cpp
        interview/coding/02-linked-list/copy_random_list.cpp
        interview/coding/02-linked-list/copy_random_list_solution.cpp
        interview/coding/02-linked-list/lru_cache.cpp
        interview/coding/02-linked-list/lru_cache_solution.cpp
)

# 03-tree
set(INTERVIEW_03_TREE
        interview/coding/03-tree/traversal.cpp
        interview/coding/03-tree/traversal_solution.cpp
        interview/coding/03-tree/tree_properties.cpp
        interview/coding/03-tree/tree_properties_solution.cpp
        interview/coding/03-tree/bst_operations.cpp
        interview/coding/03-tree/bst_operations_solution.cpp
        interview/coding/03-tree/lca.cpp
        interview/coding/03-tree/lca_solution.cpp
        interview/coding/03-tree/path_problems.cpp
        interview/coding/03-tree/path_problems_solution.cpp
        interview/coding/03-tree/construct_tree.cpp
        interview/coding/03-tree/construct_tree_solution.cpp
        interview/coding/03-tree/serialize_tree.cpp
        interview/coding/03-tree/serialize_tree_solution.cpp
)

# 04-dp
set(INTERVIEW_04_DP
        interview/coding/04-dp/linear_dp.cpp
        interview/coding/04-dp/linear_dp_solution.cpp
        interview/coding/04-dp/knapsack.cpp
        interview/coding/04-dp/knapsack_solution.cpp
        interview/coding/04-dp/subsequence_dp.cpp
        interview/coding/04-dp/subsequence_dp_solution.cpp
        interview/coding/04-dp/coin_change.cpp
        interview/coding/04-dp/coin_change_solution.cpp
        interview/coding/04-dp/interval_dp.cpp
        interview/coding/04-dp/interval_dp_solution.cpp
)

# 05-string
set(INTERVIEW_05_STRING
        interview/coding/05-string/sliding_window.cpp
        interview/coding/05-string/sliding_window_solution.cpp
        interview/coding/05-string/palindrome.cpp
        interview/coding/05-string/palindrome_solution.cpp
        interview/coding/05-string/string_matching.cpp
        interview/coding/05-string/string_matching_solution.cpp
        interview/coding/05-string/string_operations.cpp
        interview/coding/05-string/string_operations_solution.cpp
        interview/coding/05-string/trie.cpp
        interview/coding/05-string/trie_solution.cpp
)

# 06-stack-queue
set(INTERVIEW_06_STACK_QUEUE
        interview/coding/06-stack-queue/stack_impl.cpp
        interview/coding/06-stack-queue/stack_impl_solution.cpp
        interview/coding/06-stack-queue/monotonic_stack.cpp
        interview/coding/06-stack-queue/monotonic_stack_solution.cpp
        interview/coding/06-stack-queue/monotonic_queue.cpp
        interview/coding/06-stack-queue/monotonic_queue_solution.cpp
)

# 07-hash-design
set(INTERVIEW_07_HASH_DESIGN
        interview/coding/07-hash-design/hashmap_impl.cpp
        interview/coding/07-hash-design/hashmap_impl_solution.cpp
        interview/coding/07-hash-design/lru_cache.cpp
        interview/coding/07-hash-design/lru_cache_solution.cpp
        interview/coding/07-hash-design/lfu_cache.cpp
        interview/coding/07-hash-design/lfu_cache_solution.cpp
        interview/coding/07-hash-design/random_set.cpp
        interview/coding/07-hash-design/random_set_solution.cpp
        interview/coding/07-hash-design/twitter_design.cpp
        interview/coding/07-hash-design/twitter_design_solution.cpp
)

# 08-sorting
set(INTERVIEW_08_SORTING
        interview/coding/08-sorting/comparison_sorts.cpp
        interview/coding/08-sorting/comparison_sorts_solution.cpp
        interview/coding/08-sorting/non_comparison_sorts.cpp
        interview/coding/08-sorting/non_comparison_sorts_solution.cpp
        interview/coding/08-sorting/topk.cpp
        interview/coding/08-sorting/topk_solution.cpp
        interview/coding/08-sorting/merge_k_lists.cpp
        interview/coding/08-sorting/merge_k_lists_solution.cpp
)

# 09-graph
set(INTERVIEW_09_GRAPH
        interview/coding/09-graph/traversal.cpp
        interview/coding/09-graph/traversal_solution.cpp
        interview/coding/09-graph/topological_sort.cpp
        interview/coding/09-graph/topological_sort_solution.cpp
        interview/coding/09-graph/shortest_path.cpp
        interview/coding/09-graph/shortest_path_solution.cpp
        interview/coding/09-graph/union_find.cpp
        interview/coding/09-graph/union_find_solution.cpp
        interview/coding/09-graph/mst.cpp
        interview/coding/09-graph/mst_solution.cpp
)

# 10-smart-pointer
set(INTERVIEW_10_SMART_POINTER
        interview/coding/10-smart-pointer/unique_ptr.cpp
        interview/coding/10-smart-pointer/unique_ptr_solution.cpp
        interview/coding/10-smart-pointer/shared_ptr.cpp
        interview/coding/10-smart-pointer/shared_ptr_solution.cpp
        interview/coding/10-smart-pointer/weak_ptr.cpp
        interview/coding/10-smart-pointer/weak_ptr_solution.cpp
        interview/coding/10-smart-pointer/make_shared.cpp
        interview/coding/10-smart-pointer/make_shared_solution.cpp
)

# 11-memory
set(INTERVIEW_11_MEMORY
        interview/coding/11-memory/memory_pool.cpp
        interview/coding/11-memory/memory_pool_solution.cpp
        interview/coding/11-memory/object_pool.cpp
        interview/coding/11-memory/object_pool_solution.cpp
        interview/coding/11-memory/arena_allocator.cpp
        interview/coding/11-memory/arena_allocator_solution.cpp
        interview/coding/11-memory/custom_allocator.cpp
        interview/coding/11-memory/custom_allocator_solution.cpp
)

# 12-oop-design
set(INTERVIEW_12_OOP_DESIGN
        interview/coding/12-oop-design/singleton.cpp
        interview/coding/12-oop-design/singleton_solution.cpp
        interview/coding/12-oop-design/factory.cpp
        interview/coding/12-oop-design/factory_solution.cpp
        interview/coding/12-oop-design/observer.cpp
        interview/coding/12-oop-design/observer_solution.cpp
        interview/coding/12-oop-design/strategy.cpp
        interview/coding/12-oop-design/strategy_solution.cpp
        interview/coding/12-oop-design/pimpl.cpp
        interview/coding/12-oop-design/pimpl_solution.cpp
        interview/coding/12-oop-design/crtp.cpp
        interview/coding/12-oop-design/crtp_solution.cpp
        interview/coding/12-oop-design/type_erasure.cpp
        interview/coding/12-oop-design/type_erasure_solution.cpp
)

# 13-thread
set(INTERVIEW_13_THREAD
        interview/coding/13-thread/basic_sync.cpp
        interview/coding/13-thread/basic_sync_solution.cpp
        interview/coding/13-thread/producer_consumer.cpp
        interview/coding/13-thread/producer_consumer_solution.cpp
        interview/coding/13-thread/thread_safe_queue.cpp
        interview/coding/13-thread/thread_safe_queue_solution.cpp
        interview/coding/13-thread/read_write_lock.cpp
        interview/coding/13-thread/read_write_lock_solution.cpp
        interview/coding/13-thread/thread_pool.cpp
        interview/coding/13-thread/thread_pool_solution.cpp
        interview/coding/13-thread/print_alternately.cpp
        interview/coding/13-thread/print_alternately_solution.cpp
        interview/coding/13-thread/dining_philosophers.cpp
        interview/coding/13-thread/dining_philosophers_solution.cpp
        interview/coding/13-thread/barrier.cpp
        interview/coding/13-thread/barrier_solution.cpp
)

# 14-lockfree
set(INTERVIEW_14_LOCKFREE
        interview/coding/14-lockfree/spinlock.cpp
        interview/coding/14-lockfree/spinlock_solution.cpp
        interview/coding/14-lockfree/lockfree_stack.cpp
        interview/coding/14-lockfree/lockfree_stack_solution.cpp
        interview/coding/14-lockfree/spsc_queue.cpp
        interview/coding/14-lockfree/spsc_queue_solution.cpp
        interview/coding/14-lockfree/mpmc_queue.cpp
        interview/coding/14-lockfree/mpmc_queue_solution.cpp
        interview/coding/14-lockfree/hazard_pointer.cpp
        interview/coding/14-lockfree/hazard_pointer_solution.cpp
)

# 15-template
set(INTERVIEW_15_TEMPLATE
        interview/coding/15-template/type_traits.cpp
        interview/coding/15-template/type_traits_solution.cpp
        interview/coding/15-template/sfinae.cpp
        interview/coding/15-template/sfinae_solution.cpp
        interview/coding/15-template/variadic_templates.cpp
        interview/coding/15-template/variadic_templates_solution.cpp
        interview/coding/15-template/type_erasure_impl.cpp
        interview/coding/15-template/type_erasure_impl_solution.cpp
)

# 16-coroutine
set(INTERVIEW_16_COROUTINE
        interview/coding/16-coroutine/generator.cpp
        interview/coding/16-coroutine/generator_solution.cpp
        interview/coding/16-coroutine/task.cpp
        interview/coding/16-coroutine/task_solution.cpp
        interview/coding/16-coroutine/async_io.cpp
        interview/coding/16-coroutine/async_io_solution.cpp
        interview/coding/16-coroutine/scheduler.cpp
        interview/coding/16-coroutine/scheduler_solution.cpp
        interview/coding/16-coroutine/coroutine_pool.cpp
        interview/coding/16-coroutine/coroutine_pool_solution.cpp
)

# 17-backtracking
set(INTERVIEW_17_BACKTRACKING
        interview/coding/17-backtracking/permutation.cpp
        interview/coding/17-backtracking/permutation_solution.cpp
        interview/coding/17-backtracking/combination.cpp
        interview/coding/17-backtracking/combination_solution.cpp
        interview/coding/17-backtracking/n_queens.cpp
        interview/coding/17-backtracking/n_queens_solution.cpp
        interview/coding/17-backtracking/sudoku.cpp
        interview/coding/17-backtracking/sudoku_solution.cpp
        interview/coding/17-backtracking/word_search.cpp
        interview/coding/17-backtracking/word_search_solution.cpp
        interview/coding/17-backtracking/palindrome_partition.cpp
        interview/coding/17-backtracking/palindrome_partition_solution.cpp
        interview/coding/17-backtracking/generate_parentheses.cpp
        interview/coding/17-backtracking/generate_parentheses_solution.cpp
)

# 18-two-pointers
set(INTERVIEW_18_TWO_POINTERS
        interview/coding/18-two-pointers/two_sum_sorted.cpp
        interview/coding/18-two-pointers/two_sum_sorted_solution.cpp
        interview/coding/18-two-pointers/three_sum.cpp
        interview/coding/18-two-pointers/three_sum_solution.cpp
        interview/coding/18-two-pointers/four_sum.cpp
        interview/coding/18-two-pointers/four_sum_solution.cpp
        interview/coding/18-two-pointers/container_water.cpp
        interview/coding/18-two-pointers/container_water_solution.cpp
        interview/coding/18-two-pointers/trapping_rain.cpp
        interview/coding/18-two-pointers/trapping_rain_solution.cpp
        interview/coding/18-two-pointers/remove_duplicates.cpp
        interview/coding/18-two-pointers/remove_duplicates_solution.cpp
        interview/coding/18-two-pointers/move_zeroes.cpp
        interview/coding/18-two-pointers/move_zeroes_solution.cpp
        interview/coding/18-two-pointers/sort_colors.cpp
        interview/coding/18-two-pointers/sort_colors_solution.cpp
        interview/coding/18-two-pointers/partition_list.cpp
        interview/coding/18-two-pointers/partition_list_solution.cpp
)

# 19-bit-manipulation
set(INTERVIEW_19_BIT_MANIPULATION
        interview/coding/19-bit-manipulation/single_number.cpp
        interview/coding/19-bit-manipulation/single_number_solution.cpp
        interview/coding/19-bit-manipulation/counting_bits.cpp
        interview/coding/19-bit-manipulation/counting_bits_solution.cpp
        interview/coding/19-bit-manipulation/power_of_two.cpp
        interview/coding/19-bit-manipulation/power_of_two_solution.cpp
        interview/coding/19-bit-manipulation/reverse_bits.cpp
        interview/coding/19-bit-manipulation/reverse_bits_solution.cpp
        interview/coding/19-bit-manipulation/missing_number.cpp
        interview/coding/19-bit-manipulation/missing_number_solution.cpp
        interview/coding/19-bit-manipulation/bitwise_and_range.cpp
        interview/coding/19-bit-manipulation/bitwise_and_range_solution.cpp
        interview/coding/19-bit-manipulation/divide_without_operator.cpp
        interview/coding/19-bit-manipulation/divide_without_operator_solution.cpp
)

# 20-heap
set(INTERVIEW_20_HEAP
        interview/coding/20-heap/heap_impl.cpp
        interview/coding/20-heap/heap_impl_solution.cpp
        interview/coding/20-heap/kth_largest.cpp
        interview/coding/20-heap/kth_largest_solution.cpp
        interview/coding/20-heap/find_median.cpp
        interview/coding/20-heap/find_median_solution.cpp
        interview/coding/20-heap/top_k_frequent.cpp
        interview/coding/20-heap/top_k_frequent_solution.cpp
        interview/coding/20-heap/ugly_number.cpp
        interview/coding/20-heap/ugly_number_solution.cpp
)

# 21-system-design
set(INTERVIEW_21_SYSTEM_DESIGN
        interview/coding/21-system-design/rate_limiter.cpp
        interview/coding/21-system-design/rate_limiter_solution.cpp
        interview/coding/21-system-design/consistent_hash.cpp
        interview/coding/21-system-design/consistent_hash_solution.cpp
        interview/coding/21-system-design/bloomfilter.cpp
        interview/coding/21-system-design/bloomfilter_solution.cpp
        interview/coding/21-system-design/timer_wheel.cpp
        interview/coding/21-system-design/timer_wheel_solution.cpp
        interview/coding/21-system-design/skiplist.cpp
        interview/coding/21-system-design/skiplist_solution.cpp
)

# 汇总所有 Interview Coding 源文件
set(INTERVIEW_CODING_SOURCES
        ${INTERVIEW_TEST_RUNNER}
        ${INTERVIEW_01_BASIC}
        ${INTERVIEW_02_LINKED_LIST}
        ${INTERVIEW_03_TREE}
        ${INTERVIEW_04_DP}
        ${INTERVIEW_05_STRING}
        ${INTERVIEW_06_STACK_QUEUE}
        ${INTERVIEW_07_HASH_DESIGN}
        ${INTERVIEW_08_SORTING}
        ${INTERVIEW_09_GRAPH}
        ${INTERVIEW_10_SMART_POINTER}
        ${INTERVIEW_11_MEMORY}
        ${INTERVIEW_12_OOP_DESIGN}
        ${INTERVIEW_13_THREAD}
        ${INTERVIEW_14_LOCKFREE}
        ${INTERVIEW_15_TEMPLATE}
        ${INTERVIEW_16_COROUTINE}
        ${INTERVIEW_17_BACKTRACKING}
        ${INTERVIEW_18_TWO_POINTERS}
        ${INTERVIEW_19_BIT_MANIPULATION}
        ${INTERVIEW_20_HEAP}
        ${INTERVIEW_21_SYSTEM_DESIGN}
)

add_executable(project1 main.cpp
        ${ORIGINAL_SOURCES}
        # ${INTERVIEW_CODING_SOURCES}  # 取消注释以包含面试编程题
)
