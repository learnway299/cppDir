//
// Created by asus on 2025/7/22.
//

#include "MessageQueue.h"

#include <thread>
#include <iostream>

MessageQueue::MessageQueue(int maxValue) {
    m_iMaxSize = maxValue;
}

MessageQueue::~MessageQueue() {
}

void MessageQueue::put(int value) {
    std::unique_lock<std::mutex> lck(m_mutex);
    m_condFull.wait(lck, [this] { return m_queue.size() < m_iMaxSize; });
    m_queue.push(value);
    std::cout << "subThread ID: " << std::this_thread::get_id() << " put: " << value << std::endl;
    lck.unlock();
    m_condEmpty.notify_one();
}

int MessageQueue::get() {
    std::unique_lock<std::mutex> lck(m_mutex);
    m_condEmpty.wait(lck, [this] { return !m_queue.empty(); });
    int value = m_queue.front();
    m_queue.pop();
    std::cout << "subThread ID: " << std::this_thread::get_id() << " get: " << value << std::endl;
    lck.unlock();
    m_condFull.notify_one();
    return value;
}

namespace ProduceCustomer {
    void customerFunc(MessageQueue &queue, int id, int count) {
        std::thread::id this_thread_id = std::this_thread::get_id(); // »ñÈ¡µ±Ç°Ïß³ÌµÄID
        for (int i = 0; i < count; ++i) {
            queue.get();
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }

    void producerFunc(MessageQueue &queue, int id, int count) {
        std::thread::id this_thread_id = std::this_thread::get_id(); // »ñÈ¡µ±Ç°Ïß³ÌµÄID
        for (int i = 0; i < count; ++i) {
            queue.put(id);
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }

    void solution() {
        std::cout << "ProduceCustomer::solution" << std::endl;
        const int ququeSize = 10;
        const int producerCount = 3;
        const int customerCount = 2;
        const int valueCont = 10;

        MessageQueue MessageQu(ququeSize);

        std::vector<std::thread> producers;
        std::vector<std::thread> customers;

        for (int i = 0; i < producerCount; ++i) {
            producers.emplace_back(producerFunc, std::ref(MessageQu), i, valueCont);
        }
        for (int i = 0; i < customerCount; ++i) {
            customers.emplace_back(customerFunc, std::ref(MessageQu), i, valueCont);
        }

        for (auto &produ: producers) {
            produ.join();
        }
        for (auto &cust: customers) {
            cust.join();
        }
    }
}
