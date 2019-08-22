//
// Created by QITMAC000260 on 2019-06-18.
//

#ifndef QTALK_V2_MACNOTIFICATION_H
#define QTALK_V2_MACNOTIFICATION_H

/**
* @description: MacNotification
* @author: cc
* @create: 2019-06-18 10:46
**/

#include "../QtUtil/Enum/im_enum.h"
#include "../QtUtil/Utils/Log.h"
#include "ChatViewMainPanel.h"
#include "../Platform/Platform.h"
#include "../QtUtil/Utils/utils.h"

namespace QTalk {
    namespace mac {

            struct StNotificationParam {
                std::string title{};
//                std::string subTitle{};
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


            class Notification {
            public:
                static void showNotification(StNotificationParam* param);
            };
    }
}

#endif //QTALK_V2_MACNOTIFICATION_H
