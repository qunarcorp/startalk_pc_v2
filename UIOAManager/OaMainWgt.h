//
// Created by cc on 18-12-17.
//
#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif

#ifndef QTALK_V2_OAMAINWIDGETITEM_H
#define QTALK_V2_OAMAINWIDGETITEM_H

#include <QFrame>
#include <QTableWidget>
#include "../include/CommonDefine.h"
#include "../include/CommonStrcut.h"

typedef QTalk::StOAUIData::StMember StMember;

/**
 *
 */
class CellWgtItem : public QFrame
{
public:
    CellWgtItem(QString name, QString icon, QString  url, QWidget *parent = nullptr);
    ~CellWgtItem() override;

public:
    void mousePressEvent(QMouseEvent* e) override;
    void paintEvent(QPaintEvent* e) override ;

private:
    QString _name;
    QString _icon;
    QString _url;
};

class OAManagerPanel;
class OaMainWgt : public QFrame
{
public:
    OaMainWgt(const int& id, QString  name, const std::vector<StMember>& member, QWidget* parent = nullptr);
    ~OaMainWgt() override;

private:
    void initUi();

private:
    int _id;
    QString _name;
    const std::vector<StMember>& _members;

private:
    QTableWidget* _pTabWgt = nullptr;
};


#endif //QTALK_V2_OAMAINWIDGETITEM_H
