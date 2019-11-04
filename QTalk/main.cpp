#include "QTalkApp.h"
#include <iostream>
#include <QSettings>
#include <QTextCodec>

int main(int argc, char *argv[]) {
    unsigned short pid = QCoreApplication::applicationPid();
    qputenv("QTWEBENGINE_REMOTE_DEBUGGING", std::to_string(pid).data());


    QCoreApplication::setAttribute(Qt::AA_ShareOpenGLContexts);
    // 缩放因子
    QSettings settings(QSettings::NativeFormat, QSettings::UserScope,
#ifdef _STARTALK
                       "qunar.com", "startalk");
#else
                       "qunar.com", "qtalk");
#endif
    bool scale_enable = settings.value("QT_SCALE_ENABLE").toBool();
    if (scale_enable) {
        QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
		QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
    }
    else
	    QCoreApplication::setAttribute(Qt::AA_DisableHighDpiScaling);

    QTalkApp a(argc, argv);

    return 0;
}
