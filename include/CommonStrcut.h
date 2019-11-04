#ifndef _COMMONSTRCUT_H_
#define _COMMONSTRCUT_H_

#include <string>
#include <map>
#include <vector>
#include "CommonDefine.h"
#include <memory>

typedef std::map<std::string, std::string> HttpRequestHeader;

namespace QTalk
{
    struct StUserCard
    {
        std::string xmppId;
        std::string headerSrc;
        std::string userName;
        std::string nickName;
        std::string searchKey;
        std::string mood;
        int         gender; // 1男 2 女
        int         version;
    };

    //// 数据库返回单人或者群历史消息
    //struct StUserHistoryMessage
    //{
    //    std::string msgId;
    //    std::string xmppId;
    //    QUInt8      chatType;
    //    QInt32      msgType;
    //    std::string headSrc;
    //    std::string userName;
    //    std::string messgae;
    //    std::string from;
    //    QUInt64     time;
    //    QUInt8      state;
    //    QUInt8      readTag;
    //    QUInt8      direction;
    //    std::string extendedInfo;
    //};

    struct StUserCardF
    {
        std::string userId;
        std::string xmppId;
        std::string strName;
        std::string desc;
        std::string headSrc;

    };

    struct StGroupInfo
    {
        std::string groupId;
        std::string desc; //简介
        std::string name;
        std::string headSrc;
        std::string title; // 公告
        int         version = 0;
    };

    struct StGroupMember
    {
        std::string groupId;
        std::string memberJid;
        std::string domain;
        std::string nick;
        int affiliation;
    };

    struct StNav 
    {
        QInt64      version = 0;
		int         rsaEncodeType = 0;
        std::string xmppHost;
        std::string domain;
        int protobufPcPort;
        std::string apiurl;
        std::string javaurl;
        std::string httpurl;
        std::string pubkey;
        std::string fileurl;
        std::string mobileurl;
        std::string leaderUrl;
        std::string shareUrl;
        std::string opsHost;
        std::string qCloudHost;
        std::string searchUrl;
        std::string uploadLog;

        std::string videoUrl;

        std::string qcadminHost;

        std::string mailSuffix;
        // 发现页配置
        std::string foundConfigUrl;

        bool showmsgstat;
        std::string qcGrabOrder;

        std::string loginType = "password";
    };

    struct StOAUIData
    {
        struct StMember
        {
            int          memberId;
            std::string  memberName;
            std::string  memberIcon;
            std::string  memberAction;
        };

        int                   groupId;
        std::string           groupName;
        std::string           groupIcon;
        std::vector<StMember> members;
    };

    struct StActLog
    {
        std::string desc;
        QInt64 operatorTime;
    };

    struct StShareSession
    {
        int         chatType;
        std::string xmppId;
        std::string realJid;
        std::string name;
        std::string headUrl;
        std::string searchKey;
    };

    struct StUserMedal
    {
        int medalId = -1;
        int medalStatus= 0;

        StUserMedal() : medalId(-1), medalStatus(0) {

        }

        StUserMedal(int id, int status) : medalId(id), medalStatus(status) {

        }

        bool operator<(const StUserMedal& other) const
        {
            if(this->medalId == other.medalId)
                return false;

            if(this->medalStatus == other.medalStatus)
                return this->medalId < other.medalId;
            else
                return this->medalStatus > other.medalStatus;
        }
    };

    struct StMedalUser {
        std::string xmppId{};
        std::string userHead{};
        std::string userName{};
    };
}


namespace QTalk {

//    enum RequestMethod {
//        UNSET, POST, GET
//    };

    struct HttpRequest {
        std::string url;
        unsigned char method{};
        HttpRequestHeader header;
        std::string formFile;
        std::string body;
        // process callback
        bool addProcessCallback{};
        std::string processCallbackKey;


        explicit HttpRequest(std::string url, unsigned char method = 2)
                : url(std::move(url)), method(method), addProcessCallback(false) {
        }

        HttpRequest()= default;
    };
};

namespace QTalk
{
    namespace Search {

        enum Action {
            EM_ACTION_INVALID = 0,
            EM_ACTION_USER = 1,
            EM_ACTION_MUC = 2,
            EM_ACTION_COMMON_MUC = 4,
            EM_ACTION_HS_SINGLE = 8,
            EM_ACTION_HS_MUC = 16,
            EM_ACTION_HS_FILE = 32,
        };

        struct StUserItem
        {
            std::string xmppId;
            std::string name; // 名称
//            std::string pingying;
            std::string tips; // 签名
            std::string icon; // 头像
            std::string structure; // 架构
        };

        struct StGroupItem
        {
            int type;
            std::string xmppId;
            std::string name;
            std::string icon;
//            std::string topic;
            std::vector<std::string> _hits;
        };

        struct StHistory
        {
            int         type;
            std::string key;
            std::string name;
            std::string icon;
            int         count;       // 匹配消息数
            std::string msg_id;
            std::string body;
            QInt64      time;
            std::string from;
            std::string to;
//            std::string real_from;
//            std::string real_to;
        };

        struct StHistoryFile {
            std::string key;
            std::string source;
            std::string icon;
            std::string msg_id;
            std::string body;
            std::string extend_info;
            QInt64      time;
            std::string from;
            std::string to;

            std::string file_md5;
            std::string file_name;
            std::string file_size;
            std::string file_url;
        };

        struct StSearchResult
        {
            int resultType = 0;
            bool hasMore = false;
            std::string default_portrait;
            std::string groupLabel;
            // user
            std::vector<StUserItem> _users;
            // group
            std::vector<StGroupItem> _groups;
            // history
            std::vector<StHistory> _history;

            std::vector<StHistoryFile> _files;
        };
    }
}


struct StNetEmoticon
{
    std::string pkgid;
    std::string emoName;
    std::string emoFile;
    std::string desc;
    std::string iconPath;
    QInt32      filesize;
    std::string md5;
};
typedef std::vector<std::shared_ptr<StNetEmoticon> > ArStNetEmoticon;
typedef std::map<int, QTalk::Search::StSearchResult> SearchResult;



namespace QTalk {
    struct StNotificationParam {
        std::string title{};
        std::string message{};
        std::string icon{};
        bool playSound{};
        //
        std::string xmppId{};
        std::string from{};
        std::string realJid{};
        std::string loginUser{};
        int   chatType{};
    };
}

#endif//_COMMONSTRCUT_H_
