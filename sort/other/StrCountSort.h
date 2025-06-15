//
// Created by learyuan on 25-6-15.
//

#ifndef STRCOUNTSORT_H
#define STRCOUNTSORT_H
#include <string>
#include <vector>

namespace StrCountSort {
    void solution();
    /**
     * 输入字符串 输出排序结果
     * 排序规则: 1出现次数多的顺序 2次数相同的以先后出现实现
     * @param input
     */
    std::vector<std::pair<char, int>> CountSort(const std::string& input);

} // StrCountSort

#endif //STRCOUNTSORT_H
