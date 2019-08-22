//
// Created by may on 2018/8/16.
//

#ifndef QTALK_IO_UTILS_H
#define QTALK_IO_UTILS_H

#include <queue>
#include <mutex>
#include <condition_variable>
#include <algorithm>
#include <stdexcept>
#include <string>
#include <functional>

#include "../qtutil_global.h"

namespace QTalk {
    namespace utils {
        namespace strings {
            std::string QTALK_UTIL_EXPORT struct2string(void *ptr, size_t size);

            bool QTALK_UTIL_EXPORT
            string2struct(std::string &input, std::function<void(void *, int)> callback);
        };

        std::string QTALK_UTIL_EXPORT string_to_hex(const std::string &input, size_t len);

        std::string QTALK_UTIL_EXPORT hex_to_string(const std::string &input);

        bool QTALK_UTIL_EXPORT smallEndian();

        uint32_t QTALK_UTIL_EXPORT EndianIntConvertBToLittle(uint32_t InputNum);

        int32_t QTALK_UTIL_EXPORT EndianIntConvertLToBig(int32_t InputNum);

        std::string QTALK_UTIL_EXPORT encodeWithGZip(std::string *input);

        std::string QTALK_UTIL_EXPORT decodeWithGZip(std::string *input);

        std::string QTALK_UTIL_EXPORT UrlEncode(const std::string& str);

        void QTALK_UTIL_EXPORT generateUUID(char *str);

        std::string QTALK_UTIL_EXPORT format(const char *format, ...);

        std::string QTALK_UTIL_EXPORT getMessageId();

        std::string QTALK_UTIL_EXPORT getFileMd5(const std::string& filePath);

        std::string QTALK_UTIL_EXPORT getFileSuffix(const std::string& filePath);

        // 暂时不可用
        std::string QTALK_UTIL_EXPORT getImageSuffix(const std::string& filePath);

        template<typename T>
        class SharedQueue {
        public:
            SharedQueue();

            ~SharedQueue();

            T &front();

            T &back();

            void pop_front();

            void push_back(const T &item);

            void push_back(T &&item);

            int size();

            bool empty();

        private:
            std::deque<T> queue_;
            std::mutex mutex_;
            std::condition_variable cond_;
        };

        template<typename T>
        SharedQueue<T>::SharedQueue() {}

        template<typename T>
        SharedQueue<T>::~SharedQueue() {}

        template<typename T>
        T &SharedQueue<T>::front() {
            std::unique_lock<std::mutex> mlock(mutex_);
            while (queue_.empty()) {
                cond_.wait(mlock);
            }
            return queue_.front();
        }

        template<typename T>
        T &SharedQueue<T>::back() {
            std::unique_lock<std::mutex> mlock(mutex_);
            while (queue_.empty()) {
                cond_.wait(mlock);
            }
            return queue_.back();
        }

        template<typename T>
        void SharedQueue<T>::pop_front() {
            std::unique_lock<std::mutex> mlock(mutex_);
            while (queue_.empty()) {
                cond_.wait(mlock);
            }
            queue_.pop_front();
        }

        template<typename T>
        void SharedQueue<T>::push_back(const T &item) {
            std::unique_lock<std::mutex> mlock(mutex_);
            queue_.push_back(item);
            mlock.unlock();     // unlock before notificiation to minimize mutex con
            cond_.notify_one(); // notify one waiting thread

        }

        template<typename T>
        void SharedQueue<T>::push_back(T &&item) {
            std::unique_lock<std::mutex> mlock(mutex_);
            queue_.push_back(std::move(item));
            mlock.unlock();     // unlock before notificiation to minimize mutex con
            cond_.notify_one(); // notify one waiting thread
        }

        template<typename T>
        int SharedQueue<T>::size() {
            std::unique_lock<std::mutex> mlock(mutex_);
            int size = queue_.size();
            mlock.unlock();
            return size;
        }
    }
}

#endif //QTALK_IO_UTILS_H
