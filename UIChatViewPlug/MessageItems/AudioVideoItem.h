//
// Created by cc on 2019-02-28.
//

#ifndef QTALK_V2_AudioVideoItem_H
#define QTALK_V2_AudioVideoItem_H

#include "MessageItemBase.h"

#include <QLabel>


class AudioVideoItem : public MessageItemBase {

Q_OBJECT
public:
    explicit AudioVideoItem(const QTalk::Entity::ImMessageInfo &msgInfo, QWidget *parent = Q_NULLPTR);
    ~AudioVideoItem() override = default;;

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
    QLabel *_pIconLabel;
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

private:
};


#endif //QTALK_V2_AudioVideoItem_H
