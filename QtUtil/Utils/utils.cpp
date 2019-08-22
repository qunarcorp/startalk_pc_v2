//
// Created by may on 2018/8/16.
//

#include <zlib.h>
#include <sstream>
#include <string.h>
#include "../lib/Base64/base64.h"
#include "utils.h"
#include <fstream>
#include <lib/Md5/Md5.h>

#ifdef _WINDOWS
#include <objbase.h>
#include <lib/Md5/Md5.h>

#else

#include <uuid/uuid.h>

#endif // _WINDOWS

namespace QTalk {
    namespace utils {
        namespace strings {
            const int struct2string_version = 0x01;

            typedef struct struct2string_ {
                int length;
                uint16_t version;

                struct2string_() {
                    length = 0;
                    version = struct2string_version;
                }

            } str2str_block;

            std::string struct2string(void *ptr, size_t size) {
                if (ptr != nullptr && size > 0) {
                    str2str_block block;
                    block.length = static_cast<int>(size);

                    int total = sizeof(block) + size;

                    auto *pItem = (unsigned char *) &block;

                    std::string monitor((char *) pItem, total);

                    std::string value = base64_encode((unsigned char const *) &block,
                                                      total);

                    return value;
                }
                return std::string();
            }

            bool string2struct(std::string &input, std::function<void(void *, int)> callback) {
//                std::string output = base64_decode(input);
//
//                if (!output.empty()) {
//                    auto *pblock = (str2str_block *) output.c_str();
//
//                    if (pblock->version == struct2string_version && pblock->ptr.size() > 0) {
//
//                        if (callback != nullptr) {
//                            callback((void *) pblock->ptr.c_str(), pblock->length);
//                        }
//                    }
//                    return true;
//                }
                return false;
            }
        };

        static const char *const hexTable = "0123456789ABCDEF";

        std::string string_to_hex(const std::string &input, size_t len) {

            std::string output;
            output.reserve(2 * len);
            for (size_t i = 0; i < len; ++i) {
                const auto c = static_cast<const unsigned char>(input[i]);
                output.push_back(hexTable[c >> 4]); // NOLINT
                output.push_back(hexTable[c & 15]);
            }
            return output;
        }

        std::string hex_to_string(const std::string &input) {
            size_t len = input.length();
            if (len & 1) throw std::invalid_argument("odd length");

            std::string output;
            output.reserve(len / 2);
            for (size_t i = 0; i < len; i += 2) {
                char a = input[i];
                const char *p = std::lower_bound(hexTable, hexTable + 16, a);
                if (*p != a) throw std::invalid_argument("not a hex digit");

                char b = input[i + 1];
                const char *q = std::lower_bound(hexTable, hexTable + 16, b);
                if (*q != b) throw std::invalid_argument("not a hex digit");

                output.push_back(static_cast<char>(((p - hexTable) << 4) | (q - hexTable))); // NOLINT
            }
            return output;
        }


        std::string format(const char *format, ...) {
            va_list args;
            va_start(args, format);
            auto len = static_cast<unsigned long>(std::vsnprintf(nullptr, 0, format, args));
            va_end(args);
            std::vector<char>
                    vec(len + 1);
            va_start(args, format);
            std::vsnprintf(&vec[0], len + 1, format, args);
            va_end(args);
            return &vec[0];
        }

        bool smallEndian() {
            int n = 1;
            return *(char *) &n;
        }

        uint32_t EndianIntConvertBToLittle(uint32_t InputNum) {
            if (!smallEndian()) {
                char *p = (char *) &InputNum;
                for (int i = 0; i < sizeof(InputNum); i++) {
                    //      NSLog("%d",p[i]);
                }
                uint32_t num, num1, num2, num3, num4;
                num1 = (uint32_t) (*p) << 24;
                num2 = ((uint32_t) *(p + 1)) << 16;
                num3 = ((uint32_t) *(p + 2)) << 8;
                num4 = ((uint32_t) *(p + 3));
                num = num1 + num2 + num3 + num4;
                //NSLog("num is %d",num);
                char *q = (char *) num;
                for (int i = 0; i < sizeof(num); i++) {
                    // NSLog("%d",q[i]);
                }
                return num;
            }
            return InputNum;
        }

        int32_t EndianIntConvertLToBig(int32_t InputNum) {
            //return InputNum;
            if (smallEndian()) {
                char *p = (char *) &InputNum;
                //printf("S");
                //for (int i = 0; i<sizeof(InputNum); i++) {
                //      NSLog("%d",p[i]);
                //  printf("%d\n",p[i]);
                //}
                char r[4];
                r[3] = p[0];
                r[2] = p[1];
                r[1] = p[2];
                r[0] = p[3];
                int32_t num;
                memcpy(&num, r, 4);
                /* int32_t num,num1,num2,num3,num4;
                 num1=(int32_t)(*p)<<24;
                 num2=((int32_t)*(p+1))<<16;
                 num3=((int32_t)*(p+2))<<8;
                 num4=((int32_t)*(p+3));
                 num=num1+num2+num3+num4;*/
                //NSLog("num is %d",num);
                char *q = (char *) &num;
                //printf("R");
                //for (int i = 0; i<sizeof(num); i++) {
                // NSLog("%d",q[i]);
                //printf("%d\n",q[i]);
                //}
                return num;
            }
            return InputNum;
        }

        typedef struct _tagCompreessBuffer {
            char *p;
            uLong length;
        } compressbuff;

        compressbuff *compressbuff_new(int size) {
            auto *buff = static_cast<compressbuff *>(malloc(sizeof(compressbuff)));
            buff->p = static_cast<char *>(malloc(sizeof(char) * size));
            buff->length = 0;
            return buff;
        }

        Bytef *compressbuff_pos(compressbuff *buf, int pos) {
            return (Bytef *) (buf->p + pos);
        }

        void compressbuff_free(compressbuff *buff) {
            free(buff->p);
            free(buff);
        }

        std::string encodeWithGZip(std::string *input) {
            if (input != nullptr && input->length() > 0) {
                int finished = 0;
                z_stream zStream;
                {
                    //
                    // init
                    memset(&zStream, 0, sizeof(zStream));

                    zStream.zalloc = Z_NULL;
                    zStream.zfree = Z_NULL;
                    zStream.opaque = Z_NULL;
                    zStream.avail_in = 0;
                    zStream.next_in = 0;

                    int windowsBits = 15;
                    int GZIP_ENCODING = 16;

                    int status = deflateInit2(&zStream,
                                              Z_DEFAULT_COMPRESSION,
                                              Z_DEFLATED,
                                              windowsBits | GZIP_ENCODING,
                                              8,
                                              Z_DEFAULT_STRATEGY);

                    if (status != Z_OK) {
                        return "";
                    }
                }

                std::ostringstream ostringstream;

                uint32_t buflen = 16;
                int status = 0;
                zStream.next_in = (Bytef *) input->c_str();
                zStream.avail_in = (uInt) input->length();
                zStream.avail_out = 0;

                uLong totalLen = 0;

                while (zStream.avail_out == 0) {
                    compressbuff *buf = compressbuff_new(buflen);

                    zStream.next_out = compressbuff_pos(buf, 0);
                    zStream.avail_out = buflen;
                    status = deflate(&zStream, (finished == 0) ? Z_FINISH : Z_NO_FLUSH);
                    if (status == Z_STREAM_END) {
                        buf->length = zStream.total_out - totalLen;
                        totalLen += buf->length;

                        std::string s(buf->p, buf->length);

                        ostringstream << s;
                        compressbuff_free(buf);
                        break;
                    } else if (status != Z_OK) {
                        compressbuff_free(buf);
                        return "";
                    }
                    buf->length = buflen;
                    totalLen += buf->length;

                    std::string s(buf->p, buf->length);
                    ostringstream << s;
                    compressbuff_free(buf);
                }
                // 清楚数据
                deflateEnd(&zStream);
                return ostringstream.str();
            }
            return "";
        }

        std::string decodeWithGZip(std::string *input) {

            if (input->length() > 0) {
                z_stream zStream;
                {
                    //
                    // init
                    memset(&zStream, 0, sizeof(zStream));

                    // Setup the inflate stream
                    zStream.zalloc = Z_NULL;
                    zStream.zfree = Z_NULL;
                    zStream.opaque = Z_NULL;
                    zStream.avail_in = 0;
                    zStream.next_in = 0;
                    int status = inflateInit2(&zStream, 16 + MAX_WBITS);
                    if (status != Z_OK) {
                        return "";
                    }
                }

                std::ostringstream ostringstream;
//
//
//            NSMutableData * buffers = [NSMutableData
//            data];
                int status;

                int buflen = 16;

                zStream.next_in = (z_const
                Bytef *) input->c_str();
                zStream.avail_in = (uInt) input->length();
                zStream.avail_out = 0;

                uLong totalLen = 0;

                while (zStream.avail_in != 0) {
                    compressbuff *buf = compressbuff_new(buflen);

                    zStream.next_out = compressbuff_pos(buf, 0);
                    zStream.avail_out = static_cast<uInt>(buflen);
                    status = inflate(&zStream, Z_NO_FLUSH);
                    //if (status == Z_STREAM_END) {
                    //    buf->length = zStream.total_out - totalLen;
                    //    totalLen += buf->length;
                    //    break;
                    //} else
                    if (status != Z_OK && status != Z_STREAM_END) {
                        compressbuff_free(buf);
                        return "";
                    }
                    buf->length = buflen - zStream.avail_out;
                    totalLen += buf->length;
                    std::string s(buf->p, buf->length);
                    ostringstream << s;
                    compressbuff_free(buf);
                }
                // 清楚数据
                inflateEnd(&zStream);
                return ostringstream.str();
            }
            return "";
        }

        unsigned char ToHex(unsigned char x)
        {
            return  x > 9 ? x + 55 : x + 48;
        }

        std::string UrlEncode(const std::string& str)
        {
            std::string strTemp = "";
            size_t length = str.length();
            for (size_t i = 0; i < length; i++)
            {
                if (isalnum((unsigned char)str[i]) ||
                    (str[i] == '-') ||
                    (str[i] == '_') ||
                    (str[i] == '.') ||
                    (str[i] == '~'))
                    strTemp += str[i];
                else if (str[i] == ' ')
                    strTemp += "+";
                else
                {
                    strTemp += '%';
                    strTemp += ToHex((unsigned char)str[i] >> 4);
                    strTemp += ToHex((unsigned char)str[i] % 16);
                }
            }
            return strTemp;
        }

        void generateUUID(char *str) {
#ifdef _WINDOWS
            GUID guid;
            CoCreateGuid(&guid);
            snprintf(str, 35, "%08X%04X%04x%02X%02X%02X%02X%02X%02X%02X%02X",
                guid.Data1, guid.Data2, guid.Data3, guid.Data4[0], guid.Data4[1], guid.Data4[2],
                guid.Data4[3], guid.Data4[4], guid.Data4[5], guid.Data4[6], guid.Data4[7]);
#else
            uuid_t uuid;
            uuid_generate(uuid);
            uuid_unparse(uuid, str);
#endif // _WINDOWS
        }

        std::string getMessageId() {
            char str[36];
            generateUUID(str);

            std::string msgId(str);
            return msgId;
        }

        std::string getFileMd5(const std::string &filePath)
        {
            using namespace std;
            ifstream in(filePath.c_str(), ios::binary);
            if (!in)
                return "";

            MD5 md5;
            streamsize length;
            char buffer[1024];
            while (!in.eof()) {
                in.read(buffer, 1024);
                length = in.gcount();
                if (length > 0)
                    md5.update(buffer, length);
            }
            in.close();

            return md5.toString();
        }

        std::string getFileSuffix(const std::string &url)
        {
            std::string suffix;
            unsigned long t = url.find_last_of('.');
            if (t != -1) {
                suffix = url.substr(t + 1);

                t = suffix.find_first_of('?');
                if (t != -1) {
                    suffix = suffix.substr(0, t);
                }

                t = suffix.find_first_of('&');
                if (t != -1) {
                    suffix = suffix.substr(0, t);
                }
            }
            return suffix;
        }

        std::string getImageSuffix(const std::string &filePath)
        {
            return "";
        }
    }
}
