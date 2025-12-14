/**
 * @file word_search.h
 * @brief 单词搜索 - 声明
 */
#ifndef WORD_SEARCH_H
#define WORD_SEARCH_H

#include <vector>
#include <string>

namespace WordSearch {

// 面试者实现的函数
bool exist(std::vector<std::vector<char>>& board, const std::string& word);
std::vector<std::string> findWords(std::vector<std::vector<char>>& board,
                                    std::vector<std::string>& words);

// 参考答案函数
bool existSolution(std::vector<std::vector<char>>& board, const std::string& word);
std::vector<std::string> findWordsSolution(std::vector<std::vector<char>>& board,
                                            std::vector<std::string>& words);

// 测试函数
void runTests();

} // namespace WordSearch

#endif // WORD_SEARCH_H
