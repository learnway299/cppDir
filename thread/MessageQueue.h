//
// Created by asus on 2025/7/22.
//

#ifndef MESSAGEQUEUE_H
#define MESSAGEQUEUE_H

#include <queue>
#include <condition_variable>

class MessageQueue {
public:
    MessageQueue(int maxValue);

    ~MessageQueue();

    void put(int value);

    int get();

private:
    int m_iMaxSize{0};
    std::queue<int> m_queue;
    std::mutex m_mutex;
    std::condition_variable m_condFull, m_condEmpty;
};

namespace ProduceCustomer {
    void customerFunc(MessageQueue &queue, int id, int count);

    void producerFunc(MessageQueue &queue, int id, int count);

    void solution();
}


#endif //MESSAGEQUEUE_H
