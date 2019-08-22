//
// Created by may on 2019-03-18.
//

#ifndef STLAZYQUEUE_H
#define STLAZYQUEUE_H

#define QUEUE_DEBUG 0


#include <mutex>
#include <queue>
#include <thread>

#include <iostream>
#include <functional>
#include <atomic>
#ifdef _WINDOWS
#include <windows.h>
#else
#include <unistd.h>
#endif
#include "../QtUtil/Utils/Log.h"

#define NANO_SECOND_MULTIPLIER  1000000  // 1 millisecond = 1,000,000 Nanoseconds
const unsigned long INTERVAL_MS = 10 * NANO_SECOND_MULTIPLIER;

template<typename T>
class STLazyQueue {
public:
    inline void init(unsigned long delayMs, std::function<int(STLazyQueue<T> *)> &func) {
        _delayMs = delayMs * NANO_SECOND_MULTIPLIER;
        localFunc = [this, func, delayMs]() {
#if QUEUE_DEBUG
            std::cout << "queue func in, counter is " << _checkCounter << std::endl;
#endif

            while (_checkCounter > 0) {
                unsigned long size = this->size();
                std::lock_guard<std::mutex> lock(runner_mutex);

#ifdef _WINDOWS
				Sleep(delayMs);
#else
				if (_delayMs > 0) {
					struct timespec tim {};
					tim.tv_sec = 0;
					tim.tv_nsec = _delayMs;
					nanosleep(&tim, nullptr);
				}
#endif // _WINDOWS

                

                unsigned long newSize = this->size();
#if QUEUE_DEBUG
                std::cout << "delayed, counter is " << _checkCounter << ", and first size is " << size
                          << ", and second size is " << newSize << std::endl;
#endif

                if (size != newSize) {
#if QUEUE_DEBUG
                    std::cout << "size != newSize, continue, counter is " << _checkCounter << std::endl;
#endif


                    continue;
                } else {
#if QUEUE_DEBUG
                    std::cout << "same size, size is " << newSize << std::endl;
#endif
                    if (!this->empty()) {

                        int count = this->size();
                        info_log("LAZY QUEUE: will process items count: {0}", count);

                        try {
                            int oked = func(this);
                        } catch (std::exception &exception) {
                            error_log("error occoured in LazyQueue, {}", exception.what());
                        }

                    }
                    _checkCounter--;
                }
            }
//            std::cout << "Bye Bye!" << std::endl;
        };
    }

    explicit STLazyQueue(std::function<int(STLazyQueue<T> *)> &func) : _checkCounter(0) {
        init(INTERVAL_MS, func);
    }

    STLazyQueue(unsigned long delayMs, std::function<int(STLazyQueue<T> *)> &func) : _checkCounter(0) {
        init(delayMs, func);
    }

    void checkRunner() {
        if (runner_mutex.try_lock()) {
            std::thread runner(localFunc);
            runner_mutex.unlock();
            runner.detach();
        }
    }

    void push(const T &value) {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_queque.push(value);

        _checkCounter++;

        checkRunner();
    }

    unsigned long size() {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_queque.size();
    }

    bool empty() {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_queque.empty();
    }

    T tail() {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_queque.back();
    }

    T front() {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_queque.front();
    }

    void pop() {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_queque.pop();
    }

    void clear() {
        std::lock_guard<std::mutex> lock(m_mutex);
        while(!m_queque.empty()) m_queque.pop();;
    }

private:

	unsigned long _delayMs;

    std::atomic<int> _checkCounter;
    std::function<void()> localFunc;
    std::queue<T> m_queque;
    mutable std::mutex m_mutex;
    mutable std::mutex runner_mutex;
};

#endif //STLAZYQUEUE_H
