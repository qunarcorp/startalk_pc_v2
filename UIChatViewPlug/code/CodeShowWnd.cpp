//
// Created by QITMAC000260 on 2019-02-28.
//

#include "CodeShowWnd.h"
#include <QVBoxLayout>
#include "../ChatViewMainPanel.h"
#include "../../Platform/Platform.h"
#include "../../quazip/JlCompress.h"
#include "../../QtUtil/Utils/Log.h"
#include <QFile>
#include <QDir>
#include <QFileInfo>
#include <QWebEngineScript>

CodeShowWnd::CodeShowWnd(QWidget* parent)
        : UShadowDialog(parent, true)
{
    initUi();
    initRes();
}

CodeShowWnd::~CodeShowWnd()
{

}

/**
 * 初始化
 */
void CodeShowWnd::initUi()
{
    _pWebView = new QWebEngineView(this);
	_pWebView->setObjectName("CodeWebView");
    _pCodeShell = new CodeShell("查看代码片段", _pWebView, this);

    setMoverAble(true, _pCodeShell->getTitleFrm());
    //
    auto* lay = new QHBoxLayout(_pCenternWgt);
    lay->setMargin(0);
    lay->addWidget(_pCodeShell);

    connect(_pCodeShell, &CodeShell::closeWnd, [this](){setVisible(false);});
    connect(_pCodeShell, &CodeShell::styleChanged, this, &CodeShowWnd::loadCodeFile);

#ifdef _MACOS
    setWindowFlags(this->windowFlags() | Qt::Tool);
#endif
}

/**
 *
 * @param type
 * @param language
 * @param content
 */
void CodeShowWnd::showCode(const QString& type, const QString& language, const QString& content)
{
    if(_pCodeShell && _pWebView)
    {
        bool isStyle = _pCodeShell->setCodeStyle(type);
        bool isLanguage = _pCodeShell->setCodeLanguage(language);
        QString t = type;
        QString l = language;
        if(!isStyle)
            t = _pCodeShell->getCodeStyle();
        if(!isLanguage)
            l = _pCodeShell->getCodeLanguage();
        _strCodeContent = content;
        loadCodeFile(t, l);
    }
}

/**
 *
 */
void CodeShowWnd::initRes()
{
    QString userDir = Platform::instance().getAppdataRoamingPath().data();
    QString destDir = QString("%1/html").arg(userDir);
    QString srcFile = ":/code/html.zip";
    if(!QFile::exists(destDir) && QFile::exists(srcFile))
    {
        QString destFile = destDir + ".zip";
        bool ret = QFile::copy(srcFile, destFile);
        if(ret)
        {
            QStringList lstFile = JlCompress::extractDir(destFile, userDir);
            if(lstFile.empty())
            {
                error_log("code资源文件解压失败");
            }
            QFile::remove(destFile);
        }
    }
}

void CodeShowWnd::loadCodeFile(const QString &type, const QString &language)
{
    QString path = QString::fromStdString(Platform::instance().getAppdataRoamingPath());

    auto* o = sender();
    if(nullptr == o)
    {
        _strCodeContent.replace("&",  "&amp;");
        _strCodeContent.replace("\"", "&quot;");
        _strCodeContent.replace("'", "&apos;");
        _strCodeContent.replace("<",  "&lt;");
        _strCodeContent.replace(">",  "&gt;");
        _strCodeContent.replace("\U00002028",  "\n");
    }

    QString html = QString("<!DOCTYPE html>  "
                           "<html lang=\"en\">  "
                           "<head>  "
                           "<meta charset=\"UTF-8\">  "
                           "<title>代码预览</title>  "
                           "<link rel=\"stylesheet\" href=\"styles/%1.css\">  "
                           "<script src=\"highlight.pack.js\"></script>  "
                           "<script>hljs.initHighlightingOnLoad();</script>  "
                           "</head>"
                           "<body>"
                           "<pre>"
                           "<code class=\"%2\">%3"
                           "</code>"
                           "</pre>"
                           "</body>"
                           "</html>").arg(type).arg(language).arg(_strCodeContent);

    QString codeFilePath = path + "/html/code.html";
    QFile codeFile(codeFilePath);
    if(codeFile.open(QIODevice::WriteOnly))
    {
        codeFile.resize(0);
        codeFile.write(html.toUtf8());
    }
    _pWebView->load(QUrl::fromLocalFile(codeFilePath));
}

void copyFile(const QString& oldPath, const QString& newPath)
{
    QFileInfo oldinfo(oldPath);
    QFileInfo newinfo(oldPath);
    if(oldinfo.isDir())
    {
        if(!newinfo.exists())
        {
            QDir dir = QDir(QString::fromStdString(Platform::instance().getAppdataRoamingPath()));
            dir.mkpath(newPath);

            QDir oldDir(oldPath);
            auto infoList = oldDir.entryInfoList();
            for(const auto& info : infoList)
            {
                copyFile(info.absoluteFilePath(), QString("%1/%2").arg(newPath, info.baseName()));
            }
        }

    }
    else
    {
        QFile::copy(oldPath, newPath);
    }

}