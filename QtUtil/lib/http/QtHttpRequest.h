//
// Created by may on 2018/7/20.
//

#ifndef QTALK_QTHTTPREQUEST_H
#define QTALK_QTHTTPREQUEST_H

#include "../../qtutil_global.h"

#include "HttpRequest.h"


typedef int(*CallbackFunc)(void *, double, double, double, double);

struct curl_slist;

namespace QTalk {



    class QTALK_UTIL_EXPORT QtHttpRequest : public QTalk::http::HttpRequest {

    public:
        explicit QtHttpRequest(const char *url);

        explicit QtHttpRequest(std::string &url);

        ~QtHttpRequest();

        void setRequestMethod(RequestMethod method) override;

        void addRequestHeader(const char *key, const char *value) override;

        void addRequestHeader(std::string &key, std::string &value) override;

        void appendPostData(const char *body, int len) override;

        bool startSynchronous() override;

        int getResponseCode() override;

        std::string *getResponseData() override;

        void addFromFile(const std::string &filePath) override;

        void setProcessCallback(const std::string& key,
                std::function<void(StProcessParam)> fun) override;

    private:
        void *_httpCore;
        std::string _originUrl;
        std::ostringstream _readBuffer;
        std::string buffer;
        curl_slist *_headers;

    };
};

#endif //QTALK_QTHTTPREQUEST_H
