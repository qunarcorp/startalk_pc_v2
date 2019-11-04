//
// Created by QITMAC000260 on 2019-03-29.
//

#ifndef QTALK_V2_WEBJSOBJ_H
#define QTALK_V2_WEBJSOBJ_H

#include <QObject>

/**
* @description: WebJsObj
* @author: cc
* @create: 2019-03-29 18:21
**/
class WebJsObj : public QObject {
Q_OBJECT
public:
    explicit WebJsObj(QObject *o);

    ~WebJsObj() override;

Q_SIGNALS:

    void runScript(const QString &js);

    void sgFullScreen();

    void closeGroupRoom();

    void sgSendSignal(const QString&, const QString&);

    void sgCloseRtcWindow(const QString&, long long);

public:
    Q_INVOKABLE void getCkey();

    Q_INVOKABLE void getUserId();

    Q_INVOKABLE void getMyNick();

    Q_INVOKABLE void getRoomID();

    Q_INVOKABLE void getTopic();

    Q_INVOKABLE void getStartTime();

    Q_INVOKABLE void close_video_room(const QString &roomid);

    Q_INVOKABLE void jsgotoVideoRoom(const QString &roomjid);

    Q_INVOKABLE void getNickByUserId(const QString &id);

    Q_INVOKABLE void enableFullScreen(const QString &roomid);

    Q_INVOKABLE void cancelFullScreen(const QString &roomid);
    // 二人视频
    Q_INVOKABLE void sendSignal(const QString& json, const QString&id);

    Q_INVOKABLE void closeRtcWindow( const QString&id, long long occupied_time);

    Q_INVOKABLE void StartVideoSound(const QString&id);

    Q_INVOKABLE void StopVideoSound(const QString&id);

    Q_INVOKABLE void WriteLocalLog(const QString&id, const QString&log);
};


#endif //QTALK_V2_WEBJSOBJ_H
