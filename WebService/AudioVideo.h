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
class AudioVideo : public QWidget {
Q_OBJECT
public:
    AudioVideo();
    ~AudioVideo() override;

public:
    void closeWeb();

protected:
    void closeEvent(QCloseEvent* e) override;

private:
    void loadQss();

Q_SIGNALS:
    void closeAudioVideo();
    void sgClose2AudioVideo(long long);

public:
    WebView* _webView;
    WebJsObj* obj;

public:
    bool isGroupVideo;

public:
    QString _conversationId;
    QString _json;

    bool _isCaller = false;
    bool _isVideo = false;
    QString _peerId ;
};


class WEBSERVICE_EXPORT AudioVideoManager : public QObject
{
    Q_OBJECT
public:
    ~AudioVideoManager() override;

public:
    void start2Talk(const std::string& caller, const std::string& callee);
    void start2Talk_old(const QString& json, const std::string& peerId, bool isVideo, bool isCall);
    void startGroupTalk(const QString& id, const QString &name);
    void onRecvWebRtcCommand(const QString& peerId, const QString& cmd);
    bool isCall(const std::string& peerId);
    bool isVideo(const std::string& peerId);
    bool isBusy();
    void onCloseAudioVideo(const std::string& peerId);

Q_SIGNALS:
    void sgSendSignal(const QString&, const QString&);
    void sgClose2AudioVideo(const QString&, bool, long long);

private:
    QMap<QString, AudioVideo*> _2Talks;
    QMap<QString, AudioVideo*> _groupTalks;
    QMap<QString, bool> _isCall;

};
#endif //QTALK_V2_AUDIOVIDEO_H
