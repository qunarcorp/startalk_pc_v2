//
// Created by may on 2019-02-13.
//

#ifndef QTALK_V2_SFORMAT_H
#define QTALK_V2_SFORMAT_H


#pragma once

#include <tuple>
#include <string>
#include <algorithm>
#include <sstream>

template<class T>
inline void ToString(std::ostringstream &ret, T &&val) {
//    ret << std::to_string(std::forward<T>(val));
    ret << val;

//    ret.append(std::to_string(std::forward<T>(val)));
}

inline void ToString(std::ostringstream &ret, const tm *val) {
    ret << val;
}

inline void ToString(std::ostringstream &ret, int &val) {
    ret << val;
//    ret.append(std::to_string(val));
}

inline void ToString(std::ostringstream &ret, const bool val) {
    static std::string trueString = "bool(true)";
    static std::string falseString = "bool(false)";

    ret << (val ? trueString : falseString);
//    ret.append(val ? "bool(true)" : "bool(false)");
}

inline void ToString(std::ostringstream &ret, std::string &val) {
    ret << val;
//    ret.append(val);
}

inline void ToString(std::ostringstream &ret, const std::string &val) {
    ret << val;
//    ret.append(val);
}

inline void ToString(std::ostringstream &ret, const char *val) {
    ret << val;
//    ret.append(val);
}

inline void ToString(std::ostringstream &ret, char *val) {
    ret << val;
//    ret.append(val);
}

//template<typename T, typename=std::enable_if_t<std::is_enum<T>::value>>
//inline void ToString(std::string &ret, const T val) {
//    ret.append(val);
//}

template<int N>
struct SFormatN {
    static std::string Format(const char *fmt) {
//        static_assert(false, "");
        int i = 10;
    }
};

template<>
struct SFormatN<0> {
    template<class ...ARGS>
    static std::string Format(const char *fmt, const std::tuple<ARGS...> &) {
        return fmt;
    }
};

template<class ...ARGS>
std::string SFormat(const char *fmt, const ARGS &...args) {
    const auto tuple = std::forward_as_tuple(args...);
    return SFormatN<sizeof...(args)>::Format(fmt, tuple);
}

#define FMT_N(idx)    case idx: ToString(os, std::get<idx>(args)); break;

#define FMT_PARSE(N, ...)                                                            \
template <>                                                                            \
struct SFormatN<N> {                                                                \
    template <class... ARGS>                                                        \
    static std::string Format(const char * fmt, const std::tuple<ARGS...> & args)    \
    {    std::ostringstream os;                                                            \
        while (*fmt != '\0') { auto idx = -1;                                        \
            if (*fmt == '{') { idx = 0; ++fmt;                                        \
                while (*fmt >= '0' && *fmt <= '9')                                    \
                    { idx *= 10; idx += (int)(*fmt++ - '0'); }                        \
                if (*fmt != '}') idx = -1; else ++fmt;                                \
            }                                                                        \
            switch (idx) { __VA_ARGS__ default: /*ret.append(1, *fmt++);*/ os << *fmt++; break; }        \
        }                                                                            \
        return os.str();                                                                    \
    }                                                                                \
};

FMT_PARSE(1, FMT_N(0))

FMT_PARSE(2, FMT_N(0)
        FMT_N(1))

FMT_PARSE(3, FMT_N(0)
        FMT_N(1)
        FMT_N(2))

FMT_PARSE(4, FMT_N(0)
        FMT_N(1)
        FMT_N(2)
        FMT_N(3))

FMT_PARSE(5, FMT_N(0)
        FMT_N(1)
        FMT_N(2)
        FMT_N(3)
        FMT_N(4))

FMT_PARSE(6, FMT_N(0)
        FMT_N(1)
        FMT_N(2)
        FMT_N(3)
        FMT_N(4)
        FMT_N(5))

FMT_PARSE(7, FMT_N(0)
        FMT_N(1)
        FMT_N(2)
        FMT_N(3)
        FMT_N(4)
        FMT_N(5)
        FMT_N(6))

FMT_PARSE(8, FMT_N(0)
        FMT_N(1)
        FMT_N(2)
        FMT_N(3)
        FMT_N(4)
        FMT_N(5)
        FMT_N(6)
        FMT_N(7))

FMT_PARSE(9, FMT_N(0)
        FMT_N(1)
        FMT_N(2)
        FMT_N(3)
        FMT_N(4)
        FMT_N(5)
        FMT_N(6)
        FMT_N(7)
        FMT_N(8))

FMT_PARSE(10, FMT_N(0)
        FMT_N(1)
        FMT_N(2)
        FMT_N(3)
        FMT_N(4)
        FMT_N(5)
        FMT_N(6)
        FMT_N(7)
        FMT_N(8)
        FMT_N(9))

FMT_PARSE(11, FMT_N(0)
        FMT_N(1)
        FMT_N(2)
        FMT_N(3)
        FMT_N(4)
        FMT_N(5)
        FMT_N(6)
        FMT_N(7)
        FMT_N(8)
        FMT_N(9)
        FMT_N(10))

FMT_PARSE(12, FMT_N(0)
        FMT_N(1)
        FMT_N(2)
        FMT_N(3)
        FMT_N(4)
        FMT_N(5)
        FMT_N(6)
        FMT_N(7)
        FMT_N(8)
        FMT_N(9)
        FMT_N(10)
        FMT_N(11))

FMT_PARSE(13, FMT_N(0)
        FMT_N(1)
        FMT_N(2)
        FMT_N(3)
        FMT_N(4)
        FMT_N(5)
        FMT_N(6)
        FMT_N(7)
        FMT_N(8)
        FMT_N(9)
        FMT_N(10)
        FMT_N(11)
        FMT_N(12))

FMT_PARSE(14, FMT_N(0)
        FMT_N(1)
        FMT_N(2)
        FMT_N(3)
        FMT_N(4)
        FMT_N(5)
        FMT_N(6)
        FMT_N(7)
        FMT_N(8)
        FMT_N(9)
        FMT_N(10)
        FMT_N(11)
        FMT_N(12)
        FMT_N(13))

FMT_PARSE(15, FMT_N(0)
        FMT_N(1)
        FMT_N(2)
        FMT_N(3)
        FMT_N(4)
        FMT_N(5)
        FMT_N(6)
        FMT_N(7)
        FMT_N(8)
        FMT_N(9)
        FMT_N(10)
        FMT_N(11)
        FMT_N(12)
        FMT_N(13)
        FMT_N(14))

FMT_PARSE(16, FMT_N(0)
        FMT_N(1)
        FMT_N(2)
        FMT_N(3)
        FMT_N(4)
        FMT_N(5)
        FMT_N(6)
        FMT_N(7)
        FMT_N(8)
        FMT_N(9)
        FMT_N(10)
        FMT_N(11)
        FMT_N(12)
        FMT_N(13)
        FMT_N(14)
        FMT_N(15))

FMT_PARSE(17, FMT_N(0)
        FMT_N(1)
        FMT_N(2)
        FMT_N(3)
        FMT_N(4)
        FMT_N(5)
        FMT_N(6)
        FMT_N(7)
        FMT_N(8)
        FMT_N(9)
        FMT_N(10)
        FMT_N(11)
        FMT_N(12)
        FMT_N(13)
        FMT_N(14)
        FMT_N(15)
        FMT_N(16))

FMT_PARSE(18, FMT_N(0)
        FMT_N(1)
        FMT_N(2)
        FMT_N(3)
        FMT_N(4)
        FMT_N(5)
        FMT_N(6)
        FMT_N(7)
        FMT_N(8)
        FMT_N(9)
        FMT_N(10)
        FMT_N(11)
        FMT_N(12)
        FMT_N(13)
        FMT_N(14)
        FMT_N(15)
        FMT_N(16)
        FMT_N(17))

FMT_PARSE(19, FMT_N(0)
        FMT_N(1)
        FMT_N(2)
        FMT_N(3)
        FMT_N(4)
        FMT_N(5)
        FMT_N(6)
        FMT_N(7)
        FMT_N(8)
        FMT_N(9)
        FMT_N(10)
        FMT_N(11)
        FMT_N(12)
        FMT_N(13)
        FMT_N(14)
        FMT_N(15)
        FMT_N(16)
        FMT_N(17)
        FMT_N(18))

FMT_PARSE(20, FMT_N(0)
        FMT_N(1)
        FMT_N(2)
        FMT_N(3)
        FMT_N(4)
        FMT_N(5)
        FMT_N(6)
        FMT_N(7)
        FMT_N(8)
        FMT_N(9)
        FMT_N(10)
        FMT_N(11)
        FMT_N(12)
        FMT_N(13)
        FMT_N(14)
        FMT_N(15)
        FMT_N(16)
        FMT_N(17)
        FMT_N(18)
        FMT_N(19))

FMT_PARSE(21, FMT_N(0)
        FMT_N(1)
        FMT_N(2)
        FMT_N(3)
        FMT_N(4)
        FMT_N(5)
        FMT_N(6)
        FMT_N(7)
        FMT_N(8)
        FMT_N(9)
        FMT_N(10)
        FMT_N(11)
        FMT_N(12)
        FMT_N(13)
        FMT_N(14)
        FMT_N(15)
        FMT_N(16)
        FMT_N(17)
        FMT_N(18)
        FMT_N(19)
        FMT_N(20))

FMT_PARSE(22, FMT_N(0)
        FMT_N(1)
        FMT_N(2)
        FMT_N(3)
        FMT_N(4)
        FMT_N(5)
        FMT_N(6)
        FMT_N(7)
        FMT_N(8)
        FMT_N(9)
        FMT_N(10)
        FMT_N(11)
        FMT_N(12)
        FMT_N(13)
        FMT_N(14)
        FMT_N(15)
        FMT_N(16)
        FMT_N(17)
        FMT_N(18)
        FMT_N(19)
        FMT_N(20)
        FMT_N(21))

FMT_PARSE(23, FMT_N(0)
        FMT_N(1)
        FMT_N(2)
        FMT_N(3)
        FMT_N(4)
        FMT_N(5)
        FMT_N(6)
        FMT_N(7)
        FMT_N(8)
        FMT_N(9)
        FMT_N(10)
        FMT_N(11)
        FMT_N(12)
        FMT_N(13)
        FMT_N(14)
        FMT_N(15)
        FMT_N(16)
        FMT_N(17)
        FMT_N(18)
        FMT_N(19)
        FMT_N(20)
        FMT_N(21)
        FMT_N(22))

FMT_PARSE(24, FMT_N(0)
        FMT_N(1)
        FMT_N(2)
        FMT_N(3)
        FMT_N(4)
        FMT_N(5)
        FMT_N(6)
        FMT_N(7)
        FMT_N(8)
        FMT_N(9)
        FMT_N(10)
        FMT_N(11)
        FMT_N(12)
        FMT_N(13)
        FMT_N(14)
        FMT_N(15)
        FMT_N(16)
        FMT_N(17)
        FMT_N(18)
        FMT_N(19)
        FMT_N(20)
        FMT_N(21)
        FMT_N(22)
        FMT_N(23))

#endif //QTALK_V2_SFORMAT_H
