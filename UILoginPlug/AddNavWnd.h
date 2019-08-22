//
// Created by cc on 18-12-27.
//
#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif

#ifndef QTALK_V2_ADDNAVWND_H
#define QTALK_V2_ADDNAVWND_H

#include <QFrame>
#include <QLineEdit>
#include <QTextEdit>
#include <QCheckBox>
#include <QPushButton>
#include "../CustomUi/UShadowWnd.h"
#include "../CustomUi/LinkButton.h"

enum {
    EM_NO,
    EM_YES
};

/**
 *
 */
 class NavView;
class AddNavWnd : public UShadowDialog
{
    Q_OBJECT
public:
    explicit AddNavWnd(NavView* view );
    ~AddNavWnd() override;

public:
    void resetWnd();

signals:
    void addNavSinal(const QString &name, const QString &navAddr, const bool &isDebug);

private:
    QLineEdit*  _pNameEdit;
    QTextEdit*  _pAddrEdit;
    LinkButton* _pNavDetail;

    QPushButton* _pCancelBtn;
    QPushButton* _pAddBtn;

private:
    NavView*     _pNavView;
};


#endif //QTALK_V2_ADDNAVWND_H
