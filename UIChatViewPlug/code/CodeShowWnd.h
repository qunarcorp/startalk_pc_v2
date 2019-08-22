//
// Created by QITMAC000260 on 2019-02-28.
//
#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif
#ifndef QTALK_V2_CODESHOWWND_H
#define QTALK_V2_CODESHOWWND_H

#include "../../CustomUi/UShadowWnd.h"
#include "CodeShell.h"
#include <QWebEngineView>

/**
* @description: CodeShowWnd
* @author: cc
* @create: 2019-02-28 21:52
**/
class ChatViewItem;
class CodeShowWnd : public UShadowDialog{
public:
    explicit CodeShowWnd(QWidget* parent = nullptr);
    ~CodeShowWnd() override;

public:
    void showCode(const QString& type, const QString& language, const QString& content);

private:
    void initUi();
    void initRes();
    void loadCodeFile(const QString &type, const QString &language);

private:
    CodeShell* _pCodeShell;
    QWebEngineView* _pWebView;

private:
    QString _strCodeContent;

};



#endif //QTALK_V2_CODESHOWWND_H
