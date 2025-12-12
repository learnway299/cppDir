/**
 * @file twitter_design.cpp
 * @brief Twitter 系统设计实现 - 题目文件
 *
 * 设计 Twitter 系统，支持发推、关注、取关、获取新闻流。
 * 核心考点：OOP 设计 + 哈希表 + 堆（多路归并）
 */

#include <iostream>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <list>
#include <queue>

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
 *   Twitter twitter = new Twitter();
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
    Twitter() {
        // TODO: 初始化
    }

    void postTweet(int userId, int tweetId) {
        // TODO: 发布推文
    }

    std::vector<int> getNewsFeed(int userId) {
        // TODO: 获取新闻流（最近 10 条）
        return {};
    }

    void follow(int followerId, int followeeId) {
        // TODO: 关注
    }

    void unfollow(int followerId, int followeeId) {
        // TODO: 取关
    }
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
 *   FileSystem fs = new FileSystem();
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
    FileSystem() {
        // TODO: 初始化
    }

    bool createPath(const std::string& path, int value) {
        // TODO: 创建路径
        return false;
    }

    int get(const std::string& path) {
        // TODO: 获取路径关联的值
        return -1;
    }
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
    BrowserHistory(const std::string& homepage) {
        // TODO: 初始化
    }

    void visit(const std::string& url) {
        // TODO: 访问新页面
    }

    std::string back(int steps) {
        // TODO: 后退
        return "";
    }

    std::string forward(int steps) {
        // TODO: 前进
        return "";
    }
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
 *   - cuisineHeap: cuisine -> priority_queue<{rating, food}>（懒删除）
 *   - 或使用 set 支持删除
 */
class FoodRatings {
public:
    FoodRatings(std::vector<std::string>& foods,
                std::vector<std::string>& cuisines,
                std::vector<int>& ratings) {
        // TODO: 初始化
    }

    void changeRating(const std::string& food, int newRating) {
        // TODO: 修改评分
    }

    std::string highestRated(const std::string& cuisine) {
        // TODO: 获取最高评分食物
        return "";
    }
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
 *   - maxHeap/minHeap: 懒删除堆，或使用 multiset
 */
class StockPrice {
public:
    StockPrice() {
        // TODO: 初始化
    }

    void update(int timestamp, int price) {
        // TODO: 更新价格
    }

    int current() {
        // TODO: 获取最新价格
        return 0;
    }

    int maximum() {
        // TODO: 获取最高价格
        return 0;
    }

    int minimum() {
        // TODO: 获取最低价格
        return 0;
    }
};


// ==================== 测试代码 ====================
#include <cassert>

void testTwitter() {
    Twitter twitter;
    twitter.postTweet(1, 5);
    auto feed = twitter.getNewsFeed(1);
    assert(feed.size() == 1 && feed[0] == 5);

    twitter.follow(1, 2);
    twitter.postTweet(2, 6);
    feed = twitter.getNewsFeed(1);
    assert(feed.size() == 2 && feed[0] == 6 && feed[1] == 5);

    twitter.unfollow(1, 2);
    feed = twitter.getNewsFeed(1);
    assert(feed.size() == 1 && feed[0] == 5);

    std::cout << "Twitter: PASSED\n";
}

void testFileSystem() {
    FileSystem fs;
    assert(fs.createPath("/a", 1) == true);
    assert(fs.get("/a") == 1);
    assert(fs.createPath("/a/b", 2) == true);
    assert(fs.get("/a/b") == 2);
    assert(fs.createPath("/c/d", 3) == false);

    std::cout << "FileSystem: PASSED\n";
}

void testBrowserHistory() {
    BrowserHistory bh("leetcode.com");
    bh.visit("google.com");
    bh.visit("facebook.com");
    bh.visit("youtube.com");
    assert(bh.back(1) == "facebook.com");
    assert(bh.back(1) == "google.com");
    assert(bh.forward(1) == "facebook.com");
    bh.visit("linkedin.com");
    assert(bh.forward(2) == "linkedin.com");
    assert(bh.back(2) == "leetcode.com");
    assert(bh.back(7) == "leetcode.com");

    std::cout << "BrowserHistory: PASSED\n";
}

void testFoodRatings() {
    std::vector<std::string> foods = {"kimchi", "miso", "sushi", "moussaka", "ramen", "bulgogi"};
    std::vector<std::string> cuisines = {"korean", "japanese", "japanese", "greek", "japanese", "korean"};
    std::vector<int> ratings = {9, 12, 8, 15, 14, 7};

    FoodRatings fr(foods, cuisines, ratings);
    assert(fr.highestRated("korean") == "kimchi");
    assert(fr.highestRated("japanese") == "ramen");
    fr.changeRating("sushi", 16);
    assert(fr.highestRated("japanese") == "sushi");

    std::cout << "FoodRatings: PASSED\n";
}

void testStockPrice() {
    StockPrice sp;
    sp.update(1, 10);
    sp.update(2, 5);
    assert(sp.current() == 5);
    assert(sp.maximum() == 10);
    sp.update(1, 3);  // 更正
    assert(sp.maximum() == 5);
    sp.update(4, 2);
    assert(sp.minimum() == 2);

    std::cout << "StockPrice: PASSED\n";
}

int main() {
    std::cout << "=== Twitter Design Tests ===\n";

    // 取消注释以运行测试（实现解法后）
    // testTwitter();
    // testFileSystem();
    // testBrowserHistory();
    // testFoodRatings();
    // testStockPrice();

    std::cout << "All tests passed!\n";
    return 0;
}
