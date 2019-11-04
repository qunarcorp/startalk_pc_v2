//
// Created by lihaibin on 2019-06-05.
//

#ifndef QTALK_V2_NOTEMESSAGEITEM_H
#define QTALK_V2_NOTEMESSAGEITEM_H

#include "MessageItemBase.h"

class NoteMessageItem : public MessageItemBase
{
    Q_OBJECT
public:
    explicit NoteMessageItem(const QTalk::Entity::ImMessageInfo &msgInfo,
            QWidget *parent = nullptr);

    ~NoteMessageItem() override;

public:
    QSize itemWdtSize() override;

    // QWidget interface
protected:
    void resizeEvent(QResizeEvent *event) override;

    void mousePressEvent(QMouseEvent* e) override;
    void setIcon(const QString& iconPath);

Q_SIGNALS:
    void sgDownloadedIcon(const QString&);

private:
    QLabel *titleLabel;
    QLabel *tagLabel;
    QLabel *iconLabel;
    QLabel *priceLabel;

    QSize _headPixSize;
    QMargins _mainMargin;
    QMargins _leftMargin;
    QMargins _rightMargin;
    QSize _contentSize;
    int _mainSpacing;

    int _leftSpacing;
    int _rightSpacing;
    int _nameLabHeight;

    void init();
    void initLayout();
    void initSendLayout();
    void initReceiveLayout();
    void initContentLayout();
};

#endif //QTALK_V2_NOTEMESSAGEITEM_H
