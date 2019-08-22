#ifndef IM_GROUP_H
#define IM_GROUP_H
#include <string>
#include "../include/CommonDefine.h"
/**
 * @brief The ImUserInfo struct 群组信息结构体
 * @author cyz
 * @date 2018.9.29
 */
namespace QTalk {
    namespace Entity {
        struct ImGroupInfo
        {
            ImGroupInfo()
            {
                GroupId = "";
                Name = "";
                Introduce = "";
                HeaderSrc = "";
                Topic = "";
                LastUpdateTime = 0;
                ExtendedFlag = 0;
            }

            std::string GroupId;
            std::string Name;
            std::string Introduce;
            std::string HeaderSrc;
            std::string Topic;
			QInt64 LastUpdateTime;
            QInt64 ExtendedFlag;
        };
    }
}

#endif // IM_GROUP_H
