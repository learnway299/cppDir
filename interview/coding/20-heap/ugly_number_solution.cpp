/**
 * @file ugly_number_solution.cpp
 * @brief 丑数 - 解答
 */
#include <iostream>
#include <vector>
#include <queue>
#include <unordered_set>
using namespace std;

// 判断丑数
bool isUgly(int n) {
    if (n <= 0) return false;
    while (n % 2 == 0) n /= 2;
    while (n % 3 == 0) n /= 3;
    while (n % 5 == 0) n /= 5;
    return n == 1;
}

// 第n个丑数 - 三指针法
int nthUglyNumber(int n) {
    vector<int> ugly(n);
    ugly[0] = 1;
    int p2 = 0, p3 = 0, p5 = 0;

    for (int i = 1; i < n; ++i) {
        int next2 = ugly[p2] * 2;
        int next3 = ugly[p3] * 3;
        int next5 = ugly[p5] * 5;

        ugly[i] = min({next2, next3, next5});

        if (ugly[i] == next2) p2++;
        if (ugly[i] == next3) p3++;
        if (ugly[i] == next5) p5++;
    }
    return ugly[n - 1];
}

// 第n个丑数 - 最小堆法
int nthUglyNumberHeap(int n) {
    priority_queue<long, vector<long>, greater<long>> minHeap;
    unordered_set<long> seen;
    minHeap.push(1);
    seen.insert(1);

    vector<int> factors = {2, 3, 5};
    long ugly = 1;

    for (int i = 0; i < n; ++i) {
        ugly = minHeap.top();
        minHeap.pop();
        for (int f : factors) {
            long next = ugly * f;
            if (seen.find(next) == seen.end()) {
                seen.insert(next);
                minHeap.push(next);
            }
        }
    }
    return ugly;
}

int main() {
    cout << "isUgly(6): " << isUgly(6) << "\n";   // 1 (2*3)
    cout << "isUgly(8): " << isUgly(8) << "\n";   // 1 (2^3)
    cout << "isUgly(14): " << isUgly(14) << "\n"; // 0 (2*7)

    cout << "nthUglyNumber(10): " << nthUglyNumber(10) << "\n";  // 12
    cout << "nthUglyNumber(1): " << nthUglyNumber(1) << "\n";    // 1

    cout << "nthUglyNumberHeap(10): " << nthUglyNumberHeap(10) << "\n";  // 12
    return 0;
}
