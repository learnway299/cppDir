/**
 * @file twitter_design.h
 * @brief Twitter 系统设计实现 - 题目说明与声明
 *
 * 设计 Twitter 系统，支持发推、关注、取关、获取新闻流。
 * 核心考点：OOP 设计 + 哈希表 + 堆（多路归并）
 */

#ifndef TWITTER_DESIGN_H
#define TWITTER_DESIGN_H

#include <vector>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <list>
#include <queue>
#include <set>

namespace TwitterDesign {

// ==================== 题目声明 ====================

/**
 * 题目1: 设计推特
 *
 * 设计一个简化版的 Twitter，可以让用户实现发送推文、关注/取消关注其他用户、
 * 能够看见关注人（包括自己）的最近 10 条推文。
 *
 * 实现 Twitter 类：
 *   - Twitter() 初始化简易版推特对象
 *   - void postTweet(int userId, int tweetId) 根据给定的 tweetId 和 userId 创建一条新推文。
 *     每次调用此函数都会使用一个不同的 tweetId
 *   - vector<int> getNewsFeed(int userId) 检索当前用户新闻推送中最近 10 条推文的 ID。
 *     新闻推送中的每一项都必须是由用户关注的人或者是用户自己发布的推文。
 *     推文必须按照时间顺序由最近到最远排序
 *   - void follow(int followerId, int followeeId) followerId 用户关注 followeeId 用户
 *   - void unfollow(int followerId, int followeeId) followerId 用户取消关注 followeeId 用户
 *
 * 示例:
 *   Twitter twitter;
 *   twitter.postTweet(1, 5);       // 用户 1 发送了一条新推文 (id = 5)
 *   twitter.getNewsFeed(1);        // 返回 [5]
 *   twitter.follow(1, 2);          // 用户 1 关注了用户 2
 *   twitter.postTweet(2, 6);       // 用户 2 发送了一条新推文 (id = 6)
 *   twitter.getNewsFeed(1);        // 返回 [6, 5]
 *   twitter.unfollow(1, 2);        // 用户 1 取消关注了用户 2
 *   twitter.getNewsFeed(1);        // 返回 [5]
 *
 * 思路:
 *   - 用户结构：userId -> 关注列表 + 推文列表
 *   - 推文结构：tweetId + timestamp
 *   - getNewsFeed：使用堆进行多路归并，合并所有关注者的推文流
 */
class Twitter {
public:
    Twitter();
    void postTweet(int userId, int tweetId);
    std::vector<int> getNewsFeed(int userId);
    void follow(int followerId, int followeeId);
    void unfollow(int followerId, int followeeId);
private:
    int timestamp_;
    std::unordered_map<int, std::vector<std::pair<int, int>>> tweets_;
    std::unordered_map<int, std::unordered_set<int>> following_;
};

class TwitterSolution {
public:
    TwitterSolution();
    void postTweet(int userId, int tweetId);
    std::vector<int> getNewsFeed(int userId);
    void follow(int followerId, int followeeId);
    void unfollow(int followerId, int followeeId);
private:
    int timestamp_;
    std::unordered_map<int, std::vector<std::pair<int, int>>> tweets_;
    std::unordered_map<int, std::unordered_set<int>> following_;
};

/**
 * 题目2: 设计文件系统
 *
 * 你需要设计一个文件系统，可以创建新路径并将它们与不同的值关联。
 *
 * 实现 FileSystem 类:
 *   - bool createPath(string path, int value) 创建一个与 value 相关联的新路径，
 *     如果路径已经存在或者其父路径不存在，则返回 false。否则返回 true 并创建路径。
 *   - int get(string path) 返回与 path 相关联的值，如果路径不存在则返回 -1。
 *
 * 示例:
 *   FileSystem fs;
 *   fs.createPath("/a", 1);         // 返回 true
 *   fs.get("/a");                   // 返回 1
 *   fs.createPath("/a/b", 2);       // 返回 true
 *   fs.get("/a/b");                 // 返回 2
 *   fs.createPath("/c/d", 3);       // 返回 false，因为 "/c" 不存在
 *
 * 思路:
 *   - 使用 Trie 或哈希表存储路径
 *   - 解析路径，检查父路径是否存在
 */
class FileSystem {
public:
    FileSystem();
    bool createPath(const std::string& path, int value);
    int get(const std::string& path);
private:
    std::unordered_map<std::string, int> paths_;
    std::string getParent(const std::string& path);
};

class FileSystemSolution {
public:
    FileSystemSolution();
    bool createPath(const std::string& path, int value);
    int get(const std::string& path);
private:
    std::unordered_map<std::string, int> paths_;
    std::string getParent(const std::string& path);
};

/**
 * 题目3: 设计浏览器历史记录
 *
 * 你有一个只支持单个标签页的浏览器，最开始你浏览的网页是 homepage。
 *
 * 实现 BrowserHistory 类：
 *   - BrowserHistory(string homepage) 用 homepage 初始化浏览器类
 *   - void visit(string url) 从当前页跳转访问 url 对应的页面。
 *     执行此操作会把浏览历史前进的记录全部删除
 *   - string back(int steps) 在浏览历史中后退 steps 步。
 *     如果只能后退 x 步（x < steps），只后退 x 步。返回后退后的当前页面的 url
 *   - string forward(int steps) 在浏览历史中前进 steps 步。
 *     如果只能前进 x 步（x < steps），只前进 x 步。返回前进后的当前页面的 url
 *
 * 示例:
 *   BrowserHistory bh("leetcode.com");
 *   bh.visit("google.com");
 *   bh.visit("facebook.com");
 *   bh.visit("youtube.com");
 *   bh.back(1);                  // 返回 "facebook.com"
 *   bh.back(1);                  // 返回 "google.com"
 *   bh.forward(1);               // 返回 "facebook.com"
 *   bh.visit("linkedin.com");
 *   bh.forward(2);               // 返回 "linkedin.com"，无法前进
 *   bh.back(2);                  // 返回 "leetcode.com"
 *
 * 思路:
 *   - 方法1：使用数组 + 指针
 *   - 方法2：使用双向链表
 */
class BrowserHistory {
public:
    BrowserHistory(const std::string& homepage);
    void visit(const std::string& url);
    std::string back(int steps);
    std::string forward(int steps);
private:
    std::vector<std::string> history_;
    int curr_;
    int end_;
};

class BrowserHistorySolution {
public:
    BrowserHistorySolution(const std::string& homepage);
    void visit(const std::string& url);
    std::string back(int steps);
    std::string forward(int steps);
private:
    std::vector<std::string> history_;
    int curr_;
    int end_;
};

/**
 * 题目4: 设计食物评分系统
 *
 * 设计一个支持下述操作的食物评分系统：
 *   - 修改系统中食物的评分
 *   - 返回某一类烹饪方式中评分最高的食物
 *
 * 实现 FoodRatings 类：
 *   - FoodRatings(vector<string>& foods, vector<string>& cuisines, vector<int>& ratings)
 *     初始化系统。foods、cuisines 和 ratings 长度相同，foods[i] 是第 i 种食物的名称，
 *     cuisines[i] 是第 i 种食物的烹饪方式，ratings[i] 是第 i 种食物的初始评分
 *   - void changeRating(string food, int newRating) 修改名称为 food 的食物的评分
 *   - string highestRated(string cuisine) 返回指定烹饪方式中评分最高的食物名称。
 *     如果存在并列，返回字典序较小的
 *
 * 思路:
 *   - foodInfo: food -> {cuisine, rating}
 *   - cuisineSet: cuisine -> set<{-rating, food}>（使用负评分实现降序）
 */
class FoodRatings {
public:
    FoodRatings(std::vector<std::string>& foods,
                std::vector<std::string>& cuisines,
                std::vector<int>& ratings);
    void changeRating(const std::string& food, int newRating);
    std::string highestRated(const std::string& cuisine);
private:
    std::unordered_map<std::string, std::pair<std::string, int>> foodInfo_;
    std::unordered_map<std::string, std::set<std::pair<int, std::string>>> cuisineSet_;
};

class FoodRatingsSolution {
public:
    FoodRatingsSolution(std::vector<std::string>& foods,
                        std::vector<std::string>& cuisines,
                        std::vector<int>& ratings);
    void changeRating(const std::string& food, int newRating);
    std::string highestRated(const std::string& cuisine);
private:
    std::unordered_map<std::string, std::pair<std::string, int>> foodInfo_;
    std::unordered_map<std::string, std::set<std::pair<int, std::string>>> cuisineSet_;
};

/**
 * 题目5: 设计股票价格波动
 *
 * 给定一些股票价格记录，每条记录包含时间戳和该时间戳对应的价格。
 * 可能会有错误的记录需要更正。
 *
 * 实现 StockPrice 类：
 *   - StockPrice() 初始化对象
 *   - void update(int timestamp, int price) 在时间戳 timestamp 更新股票价格为 price。
 *     如果以前有一条记录的时间戳为 timestamp，则更正
 *   - int current() 返回股票最新价格
 *   - int maximum() 返回所有记录中的最高价
 *   - int minimum() 返回所有记录中的最低价
 *
 * 思路:
 *   - prices: timestamp -> price（记录所有价格）
 *   - priceSet: multiset 维护所有价格，支持 O(log n) 的最大最小值查询
 */
class StockPrice {
public:
    StockPrice();
    void update(int timestamp, int price);
    int current();
    int maximum();
    int minimum();
private:
    std::unordered_map<int, int> prices_;
    std::multiset<int> priceSet_;
    int latestTime_;
};

class StockPriceSolution {
public:
    StockPriceSolution();
    void update(int timestamp, int price);
    int current();
    int maximum();
    int minimum();
private:
    std::unordered_map<int, int> prices_;
    std::multiset<int> priceSet_;
    int latestTime_;
};

/**
 * 题目6: 设计搜索自动补全系统
 *
 * 为搜索引擎设计一个搜索自动补全系统。用户会输入一条语句（最少包含一个字母，以特殊字符 '#' 结尾）。
 *
 * 实现 AutocompleteSystem 类：
 *   - AutocompleteSystem(vector<string>& sentences, vector<int>& times)
 *     使用给定的历史数据初始化对象
 *   - vector<string> input(char c) 输入字符 c，返回热度前三的历史记录
 *
 * 思路:
 *   - 使用 Trie 存储所有句子
 *   - 每个节点维护以当前前缀开头的所有句子及其热度
 */
class AutocompleteSystem {
public:
    AutocompleteSystem(std::vector<std::string>& sentences, std::vector<int>& times);
    std::vector<std::string> input(char c);
private:
    struct TrieNode;
    TrieNode* root_;
    TrieNode* curr_;
    std::string currentInput_;
    void addSentence(const std::string& sentence, int count);
};

class AutocompleteSystemSolution {
public:
    AutocompleteSystemSolution(std::vector<std::string>& sentences, std::vector<int>& times);
    std::vector<std::string> input(char c);
private:
    struct TrieNode;
    TrieNode* root_;
    TrieNode* curr_;
    std::string currentInput_;
    void addSentence(const std::string& sentence, int count);
};

// ==================== 测试函数声明 ====================

void testTwitterDesign();          // 测试面试者实现
void testTwitterDesignSolution();  // 测试解答实现

// runTests 在头文件 inline 定义，调用两个测试函数
inline void runTests() {
    testTwitterDesign();
    testTwitterDesignSolution();
}

} // namespace TwitterDesign

#endif // TWITTER_DESIGN_H
