//
// Created by cc on 18-12-3.
//

#ifndef QTALK_V2_IM_CONFIG_H
#define QTALK_V2_IM_CONFIG_H

#include <string>

namespace QTalk {
    namespace Entity {
        struct ImConfig
        {
            ImConfig()
            {
                ConfigKey = "";
                ConfigSubKey = "";
                ConfigValue = "";
                Version = 0;
                ExtendedFlag = "";
            }

            std::string ConfigKey;
            std::string ConfigSubKey;
            std::string ConfigValue;
            int Version;
            std::string ExtendedFlag;
        };
    }
}

#endif //QTALK_V2_IM_CONFIG_H
