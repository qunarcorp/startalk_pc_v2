#ifndef IM_SESSION_H
#define IM_SESSION_H
#include <string>
#include "../include/CommonDefine.h"
/**
 * @brief The ImSessionInfo struct 会话信息结构体
 * @author cyz
 * @date 2018.9.21
 */
namespace QTalk {
    namespace Entity {
        struct ImSessionInfo
        {
            ImSessionInfo()
            {
                XmppId = "";
                RealJid = "";
                UserId = "";
                LastMessageId = "";
                LastUpdateTime = 0;
                ChatType = 0;
                ExtendedFlag = 0;
                UnreadCount = 0;
                MessageState = 0;
                Content = "";
                MessType = -1;
            }

            bool operator ==( const ImSessionInfo &right)
            {
                return this->XmppId == right.XmppId &&
                        this->RealJid == right.RealJid &&
                        this->UserId == right.UserId &&
                        this->LastMessageId == right.LastMessageId &&
                        this->LastUpdateTime == right.LastUpdateTime &&
                        this->ChatType == right.ChatType &&
                        this->ExtendedFlag == right.ExtendedFlag &&
                        this->UnreadCount == right.UnreadCount &&
                        this->MessageState == right.MessageState &&
                        this->Content == right.Content &&
                        this->MessType == right.MessType;

            }

            std::string XmppId;
            std::string RealJid;
            std::string UserId;
            std::string LastMessageId;
            QInt64 LastUpdateTime;
            int ChatType;
            int ExtendedFlag;
            int UnreadCount;
            int MessageState;
            std::string Content;
            int MessType;
        };
    }
}
#endif // IM_SESSION_H
