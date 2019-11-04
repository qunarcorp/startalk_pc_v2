//
// Created by cc on 2019-02-28.
//

#ifndef QTALK_V2_MedalMind_H
#define QTALK_V2_MedalMind_H

#include "MessageItemBase.h"
#include <QLabel>


class MedalMind : public MessageItemBase {

Q_OBJECT
public:
    explicit MedalMind(const QTalk::Entity::ImMessageInfo &msgInfo, QWidget *parent = Q_NULLPTR);
    ~MedalMind() override = default;

public:
    QSize itemWdtSize() override;

protected:
    void mousePressEvent(QMouseEvent* e) override;

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
};


#endif //QTALK_V2_MedalMind_H
