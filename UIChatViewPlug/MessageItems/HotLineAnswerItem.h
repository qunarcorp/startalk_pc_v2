//
// Created by QITMAC000260 on 2019/08/29.
//

#ifndef QTALK_V2_HOTLINEANSWERITEM_H
#define QTALK_V2_HOTLINEANSWERITEM_H


#include "MessageItemBase.h"
#include <QLabel>
#include <QPainter>
#include <QHBoxLayout>
#include <utility>
#include <QStyledItemDelegate>
#include <QListView>
#include <QStandardItemModel>
#include <QToolButton>
#include "../../CustomUi/HeadPhotoLab.h"

/**
* @description: HotLineAnswer
* @author: cc
* @create: 2019-08-29 18:52
**/

class ReqButton : public QFrame {
Q_OBJECT
public:
    explicit ReqButton(int id, const QString& text, QString url, QWidget* parent)
            : QFrame(parent), _url(std::move(url)), _id(id)
    {
        _iconBtn = new QToolButton(this);
        _iconBtn->setFixedSize(30, 30);
        _iconBtn->setCheckable(true);
        if(0 == id)
            _iconBtn->setObjectName("NoBtn");
        else if(1 == id)
            _iconBtn->setObjectName("YesBtn");
        else {}

        auto* ansLabel = new QLabel(text, this);
        ansLabel->setObjectName("hotLineAnswerLabel");
        ansLabel->setFixedHeight(30);
        ansLabel->setWordWrap(true);

        auto* itemLay = new QHBoxLayout(this);
        itemLay->setMargin(0);
        itemLay->setSpacing(5);
        itemLay->addWidget(_iconBtn);
        itemLay->addWidget(ansLabel);

        connect(this, &ReqButton::sgSendReq, [this, ansLabel, id, text](int, const QString&){
            _iconBtn->setChecked(true);
            ansLabel->setText(QString("<font color=#00CABE>%1</font>").arg(text));
        });

        connect(_iconBtn, &QToolButton::clicked, [this](){
            if(!_url.isEmpty() && _reqEnable)
                emit sgSendReq(_id, _url);
        });
    }

public:
    void setReqEnable(bool enable) {
        _reqEnable = enable;
        _iconBtn->setEnabled(enable);
    }

Q_SIGNALS:
    void sgSendReq(int, const QString& );

protected:
    void mousePressEvent(QMouseEvent* e) override
    {
        if(!_url.isEmpty() && _reqEnable)
            emit sgSendReq(_id, _url);

        QFrame::mousePressEvent(e);
    }

private:
    int _id;
    QString _url;
    bool _reqEnable = true;

    QToolButton* _iconBtn;
};

/** **/
enum {
    EM_ITEM_DATA_TYPE_TEXT = Qt::UserRole + 1,
    EM_ITEM_DATA_TYPE_ACTION_TYPE,
    EM_ITEM_DATA_TYPE_ACTION_UID,
    EM_ITEM_DATA_TYPE_ACTION_URL,
    EM_ITEM_DATA_TYPE_ACTION_HEIGHT,

};

class HotLineTipDelegate : public QStyledItemDelegate{
    Q_OBJECT
public:
    explicit HotLineTipDelegate(QWidget *parent);
    ~HotLineTipDelegate() override = default;

protected:
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index) override;

public:
    QFont _font;

};

/** **/
class TextBrowser;
class ChatMainWgt;
class HotLineAnswerItem : public MessageItemBase {

Q_OBJECT
public:
    explicit HotLineAnswerItem(const QTalk::Entity::ImMessageInfo &msgInfo, ChatMainWgt *parent = nullptr);
    ~HotLineAnswerItem() override;

public:
    QSize itemWdtSize() override;
    void onImageDownloaded(const QString& link);

private:
    void init();
    void initLayout();
    void initSendLayout();
    void initReceiveLayout();
    void initContentLayout();

private:
    qreal getRealString(const QString &src, qreal &lastLineWidth);

private slots:
    void onAnchorClicked(const QUrl &url);
    void onImageClicked(const QString& path, const QString& link);

private:
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
    TextBrowser * _textBrowser;
    ChatMainWgt*  _pMainWgt;
    QListView*    _pTipListView;
    QStandardItemModel* _pTipListModel;

private:
    QVector<StTextMessage> msgs;
    QString content;

};

#endif //QTALK_V2_HOTLINEANSWERITEM_H
