#include <utility>

//
// Created by may on 2019-02-15.
//

#ifndef QTALK_V2_PERFCOUNTER_H
#define QTALK_V2_PERFCOUNTER_H

#include "sformat.h"
#include "../QtUtil/Utils/Log.h"

namespace QTalk {
    namespace performance {

#define perf_counter(fmt, ...) QTalk::performance::TimeCounter counter(SFormat(fmt, ##__VA_ARGS__))

#define perf_counter_warning(value, fmt, ...) QTalk::performance::TimeCounter counter(value, SFormat(fmt, ##__VA_ARGS__))

        class TimeCounter {
        private:
            double _warningValue;
        public:
            explicit TimeCounter(double warningValue, const std::string &counterName) {
                _warningValue = warningValue;
                name = counterName;
                local_start = std::chrono::high_resolution_clock::now();
            }


            explicit TimeCounter(const std::string &counterName) : _warningValue(500) {
                name = counterName;
                local_start = std::chrono::high_resolution_clock::now();
            }

            ~TimeCounter() {
                auto end = std::chrono::high_resolution_clock::now();
                std::chrono::duration<double, std::milli> elapsed_milliseconds = end - local_start;

                double duration = elapsed_milliseconds.count();
                if (duration > _warningValue) {
                    warn_log("PERF WARNING!!! It tooks {0} ms. ({1})", duration, name);
                } else {
//                    time_t start_time = std::chrono::system_clock::to_time_t(start);
//                    time_t end_time = std::chrono::system_clock::to_time_t(end);
//                    struct tm *local_start = gmtime(&start_time);
//                    struct tm *local_end = gmtime(&end_time);

                    info_log("PERF Counter: {0}\nDuration:{1}ms", //, start at:{2}, end at:{3}
                             name,
                             duration);
//                             asctime(local_start),
//                             asctime(local_end));
                }
            }

        private:
            std::string name;
            std::chrono::high_resolution_clock::time_point local_start;
        };
    }
}

#endif //QTALK_V2_PERFCOUNTER_H
