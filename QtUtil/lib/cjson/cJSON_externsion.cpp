//
// Created by may on 2019-01-24.
//

#include "cJSON_inc.h"


namespace QTalk {
    namespace JSON {

        exception::exception(const char *errstring) : runtime_error(errstring) {
        }
    };
};

