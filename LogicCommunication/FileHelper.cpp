#include "FileHelper.h"
#include <iostream>
#include <sstream>
#include <curl/curl.h>
#include <functional>
#include "../Platform/NavigationManager.h"
#include "../Platform/Platform.h"
#include "../QtUtil/lib/cjson/cJSON_inc.h"
#include "Communication.h"
#include "../QtUtil/Utils/Log.h"
#include "../include/EncodeHelper.h"

#ifdef _WINDOWS
#include <direct.h>
#include <io.h>
#endif
#if (defined _MACOS) || (defined _LINUX)

#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

#define _access(x, y) access(x,y)
#define _mkdir(x) mkdir(x,S_IRUSR | S_IWUSR | S_IXUSR | S_IRWXG | S_IRWXO)

#endif

#ifdef _LINUX
#include <string.h>
#endif


#define DEM_TEMPFILE_PATH Platform::instance().getAppdataRoamingUserPath() + "/image/temp/"
#define DEM_HEADPAOTO_PATH Platform::instance().getAppdataRoamingUserPath() + "/image/headphoto/"


Communication *_pComm = nullptr;
using namespace std;

FileHelper::FileHelper(Communication *pComm) {
    _pComm = pComm;
}


FileHelper::~FileHelper() {

}

/**
  * @函数名   
  * @功能描述 
  * @参数
  * @author   cc
  * @date     2018/09/27
  */
std::string FileHelper::getNetImgFilePath(const std::string &filePath) {

    string fileKey = getFileDataMD5(filePath);
    QInt64 fileSize = getFileSize(filePath);
    string fileSuffix = getFileSuffix(filePath);

    string netPath = checkImgFileKey(fileKey, fileSize, fileSuffix);
    if (netPath.empty()) {
        return uploadImg(filePath, fileKey, fileSize, fileSuffix);
    }

    return netPath;
}

/**
  * @函数名   
  * @功能描述 
  * @参数
  * @author   cc
  * @date     2018/09/27
  */
std::string FileHelper::getLocalImgFilePath(const std::string &filePath, const std::string &dirPostfix, bool thumb) {
    //
    std::string localPath =
            Platform::instance().getAppdataRoamingUserPath() + dirPostfix + QTalk::GetFileNameByUrl(filePath);

    // 判断文件存在即直接返回路径
    if (_access(localPath.c_str(), 0) != -1) {
        return localPath;
    }

    // 简单判断url
    std::string netPath(filePath);
    std::string tmp = netPath.substr(0, 5);
    if (netPath.substr(0, 5) == "file/" || netPath.substr(0, 5) == "/file") {
        netPath = NavigationManager::instance().getFileHttpHost() + "/" + netPath;
    }
    // 加压缩
    if(thumb)
    {
        if(netPath.find('?') != -1)
            netPath += "&platform=pc&imgtype=thumb";
        else
            netPath += "?platform=pc&imgtype=thumb";
    }

    // 支持后台webp
#ifndef _LINUX
    {
        if(netPath.find('?') != -1)
            netPath += "&webp=true";
        else
            netPath += "?webp=true";
    }
#endif

    // 请求
    bool ret = false;
    std::string imgData;
    auto callBack = [this, netPath, localPath, &ret, &imgData](int code, std::string responeseData) {
        if (code == 200) {
            imgData = responeseData;
            ret = true;
        } else {
            warn_log("请求失败  url: {0}", netPath);
        }
    };

    if (_pComm) {
        //
        QTalk::HttpRequest req(netPath);
        _pComm->addHttpRequest(req, callBack);

        if (ret) {
            //std::string realSuffix = getFileSuffix(&imgData);
            //if(getFileSuffix(localPath) != realSuffix)
            //{
            //    localPath += "." + realSuffix;
            //}
            writeFile(localPath, &imgData);
            return localPath;
        }
    }
    return std::string();
}

/**
  * @函数名   getEmotionPath
  * @功能描述 
  * @参数
  * @author   cc
  * @date     2018/10/19
  */
std::string FileHelper::getEmotionPath(const std::string &pid, const std::string &sid, const std::string &fileName) {

    std::string localPath = Platform::instance().getTempEmoticonPath(pid);

    std::ostringstream url;
    url << NavigationManager::instance().getFileHttpHost()
        << "/file/v2/emo/d/e/"
        << pid
        << "/" << sid
        << "/org"
        << "?u=" << Platform::instance().getSelfUserId() + "@" + NavigationManager::instance().getDomain()
        << "&k=" << Platform::instance().getServerAuthKey();

    std::string strUrl = url.str();

    bool ret = false;
    std::string data = "";
    auto callback = [this, strUrl, fileName, &ret, &data, &localPath](int code, const std::string &responseData) {
        if (code == 200) {
            data = responseData;
            localPath += "/" + fileName + "." + getFileSuffix(&data);
            ret = true;
        } else {
            warn_log("请求失败  url:{0}", strUrl);
        }
    };

    if (_pComm) {
        QTalk::HttpRequest req(strUrl);
        _pComm->
                addHttpRequest(req, callback
        );
        if (ret) {
            writeFile(localPath, &data
            );
            return
                    localPath;
        }
    }
    return

            std::string();
}

/**
  * @函数名   getFileSize
  * @功能描述 获取文件大小
  * @参数
  * @author   cc
  * @date     2018/09/27
  */
QInt64 FileHelper::getFileSize(const std::string &filePath) {
    QInt64 len = 0;
    ifstream in(filePath);
    streampos pos = in.tellg();
    in.seekg(0, ios::end);
    len = in.tellg();
    in.seekg(pos);
    in.close();

    return len;
}

/**
  * @函数名   getFileSuffix
  * @功能描述 获取文件后缀 不包含 .
  * @参数
     string 文件后缀
  * @author   cc
  * @date     2018/09/27
  */
std::string FileHelper::getFileSuffix(const std::string &fileName) {
    unsigned long t = fileName.find_last_of('.');
    if (t != -1) {
        return fileName.substr(t + 1);
    }
    return std::string();
}

/**
  * @函数名   getFileSuffix
  * @功能描述 获取文件后缀
  * @参数
     string 文件后缀
  * @author   cc
  * @date     2018/10/19
  */
std::string FileHelper::getFileSuffix(const std::string *fileData) {
    const char *data = fileData->c_str();
    if (fileData->size() > 8) {
        if ((unsigned char) data[0] == 0xff && (unsigned char) data[1] == 0xd8) {
            return "JPEG";
        } else if (data[0] == 'G' && data[1] == 'I' && data[2] == 'F' && data[3] == '8'
                   && data[5] == 'a') {
            if (data[4] == '9' || data[4] == '7') {
                return "GIF";
            }
        } else if ((unsigned char) data[0] == 0x42 && (unsigned char) data[1] == 0x4D) {
            return "BMP";
        } else if ((unsigned char) data[1] == 0x50 && (unsigned char) data[2] == 0x4e &&
                   (unsigned char) data[3] == 0x47) {
            return "PNG";
        }
    }
    return std::string();
}

/**
  * @函数名   
  * @功能描述 
  * @参数
  * @author   cc
  * @date     2018/09/30
  */
void FileHelper::batchDownloadHead(const std::vector<std::string> &urls) {

    info_log("批量从服务器请求用户头像 开始 {0}", urls.size());

    if (urls.empty())
        return;

    std::vector<std::string> workerUrls;

    std::map<std::string, std::string> mapHeadData;

    for (const auto &url : urls) {
        if (!url.empty()) {
            std::string netPath = url;
            std::string localPath = DEM_HEADPAOTO_PATH + QTalk::GetFileNameByUrl(netPath);
            //
            if (_access(localPath.c_str(), 0) != -1)
                continue;

            if (netPath.substr(0, 5) == "file/" || netPath.substr(0, 5) == "/file") {
                netPath = NavigationManager::instance().getFileHttpHost() + "/" + netPath;
            }
            if (netPath.find('?') != -1) {
                netPath += "&w=100&h=100";
            } else {
                netPath += "?w=100&h=100";
            }

            auto callback = [this, netPath, localPath, &mapHeadData](int code, const std::string &responseData) {
                if (code == 200) {
                    mapHeadData[localPath] = responseData;
                } else {
                    warn_log("{0} 请求失败", netPath);
                }
            };

            if (_pComm) {
                QTalk::HttpRequest req(netPath);
                workerUrls.push_back(netPath);
                _pComm->addHttpRequest(req, callback);
            }

        }
    }

    // 写文件
    for (auto &itr : mapHeadData) {
        writeFile(itr.first, &itr.second);
    }

    info_log("批量从服务器请求用户头像 结束 {0}", workerUrls.size());
}

/**
  * @函数名   getFileInfo
  * @功能描述 获取文件信息
  * @参数
  * @author   cc
  * @date     2018/10/15
  */
void FileHelper::getNetFileInfo(const std::string &filePath,
                                const std::function<void(const std::string &, const std::string &)>& callbackFun) {

    auto func = [filePath, callbackFun, this]() {
#ifdef _MACOS
        pthread_setname_np("FileHelper::getNetFileInfo");
#endif

#ifdef _WINDOWS
		std::string nfilePath = QTalk::Utf8ToGbk(filePath.data());
#else
		std::string nfilePath(filePath);
#endif // _WINDOWS

        std::string strUrl, fileMd5;
        fileMd5 = getFileDataMD5(nfilePath);
        QInt64 fileSize = getFileSize(nfilePath);
        string fileSuffix = getFileSuffix(nfilePath);

        strUrl = checkFileKey(fileMd5, fileSize, fileSuffix);
        if (strUrl.empty()) {
            strUrl = uploadFile(nfilePath, fileMd5, fileSize, fileSuffix);
        } else {
            if (_pComm && _pComm->_pMsgManager) {
                _pComm->_pMsgManager->updateFileProcess(filePath, 0, 0, fileSize, fileSize, 0, 0);
            }
        }
        callbackFun(strUrl, fileMd5);
    };


    if (Platform::instance().isMainThread()) {

        std::thread t(func);
        t.detach();
        return;
    }

    func();
}

/**
 *
 */
void FileHelper::uploadLogFile(const std::string &filePath,
                               std::function<void(const std::string &, const std::string &)> callbackFun) {

    auto func = [filePath, callbackFun, this]() {
#ifdef _MACOS
        pthread_setname_np("FileHelper::uploadLogFile");
#endif
        std::string strNetUrl, key;
        key = getFileDataMD5(filePath);
        QInt64 size = getFileSize(filePath);
        string suffix = getFileSuffix(filePath);

        {
            string fileName = key + "." + suffix;

            std::ostringstream url;
            url << NavigationManager::instance().getFileHttpHost()
                << "/file/v2/upload/file"
                << "?name=" << fileName
                << "&p=" << Platform::instance().getPlatformStr()
                << "&u="
                << Platform::instance().getSelfUserId() + "@" + NavigationManager::instance().getDomain()
                << "&k=" << Platform::instance().getServerAuthKey()
                << "&v=" << Platform::instance().getClientVersion()
                << "&key=" << key
                << "&size=" << size;

            std::string strUrl = url.str();
            auto callback = [strUrl, &strNetUrl](int code, const std::string &responseData) {

                if (code == 200) {
                    cJSON *data = cJSON_Parse(responseData.c_str());

                    if (data == nullptr) {
                        error_log("json paring error");
                        return;
                    }

                    if (cJSON_GetObjectItem(data, "ret")->valueint) {
                        char *netUrl = cJSON_GetObjectItem(data, "data")->valuestring;
                        if (netUrl && strlen(netUrl) > 0) {
                            strNetUrl = netUrl;
                        }
                    }
                    cJSON_Delete(data);
                }
            };

            if (_pComm) {
                QTalk::HttpRequest req(strUrl);
                req.formFile = filePath;
                req.addProcessCallback = true;
                req.processCallbackKey = filePath;
                _pComm->addHttpRequest(req, callback);
            }
        }

        callbackFun(strNetUrl, key);
    };

    if (Platform::instance().isMainThread()) {

        std::thread t(func);
        t.detach();
        return;
    }

    func();
}

/**
  * @函数名   
  * @功能描述 
  * @参数
  * @author   cc
  * @date     2018/10/16
  */
void FileHelper::downloadFile(const std::string &uri, const std::string &localPath, bool addCallBack, const std::string& processKey) {

    auto func = [this, uri, localPath, addCallBack, processKey]() {
#ifdef _MACOS
        pthread_setname_np("FileHelper::downloadFile");
#endif
        std::string netPath = uri;
        if (netPath.substr(0, 5) == "file/" || netPath.substr(0, 5) == "/file") {
            netPath = NavigationManager::instance().getFileHttpHost() + "/" + netPath;
        }

        std::string data = "";
        auto callback = [netPath, &data](int code, const std::string &responseData) {
            if (code == 200) {
                data = responseData;
            } else {
                info_log("请求失败  url: {0}", netPath);
            }
        };

        if (_pComm) {
            QTalk::HttpRequest req(netPath);
            req.addProcessCallback = addCallBack;
            req.processCallbackKey = processKey;
            _pComm->addHttpRequest(req, callback);
            if (!data.empty()) {
                writeFile(localPath, &data);
                //_pComm->_pMsgManager->downloadFileComplete(uri, localPath, true);
            } else {
                //_pComm->_pMsgManager->downloadFileComplete(uri, localPath, false);
            }
        }
    };

//    if (Platform::instance().isMainThread()) {
        std::thread t(func);
        t.detach();
//        return;
//    }

//    func();
}

///**
//  * @函数名   
//  * @功能描述 
//  * @参数
//     void
//  * @author   cc
//  * @date     2018/10/16
//  */
//void FileHelper::checkAndDowloadFile(const std::string &uri, const std::string &md5) {
//    const std::string localFilePath = QTalk::GetFilePathByUrl(uri);
//    if (_access(localFilePath.c_str(), 0) != -1)
//        return;
//
//    downloadFile(uri, localFilePath, true);
//}

/**
  * @函数名   
  * @功能描述 
  * @参数
  * @author   cc
  * @date     2018/10/22
  */
std::string FileHelper::downloadEmoticonIcon(const std::string &uri, const std::string &pkgId) {

    //
    std::string localPath = Platform::instance().getEmoticonIconPath();
    //
    std::string data = "";
    bool ret = false;
    auto callback = [this, pkgId, uri, &ret, &data, &localPath](int code, const std::string &responseData) {
        if (code == 200) {
            data = responseData;
            localPath += "/" + pkgId + "." + getFileSuffix(&data);
            ret = true;
        } else {
            info_log("请求失败  url: {0}", uri);
        }
    };

    if (_pComm) {
        QTalk::HttpRequest req(uri);
        _pComm->addHttpRequest(req, callback);
        if (ret) {
            if (_access(localPath.c_str(), 0) != -1 || getFileDataMD5(&data) != getFileDataMD5(localPath)) {
                writeFile(localPath, &data);
            }
            return localPath;
        }
    }

    return std::string();
}

/**
  * @函数名   writeFile
  * @功能描述 写入本地文件
  * @参数
  * @author   cc
  * @date     2018/09/28
  */
bool FileHelper::writeFile(const std::string &filePath, const std::string *data) {

#ifdef _WINDOWS
	std::string nfilePath = QTalk::Utf8ToGbk(filePath.data());
#else
	std::string nfilePath(filePath);
#endif // _WINDOWS

    // 创建文件夹
    creatDir(nfilePath);
    //
    ofstream out;
    out.open(nfilePath.c_str(), std::ios::out | std::ios::binary);
    if (out.is_open()) {
        out.write(data->c_str(), data->size());
        out.close();
        if (_pComm && _pComm->_pMsgManager) {
            _pComm->_pMsgManager->sendFileWritedMessage(nfilePath);
        }
        return true;
    }

    return false;
}

/**
  * @函数名   creatDir
  * @功能描述 创建文件夹路径(待优化)
  * @参数
  * @author   cc
  * @date     2018/09/28
  */
void FileHelper::creatDir(const std::string &filePath) {
    if (_access(filePath.c_str(), 0) == -1) {
        int pos = filePath.find("/");
        std::string tmpPath = filePath.substr(0, pos);
        std::string tmpLast = filePath.substr(pos + 1);

        while (1) {
            if (_access(tmpPath.c_str(), 0) == -1) {
                _mkdir(tmpPath.c_str());
            }

            if (tmpLast.find("/") == -1)
                break;

            pos = tmpPath.size() + tmpLast.find("/") + 1;
            tmpPath = filePath.substr(0, pos);
            tmpLast = filePath.substr(pos + 1);
        }
    }
}


/**
  * @函数名   getFileDataMD5
  * @功能描述 MD5加密文件
  * @参数     filePath 文件路径
     md5值
  * @author   cc
  * @date     2018/09/26
  */
string FileHelper::getFileDataMD5(const string &filePath) {

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

/**
  * @函数名   getFileDataMD5
  * @功能描述 MD5加密文件
  * @参数     fileData 文件二进制data
     md5值
  * @author   cc
  * @date     2018/09/26
  */
std::string FileHelper::getFileDataMD5(const std::string *fileData) {
    MD5 md5;
    md5.update(fileData, strlen(fileData->c_str()));
    return md5.toString();
}

/**
  * @函数名   checkFileKey
  * @功能描述 从服务器获取对应key的url
  * @参数
     string
  * @author   cc
  * @date     2018/09/26
  */
string FileHelper::checkImgFileKey(const std::string &key, QInt64 fileSize, const std::string &suffix) {
    //
    std::ostringstream url;
    url << NavigationManager::instance().getFileHttpHost()
        << "/file/v2/inspection/img"
        << "?key=" << key
        << "&size=" << fileSize
        << "&name=" << key << "." << suffix
        << "&platform=" << Platform::instance().getPlatformStr()
        << "&u=" << Platform::instance().getSelfUserId() + "@" + NavigationManager::instance().getDomain()
        << "&k=" << Platform::instance().getServerAuthKey()
        << "&version=" << Platform::instance().getClientVersion();

    string strNetUrl;
    std::string strUrl = url.str();
    auto callback = [strUrl, &strNetUrl](int code, const std::string &responseData) {
        if (code == 200) {
            cJSON *data = cJSON_Parse(responseData.c_str());
            if (data == nullptr) {
                error_log("json paring error");
                return;
            }

            if (!QTalk::JSON::cJSON_SafeGetBoolValue(data, "ret", false)) {
                const char *netUrl = QTalk::JSON::cJSON_SafeGetStringValue(data, "data", "");
                if (netUrl && strlen(netUrl) > 0) {
                    strNetUrl = netUrl;
                }
            }
            cJSON_Delete(data);
        } else {
            info_log("请求失败  url: {0}", strUrl);
        }
    };

    if (_pComm) {
        QTalk::HttpRequest req(strUrl);
        _pComm->addHttpRequest(req, callback);
    }

    return strNetUrl;
}

/**
  * @函数名   
  * @功能描述 
  * @参数
  * @author   cc
  * @date     2018/10/15
  */
std::string FileHelper::checkFileKey(const std::string &key, QInt64 fileSize, const std::string &suffix) {
    std::ostringstream url;
    url << NavigationManager::instance().getFileHttpHost()
        << "/file/v2/inspection/file"
        << "?key=" << key
        << "&size=" << fileSize
        << "&name=" << key << "." << suffix
        << "&platform=" << Platform::instance().getPlatformStr()
        << "&u=" << Platform::instance().getSelfUserId() + "@" + NavigationManager::instance().getDomain()
        << "&k=" << Platform::instance().getServerAuthKey()
        << "&version=" << Platform::instance().getClientVersion();

    std::string strUrl = url.str();

    string strNetUrl = "";
    auto callback = [strUrl, &strNetUrl](int code, const std::string &responseData) {

        if (code == 200) {
            cJSON *data = cJSON_Parse(responseData.c_str());

            if (data == nullptr) {
                error_log("json paring error");
                return;
            }

            if (!cJSON_GetObjectItem(data, "ret")->valueint) {
                char *netUrl = cJSON_GetObjectItem(data, "data")->valuestring;
                if (netUrl && strlen(netUrl) > 0) {
                    strNetUrl = netUrl;
                    string fileName = QTalk::GetFileNameByUrl(strNetUrl);
                    strNetUrl += "&file=file/" + fileName;
                    strNetUrl += "&filename=file/" + fileName;
                    cJSON_Delete(data);
                    return;
                }
            }
            cJSON_Delete(data);
        } else {
            info_log("请求失败  url: {0}", strUrl);
        }
    };


    if (_pComm) {
        QTalk::HttpRequest req(strUrl);
        _pComm->addHttpRequest(req, callback);
    }

    return strNetUrl;
}

/**
  * @函数名   uploadImg
  * @功能描述 上传图片
  * @参数
     std::string 文件网络位置
  * @author   cc
  * @date     2018/09/27
  */
std::string
FileHelper::uploadImg(const std::string &filePath, const std::string &key, QInt64 size, const std::string &suffix) {
    string fileName = key + "." + suffix;

    std::ostringstream url;
    url << NavigationManager::instance().getFileHttpHost()
        << "/file/v2/upload/img"
        << "?name=" << fileName
        << "&p=" << Platform::instance().getPlatformStr()
        << "&u=" << Platform::instance().getSelfUserId() + "@" + NavigationManager::instance().getDomain()
        << "&k=" << Platform::instance().getServerAuthKey()
        << "&v=" << Platform::instance().getClientVersion()
        << "&key=" << key
        << "&size=" << size;

    std::string strUrl = url.str();
    string strNetUrl;
    auto callback = [this, strUrl, &strNetUrl](int code, const std::string &responseData) {
        info_log("请求结果: data: {0}", responseData);
        if (code == 200) {
            cJSON *data = cJSON_Parse(responseData.c_str());

            if (data == nullptr) {
                error_log("json paring error");
                return;
            }

            if (cJSON_GetObjectItem(data, "ret")->valueint) {
                char *netUrl = cJSON_GetObjectItem(data, "data")->valuestring;
                if (netUrl && strlen(netUrl) > 0) {
                    strNetUrl = netUrl;
                    return;
                }
            }
            cJSON_Delete(data);
        } else {
            info_log("请求失败  url: {0}", strUrl);
        }
    };


    if (_pComm) {
        QTalk::HttpRequest req(strUrl);
        req.formFile = filePath;
        _pComm->addHttpRequest(req, callback);
    }

    return strNetUrl;
}

/**
  * @函数名   uploadFile
  * @功能描述 上传文件
  * @参数
  * @author   cc
  * @date     2018/10/16
  */
std::string
FileHelper::uploadFile(const std::string &filePath, const std::string &key,
        QInt64 size, const std::string &suffix, bool withProcess, const std::string& processKey) {
    string fileName = key + "." + suffix;

    std::ostringstream url;
    url << NavigationManager::instance().getFileHttpHost()
        << "/file/v2/upload/file"
        << "?name=" << fileName
        << "&p=" << Platform::instance().getPlatformStr()
        << "&u=" << Platform::instance().getSelfXmppId()
        << "&k=" << Platform::instance().getServerAuthKey()
        << "&v=" << Platform::instance().getClientVersion()
        << "&key=" << key
        << "&size=" << size;

    std::string strUrl = url.str();
    std::string strNetUrl;
    auto callback = [strUrl, &strNetUrl](int code, const std::string &responseData) {
        info_log("请求结果: data: {0}", responseData);
        if (code == 200) {
            cJSON *data = cJSON_Parse(responseData.c_str());

            if (data == nullptr) {
                error_log("json paring error");
                return;
            }

            if (cJSON_GetObjectItem(data, "ret")->valueint) {
                char *netUrl = cJSON_GetObjectItem(data, "data")->valuestring;
                if (netUrl && strlen(netUrl) > 0) {
                    strNetUrl = netUrl;
                }
            }
            cJSON_Delete(data);
        } else {
            error_log("请求失败  url:{0}", strUrl);
        }

    };

    if (_pComm) {
        QTalk::HttpRequest req(strUrl);
        req.formFile = filePath;
        req.addProcessCallback = withProcess;
        req.processCallbackKey = processKey;
        _pComm->addHttpRequest(req, callback);
    }

    return strNetUrl;
}

//
bool FileHelper::DownloadPubKey() {
    std::string localPubKeyPath = Platform::instance().getAppdataRoamingUserPath()
                                  + "/" + NavigationManager::instance().getPubkey();

    if (_access(localPubKeyPath.c_str(), 0) == -1) {
        std::ostringstream url;
        url << NavigationManager::instance().getJavaHost()
            << "/qtapi/nck/rsa/get_public_key.do";

        std::string strUrl = url.str();
        std::string pubKey;
        int rsaEncodeType = NavigationManager::instance().getRsaEncodeType();
        auto callback = [this, strUrl, localPubKeyPath, rsaEncodeType, &pubKey](int code,
                                                                                const std::string &responseData) {
            if (code == 200) {
                cJSON *data = cJSON_Parse(responseData.c_str());

                if (data == nullptr) {
                    error_log("json paring error");
                    return;
                }

                if (cJSON_GetObjectItem(data, "ret")->valueint) {

                    cJSON *keys = cJSON_GetObjectItem(data, "data");
                    if (rsaEncodeType)
                        pubKey = std::string(cJSON_GetObjectItem(keys, "pub_key_fullkey")->valuestring);
                    else
                        pubKey = std::string(cJSON_GetObjectItem(keys, "rsa_pub_key_fullkey")->valuestring);

                }

                cJSON_Delete(data);
            } else {
                if (_pComm && _pComm->_pMsgManager) {
                    _pComm->_pMsgManager->sendLoginErrMessage("公钥文件下载失败");
                }
            }
        };

        if (_pComm) {
            QTalk::HttpRequest req(strUrl);
            _pComm->addHttpRequest(req, callback);
            if (!pubKey.empty()) {
                writeFile(localPubKeyPath, &pubKey);
            }
            else return false;
        }

    }
    return true;
}

/**
 *
 * @param localPath
 * @return
 */
bool FileHelper::fileExist(const std::string &localPath) {
    return _access(localPath.c_str(), 0) != -1;
}

/**
 *
 * @param fileName
 * @return
 */
std::string FileHelper::getFileBaseName(const std::string &fileName) {
    auto t = fileName.find_last_of('.');
    if (t != -1) {
        return fileName.substr(0, t);
    }
    return std::string();
}

std::string FileHelper::getFileNameByLink(const std::string &link) {
    auto t = link.find_last_of('/');
    if (t != -1) {
        return link.substr(t);
    }
    return std::string();
}

bool FileHelper::writeQvtToFile(const std::string &qvt) {
    std::string localQvtPath = Platform::instance().getAppdataRoamingPath()
                                  + "/qvt";
    writeFile(localQvtPath,&qvt);
	return 1;
}

std::string FileHelper::getQvtFromFile() {
    std::string localQvtPath = Platform::instance().getAppdataRoamingPath()
                               + "/qvt";
    std::ifstream in(localQvtPath);
    std::ostringstream tmp;
    tmp << in.rdbuf();
    std::string str = tmp.str();
    in.close();
    return str;
}

/**
 * 删除配置文件
 */
void FileHelper::clearConfig() {

}

std::string FileHelper::uploadFile(const std::string& path, bool withProcess, const std::string& processKey)
{
    std::string strUrl, fileMd5;
    fileMd5 = getFileDataMD5(path);
    QInt64 fileSize = getFileSize(path);
    string fileSuffix = getFileSuffix(path);

    strUrl = checkFileKey(fileMd5, fileSize, fileSuffix);
    if (strUrl.empty()) {
        strUrl = uploadFile(path, fileMd5, fileSize, fileSuffix, withProcess, processKey);
    }
	else
	{
		if (_pComm && _pComm->_pMsgManager) {
			_pComm->_pMsgManager->updateFileProcess(processKey, 0, 0, fileSize, fileSize, 0, 0);
		}
	}

    return strUrl;
}