//
// Created by QITMAC000260 on 2019-07-15.
//

#include "StyleDefine.h"

namespace QTalk {

    StyleDefine* StyleDefine::_defines = nullptr;
    StyleDefine & StyleDefine::instance()
    {
        if (nullptr == _defines)
        {
            static StyleDefine define;
            _defines = &define;
        }
        return *_defines;
    }
    //

}