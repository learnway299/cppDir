/**
 * @file twitter_design_solution.cpp
 * @brief Twitter 系统设计实现 - 解答文件
 *
 * 设计类题目的核心：
 * 1. 选择合适的数据结构
 * 2. 考虑时间复杂度
 * 3. 处理边界情况
 */

#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <list>
#include <queue>
#include <set>
#include <algorithm>

/**
 * 题目1: 设计推特
 *
 * 数据结构设计：
 * - followMap: userId -> set<followeeId>（关注列表）
 * - tweetMap: userId -> list<{time, tweetId}>（推文列表，新的在前）
 * - timestamp: 全局时间戳
 *
 * getNewsFeed 实现：
 * - 使用最小堆进行 K 路归并
 * - 合并自己和所有关注者的推文流
 * - 取最近 10 条
 *
 * 时间复杂度:
 * - postTweet: O(1)
 * - follow/unfollow: O(1)
 * - getNewsFeed: O(N * log N)，N 为关注人数
 */
class Twitter {
private:
    struct Tweet {
        int time;
        int id;
    };

    int timestamp;
    std::unordered_map<int, std::unordered_set<int>> followMap;
    std::unordered_map<int, std::list<Tweet>> tweetMap;

public:
    Twitter() : timestamp(0) {}

    void postTweet(int userId, int tweetId) {
        tweetMap[userId].push_front({timestamp++, tweetId});
    }

    std::vector<int> getNewsFeed(int userId) {
        // 最大堆：按时间排序
        auto cmp = [](const std::pair<Tweet, std::list<Tweet>::iterator>& a,
                      const std::pair<Tweet, std::list<Tweet>::iterator>& b) {
            return a.first.time < b.first.time;
        };
        std::priority_queue<std::pair<Tweet, std::list<Tweet>::iterator>,
                           std::vector<std::pair<Tweet, std::list<Tweet>::iterator>>,
                           decltype(cmp)> maxHeap(cmp);

        // 添加自己的推文
        if (!tweetMap[userId].empty()) {
            auto it = tweetMap[userId].begin();
            maxHeap.push({*it, it});
        }

        // 添加关注者的推文
        for (int followeeId : followMap[userId]) {
            if (!tweetMap[followeeId].empty()) {
                auto it = tweetMap[followeeId].begin();
                maxHeap.push({*it, it});
            }
        }

        std::vector<int> result;
        while (!maxHeap.empty() && result.size() < 10) {
            auto [tweet, it] = maxHeap.top();
            maxHeap.pop();
            result.push_back(tweet.id);

            // 获取该用户的下一条推文
            ++it;
            // 需要找到对应的用户来检查迭代器是否有效
            // 简化实现：直接存储更多信息或使用不同数据结构
        }

        return result;
    }

    void follow(int followerId, int followeeId) {
        if (followerId != followeeId) {
            followMap[followerId].insert(followeeId);
        }
    }

    void unfollow(int followerId, int followeeId) {
        followMap[followerId].erase(followeeId);
    }
};

/**
 * 改进版 Twitter：使用更清晰的数据结构
 */
class TwitterV2 {
private:
    int timestamp;
    // userId -> list of {timestamp, tweetId}
    std::unordered_map<int, std::vector<std::pair<int, int>>> tweets;
    // userId -> set of followeeIds
    std::unordered_map<int, std::unordered_set<int>> following;

public:
    TwitterV2() : timestamp(0) {}

    void postTweet(int userId, int tweetId) {
        tweets[userId].push_back({timestamp++, tweetId});
    }

    std::vector<int> getNewsFeed(int userId) {
        // 收集所有相关用户（自己 + 关注的人）
        std::vector<int> users = {userId};
        for (int followeeId : following[userId]) {
            users.push_back(followeeId);
        }

        // 使用堆进行多路归并
        // {timestamp, tweetId, userId, index}
        auto cmp = [](const std::tuple<int, int, int, int>& a,
                      const std::tuple<int, int, int, int>& b) {
            return std::get<0>(a) < std::get<0>(b);
        };
        std::priority_queue<std::tuple<int, int, int, int>,
                           std::vector<std::tuple<int, int, int, int>>,
                           decltype(cmp)> maxHeap(cmp);

        // 初始化：每个用户的最新推文入堆
        for (int uid : users) {
            if (!tweets[uid].empty()) {
                int idx = tweets[uid].size() - 1;
                auto& [ts, tid] = tweets[uid][idx];
                maxHeap.push({ts, tid, uid, idx});
            }
        }

        std::vector<int> result;
        while (!maxHeap.empty() && result.size() < 10) {
            auto [ts, tid, uid, idx] = maxHeap.top();
            maxHeap.pop();
            result.push_back(tid);

            // 该用户的下一条推文入堆
            if (idx > 0) {
                int newIdx = idx - 1;
                auto& [newTs, newTid] = tweets[uid][newIdx];
                maxHeap.push({newTs, newTid, uid, newIdx});
            }
        }

        return result;
    }

    void follow(int followerId, int followeeId) {
        if (followerId != followeeId) {
            following[followerId].insert(followeeId);
        }
    }

    void unfollow(int followerId, int followeeId) {
        following[followerId].erase(followeeId);
    }
};

/**
 * 题目2: 设计文件系统
 *
 * 方法1：使用哈希表
 * - 直接将路径作为 key
 * - 创建时检查父路径是否存在
 */
class FileSystem {
private:
    std::unordered_map<std::string, int> paths;

    std::string getParent(const std::string& path) {
        size_t pos = path.rfind('/');
        if (pos == 0) return "";  // 根路径的父路径
        return path.substr(0, pos);
    }

public:
    FileSystem() {
        paths[""] = 0;  // 根路径
    }

    bool createPath(const std::string& path, int value) {
        if (path.empty() || path == "/" || paths.count(path)) {
            return false;
        }

        std::string parent = getParent(path);
        if (!paths.count(parent)) {
            return false;
        }

        paths[path] = value;
        return true;
    }

    int get(const std::string& path) {
        auto it = paths.find(path);
        return it != paths.end() ? it->second : -1;
    }
};

/**
 * 方法2：使用 Trie 结构
 */
class FileSystemTrie {
private:
    struct TrieNode {
        std::unordered_map<std::string, TrieNode*> children;
        int value;
        bool hasValue;
        TrieNode() : value(0), hasValue(false) {}
    };

    TrieNode* root;

    std::vector<std::string> parsePath(const std::string& path) {
        std::vector<std::string> parts;
        std::string part;
        for (size_t i = 1; i < path.size(); ++i) {  // 跳过开头的 '/'
            if (path[i] == '/') {
                if (!part.empty()) {
                    parts.push_back(part);
                    part.clear();
                }
            } else {
                part += path[i];
            }
        }
        if (!part.empty()) {
            parts.push_back(part);
        }
        return parts;
    }

public:
    FileSystemTrie() {
        root = new TrieNode();
    }

    bool createPath(const std::string& path, int value) {
        if (path.empty() || path == "/") return false;

        std::vector<std::string> parts = parsePath(path);
        TrieNode* node = root;

        // 遍历到倒数第二层，检查父路径存在
        for (size_t i = 0; i + 1 < parts.size(); ++i) {
            auto it = node->children.find(parts[i]);
            if (it == node->children.end()) {
                return false;  // 父路径不存在
            }
            node = it->second;
        }

        // 检查最后一层
        const std::string& lastPart = parts.back();
        if (node->children.count(lastPart)) {
            return false;  // 路径已存在
        }

        node->children[lastPart] = new TrieNode();
        node->children[lastPart]->value = value;
        node->children[lastPart]->hasValue = true;
        return true;
    }

    int get(const std::string& path) {
        std::vector<std::string> parts = parsePath(path);
        TrieNode* node = root;

        for (const auto& part : parts) {
            auto it = node->children.find(part);
            if (it == node->children.end()) {
                return -1;
            }
            node = it->second;
        }

        return node->hasValue ? node->value : -1;
    }
};

/**
 * 题目3: 设计浏览器历史记录
 *
 * 方法1：使用数组 + 指针
 */
class BrowserHistory {
private:
    std::vector<std::string> history;
    int curr;  // 当前位置
    int end;   // 有效末尾

public:
    BrowserHistory(const std::string& homepage) {
        history.push_back(homepage);
        curr = 0;
        end = 0;
    }

    void visit(const std::string& url) {
        ++curr;
        if (curr < static_cast<int>(history.size())) {
            history[curr] = url;
        } else {
            history.push_back(url);
        }
        end = curr;  // 清除前进历史
    }

    std::string back(int steps) {
        curr = std::max(0, curr - steps);
        return history[curr];
    }

    std::string forward(int steps) {
        curr = std::min(end, curr + steps);
        return history[curr];
    }
};

/**
 * 方法2：使用双向链表
 */
class BrowserHistoryList {
private:
    struct Node {
        std::string url;
        Node* prev;
        Node* next;
        Node(const std::string& u) : url(u), prev(nullptr), next(nullptr) {}
    };

    Node* curr;

    void clearForward(Node* node) {
        while (node) {
            Node* next = node->next;
            delete node;
            node = next;
        }
    }

public:
    BrowserHistoryList(const std::string& homepage) {
        curr = new Node(homepage);
    }

    ~BrowserHistoryList() {
        // 回到最开始
        while (curr->prev) {
            curr = curr->prev;
        }
        clearForward(curr);
    }

    void visit(const std::string& url) {
        clearForward(curr->next);
        Node* newNode = new Node(url);
        curr->next = newNode;
        newNode->prev = curr;
        curr = newNode;
    }

    std::string back(int steps) {
        while (steps > 0 && curr->prev) {
            curr = curr->prev;
            --steps;
        }
        return curr->url;
    }

    std::string forward(int steps) {
        while (steps > 0 && curr->next) {
            curr = curr->next;
            --steps;
        }
        return curr->url;
    }
};

/**
 * 题目4: 设计食物评分系统
 *
 * 使用 set 支持高效的删除和查找最大值
 */
class FoodRatings {
private:
    // food -> {cuisine, rating}
    std::unordered_map<std::string, std::pair<std::string, int>> foodInfo;
    // cuisine -> set<{-rating, food}>（负评分用于降序排列）
    std::unordered_map<std::string, std::set<std::pair<int, std::string>>> cuisineSet;

public:
    FoodRatings(std::vector<std::string>& foods,
                std::vector<std::string>& cuisines,
                std::vector<int>& ratings) {
        int n = foods.size();
        for (int i = 0; i < n; ++i) {
            foodInfo[foods[i]] = {cuisines[i], ratings[i]};
            cuisineSet[cuisines[i]].insert({-ratings[i], foods[i]});
        }
    }

    void changeRating(const std::string& food, int newRating) {
        auto& [cuisine, oldRating] = foodInfo[food];
        cuisineSet[cuisine].erase({-oldRating, food});
        cuisineSet[cuisine].insert({-newRating, food});
        oldRating = newRating;
    }

    std::string highestRated(const std::string& cuisine) {
        return cuisineSet[cuisine].begin()->second;
    }
};

/**
 * 题目5: 设计股票价格波动
 *
 * 使用 multiset 维护最大最小值
 */
class StockPrice {
private:
    std::unordered_map<int, int> prices;  // timestamp -> price
    std::multiset<int> priceSet;  // 所有价格
    int latestTime;

public:
    StockPrice() : latestTime(0) {}

    void update(int timestamp, int price) {
        if (prices.count(timestamp)) {
            // 更正：先删除旧价格
            priceSet.erase(priceSet.find(prices[timestamp]));
        }
        prices[timestamp] = price;
        priceSet.insert(price);
        latestTime = std::max(latestTime, timestamp);
    }

    int current() {
        return prices[latestTime];
    }

    int maximum() {
        return *priceSet.rbegin();
    }

    int minimum() {
        return *priceSet.begin();
    }
};

/**
 * 使用懒删除堆的实现（空间换时间）
 */
class StockPriceLazy {
private:
    std::unordered_map<int, int> prices;
    // {price, timestamp}
    std::priority_queue<std::pair<int, int>> maxHeap;
    std::priority_queue<std::pair<int, int>, std::vector<std::pair<int, int>>,
                        std::greater<>> minHeap;
    int latestTime;

public:
    StockPriceLazy() : latestTime(0) {}

    void update(int timestamp, int price) {
        prices[timestamp] = price;
        maxHeap.push({price, timestamp});
        minHeap.push({price, timestamp});
        latestTime = std::max(latestTime, timestamp);
    }

    int current() {
        return prices[latestTime];
    }

    int maximum() {
        // 懒删除：检查堆顶是否有效
        while (!maxHeap.empty()) {
            auto [price, ts] = maxHeap.top();
            if (prices[ts] == price) {
                return price;
            }
            maxHeap.pop();  // 无效数据，删除
        }
        return -1;
    }

    int minimum() {
        while (!minHeap.empty()) {
            auto [price, ts] = minHeap.top();
            if (prices[ts] == price) {
                return price;
            }
            minHeap.pop();
        }
        return -1;
    }
};

/**
 * 附加题: 设计搜索自动补全系统
 *
 * 设计一个搜索自动补全系统，支持用户输入字符并返回热门建议。
 */
class AutocompleteSystem {
private:
    struct TrieNode {
        std::unordered_map<char, TrieNode*> children;
        std::unordered_map<std::string, int> sentences;  // 以此为前缀的句子及其热度
    };

    TrieNode* root;
    TrieNode* curr;
    std::string currentInput;

public:
    AutocompleteSystem(std::vector<std::string>& sentences, std::vector<int>& times) {
        root = new TrieNode();
        curr = root;

        for (size_t i = 0; i < sentences.size(); ++i) {
            addSentence(sentences[i], times[i]);
        }
    }

    void addSentence(const std::string& sentence, int count) {
        TrieNode* node = root;
        for (char c : sentence) {
            if (!node->children.count(c)) {
                node->children[c] = new TrieNode();
            }
            node = node->children[c];
            node->sentences[sentence] += count;
        }
    }

    std::vector<std::string> input(char c) {
        if (c == '#') {
            // 结束输入，记录句子
            addSentence(currentInput, 1);
            currentInput.clear();
            curr = root;
            return {};
        }

        currentInput += c;

        if (!curr || !curr->children.count(c)) {
            curr = nullptr;
            return {};
        }

        curr = curr->children[c];

        // 获取 top 3 热门句子
        std::vector<std::pair<int, std::string>> candidates;
        for (auto& [sentence, count] : curr->sentences) {
            candidates.push_back({count, sentence});
        }

        // 排序：按热度降序，字典序升序
        std::sort(candidates.begin(), candidates.end(), [](auto& a, auto& b) {
            if (a.first != b.first) return a.first > b.first;
            return a.second < b.second;
        });

        std::vector<std::string> result;
        for (int i = 0; i < std::min(3, static_cast<int>(candidates.size())); ++i) {
            result.push_back(candidates[i].second);
        }
        return result;
    }
};


// ==================== 测试代码 ====================
#include <cassert>

void testTwitterV2() {
    TwitterV2 twitter;
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

    std::cout << "TwitterV2: PASSED\n";
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

void testFileSystemTrie() {
    FileSystemTrie fs;
    assert(fs.createPath("/a", 1) == true);
    assert(fs.get("/a") == 1);
    assert(fs.createPath("/a/b", 2) == true);
    assert(fs.get("/a/b") == 2);
    assert(fs.createPath("/c/d", 3) == false);

    std::cout << "FileSystemTrie: PASSED\n";
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

void testBrowserHistoryList() {
    BrowserHistoryList bh("leetcode.com");
    bh.visit("google.com");
    bh.visit("facebook.com");
    bh.visit("youtube.com");
    assert(bh.back(1) == "facebook.com");
    assert(bh.back(1) == "google.com");
    assert(bh.forward(1) == "facebook.com");
    bh.visit("linkedin.com");
    assert(bh.forward(2) == "linkedin.com");
    assert(bh.back(2) == "leetcode.com");

    std::cout << "BrowserHistoryList: PASSED\n";
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

void testStockPriceLazy() {
    StockPriceLazy sp;
    sp.update(1, 10);
    sp.update(2, 5);
    assert(sp.current() == 5);
    assert(sp.maximum() == 10);
    sp.update(1, 3);
    assert(sp.maximum() == 5);
    sp.update(4, 2);
    assert(sp.minimum() == 2);

    std::cout << "StockPriceLazy: PASSED\n";
}

void testAutocompleteSystem() {
    std::vector<std::string> sentences = {"i love you", "island", "iroman", "i love leetcode"};
    std::vector<int> times = {5, 3, 2, 2};
    AutocompleteSystem ac(sentences, times);

    auto result = ac.input('i');
    assert(result.size() == 3);
    assert(result[0] == "i love you");

    result = ac.input(' ');
    assert(result.size() == 2);
    assert(result[0] == "i love you");
    assert(result[1] == "i love leetcode");

    ac.input('#');  // 结束输入

    std::cout << "AutocompleteSystem: PASSED\n";
}

int main() {
    std::cout << "=== Twitter Design Solution Tests ===\n";

    testTwitterV2();
    testFileSystem();
    testFileSystemTrie();
    testBrowserHistory();
    testBrowserHistoryList();
    testFoodRatings();
    testStockPrice();
    testStockPriceLazy();
    testAutocompleteSystem();

    std::cout << "\nAll tests passed!\n";
    return 0;
}
