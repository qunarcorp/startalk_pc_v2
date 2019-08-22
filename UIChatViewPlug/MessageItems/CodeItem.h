//
// Created by cc on 2019-02-28.
//

#ifndef QTALK_V2_CodeItem_H
#define QTALK_V2_CodeItem_H

#include "MessageItemBase.h"

#include <QLabel>


class CodeItem : public MessageItemBase {

Q_OBJECT
public:
    explicit CodeItem(const QTalk::Entity::ImMessageInfo &msgInfo, QWidget *parent = Q_NULLPTR);
    ~CodeItem() override = default;;

public:
    QSize itemWdtSize() override;

protected:
    void mousePressEvent(QMouseEvent *event) override;
    bool eventFilter(QObject* o, QEvent* e) override;

private:
    void init();
    void initLayout();
    void initSendLayout();
    void initReceiveLayout();
    void initContentLayout();

private:
    QLabel *_iconLab;
    QLabel *_titleLab;
    QLabel *_contentLab;

    QSize _headPixSize;
    QMargins _mainMargin;
    QMargins _leftMargin;
    QMargins _rightMargin;
    QSize _contentSize;
    int _mainSpacing;

    int _leftSpacing;
    int _rightSpacing;
    int _nameLabHeight;

public:
    QString _codeStyle;
    QString _codeLanguage;
    QString _code;

};


#endif //QTALK_V2_CodeItem_H
