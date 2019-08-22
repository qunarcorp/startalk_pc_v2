//
// Created by QITMAC000260 on 2018/11/16.
//

#ifndef QTALK_V2_PICTUREBROWSERPLUG_H
#define QTALK_V2_PICTUREBROWSERPLUG_H

#include <QWidget>
#include "uipicturebrowser_global.h"
#include "../interface/view/IUIPictureBroswerPlug.h"

class PictureBrowser;
class UIPICTUREBROWSER_EXPORT PictureBrowserPlug : public QObject, public IUIPictureBroswerPlug
{
	Q_OBJECT
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    Q_PLUGIN_METADATA(IID "com.plugin.pictureBroswer.www/1.0" FILE "pictureBroswer.json")
#endif
    Q_INTERFACES(IUIPictureBroswerPlug)

public:
    PictureBrowserPlug();
    ~PictureBrowserPlug();
public:
    QWidget *widget();

private:
    void init();

private:
    PictureBrowser *_mainPanel;
};


#endif //QTALK_V2_PICTUREBROWSERPLUG_H
