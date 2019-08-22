#include "QTalkApp.h"
#include <iostream>
#include <QTextCodec>

#ifdef _DEBUG

#include "test_inc.h"

#endif

int main(int argc, char *argv[]) {

    QCoreApplication::setAttribute(Qt::AA_ShareOpenGLContexts);
    QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
#if (QT_VERSION >= QT_VERSION_CHECK(5, 6, 0))
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

    QTalkApp a(argc, argv);

    return 0;
}
