//
// Created by lihaibin on 2019-07-03.
//

#ifndef QTALK_V2_IM_QR_CONTENT_H
#define QTALK_V2_IM_QR_CONTENT_H

#include <string>

namespace QTalk {
    namespace Entity {
        struct IMQRContent
        {
            IMQRContent()
            {
                sid = 0;
                gid = 0;
                content = "";
                contentseq = 0;
                version = 0;
            }

            int sid;
            int gid;
            std::string content;
            int contentseq;
            int version;
        };
    }
}

#endif //QTALK_V2_IM_QR_CONTENT_H
