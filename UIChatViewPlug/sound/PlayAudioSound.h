//
// Created by may on 2019-03-18.
//

#ifndef QTALK_V2_PLAYAUDIOSOUND_H
#define QTALK_V2_PLAYAUDIOSOUND_H


namespace QTalk {
    namespace mac {
        namespace util {
            class PlayAudioSound {
            public:
                static void PlaySound(const char* path);
                static void removeSound(const char* path);
            };
        }
    }
}


#endif //QTALK_V2_PLAYAUDIOSOUND_H
