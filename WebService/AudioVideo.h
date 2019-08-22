//
// Created by QITMAC000260 on 2019-02-22.
//

#ifndef QTALK_V2_AUDIOVIDEO_H
#define QTALK_V2_AUDIOVIDEO_H


#include "../CustomUi/UShadowWnd.h"
#include "WebView.h"
#include "webservice_global.h"

/**
* @description: AudioVideo
* @author: cc
* @create: 2019-02-22 20:52
**/
class WEBSERVICE_EXPORT AudioVideo : public QWidget {
public:
    AudioVideo();
    ~AudioVideo() override;

public:
    static void start2Talk(const std::string& caller, const std::string& callee);
    static void startGroupTalk();

protected:
    void closeEvent(QCloseEvent* e) override;
    void closeWeb();

private:
    void loadQss();

private:
    WebView* _webView;
    static AudioVideo* _pAudioVideo;

private:
    bool isGroupVideo;
};


#endif //QTALK_V2_AUDIOVIDEO_H
