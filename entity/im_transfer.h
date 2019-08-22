//
// Created by lihaibin on 2019-07-03.
//

#ifndef QTALK_V2_IM_TRANSFER_H
#define QTALK_V2_IM_TRANSFER_H

#include <string>

namespace QTalk {
    namespace Entity {
        struct ImTransfer
        {
            ImTransfer()
            {
                newCsrName = "";
                nickName = "";
            }

            std::string nickName;
            std::string newCsrName;
        };
    }
}

#endif //QTALK_V2_IM_TRANSFER_H
