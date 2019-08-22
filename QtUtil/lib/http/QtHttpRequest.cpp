#include <utility>

//
// Created by may on 2018/7/20.
//

#include "QtHttpRequest.h"

#include "../../../QtUtil/Utils/Log.h"
#include "../../../QtUtil/Utils/utils.h"
#include <curl/curl.h>
#include <iostream>
#include <sstream>
#include <time.h>

using namespace QTalk;

std::function<void(StProcessParam)> callfun;

struct StProcessData {
    CURL *curl = nullptr;
    std::string key;

    StProcessData()= default;
    StProcessData(CURL *u, std::string k) : curl(u), key(std::move(k)) {}
};

int processCallback(void *clientp, double dltotal, double dlnow, double ultotal, double ulnow) {
    if (dltotal == dlnow == ultotal == ulnow && dltotal == 0) return 0;

    double speed = 0, leftTime = 0;
    StProcessData* data = (StProcessData*)clientp;
    CURL *curl = data->curl;
    if (curl) {
        curl_easy_getinfo(curl, CURLINFO_SPEED_DOWNLOAD, &speed);
        // 下载速度
        if (speed != 0) {
            leftTime = (dltotal - dlnow) / speed;
        } else {
            curl_easy_getinfo(curl, CURLINFO_SPEED_UPLOAD, &speed);
            // 上传速度
            if (speed != 0) {
                leftTime = (ultotal - ulnow) / speed;
            }
        }
    }
    callfun({dltotal, dlnow, ultotal, ulnow, speed, leftTime, data->key});
    return 0;
}

size_t WriteCallback(char *contents, size_t size, size_t nmemb, void *userp) {
    std::string tempo(contents, size * nmemb);
    auto *pStream = static_cast<std::ostringstream *>(userp);
    *(pStream) << tempo;
    return size * nmemb;
}

#ifndef _WINDOWS

static pthread_mutex_t global_curl_lock[18];

static void lock_cb(CURL *handle, curl_lock_data data,
                    curl_lock_access access, void *userptr) {
    pthread_mutex_lock(&global_curl_lock[data]); /* uses a global lock array */
}


static void unlock_cb(CURL *handle, curl_lock_data data,
                      void *userptr) {
    pthread_mutex_unlock(&global_curl_lock[data]); /* uses a global lock array */
}

#endif // !_WINDOWS


static CURL *innerInit(const char *url, std::ostringstream *readBuffer) {

    CURL *curl = curl_easy_init();

//#ifndef _WINDOWS
//    CURLSH *share = curl_share_init();
//    curl_share_setopt(share, CURLSHOPT_LOCKFUNC, lock_cb);
//    curl_share_setopt(share, CURLSHOPT_SHARE, CURL_LOCK_DATA_DNS);
//    curl_share_setopt(share, CURLSHOPT_UNLOCKFUNC, unlock_cb);
//    curl_easy_setopt(curl, CURLOPT_SHARE, share);
//#endif // !_WINDOWS

    /* only reuse addresses for a very short time */
    curl_easy_setopt(curl, CURLOPT_DNS_CACHE_TIMEOUT, 2L);
    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 10L);
    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1L);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, readBuffer);
//    curl_easy_setopt(curl, CURLOPT_DNS_USE_GLOBAL_CACHE, 1);
    // 关闭CA认证
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
    curl_easy_setopt(curl, CURLOPT_FORBID_REUSE, 0L);
	// 301跳转
	curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

    return curl;
}

QtHttpRequest::QtHttpRequest(const char *url) : HttpRequest(url) {
    _originUrl = url;
    _httpCore = innerInit(url, &_readBuffer);
    _headers = nullptr;
}

QtHttpRequest::QtHttpRequest(std::string &url) : HttpRequest(url) {
    _originUrl = url;
    _httpCore = innerInit(url.c_str(), &_readBuffer);
    _headers = nullptr;
}

void QtHttpRequest::setRequestMethod(RequestMethod method) {
    CURL *curl = _httpCore;
    if (curl != nullptr) {
        switch (method) {
            case QTalk::RequestMethod::POST:
                curl_easy_setopt(curl, CURLOPT_POST, 1);
                break;
            case QTalk::RequestMethod::GET:
                break;
            default:
                break;
        }
    }
}

void QtHttpRequest::addRequestHeader(const char *key, const char *value) {

    CURL *curl = _httpCore;
    if (curl != nullptr) {

        std::ostringstream ss;
        ss << key;
        ss << ": ";
        ss << value;

        std::string copyOfStr = ss.str();
        _headers = curl_slist_append(_headers, copyOfStr.c_str());
    }
}

void QtHttpRequest::appendPostData(const char *body, int len) {
    CURL *curl = _httpCore;
    if (curl != nullptr) {
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body);
    }
}

bool QtHttpRequest::startSynchronous() {
    if (nullptr != _headers) {
        curl_easy_setopt(_httpCore, CURLOPT_HTTPHEADER, _headers);
    }

    unsigned long now = static_cast<unsigned long>(std::chrono::system_clock::now().time_since_epoch() /
                                                   std::chrono::milliseconds(1));

    auto start = std::chrono::system_clock::now();

    std::string log;

    int res = curl_easy_perform(_httpCore);
    buffer = _readBuffer.str();

    auto end = std::chrono::system_clock::now();

    std::chrono::duration<double> elapsed_seconds = end - start;
    time_t start_time = std::chrono::system_clock::to_time_t(start);
    time_t end_time = std::chrono::system_clock::to_time_t(end);
    struct tm *local_start = gmtime(&start_time);
    struct tm *local_end = gmtime(&end_time);

    if (res != CURLE_OK) {
        error_log(
                "CURL: perform a url ({0}) and failed, coast ({1})s, begin at: {2} end at: {3}\ncode is {4}({5}) end at:{6}",
                _originUrl.c_str(),
                elapsed_seconds.count(),
                asctime(local_start),
                asctime(local_end),
                curl_easy_strerror((CURLcode) res),
                res,
                now);
    } else {

        info_log("CURL: perform a url request , coast ({0})s, begin at: {1} end at: {2}",
                 elapsed_seconds.count(),
                 asctime(local_start),
                 asctime(local_end));
    }

    return res == CURLE_OK;
}

int QtHttpRequest::getResponseCode() {
    long http_code = 0;
    CURL *curl = _httpCore;
    int error = curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);

    if (error != CURLE_OK) {
        error_log("CURL: getResponse code get a error:{0}({1}) \nand url is {2}",
                  curl_easy_strerror((CURLcode) error),
                  error,
                  _originUrl.c_str());
        return -1;
    }


    return static_cast<int>(http_code);
}

std::string *QtHttpRequest::getResponseData() {
    return &buffer;
}

QtHttpRequest::~QtHttpRequest() {
    CURL *curl = _httpCore;
    if (curl != nullptr) {
        curl_slist_free_all(_headers);
        curl_easy_cleanup(curl);
        _readBuffer.clear();
    }
}

void QtHttpRequest::addRequestHeader(std::string &key, std::string &value) {
    CURL *curl = _httpCore;
    if (curl != nullptr) {

        std::ostringstream ss;
        ss << key;
        ss << ": ";
        ss << value;

        std::string copyOfStr = ss.str();
        _headers = curl_slist_append(_headers, copyOfStr.c_str());
    }
}

void QTalk::QtHttpRequest::addFromFile(const std::string &filePath) {
    CURL *curl = _httpCore;
    if (curl != nullptr) {
        struct curl_httppost *post = nullptr;
        struct curl_httppost *last = nullptr;

        curl_formadd(&post, &last, CURLFORM_COPYNAME, "file", CURLFORM_FILE, filePath.c_str(), CURLFORM_END);
        curl_easy_setopt(curl, CURLOPT_HEADER, 0);
        curl_easy_setopt(curl, CURLOPT_HTTPPOST, post);
    }
}

/**
  * @函数名   setProcessCallback
  * @功能描述 进度回掉
  * @参数
  * @author   cc
  * @date     2018/10/16
  */
void QTalk::QtHttpRequest::setProcessCallback(const std::string& key,
        std::function<void(StProcessParam)> fun) {

    CURL *curl = _httpCore;
    StProcessData *data = new StProcessData(curl, key);

    if (curl != nullptr) {
        callfun = std::move(fun);
        curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);
        curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, processCallback);
        curl_easy_setopt(curl, CURLOPT_PROGRESSDATA, data);
    }
}

