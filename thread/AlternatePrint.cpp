//
// Created by asus on 2025/7/22.
//

#include "AlternatePrint.h"

#include <condition_variable>
#include <iostream>
#include <mutex>

namespace AlternatePrint {
    std::mutex printMutex;
    std::condition_variable printCondition;
    int count = 10;
    bool isZero = false;


    void printOneFunc() {
        for (int i = 0; i < count;) {
            std::unique_lock<std::mutex> lock(printMutex);
            printCondition.wait(lock, [=] {
                return !isZero;
            });
            isZero = !isZero;
            std::cout << "subThread ID: " << std::this_thread::get_id() << " print: " << 1 << std::endl;
            i++;
            lock.unlock();
            printCondition.notify_all();
        }
    }

    void printZeroFunc() {
        for (int i = 0; i < count;) {
            std::unique_lock<std::mutex> lock(printMutex);
            printCondition.wait(lock, [=] {
                return isZero;
            });
            isZero = !isZero;
            std::cout << "subThread ID: " << std::this_thread::get_id() << " print: " << 0 << std::endl;
            lock.unlock();
            i++;
            printCondition.notify_all();
        }
    }

    void solution() {
        std::cout << "AlternatePrint::solution" << std::endl;
        std::thread printOneThread(printOneFunc), printZeroThread(printZeroFunc);
        printOneThread.join();
        printZeroThread.join();
    }
};
