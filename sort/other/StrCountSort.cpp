//
// Created by asus on 25-6-15.
//

#include "StrCountSort.h"

#include <algorithm>
#include <unordered_map>
#include <iostream>
using namespace std;

namespace StrCountSort {
    void solution() {
        std::cout << "StrCountSort::solution::CountSort" << std::endl;
        std::string temp("abababccdcdefg");
        std::cout << temp << std::endl;
        std::vector<std::pair<char, int>>  result = CountSort(temp);
        for (auto item : result) {
            std::cout << item.first << " : " << item.second << std::ends;
        }
    }

    std::vector<std::pair<char, int>> CountSort(const std::string &input) {
        std::unordered_map<char, int> countCache,orderCache;
        for (int i = 0; i < input.size(); i++) {
            countCache[input[i]]++;
            if (!orderCache.contains(input[i])) {
                orderCache[input[i]] = i;
            }
        }
        std::vector<std::pair<char, int>> cache(countCache.begin(), countCache.end());
        std::sort(cache.begin(), cache.end(),[&orderCache]
            (const std::pair<char, int>& a, const std::pair<char, int>& b) {
            bool result = true;
            if (a.second == b.second) {
                result = orderCache[a.first] < orderCache[b.first];
            }
            else {
                result = a.second > b.second;
            }
            return result;
        });
        return cache;
    }
} // StrCountSort