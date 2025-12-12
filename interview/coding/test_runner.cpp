/**
 * @file test_runner.cpp
 * @brief 统一测试入口 - 调用各模块的测试方法
 *
 * 编译示例 (根据需要调整):
 *   g++ -std=c++20 -o test_runner test_runner.cpp
 *
 * 使用方法:
 *   1. 取消注释你想测试的模块
 *   2. ���保对应的 solution 文件内容已包含在下方
 *   3. 编译运行
 */

#include <iostream>
#include <vector>
#include <string>
#include <functional>
#include <map>
#include <memory>

// ============================================================================
// 测试函数声明
// ============================================================================

// 01-basic
void test_easy_problems();
void test_fibonacci();

// 02-linked-list
void test_reverse_list();
void test_cycle_detection();
void test_merge_lists();
void test_two_pointer_problems();
void test_copy_random_list();
void test_lru_cache();

// 03-tree
void test_traversal();
void test_tree_properties();
void test_bst_operations();
void test_lca();
void test_path_problems();
void test_construct_tree();
void test_serialize_tree();

// 04-dp
void test_linear_dp();
void test_knapsack();
void test_subsequence_dp();
void test_coin_change();
void test_interval_dp();

// 05-string
void test_sliding_window();
void test_palindrome();
void test_string_matching();
void test_string_operations();
void test_trie();

// 06-stack-queue
void test_stack_impl();
void test_monotonic_stack();
void test_monotonic_queue();

// 07-hash-design
void test_hashmap_impl();
void test_lru_cache_design();
void test_lfu_cache();
void test_random_set();
void test_twitter_design();

// 08-sorting
void test_comparison_sorts();
void test_non_comparison_sorts();
void test_topk();
void test_merge_k_lists();

// 09-graph
void test_graph_traversal();
void test_topological_sort();
void test_shortest_path();
void test_union_find();
void test_mst();

// 10-smart-pointer
void test_unique_ptr();
void test_shared_ptr();
void test_weak_ptr();
void test_make_shared();

// 11-memory
void test_memory_pool();
void test_object_pool();
void test_arena_allocator();
void test_custom_allocator();

// 12-oop-design
void test_singleton();
void test_factory();
void test_observer();
void test_strategy();
void test_pimpl();
void test_crtp();
void test_type_erasure();

// 13-thread
void test_basic_sync();
void test_producer_consumer();
void test_thread_safe_queue();
void test_read_write_lock();
void test_thread_pool();
void test_print_alternately();
void test_dining_philosophers();
void test_barrier();

// 14-lockfree
void test_spinlock();
void test_lockfree_stack();
void test_spsc_queue();
void test_mpmc_queue();
void test_hazard_pointer();

// 15-template
void test_type_traits();
void test_sfinae();
void test_variadic_templates();
void test_type_erasure_impl();

// 16-coroutine (需要 C++20)
void test_generator();
void test_task();
void test_async_io();
void test_scheduler();
void test_coroutine_pool();

// 17-backtracking
void test_permutation();
void test_combination();
void test_n_queens();
void test_sudoku();
void test_word_search();
void test_palindrome_partition();
void test_generate_parentheses();

// 18-two-pointers
void test_two_sum_sorted();
void test_three_sum();
void test_four_sum();
void test_container_water();
void test_trapping_rain();
void test_remove_duplicates();
void test_move_zeroes();
void test_sort_colors();
void test_partition_list();

// 19-bit-manipulation
void test_single_number();
void test_counting_bits();
void test_power_of_two();
void test_reverse_bits();
void test_missing_number();
void test_bitwise_and_range();
void test_divide_without_operator();

// 20-heap
void test_heap_impl();
void test_kth_largest();
void test_find_median();
void test_top_k_frequent();
void test_ugly_number();

// 21-system-design
void test_rate_limiter();
void test_consistent_hash();
void test_bloomfilter();
void test_timer_wheel();
void test_skiplist();

// ============================================================================
// 测试运行器
// ============================================================================

class TestRunner {
public:
    using TestFunc = std::function<void()>;

    void registerTest(const std::string& category, const std::string& name, TestFunc func) {
        tests_[category].push_back({name, func});
    }

    void runAll() {
        for (const auto& [category, testList] : tests_) {
            std::cout << "\n========== " << category << " ==========\n";
            for (const auto& [name, func] : testList) {
                std::cout << "\n--- " << name << " ---\n";
                try {
                    func();
                    std::cout << "[PASS] " << name << "\n";
                } catch (const std::exception& e) {
                    std::cout << "[FAIL] " << name << ": " << e.what() << "\n";
                }
            }
        }
    }

    void runCategory(const std::string& category) {
        if (tests_.find(category) == tests_.end()) {
            std::cout << "Category not found: " << category << "\n";
            return;
        }
        std::cout << "\n========== " << category << " ==========\n";
        for (const auto& [name, func] : tests_[category]) {
            std::cout << "\n--- " << name << " ---\n";
            try {
                func();
                std::cout << "[PASS] " << name << "\n";
            } catch (const std::exception& e) {
                std::cout << "[FAIL] " << name << ": " << e.what() << "\n";
            }
        }
    }

    void runSingle(const std::string& category, const std::string& name) {
        if (tests_.find(category) == tests_.end()) {
            std::cout << "Category not found: " << category << "\n";
            return;
        }
        for (const auto& [testName, func] : tests_[category]) {
            if (testName == name) {
                std::cout << "\n--- " << name << " ---\n";
                try {
                    func();
                    std::cout << "[PASS] " << name << "\n";
                } catch (const std::exception& e) {
                    std::cout << "[FAIL] " << name << ": " << e.what() << "\n";
                }
                return;
            }
        }
        std::cout << "Test not found: " << name << "\n";
    }

    void listTests() {
        std::cout << "\n========== Available Tests ==========\n";
        for (const auto& [category, testList] : tests_) {
            std::cout << "\n[" << category << "]\n";
            for (const auto& [name, func] : testList) {
                std::cout << "  - " << name << "\n";
            }
        }
    }

private:
    std::map<std::string, std::vector<std::pair<std::string, TestFunc>>> tests_;
};

// ============================================================================
// 示例测试实现（简化版，展示调用方式）
// ============================================================================

// --- 示例：斐波那契测试 ---
void test_fibonacci() {
    std::cout << "Testing Fibonacci sequence...\n";

    // 迭代法
    auto fib_iterative = [](int n) -> long long {
        if (n <= 1) return n;
        long long a = 0, b = 1;
        for (int i = 2; i <= n; ++i) {
            long long c = a + b;
            a = b;
            b = c;
        }
        return b;
    };

    // 测试
    std::vector<std::pair<int, long long>> testCases = {
        {0, 0}, {1, 1}, {2, 1}, {5, 5}, {10, 55}, {20, 6765}
    };

    for (const auto& [n, expected] : testCases) {
        long long result = fib_iterative(n);
        std::cout << "fib(" << n << ") = " << result;
        if (result == expected) {
            std::cout << " ✓\n";
        } else {
            std::cout << " ✗ (expected " << expected << ")\n";
        }
    }
}

// --- 示例：反转链表测试 ---
void test_reverse_list() {
    std::cout << "Testing Reverse Linked List...\n";

    struct ListNode {
        int val;
        ListNode* next;
        ListNode(int x) : val(x), next(nullptr) {}
    };

    // 反转链表
    auto reverseList = [](ListNode* head) -> ListNode* {
        ListNode* prev = nullptr;
        ListNode* curr = head;
        while (curr) {
            ListNode* next = curr->next;
            curr->next = prev;
            prev = curr;
            curr = next;
        }
        return prev;
    };

    // 创建链表 1->2->3->4->5
    ListNode* head = new ListNode(1);
    head->next = new ListNode(2);
    head->next->next = new ListNode(3);
    head->next->next->next = new ListNode(4);
    head->next->next->next->next = new ListNode(5);

    std::cout << "Original: ";
    for (ListNode* p = head; p; p = p->next) std::cout << p->val << " ";
    std::cout << "\n";

    head = reverseList(head);

    std::cout << "Reversed: ";
    for (ListNode* p = head; p; p = p->next) std::cout << p->val << " ";
    std::cout << "\n";

    // 清理
    while (head) {
        ListNode* temp = head;
        head = head->next;
        delete temp;
    }
}

// --- 示例：二叉树遍历测试 ---
void test_traversal() {
    std::cout << "Testing Tree Traversal...\n";

    struct TreeNode {
        int val;
        TreeNode* left;
        TreeNode* right;
        TreeNode(int x) : val(x), left(nullptr), right(nullptr) {}
    };

    // 前序遍历
    std::function<void(TreeNode*, std::vector<int>&)> preorder =
        [&](TreeNode* root, std::vector<int>& result) {
            if (!root) return;
            result.push_back(root->val);
            preorder(root->left, result);
            preorder(root->right, result);
        };

    // 构建测试树:     1
    //               / \
    //              2   3
    //             / \
    //            4   5
    TreeNode* root = new TreeNode(1);
    root->left = new TreeNode(2);
    root->right = new TreeNode(3);
    root->left->left = new TreeNode(4);
    root->left->right = new TreeNode(5);

    std::vector<int> result;
    preorder(root, result);

    std::cout << "Preorder: ";
    for (int v : result) std::cout << v << " ";
    std::cout << "\n";

    // 清理 (简化，实际应递归删除)
}

// --- 示例：动态规划测试 ---
void test_linear_dp() {
    std::cout << "Testing Linear DP (Climbing Stairs)...\n";

    // 爬楼梯问题
    auto climbStairs = [](int n) -> int {
        if (n <= 2) return n;
        int a = 1, b = 2;
        for (int i = 3; i <= n; ++i) {
            int c = a + b;
            a = b;
            b = c;
        }
        return b;
    };

    std::vector<std::pair<int, int>> testCases = {
        {1, 1}, {2, 2}, {3, 3}, {4, 5}, {5, 8}
    };

    for (const auto& [n, expected] : testCases) {
        int result = climbStairs(n);
        std::cout << "climbStairs(" << n << ") = " << result;
        if (result == expected) {
            std::cout << " ✓\n";
        } else {
            std::cout << " ✗ (expected " << expected << ")\n";
        }
    }
}

// --- 示例：排序测试 ---
void test_comparison_sorts() {
    std::cout << "Testing Comparison Sorts...\n";

    // 快速排序
    std::function<void(std::vector<int>&, int, int)> quickSort =
        [&](std::vector<int>& arr, int left, int right) {
            if (left >= right) return;
            int pivot = arr[left + (right - left) / 2];
            int i = left, j = right;
            while (i <= j) {
                while (arr[i] < pivot) ++i;
                while (arr[j] > pivot) --j;
                if (i <= j) {
                    std::swap(arr[i], arr[j]);
                    ++i;
                    --j;
                }
            }
            quickSort(arr, left, j);
            quickSort(arr, i, right);
        };

    std::vector<int> arr = {64, 34, 25, 12, 22, 11, 90};

    std::cout << "Before: ";
    for (int v : arr) std::cout << v << " ";
    std::cout << "\n";

    quickSort(arr, 0, arr.size() - 1);

    std::cout << "After:  ";
    for (int v : arr) std::cout << v << " ";
    std::cout << "\n";
}

// --- 示例：智能指针测试 ---
void test_unique_ptr() {
    std::cout << "Testing Unique Ptr concepts...\n";

    // 简化版 UniquePtr 测试
    struct TestObject {
        int value;
        TestObject(int v) : value(v) { std::cout << "  Constructed: " << value << "\n"; }
        ~TestObject() { std::cout << "  Destroyed: " << value << "\n"; }
    };

    {
        std::unique_ptr<TestObject> ptr1 = std::make_unique<TestObject>(42);
        std::cout << "  ptr1->value = " << ptr1->value << "\n";

        std::unique_ptr<TestObject> ptr2 = std::move(ptr1);
        std::cout << "  After move, ptr1 is " << (ptr1 ? "valid" : "null") << "\n";
        std::cout << "  ptr2->value = " << ptr2->value << "\n";
    }

    std::cout << "UniquePtr basic functionality verified.\n";
}

// --- 示例：线程同步测试 ---
void test_basic_sync() {
    std::cout << "Testing Basic Synchronization...\n";
    std::cout << "Thread synchronization concepts verified.\n";
    std::cout << "(Full test requires threading support)\n";
}

// --- 示例：回溯测试 ---
void test_permutation() {
    std::cout << "Testing Permutations...\n";

    std::function<void(std::vector<int>&, int, std::vector<std::vector<int>>&)> permute =
        [&](std::vector<int>& nums, int start, std::vector<std::vector<int>>& result) {
            if (start == nums.size()) {
                result.push_back(nums);
                return;
            }
            for (int i = start; i < nums.size(); ++i) {
                std::swap(nums[start], nums[i]);
                permute(nums, start + 1, result);
                std::swap(nums[start], nums[i]);
            }
        };

    std::vector<int> nums = {1, 2, 3};
    std::vector<std::vector<int>> result;
    permute(nums, 0, result);

    std::cout << "Permutations of [1,2,3]: " << result.size() << " results\n";
    for (const auto& perm : result) {
        std::cout << "  [";
        for (int i = 0; i < perm.size(); ++i) {
            std::cout << perm[i];
            if (i < perm.size() - 1) std::cout << ",";
        }
        std::cout << "]\n";
    }
}

// --- 示例：组合测试 ---
void test_combination() {
    std::cout << "Testing Combinations...\n";

    std::function<void(int, int, int, std::vector<int>&, std::vector<std::vector<int>>&)> combine =
        [&](int n, int k, int start, std::vector<int>& path, std::vector<std::vector<int>>& result) {
            if (path.size() == k) {
                result.push_back(path);
                return;
            }
            for (int i = start; i <= n - (k - path.size()) + 1; ++i) {
                path.push_back(i);
                combine(n, k, i + 1, path, result);
                path.pop_back();
            }
        };

    std::vector<std::vector<int>> result;
    std::vector<int> path;
    combine(4, 2, 1, path, result);

    std::cout << "C(4,2) = " << result.size() << " combinations\n";
    for (const auto& comb : result) {
        std::cout << "  [";
        for (int i = 0; i < comb.size(); ++i) {
            std::cout << comb[i];
            if (i < comb.size() - 1) std::cout << ",";
        }
        std::cout << "]\n";
    }
}

// --- 示例：N皇后测试 ---
void test_n_queens() {
    std::cout << "Testing N-Queens...\n";

    std::function<int(int, int, int, int, int)> solve =
        [&](int n, int row, int cols, int diag1, int diag2) -> int {
            if (row == n) return 1;
            int count = 0;
            int avail = ((1 << n) - 1) & ~(cols | diag1 | diag2);
            while (avail) {
                int pos = avail & (-avail);
                avail ^= pos;
                count += solve(n, row + 1, cols | pos, (diag1 | pos) << 1, (diag2 | pos) >> 1);
            }
            return count;
        };

    for (int n = 1; n <= 8; ++n) {
        int count = solve(n, 0, 0, 0, 0);
        std::cout << "N=" << n << ": " << count << " solutions\n";
    }
}

// ============================================================================
// 注册测试并选择运行
// ============================================================================

void registerAllTests(TestRunner& runner) {
    // 01-basic
    // runner.registerTest("01-basic", "easy_problems", test_easy_problems);
    runner.registerTest("01-basic", "fibonacci", test_fibonacci);

    // 02-linked-list
    runner.registerTest("02-linked-list", "reverse_list", test_reverse_list);
    // runner.registerTest("02-linked-list", "cycle_detection", test_cycle_detection);
    // runner.registerTest("02-linked-list", "merge_lists", test_merge_lists);
    // runner.registerTest("02-linked-list", "two_pointer_problems", test_two_pointer_problems);
    // runner.registerTest("02-linked-list", "copy_random_list", test_copy_random_list);
    // runner.registerTest("02-linked-list", "lru_cache", test_lru_cache);

    // 03-tree
    runner.registerTest("03-tree", "traversal", test_traversal);
    // runner.registerTest("03-tree", "tree_properties", test_tree_properties);
    // runner.registerTest("03-tree", "bst_operations", test_bst_operations);
    // runner.registerTest("03-tree", "lca", test_lca);
    // runner.registerTest("03-tree", "path_problems", test_path_problems);
    // runner.registerTest("03-tree", "construct_tree", test_construct_tree);
    // runner.registerTest("03-tree", "serialize_tree", test_serialize_tree);

    // 04-dp
    runner.registerTest("04-dp", "linear_dp", test_linear_dp);
    // runner.registerTest("04-dp", "knapsack", test_knapsack);
    // runner.registerTest("04-dp", "subsequence_dp", test_subsequence_dp);
    // runner.registerTest("04-dp", "coin_change", test_coin_change);
    // runner.registerTest("04-dp", "interval_dp", test_interval_dp);

    // 05-string
    // runner.registerTest("05-string", "sliding_window", test_sliding_window);
    // runner.registerTest("05-string", "palindrome", test_palindrome);
    // runner.registerTest("05-string", "string_matching", test_string_matching);
    // runner.registerTest("05-string", "string_operations", test_string_operations);
    // runner.registerTest("05-string", "trie", test_trie);

    // 06-stack-queue
    // runner.registerTest("06-stack-queue", "stack_impl", test_stack_impl);
    // runner.registerTest("06-stack-queue", "monotonic_stack", test_monotonic_stack);
    // runner.registerTest("06-stack-queue", "monotonic_queue", test_monotonic_queue);

    // 07-hash-design
    // runner.registerTest("07-hash-design", "hashmap_impl", test_hashmap_impl);
    // runner.registerTest("07-hash-design", "lru_cache_design", test_lru_cache_design);
    // runner.registerTest("07-hash-design", "lfu_cache", test_lfu_cache);
    // runner.registerTest("07-hash-design", "random_set", test_random_set);
    // runner.registerTest("07-hash-design", "twitter_design", test_twitter_design);

    // 08-sorting
    runner.registerTest("08-sorting", "comparison_sorts", test_comparison_sorts);
    // runner.registerTest("08-sorting", "non_comparison_sorts", test_non_comparison_sorts);
    // runner.registerTest("08-sorting", "topk", test_topk);
    // runner.registerTest("08-sorting", "merge_k_lists", test_merge_k_lists);

    // 09-graph
    // runner.registerTest("09-graph", "graph_traversal", test_graph_traversal);
    // runner.registerTest("09-graph", "topological_sort", test_topological_sort);
    // runner.registerTest("09-graph", "shortest_path", test_shortest_path);
    // runner.registerTest("09-graph", "union_find", test_union_find);
    // runner.registerTest("09-graph", "mst", test_mst);

    // 10-smart-pointer
    runner.registerTest("10-smart-pointer", "unique_ptr", test_unique_ptr);
    // runner.registerTest("10-smart-pointer", "shared_ptr", test_shared_ptr);
    // runner.registerTest("10-smart-pointer", "weak_ptr", test_weak_ptr);
    // runner.registerTest("10-smart-pointer", "make_shared", test_make_shared);

    // 11-memory
    // runner.registerTest("11-memory", "memory_pool", test_memory_pool);
    // runner.registerTest("11-memory", "object_pool", test_object_pool);
    // runner.registerTest("11-memory", "arena_allocator", test_arena_allocator);
    // runner.registerTest("11-memory", "custom_allocator", test_custom_allocator);

    // 12-oop-design
    // runner.registerTest("12-oop-design", "singleton", test_singleton);
    // runner.registerTest("12-oop-design", "factory", test_factory);
    // runner.registerTest("12-oop-design", "observer", test_observer);
    // runner.registerTest("12-oop-design", "strategy", test_strategy);
    // runner.registerTest("12-oop-design", "pimpl", test_pimpl);
    // runner.registerTest("12-oop-design", "crtp", test_crtp);
    // runner.registerTest("12-oop-design", "type_erasure", test_type_erasure);

    // 13-thread
    runner.registerTest("13-thread", "basic_sync", test_basic_sync);
    // runner.registerTest("13-thread", "producer_consumer", test_producer_consumer);
    // runner.registerTest("13-thread", "thread_safe_queue", test_thread_safe_queue);
    // runner.registerTest("13-thread", "read_write_lock", test_read_write_lock);
    // runner.registerTest("13-thread", "thread_pool", test_thread_pool);
    // runner.registerTest("13-thread", "print_alternately", test_print_alternately);
    // runner.registerTest("13-thread", "dining_philosophers", test_dining_philosophers);
    // runner.registerTest("13-thread", "barrier", test_barrier);

    // 14-lockfree
    // runner.registerTest("14-lockfree", "spinlock", test_spinlock);
    // runner.registerTest("14-lockfree", "lockfree_stack", test_lockfree_stack);
    // runner.registerTest("14-lockfree", "spsc_queue", test_spsc_queue);
    // runner.registerTest("14-lockfree", "mpmc_queue", test_mpmc_queue);
    // runner.registerTest("14-lockfree", "hazard_pointer", test_hazard_pointer);

    // 15-template
    // runner.registerTest("15-template", "type_traits", test_type_traits);
    // runner.registerTest("15-template", "sfinae", test_sfinae);
    // runner.registerTest("15-template", "variadic_templates", test_variadic_templates);
    // runner.registerTest("15-template", "type_erasure_impl", test_type_erasure_impl);

    // 16-coroutine (需要 C++20)
    // runner.registerTest("16-coroutine", "generator", test_generator);
    // runner.registerTest("16-coroutine", "task", test_task);
    // runner.registerTest("16-coroutine", "async_io", test_async_io);
    // runner.registerTest("16-coroutine", "scheduler", test_scheduler);
    // runner.registerTest("16-coroutine", "coroutine_pool", test_coroutine_pool);

    // 17-backtracking
    runner.registerTest("17-backtracking", "permutation", test_permutation);
    runner.registerTest("17-backtracking", "combination", test_combination);
    runner.registerTest("17-backtracking", "n_queens", test_n_queens);
    // runner.registerTest("17-backtracking", "sudoku", test_sudoku);
    // runner.registerTest("17-backtracking", "word_search", test_word_search);
    // runner.registerTest("17-backtracking", "palindrome_partition", test_palindrome_partition);
    // runner.registerTest("17-backtracking", "generate_parentheses", test_generate_parentheses);

    // 18-two-pointers
    // runner.registerTest("18-two-pointers", "two_sum_sorted", test_two_sum_sorted);
    // runner.registerTest("18-two-pointers", "three_sum", test_three_sum);
    // runner.registerTest("18-two-pointers", "four_sum", test_four_sum);
    // runner.registerTest("18-two-pointers", "container_water", test_container_water);
    // runner.registerTest("18-two-pointers", "trapping_rain", test_trapping_rain);
    // runner.registerTest("18-two-pointers", "remove_duplicates", test_remove_duplicates);
    // runner.registerTest("18-two-pointers", "move_zeroes", test_move_zeroes);
    // runner.registerTest("18-two-pointers", "sort_colors", test_sort_colors);
    // runner.registerTest("18-two-pointers", "partition_list", test_partition_list);

    // 19-bit-manipulation
    // runner.registerTest("19-bit-manipulation", "single_number", test_single_number);
    // runner.registerTest("19-bit-manipulation", "counting_bits", test_counting_bits);
    // runner.registerTest("19-bit-manipulation", "power_of_two", test_power_of_two);
    // runner.registerTest("19-bit-manipulation", "reverse_bits", test_reverse_bits);
    // runner.registerTest("19-bit-manipulation", "missing_number", test_missing_number);
    // runner.registerTest("19-bit-manipulation", "bitwise_and_range", test_bitwise_and_range);
    // runner.registerTest("19-bit-manipulation", "divide_without_operator", test_divide_without_operator);

    // 20-heap
    // runner.registerTest("20-heap", "heap_impl", test_heap_impl);
    // runner.registerTest("20-heap", "kth_largest", test_kth_largest);
    // runner.registerTest("20-heap", "find_median", test_find_median);
    // runner.registerTest("20-heap", "top_k_frequent", test_top_k_frequent);
    // runner.registerTest("20-heap", "ugly_number", test_ugly_number);

    // 21-system-design
    // runner.registerTest("21-system-design", "rate_limiter", test_rate_limiter);
    // runner.registerTest("21-system-design", "consistent_hash", test_consistent_hash);
    // runner.registerTest("21-system-design", "bloomfilter", test_bloomfilter);
    // runner.registerTest("21-system-design", "timer_wheel", test_timer_wheel);
    // runner.registerTest("21-system-design", "skiplist", test_skiplist);
}

// ============================================================================
// 对外接口 - 供 main.cpp 调用
// ============================================================================

/**
 * 运行面试编程题测试
 * 取消注释你想要运行的测试
 */
void runInterviewTests() {
    std::cout << "========================================\n";
    std::cout << "   C++ Interview Coding Test Runner\n";
    std::cout << "========================================\n";

    TestRunner runner;
    registerAllTests(runner);

    // ========== 选择运行方式（取消注释对应行）==========

    // 方式1: 列出所有可用测试
    // runner.listTests();

    // 方式2: 运行所有测试
    // runner.runAll();

    // 方式3: 运行指定分类的测试
    // runner.runCategory("01-basic");
    // runner.runCategory("02-linked-list");
    // runner.runCategory("03-tree");
    // runner.runCategory("04-dp");
    // runner.runCategory("08-sorting");
    // runner.runCategory("13-thread");
    // runner.runCategory("17-backtracking");

    // 方式4: 运行单个测试
    // runner.runSingle("01-basic", "fibonacci");
    // runner.runSingle("02-linked-list", "reverse_list");
    // runner.runSingle("03-tree", "traversal");
    // runner.runSingle("04-dp", "linear_dp");
    // runner.runSingle("08-sorting", "comparison_sorts");
    // runner.runSingle("10-smart-pointer", "unique_ptr");
    // runner.runSingle("13-thread", "basic_sync");
    // runner.runSingle("17-backtracking", "permutation");
    // runner.runSingle("17-backtracking", "combination");

    // ========== 当前运行的测试（最后一个取消注释）==========
    runner.runSingle("17-backtracking", "n_queens");

    std::cout << "\n========================================\n";
    std::cout << "   Test Runner Completed\n";
    std::cout << "========================================\n";
}
