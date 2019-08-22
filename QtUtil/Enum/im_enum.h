#ifndef IM_ENUM_H
#define IM_ENUM_H
namespace QTalk {
    namespace Enum {
        enum ChatType
        {
            TwoPersonChat = 0,  // 二人消息
            GroupChat,      // 群会话
            System ,         // 系统消息
            Consult = 4,        // 咨询用户端会话
            ConsultServer,  // 咨询客服端会话
            Robot,          // 公众号
            Collection      // 收藏
        };
        enum OnlineStatus
        {
            OFFLINE = 1,        // 离线
            ONLINE,             // 在线
            AWAY,               // 离开
            BUSY                // 忙碌
        };
    }
}

#endif // IM_ENUM_H
