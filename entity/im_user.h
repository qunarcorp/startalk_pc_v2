#ifndef IM_USER_H
#define IM_USER_H

#include <string>
#include "../include/CommonDefine.h"
/**
 * @brief The ImSessionInfo struct 组织架构信息结构体
 * @author cyz
 * @date 2018.9.21
 */
namespace QTalk {
    namespace Entity {
        struct ImUserInfo {
            ImUserInfo() {

                HeadVersion = 0;
                IncrementVersion = 0;
                ExtendedFlag = 0;
                Gender = 0;
                isVisible = false;
            }

            std::string UserId;
            std::string XmppId;
            std::string Name;
            std::string DescInfo;
            std::string HeaderSrc;
            std::string SearchIndex;
            std::string Mood;
            std::string NickName;
            // add at 2018.12.21
            int Gender; // 1 男 2 女
            // add end
            // add at 2019.6.17
            std::string  userType;   // 是否为机器人
            bool isVisible; // 组织架构是否显示
            // add end
            QInt64 HeadVersion;
            int IncrementVersion;
            int ExtendedFlag;
        };
    }
}
#endif // IM_USER_H
