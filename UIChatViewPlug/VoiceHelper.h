//
// Created by QITMAC000260 on 2019-05-08.
//

#ifndef QTALK_V2_VOICEHELPER_H
#define QTALK_V2_VOICEHELPER_H

#include <string>
#include <sstream>

/**
* @description: VoiceHelper
* @author: cc
* @create: 2019-05-08 21:31
**/
namespace QTalk {

    namespace VoiceHelper
    {
        bool amrToWav(const std::string& szAmrFileName, const std::string& wavFileName);
    };

};


#endif //QTALK_V2_VOICEHELPER_H
