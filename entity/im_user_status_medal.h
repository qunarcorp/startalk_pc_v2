//
// Created by QITMAC000260 on 2019/10/16.
//

#ifndef QTALK_V2_IM_USER_STATUS_MEDAL_H
#define QTALK_V2_IM_USER_STATUS_MEDAL_H

#include <string>

namespace QTalk {
    namespace Entity {
        struct ImUserStatusMedal
        {
            int medalId = 0;
            std::string userId = std::string();
            std::string host = std::string();
            int medalStatus = 0;
            int mappingVersion = 0;
            long long updateTime = 0;
        };
    }
}

#endif //QTALK_V2_IM_USER_STATUS_MEDAL_H
