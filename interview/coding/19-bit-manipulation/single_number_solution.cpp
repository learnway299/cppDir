/**
 * @file single_number_solution.cpp
 * @brief 只出现一次的数字 - 解答
 */
#include <vector>
#include <iostream>
using namespace std;

// I: 异或所有数，成对的抵消
int singleNumber(vector<int>& nums) {
    int result = 0;
    for (int n : nums) result ^= n;
    return result;
}

// II: 统计每个位上 1 的个数，模 3
int singleNumber2(vector<int>& nums) {
    int result = 0;
    for (int i = 0; i < 32; ++i) {
        int sum = 0;
        for (int n : nums) {
            sum += (n >> i) & 1;
        }
        if (sum % 3) result |= (1 << i);
    }
    return result;
}

// III: 异或得两数异或值，按某位分组
vector<int> singleNumber3(vector<int>& nums) {
    unsigned int xorAll = 0;
    for (int n : nums) xorAll ^= n;

    unsigned int lowBit = xorAll & (-xorAll);
    int a = 0, b = 0;
    for (int n : nums) {
        if (n & lowBit) a ^= n;
        else b ^= n;
    }
    return {a, b};
}

int main() {
    vector<int> nums1 = {2, 2, 1};
    cout << "singleNumber([2,2,1]): " << singleNumber(nums1) << "\n";

    vector<int> nums2 = {2, 2, 3, 2};
    cout << "singleNumber2([2,2,3,2]): " << singleNumber2(nums2) << "\n";

    vector<int> nums3 = {1, 2, 1, 3, 2, 5};
    auto res = singleNumber3(nums3);
    cout << "singleNumber3([1,2,1,3,2,5]): [" << res[0] << ", " << res[1] << "]\n";
    return 0;
}
