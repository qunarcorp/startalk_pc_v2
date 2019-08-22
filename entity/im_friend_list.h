//
// Created by cc on 18-12-18.
//

#ifndef QTALK_V2_IM_FRIEND_LIST_H
#define QTALK_V2_IM_FRIEND_LIST_H

namespace QTalk {
    namespace Entity {

        struct IMFriendList
        {
            IMFriendList()
            {
                UserId = "";
                XmppId = "";
                Version = 0;
                ExtendedFlag = "";
            }
            std::string UserId;
            std::string XmppId;
            int         Version;
            std::string ExtendedFlag;
        };
    }
}

#endif //QTALK_V2_IM_FRIEND_LIST_H
