/**
 * @file twitter_design_solution.cpp
 * @brief Twitter 系统设计实现 - 参考答案
 */

#include "twitter_design.h"
#include <iostream>
#include <cassert>
#include <algorithm>

namespace TwitterDesign {

// ==================== 参考答案实现 ====================

// TwitterSolution 实现
TwitterSolution::TwitterSolution() : timestamp_(0) {}

void TwitterSolution::postTweet(int userId, int tweetId) {
    tweets_[userId].push_back({timestamp_++, tweetId});
}

std::vector<int> TwitterSolution::getNewsFeed(int userId) {
    std::vector<int> users = {userId};
    for (int followeeId : following_[userId]) {
        users.push_back(followeeId);
    }

    auto cmp = [](const std::tuple<int, int, int, int>& a,
                  const std::tuple<int, int, int, int>& b) {
        return std::get<0>(a) < std::get<0>(b);
    };
    std::priority_queue<std::tuple<int, int, int, int>,
                       std::vector<std::tuple<int, int, int, int>>,
                       decltype(cmp)> maxHeap(cmp);

    for (int uid : users) {
        if (!tweets_[uid].empty()) {
            int idx = static_cast<int>(tweets_[uid].size()) - 1;
            auto& [ts, tid] = tweets_[uid][idx];
            maxHeap.push({ts, tid, uid, idx});
        }
    }

    std::vector<int> result;
    while (!maxHeap.empty() && result.size() < 10) {
        auto [ts, tid, uid, idx] = maxHeap.top();
        maxHeap.pop();
        result.push_back(tid);

        if (idx > 0) {
            int newIdx = idx - 1;
            auto& [newTs, newTid] = tweets_[uid][newIdx];
            maxHeap.push({newTs, newTid, uid, newIdx});
        }
    }

    return result;
}

void TwitterSolution::follow(int followerId, int followeeId) {
    if (followerId != followeeId) {
        following_[followerId].insert(followeeId);
    }
}

void TwitterSolution::unfollow(int followerId, int followeeId) {
    following_[followerId].erase(followeeId);
}

// FileSystemSolution 实现
FileSystemSolution::FileSystemSolution() {
    paths_[""] = 0;
}

std::string FileSystemSolution::getParent(const std::string& path) {
    size_t pos = path.rfind('/');
    if (pos == 0) return "";
    return path.substr(0, pos);
}

bool FileSystemSolution::createPath(const std::string& path, int value) {
    if (path.empty() || path == "/" || paths_.count(path)) {
        return false;
    }

    std::string parent = getParent(path);
    if (!paths_.count(parent)) {
        return false;
    }

    paths_[path] = value;
    return true;
}

int FileSystemSolution::get(const std::string& path) {
    auto it = paths_.find(path);
    return it != paths_.end() ? it->second : -1;
}

// BrowserHistorySolution 实现
BrowserHistorySolution::BrowserHistorySolution(const std::string& homepage) : curr_(0), end_(0) {
    history_.push_back(homepage);
}

void BrowserHistorySolution::visit(const std::string& url) {
    ++curr_;
    if (curr_ < static_cast<int>(history_.size())) {
        history_[curr_] = url;
    } else {
        history_.push_back(url);
    }
    end_ = curr_;
}

std::string BrowserHistorySolution::back(int steps) {
    curr_ = std::max(0, curr_ - steps);
    return history_[curr_];
}

std::string BrowserHistorySolution::forward(int steps) {
    curr_ = std::min(end_, curr_ + steps);
    return history_[curr_];
}

// FoodRatingsSolution 实现
FoodRatingsSolution::FoodRatingsSolution(std::vector<std::string>& foods,
                                         std::vector<std::string>& cuisines,
                                         std::vector<int>& ratings) {
    int n = static_cast<int>(foods.size());
    for (int i = 0; i < n; ++i) {
        foodInfo_[foods[i]] = {cuisines[i], ratings[i]};
        cuisineSet_[cuisines[i]].insert({-ratings[i], foods[i]});
    }
}

void FoodRatingsSolution::changeRating(const std::string& food, int newRating) {
    auto& [cuisine, oldRating] = foodInfo_[food];
    cuisineSet_[cuisine].erase({-oldRating, food});
    cuisineSet_[cuisine].insert({-newRating, food});
    oldRating = newRating;
}

std::string FoodRatingsSolution::highestRated(const std::string& cuisine) {
    return cuisineSet_[cuisine].begin()->second;
}

// StockPriceSolution 实现
StockPriceSolution::StockPriceSolution() : latestTime_(0) {}

void StockPriceSolution::update(int timestamp, int price) {
    if (prices_.count(timestamp)) {
        priceSet_.erase(priceSet_.find(prices_[timestamp]));
    }
    prices_[timestamp] = price;
    priceSet_.insert(price);
    latestTime_ = std::max(latestTime_, timestamp);
}

int StockPriceSolution::current() {
    return prices_[latestTime_];
}

int StockPriceSolution::maximum() {
    return *priceSet_.rbegin();
}

int StockPriceSolution::minimum() {
    return *priceSet_.begin();
}

// AutocompleteSystemSolution 实现
struct AutocompleteSystemSolution::TrieNode {
    std::unordered_map<char, TrieNode*> children;
    std::unordered_map<std::string, int> sentences;
};

AutocompleteSystemSolution::AutocompleteSystemSolution(std::vector<std::string>& sentences, std::vector<int>& times) {
    root_ = new TrieNode();
    curr_ = root_;

    for (size_t i = 0; i < sentences.size(); ++i) {
        addSentence(sentences[i], times[i]);
    }
}

void AutocompleteSystemSolution::addSentence(const std::string& sentence, int count) {
    TrieNode* node = root_;
    for (char c : sentence) {
        if (!node->children.count(c)) {
            node->children[c] = new TrieNode();
        }
        node = node->children[c];
        node->sentences[sentence] += count;
    }
}

std::vector<std::string> AutocompleteSystemSolution::input(char c) {
    if (c == '#') {
        addSentence(currentInput_, 1);
        currentInput_.clear();
        curr_ = root_;
        return {};
    }

    currentInput_ += c;

    if (!curr_ || !curr_->children.count(c)) {
        curr_ = nullptr;
        return {};
    }

    curr_ = curr_->children[c];

    std::vector<std::pair<int, std::string>> candidates;
    for (auto& [sentence, count] : curr_->sentences) {
        candidates.push_back({count, sentence});
    }

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

// ==================== 测试函数 ====================

void testTwitterDesignSolution() {
    std::cout << "=== Twitter Design Tests (Solution) ===\n";

    // TwitterSolution 测试
    TwitterSolution twitter;
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
    std::cout << "  TwitterSolution: PASSED\n";

    // FileSystemSolution 测试
    FileSystemSolution fs;
    assert(fs.createPath("/a", 1) == true);
    assert(fs.get("/a") == 1);
    assert(fs.createPath("/a/b", 2) == true);
    assert(fs.get("/a/b") == 2);
    assert(fs.createPath("/c/d", 3) == false);
    std::cout << "  FileSystemSolution: PASSED\n";

    // BrowserHistorySolution 测试
    BrowserHistorySolution bh("leetcode.com");
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
    std::cout << "  BrowserHistorySolution: PASSED\n";

    // FoodRatingsSolution 测试
    std::vector<std::string> foods = {"kimchi", "miso", "sushi", "moussaka", "ramen", "bulgogi"};
    std::vector<std::string> cuisines = {"korean", "japanese", "japanese", "greek", "japanese", "korean"};
    std::vector<int> ratings = {9, 12, 8, 15, 14, 7};
    FoodRatingsSolution fr(foods, cuisines, ratings);
    assert(fr.highestRated("korean") == "kimchi");
    assert(fr.highestRated("japanese") == "ramen");
    fr.changeRating("sushi", 16);
    assert(fr.highestRated("japanese") == "sushi");
    std::cout << "  FoodRatingsSolution: PASSED\n";

    // StockPriceSolution 测试
    StockPriceSolution sp;
    sp.update(1, 10);
    sp.update(2, 5);
    assert(sp.current() == 5);
    assert(sp.maximum() == 10);
    sp.update(1, 3);
    assert(sp.maximum() == 5);
    sp.update(4, 2);
    assert(sp.minimum() == 2);
    std::cout << "  StockPriceSolution: PASSED\n";

    // AutocompleteSystemSolution 测试
    std::vector<std::string> sentences = {"i love you", "island", "iroman", "i love leetcode"};
    std::vector<int> times = {5, 3, 2, 2};
    AutocompleteSystemSolution ac(sentences, times);
    auto result = ac.input('i');
    assert(result.size() == 3);
    assert(result[0] == "i love you");
    result = ac.input(' ');
    assert(result.size() == 2);
    assert(result[0] == "i love you");
    assert(result[1] == "i love leetcode");
    ac.input('#');
    std::cout << "  AutocompleteSystemSolution: PASSED\n";
}

} // namespace TwitterDesign
