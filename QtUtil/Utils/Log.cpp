//
// Created by QITMAC000260 on 2018/12/4.
//

#include <sstream>
#include "Log.h"
#include "../QtUtil/lib/spdlog/spdlog.h"
#include "../QtUtil/lib/spdlog/async.h"
#include "../QtUtil/lib/spdlog/common.h"
#include "../QtUtil/lib/spdlog/sinks/stdout_color_sinks.h"
#include "../QtUtil/lib/spdlog/sinks/rotating_file_sink.h"
#include "../QtUtil/lib/spdlog/sinks/daily_file_sink.h"

namespace QTalk {

    namespace logger {

        unsigned char _level = LEVEL_INFO;

        void initLog(const std::string &logDir, unsigned char level) {

            _level = level;

            static const int fileSize = 1024 * 1024 * 20;

            time_t t;
            t = time(nullptr);
			struct tm *local = nullptr;
            local = localtime(&t);
            char buf[128] = {0};
            strftime(buf, 64, "%Y-%m-%d_%H-%M-%S", local);

            std::string filname;
            filname.append("started_at_");
            filname.append(buf);
            filname.append(".log");

            std::string fullpath;
            fullpath.append(logDir);
            fullpath.append(filname);

            spdlog::init_thread_pool(8192, 1);

            auto rotating_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(fullpath.c_str(),
                                                                                        fileSize,
                                                                                        3);
            std::vector<spdlog::sink_ptr> sinks;
            if(LEVEL_INFO == _level) {
                auto stdout_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
                sinks = {stdout_sink, rotating_sink};
            } else {
                sinks = {rotating_sink};
            }

            auto logger = std::make_shared<spdlog::async_logger>("LOG", sinks.begin(), sinks.end(),
                                                                 spdlog::thread_pool(),
                                                                 spdlog::async_overflow_policy::block);

            spdlog::register_logger(logger);

        }

        void setLevel(unsigned char level)
        {
            _level = level;
        }

        bool info(const std::string &msg) {

            if(LEVEL_INFO < _level)
                return false;

            std::thread::id this_id = std::this_thread::get_id();

            std::stringstream ss;
            ss << msg;
            ss << "\nat ThreadID:";
            ss << this_id;

            spdlog::get("LOG")->info(ss.str());
            spdlog::get("LOG")->flush();
//            std::cout << " --- LOG_INFO --- " << msg << std::endl;
            return true;
        }

        bool warning(const std::string &msg) {
            if(LEVEL_WARING < _level)
                return false;

            spdlog::get("LOG")->warn(msg);
            spdlog::get("LOG")->flush();
//            std::cout << " --- LOG_WARN --- " << msg << std::endl;
            return true;
        }

        bool error(const std::string &msg) {

            if(LEVEL_ERROR < _level)
                return false;

            spdlog::get("LOG")->error(msg);
            spdlog::get("LOG")->flush();

//             std::cout << " --- LOG_CRIT --- " << msg << std::endl;
            return true;
        }

        bool critical(const std::string &msg) {

            spdlog::get("LOG")->critical(msg);
            spdlog::get("LOG")->flush();

//             std::cout << " --- LOG_CRIT --- " << msg << std::endl;
            return true;
        }

        void flush() {
            spdlog::get("LOG")->flush();
        }

        void exit() {
            spdlog::shutdown();
        }
    }

}