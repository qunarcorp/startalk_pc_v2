#ifndef NAVIGATIONMANAGER_H
#define NAVIGATIONMANAGER_H

#include <string>
#include "platform_global.h"
#include "../include/CommonDefine.h"

class PLATFORMSHARED_EXPORT NavigationManager {

private:
    static NavigationManager* _navManager;

public:
    static NavigationManager& instance();

private:
    // 导航相关
    long long navVersion;
    std::string navUrl;
    bool debug;

    // Xmpp服务地址
    std::string xmppHost;
    std::string domain;
    int         probufPort;

    // HTTP 请求
    std::string apiurl;
    std::string javaUrl;
    std::string httpurl;
    std::string pubkey;
    std::string fileurl;
    std::string opsApiHost;
    std::string qcHost;
	// imconfig
	int         rsaEncodeType;
    std::string uploadLog;

    std::string phoneAddr;
    std::string leaderUrl;
    std::string shareUrl;

    std::string foundConfigUrl;

    // 请求地址
    std::string healthcheckUrl; //网络检测地址
    std::string qcGrabOrderUrl; //抢单
    std::string qcOrderManager; //订单管理
    std::string accountConfigUrl; //账号管理
    bool readFlag;
    //
    std::string searchUrl;

    // 广告相关
    std::string adUrl;
    int adSec;

    // othet
    std::string mailSuffix;

    //qcadmin
    std::string qcadminHost;

    //ability
    bool showmsgstat;
    std::string qcGrabOrder;

public:
    NavigationManager();

public:
    void setVersion(QInt64 ver);

public: // base address

    void setXmppHost(const std::string& xmppHost);
    std::string getXmppHost();

    void setDomain(const std::string& domain);
    std::string getDomain();

    void setProbufPort(int port);
    int getProbufPort();

    void setApiHost(const std::string &url);
    std::string getApiUrl();

    void setJavaHost(const std::string& javaUrl);
    std::string getJavaHost();

    void setHttpHost(const std::string& httpurl);
    std::string getHttpHost();

    void setPubKey(const std::string& pubKey);
    std::string getPubkey();

    void setFileHttpHost(const std::string& fileHttpHost);
    std::string getFileHttpHost();

    void setPhoneNumAddr(const std::string &phone);
    std::string getPhoneNumAddr();

    void setLeaderUrl(const std::string& leaderUrl);
    std::string getLeaderUrl();

    void setShareUrl(const std::string& shareUrl);
    std::string getShareUrl();

public:// imconfig
	void setRsaEncodeType(int t);
	int getRsaEncodeType();
	void setUploadLog(const std::string& lodAddr);
	std::string getUploadLog();

	void setFoundConfigUrl(const std::string& url);
	std::string getFoundConfigUrl();

public:// ops
    void setOpsApiHost(const std::string& opsHost);
    std::string getOpsApiHost();

public://ability
    void setQCHost(const std::string& qcHost);
    std::string getQCHost();

    void setSearchUrl(const std::string& searchUrl);
    std::string getSearchUrl();

public://qcadmin
	void setQcadminHost(const std::string& adminHost);
    std::string getQcadminHost();

public://ability
	void setShowmsgstat(const bool showmsgstat);
	bool isShowmsgstat();
	void setQcGrabOrder(const std::string qcGrabOrder);
	std::string getQcGrabOrder();

public://other
    std::string getHealthCheckUrl();

    std::string getQCGrabOrderUrl();

    std::string getQCOrderManager();

    std::string getAccountConfigUrl();

    bool getReadFlag();

    std::string getADUrl();

    int getADSec();

    void setMailSuffix(const std::string& mailSuffix);
    std::string getMailSuffix();
};

#endif // NAVIGATIONMANAGER_H
