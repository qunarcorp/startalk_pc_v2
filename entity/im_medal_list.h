//
// Created by QITMAC000260 on 2019/10/15.
//

#ifndef QTALK_V2_IM_MEDAL_LIST_H
#define QTALK_V2_IM_MEDAL_LIST_H

#include <string>

namespace QTalk {
    namespace Entity {
        struct ImMedalList
        {
            int medalId = -1;
            std::string medalName = std::string();
            std::string obtainCondition = std::string();
            std::string smallIcon = std::string();
            std::string bigLightIcon = std::string();
            std::string bigGrayIcon = std::string();
            std::string bigLockIcon = std::string();
            int status = 0;
        };
    }
}

#endif //QTALK_V2_IM_MEDAL_LIST_H
