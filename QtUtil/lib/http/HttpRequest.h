//
// Created by may on 2019-01-31.
//

#ifndef QTALK_V2_HTTPREQUEST_H
#define QTALK_V2_HTTPREQUEST_H

#include <string>
#include <functional>
#include <sstream>

namespace QTalk {

    enum RequestMethod {
        UNSET, POST, GET
    };

    struct StProcessParam
    {
        double dt = 0;
        double dn = 0;
        double ut = 0;
        double un = 0;
        double speed = 0;
        double leftTime = 0;

        std::string key;
    };

    namespace http {

        class HttpRequest {
        public:
            explicit HttpRequest(const char *url) { _originUrl = url; };

            explicit HttpRequest(std::string &url) { _originUrl = url; };

            ~HttpRequest() = default;

            virtual void setRequestMethod(RequestMethod method) = 0;

            virtual void addRequestHeader(const char *key, const char *value) = 0;

            virtual void addRequestHeader(std::string &key, std::string &value) = 0;

            virtual void appendPostData(const char *body, int len) = 0;

            virtual bool startSynchronous() = 0;

            virtual int getResponseCode() = 0;

            virtual std::string *getResponseData() = 0;

            virtual void addFromFile(const std::string &filePath) = 0;

            virtual void
            setProcessCallback(const std::string& key,
                    std::function<void(StProcessParam)> fun) = 0;

        private:
            std::string _originUrl;
        };
    };
};

#endif //QTALK_V2_HTTPREQUEST_H
