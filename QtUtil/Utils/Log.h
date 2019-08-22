//
// Created by QITMAC000260 on 2018/12/4.
//

#ifndef QTALK_V2_LOG_H
#define QTALK_V2_LOG_H

#include <string>
#include "../qtutil_global.h"

#include <iostream>
#include "../../include/sformat.h"


#define __text_out(...) (SFormat(__VA_ARGS__).c_str())

#define LOGGER_LEVEL_INFO

#ifndef _WINDOWS

#define info_log(fmt, ...) \
do {   \
    QTalk::logger::info(SFormat(SFormat("{3}\n{0}({1}):{2}", __FILE__, __LINE__, __PRETTY_FUNCTION__, fmt).c_str(), ##__VA_ARGS__)); \
} while (0)

#define warn_log(fmt, ...) \
do {   \
    QTalk::logger::warning(SFormat(SFormat("{3}\n{0}({1}):{2}", __FILE__, __LINE__, __PRETTY_FUNCTION__, fmt).c_str(), ##__VA_ARGS__)); \
} while (0)

#define error_log(fmt, ...) \
do {   \
    QTalk::logger::error(SFormat(SFormat("{3}\n{0}({1}):{2}", __FILE__, __LINE__, __PRETTY_FUNCTION__, fmt).c_str(), ##__VA_ARGS__)); \
} while (0)

#define critical_log(fmt, ...) \
do {   \
    QTalk::logger::critical(SFormat(SFormat("{3}\n{0}({1}):{2}", __FILE__, __LINE__, __PRETTY_FUNCTION__, fmt).c_str(), ##__VA_ARGS__)); \
} while (0)


#else
#define info_log(fmt, ...) \
do {   \
    QTalk::logger::info(SFormat(SFormat("{3}\n{0}({1}):{2}", __FILE__, __LINE__, __FUNCTION__, fmt).c_str(), ##__VA_ARGS__)); \
} while (0)

#define warn_log(fmt, ...) \
do {   \
    QTalk::logger::warning(SFormat(SFormat("{3}\n{0}({1}):{2}", __FILE__, __LINE__, __FUNCTION__, fmt).c_str(), ##__VA_ARGS__)); \
} while (0)


#define error_log(fmt, ...) \
do {   \
    QTalk::logger::error(SFormat(SFormat("{3}\n{0}({1}):{2}", __FILE__, __LINE__, __FUNCTION__, fmt).c_str(), ##__VA_ARGS__)); \
} while (0)

#define critical_log(fmt, ...) \
do {   \
    QTalk::logger::critical(SFormat(SFormat("{3}\n{0}({1}):{2}", __FILE__, __LINE__, __FUNCTION__, fmt).c_str(), ##__VA_ARGS__)); \
} while (0)
#endif


/**
 * log 日志类
 */
namespace QTalk {

    namespace logger {

        struct None {
        };

        template<typename First, typename Second>
        struct Pair {
            First first;
            Second second;
        };

        template<typename List>
        struct LogData {
            List list;
        };

        template<typename Begin, typename Value>
        LogData<Pair<Begin, const Value &>>
        operator<<(LogData<Begin> begin, const Value &value) {
            return {{begin.list, value}};
        }

        template<typename Begin, size_t n>
        LogData<Pair<Begin, const char *>>
        operator<<(LogData<Begin> begin, const char (&value)[n]) {
            return {{begin.list, value}};
        }

        inline void printList(std::ostream &os, None) {
        }


        template<typename Begin, typename Last>
        void printList(std::ostream &os, const Pair<Begin, Last> &data) {
            printList(os, data.first);
            os << data.second;
        }

        template<typename List>
        void log(const char *file, int line, const LogData<List> &data) {
            std::cout << file << " (" << line << "): ";
            printList(std::cout, data.list);
            std::cout << "\n";
        }


        enum {
            LEVEL_INVALID, LEVEL_INFO, LEVEL_WARING, LEVEL_ERROR
        };

        // 初始化log
        void QTALK_UTIL_EXPORT initLog(const std::string &logDir, unsigned char level);

        // 提示类消息
        bool QTALK_UTIL_EXPORT info(const std::string &msg);

        // 警告类消息
        bool QTALK_UTIL_EXPORT warning(const std::string &msg);

        // 错误消息
        bool QTALK_UTIL_EXPORT error(const std::string &msg);

        bool QTALK_UTIL_EXPORT critical(const std::string &msg);

        // 手动刷新消息
        void QTALK_UTIL_EXPORT flush();

        // 退出
        void QTALK_UTIL_EXPORT exit();

        // 设置日志level
        void QTALK_UTIL_EXPORT setLevel(unsigned char level);

        class log {

        public:
            template<class ...ARGS>
            static void info(const char *fmt, const ARGS &...args) {
                const auto tuple = std::forward_as_tuple(args...);
                std::string result = SFormatN<sizeof...(args)>::Format(fmt, tuple);
                QTalk::logger::info(result);
            }

            template<class ...ARGS>
            static void warning(const char *fmt, const ARGS &...args) {
                const auto tuple = std::forward_as_tuple(args...);
                std::string result = SFormatN<sizeof...(args)>::Format(fmt, tuple);

            }

            template<class ...ARGS>
            static void error(const char *fmt, const ARGS &...args) {
                const auto tuple = std::forward_as_tuple(args...);
                std::string result = SFormatN<sizeof...(args)>::Format(fmt, tuple);
                QTalk::logger::error(result);
            }

            template<class ...ARGS>
            static void critical(const char *fmt, const ARGS &...args) {
                const auto tuple = std::forward_as_tuple(args...);
                std::string result = SFormatN<sizeof...(args)>::Format(fmt, tuple);
                QTalk::logger::critical(result);

            }

        };
    }
}
#endif //QTALK_V2_LOG_H
