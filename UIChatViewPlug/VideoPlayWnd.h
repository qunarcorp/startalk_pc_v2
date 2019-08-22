//
// Created by QITMAC000260 on 2019-03-01.
//

#ifndef QTALK_V2_VIDEOPLAYWND_H
#define QTALK_V2_VIDEOPLAYWND_H

#include "../CustomUi/UShadowWnd.h"
#include <QFrame>
#include <QDateTime>

/**
* @description: VideoPlayWnd
* @author: cc
* @create: 2019-03-01 19:15
**/
class VideoPlayWnd : public QFrame {
    Q_OBJECT
public:
    explicit VideoPlayWnd(QWidget* parent = nullptr);
    ~VideoPlayWnd() override;

private:
    void initUi();

public slots:
    void showVideo(const QString& url, int width, int height);

};


#endif //QTALK_V2_VIDEOPLAYWND_H
