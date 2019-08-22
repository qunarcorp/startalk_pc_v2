//
// Created by may on 2018/8/14.
//


#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <vector>
#include <queue>
#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <functional>
#include <stdexcept>
#include <sstream>


class ThreadPool {
public:
    explicit ThreadPool(size_t threads = 1, const char *threadName = "QTalk Thread");

    explicit ThreadPool(const char *threadName = "QTalk Thread");

    template<class F, class... Args>
    auto enqueue(F &&f, Args &&... args)
    -> std::future<typename std::result_of<F(Args...)>::type>;

    ~ThreadPool();

private:
    // need to keep track of threads so we can join them
    std::vector<std::thread> workers;
    // the task queue
    std::queue<std::function<void()> > tasks;

    // synchronization
    std::mutex queue_mutex;
    std::condition_variable condition;
    bool stop;
};

inline ThreadPool::ThreadPool(const char *threadName) : stop(false) {

    workers.emplace_back([this, threadName] {
#ifdef _MACOS
        pthread_setname_np(threadName);
#endif
        for (;;) {

            std::function<void()> task;

            {
                std::unique_lock<std::mutex> lock(this->queue_mutex);
                this->condition.wait(lock,
                                     [this] { return this->stop || !this->tasks.empty(); });
                if (this->stop && this->tasks.empty())
                    return;
                task = std::move(this->tasks.front());
                this->tasks.pop();
            }

            task();
        }
    });
}

// the constructor just launches some amount of workers
inline ThreadPool::ThreadPool(size_t threads, const char *threadName)
        : stop(false) {

    for (size_t i = 0; i < threads; ++i) {

        std::stringstream ss;

        ss << threadName;
        ss << " - ";
        ss << i;

        std::string yourName = ss.str();
        workers.emplace_back(
                [this, yourName] {
#ifdef _MACOS
                    pthread_setname_np(yourName.c_str());
#endif
                    for (;;) {
                        std::function<void()> task;

                        {
                            std::unique_lock<std::mutex> lock(this->queue_mutex);
                            this->condition.wait(lock,
                                                 [this] { return this->stop || !this->tasks.empty(); });
                            if (this->stop && this->tasks.empty())
                                return;
                            task = std::move(this->tasks.front());
                            this->tasks.pop();
                        }

                        task();
                    }
                }
        );
    }

}

// add new work item to the pool
template<class F, class... Args>
auto ThreadPool::enqueue(F &&f, Args &&... args)
-> std::future<typename std::result_of<F(Args...)>::type> {
    using return_type = typename std::result_of<F(Args...)>::type;

    auto task = std::make_shared<std::packaged_task<return_type()> >(
            std::bind(std::forward<F>(f), std::forward<Args>(args)...)
    );

    std::future<return_type> res = task->get_future();
    {
        std::unique_lock<std::mutex> lock(queue_mutex);

        // don't allow enqueueing after stopping the pool
        if (stop)
            throw std::runtime_error("enqueue on stopped ThreadPool");

        tasks.emplace([task]() { (*task)(); });
    }
    condition.notify_one();
    return res;
}

// the destructor joins all threads
inline ThreadPool::~ThreadPool() {
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        stop = true;
    }
    condition.notify_all();
    for (std::thread &worker: workers) {
        worker.join();
    }
}


#endif //THREAD_POOL_H