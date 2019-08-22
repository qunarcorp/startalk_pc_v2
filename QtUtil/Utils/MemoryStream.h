//
// Created by may on 2018/8/15.
//

#ifndef LIBEVENTTEST_MEMORYSTREAM_H
#define LIBEVENTTEST_MEMORYSTREAM_H


#include <cstdint>
#include <list>
#include <sstream>

namespace QTalk {
    namespace utils {
        class MemoryStream {
            std::ostringstream ostringstream;
            std::string output;

        public:
            MemoryStream() {}

            ~MemoryStream() {
            }

            void clear() {
                ostringstream.str("");
                output = ostringstream.str();
                length = 0;
            }

            template<typename insert_type>
            MemoryStream &operator<<(insert_type value) {
                ostringstream << value;
                length = static_cast<int>(ostringstream.str().length());
                output = ostringstream.str();
                return *this;
            }

            int length = 0;

            std::basic_string<char, std::char_traits<char>, std::allocator<char>> getDataWithRange(int pos, int length) {
                return output.substr(pos, length);
            }

            const char *bytes() {
                return output.c_str();
            }
        };
    }
}

#endif //LIBEVENTTEST_MEMORYSTREAM_H
