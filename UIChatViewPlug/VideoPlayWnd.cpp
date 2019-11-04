//
// Created by QITMAC000260 on 2019-03-01.
//

#include "VideoPlayWnd.h"
#include "../WebService/WebService.h"
#include "../QtUtil/Utils/Log.h"
#include <QApplication>
#include <QDesktopWidget>
#include <QWebEnginePage>
#include <QDesktopServices>

VideoPlayWnd::VideoPlayWnd(QWidget* parent)
    : QFrame(nullptr)
{
    initUi();
}

VideoPlayWnd::~VideoPlayWnd() {
}

/**
 * 初始化Ui
 */
void VideoPlayWnd::initUi()
{

}

/**
 *
 */
void VideoPlayWnd::showVideo(const QString& url, int width, int height)
{
//    QString newUrl = url;
//    if(newUrl.startsWith("file/") || newUrl.startsWith("/file"))
//    {
//        newUrl = QString("%1/%2").arg(NavigationManager::instance().getFileHttpHost().data()).arg(newUrl);
//    }
    QApplication::processEvents(QEventLoop::AllEvents, 100);

//    QDesktopWidget *deskTop = QApplication::desktop();
//    int curMonitor = deskTop->screenNumber(this);
//    QRect deskRect = deskTop->screenGeometry(curMonitor);
//    double maxHeight = deskRect.height() * 0.8;
//    double maxWidth = deskRect.width() * 0.8;
//    double factor = 1.0;
//    double factor1 = maxWidth / width;
//    double factor2 = maxHeight / height;
//    if (factor1 < 1.0 || factor2 < 1.0) {
//        factor = qMin(factor1, factor2);
//    }
//    width = factor * width;
//    height = factor * height;
//
//    QString html = QString("<!DOCTYPE HTML> \
//		<html lang = \"en\"> \
//		<head> \
//		<meta charset = \"UTF-8\"> \
//        <title>视频播放</title>\
//		</head> \
//		<body> \
//		<video controls = \"controls \" height = \"%2\" style=\"z-index: 100; position: absolute; top: 50%;left: 50%;\
//        width: auto; -ms-transform: translateX(-50%) translateY(-50%); -webkit-transform: translateX(-50%) translateY(-50%);\
//        transform: translateX(-50%) translateY(-50%);background-size: cover;\"> \
//		<source src = \"%1\" type = 'video/mp4; codecs=\"avc1.4D401E, mp4a.40.2\"'> \
//		</video> \
//		</body> \
//		</html>").arg(url).arg(height);//.arg(width).arg(height);
//
//    QString path = QString::fromStdString(Platform::instance().getAppdataRoamingPath());
//    QString videoFilePath = path + "/html/video.html";
//    QFile codeFile(videoFilePath);
//    if(codeFile.open(QIODevice::WriteOnly))
//    {
//        codeFile.resize(0);
//        codeFile.write(html.toUtf8());
//    }
#ifdef _WINDOWS
    QDesktopServices::openUrl(QUrl::fromLocalFile(url));
#else
    WebService::loadUrl(QUrl::fromLocalFile(url), false);
#endif
}