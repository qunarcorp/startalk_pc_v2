#ifndef _FileHelper_H_
#define _FileHelper_H_

#include "../QtUtil/lib/Md5/Md5.h"
#include "../include/CommonDefine.h"

#include <vector>
#include <mutex>
#include "../include/ThreadPool.h"

class Communication;

class FileHelper {
public:
    explicit FileHelper(Communication *pComm);

    ~FileHelper();

public:
    std::string getNetImgFilePath(const std::string &filePath);

    std::string getLocalImgFilePath(const std::string &filePath, const std::string &dirPostfix, bool thumb = false);

    std::string getEmotionPath(const std::string &pid, const std::string &sid, const std::string &fileName);

    QInt64 getFileSize(const std::string &filePath);

    std::string getFileSuffix(const std::string &fileName);

    std::string getFileSuffix(const std::string *fileData);

    std::string getFileBaseName(const std::string &fileName);

    std::string getFileNameByLink(const std::string &link);

    void batchDownloadHead(const std::vector<std::string> &urls);

    void getNetFileInfo(const std::string &filePath,
                        const std::function<void(const std::string &, const std::string &)>& callbackFun);

    std::string uploadFile(const std::string& localPath, bool withProcess, const std::string& processKey);

    void uploadLogFile(const std::string &localFile,
                       std::function<void(const std::string &, const std::string &)> callbackFun);

    void downloadFile(const std::string &uri, const std::string &localPath, bool addCallBack, const std::string& processKey = std::string());

   // void checkAndDowloadFile(const std::string &uri, const std::string &md5);

    std::string downloadEmoticonIcon(const std::string &uri, const std::string &pkgId);

    bool DownloadPubKey();

    // 判断文件或者文件夹是否存在
    bool fileExist(const std::string &localPath);

    // 创建文件夹
    void creatDir(const std::string &filePath);

    void clearConfig();

public:
    // 根据文件路径获取文件md5
    std::string getFileDataMD5(const std::string &filePath);

    // 根据文件内容获取文件md5
    std::string getFileDataMD5(const std::string *fileData);

    bool writeQvtToFile(const std::string &qvt);
    std::string getQvtFromFile();
    //
    bool writeFile(const std::string &filePath, const std::string *data);

protected:

    std::string checkImgFileKey(const std::string &key, QInt64 fileSize, const std::string &suffix);

    std::string checkFileKey(const std::string &key, QInt64 fileSize, const std::string &suffix);

    std::string uploadImg(const std::string &filePath, const std::string &key, QInt64 size, const std::string &suffix);

    std::string uploadFile(const std::string &filePath, const std::string &key,
            QInt64 size, const std::string &suffix, bool withProcess = true, const std::string& processKey = std::string());

private:
    std::mutex _mutex;
};

#endif//_FileHelper_H_
