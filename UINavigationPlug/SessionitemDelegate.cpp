//
// Created by QITMAC000260 on 2018/11/20.
//

#include "SessionitemDelegate.h"
#include "../QtUtil/Utils/Log.h"
#include "../CustomUi/HeadPhotoLab.h"
#include <QPainter>
#include <iostream>
#include <QEvent>
#include <QStandardItemModel>
#include <QPen>
#include "../Platform/Platform.h"
#include "../UICom/qimage/qimage.h"
#include "../Platform/dbPlatForm.h"
#include "../UICom/StyleDefine.h"

#define HEAD_WIDTH 42
#define ISTOP_TEXT "[置顶] "
#define AT_TEXT_SELF "[有人@我]"
#define AT_TEXT_ALL "[@all]"

using namespace QTalk;

SessionSortModel::SessionSortModel(QObject *parent)
    : QSortFilterProxyModel(parent)
{

}

SessionSortModel::~SessionSortModel()
= default;

bool SessionSortModel::lessThan(const QModelIndex &source_left, const QModelIndex &source_right) const {

    if (!source_left.isValid() || !source_right.isValid())
        return false;

    bool leftTop = source_left.data(ITEM_DATATYPE_ISTOP).toBool();
    bool rightTop = source_right.data(ITEM_DATATYPE_ISTOP).toBool();

    bool ret = false;
    if(leftTop != rightTop)
        ret = leftTop > rightTop;
    else
        ret = source_left.data(ITEM_DATATYPE_LASTTIME).toLongLong() > source_right.data(ITEM_DATATYPE_LASTTIME).toLongLong();
    return ret;
}

//
SessionitemDelegate::SessionitemDelegate(QWidget* parent)
    :QStyledItemDelegate(parent), _pParentWgt(parent)
{
    _nameFont.setPixelSize(14);
    _contentFont.setPixelSize(12);
    _timeFont.setPixelSize(11);
    _unreadFont.setPixelSize(10);
}

SessionitemDelegate::~SessionitemDelegate()
{

}

QSize SessionitemDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    const QSize& size = QStyledItemDelegate::sizeHint(option, index);

    return {size.width(), 66};
}

void SessionitemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if(!index.isValid())
        return;

    painter->save();
    painter->setRenderHint(QPainter::TextAntialiasing);       //
    bool isTop = index.data(ITEM_DATATYPE_ISTOP).toBool();
    bool unNotice = index.data(ITEM_DATATYPE_UNNOTICE).toBool();
    // 背景色
    QRect rect = option.rect;
    bool select = option.state & QStyle::State_Selected;
    if (select)
    {
        painter->fillRect(rect, StyleDefine::instance().getNavSelectColor());
    }
    else
    {
        painter->fillRect(rect, isTop ? StyleDefine::instance().getNavTopColor() :
                                StyleDefine::instance().getNavNormalColor());

    }
    if(isTop)
    {
        QPainterPath path;
        path.moveTo(rect.left(), rect.top());
        path.lineTo(rect.left() + 12, rect.top());
        path.lineTo(rect.left(), rect.top() + 12);
        painter->fillPath(path, QBrush(StyleDefine::instance().getNavTipColor()));
    }

    QString headPath = index.data(ITEM_DATATYPE_HEADPATH).toString();
    QString strName = index.data(ITEM_DATATYPE_USERNAME).toString();
    QString content = index.data(ITEM_DATATYPE_MESSAGECONTENT).toString();
    QString time = index.data(ITEM_DATATYPE_LASTSTRTIME).toString();
    QString realJid = index.data(ITEM_DATATYPE_REALJID).toString();
    int chattype = index.data(ITEM_DATATYPE_CHATTYPE).toInt();
    if(chattype == QTalk::Enum::ConsultServer && !realJid.isNull()){//自己是客服 需要拼接显示name
        std::shared_ptr<QTalk::Entity::ImUserInfo> userInfo  =
                dbPlatForm::instance().getUserInfo(realJid.toStdString());
        if(userInfo != nullptr){
            strName = QString::fromStdString(QTalk::getUserName(userInfo) + "-") + strName;
        } else{
            std::string remark = dbPlatForm::instance().getMaskName(realJid.toStdString());
            if(remark.empty()){
                strName = realJid + "-" + strName;
            } else{
                strName = QString::fromStdString(remark) + "-" + strName;
            }
        }
    }
    //
    strName.replace("\n", " ");
    strName = strName.trimmed().replace(QRegExp("\\s{1,}"), " ");

    bool isOnline = index.data(ITEM_DATATYPE_ISONLINE).toBool();
    int dpi = QTalk::qimage::instance().dpi();
    int unreadCount = index.data(ITEM_DATATYPE_UNREADCOUNT).toInt();;
    QString strUnreadCount = QString::number(unreadCount);
    if(unreadCount > 99) strUnreadCount = "99+";
    //
    int atCount = index.data(ITEM_DATATYPE_ATCOUNT).toInt();
    //
    QFontMetricsF timeF(_timeFont);
    QFontMetricsF nameF(_nameFont);
    QFontMetricsF contentF(_contentFont);

    double maxNameWidth = rect.width() - 80 - nameF.width(time);
    strName = nameF.elidedText(strName, Qt::ElideRight, maxNameWidth);
    painter->restore();
    painter->save();
    // 名称
    painter->setPen(QPen(select ? StyleDefine::instance().getNavNameSelectFontColor() : StyleDefine::instance().getNavNameFontColor()));
    painter->setFont(_nameFont);
    QRectF textRect(rect.x() + 70, rect.y(), (int)maxNameWidth, rect.height()/2);

    painter->drawText(textRect, Qt::AlignBottom, strName);
    painter->restore();
    painter->save();
    //
    qreal atMsgWidth = 0;
    // 消息免打扰
    if(unNotice)
    {
        painter->setRenderHints(QPainter::Antialiasing,true);
        QPixmap pixmap = QTalk::qimage::instance().loadPixmap(":/UINavigationPlug/image1/noNotice.png", true, true, HEAD_WIDTH * dpi);
        QRect unNoticeRect(rect.right() - 35, rect.y() + rect.height()/2 + 6, 16, 16);
        painter->drawPixmap(unNoticeRect, pixmap);
        painter->restore();
        painter->save();

        if(unreadCount > 0)
        {
            content = QString("[%1条] %2").arg(unreadCount).arg(content);
        }
    }
    if (unreadCount > 0 && atCount > 0)
    {
        QString strAtCount = (bool) (atCount & 0xF0) ? AT_TEXT_SELF : AT_TEXT_ALL;

        atMsgWidth = contentF.width(strAtCount) + 2;
        painter->setPen(QPen(StyleDefine::instance().getNavAtFontColor()));
        painter->setFont(_contentFont);
        QRectF atRect(rect.x() + 65, rect.y() + rect.height() / 2 + 7, atMsgWidth, contentF.height() + 5);
        painter->drawText(atRect, Qt::AlignTop, strAtCount);
        painter->restore();
        painter->save();
    }
    // 最近消息内容
    painter->setPen(QPen(select ? StyleDefine::instance().getNavContentSelectFontColor() : StyleDefine::instance().getNavContentFontColor()));
    painter->setFont(_contentFont);
    qreal contentWidth = rect.width() - 80 - atMsgWidth - 20;
    content = QFontMetricsF(_contentFont).elidedText(content, Qt::ElideRight, contentWidth);
    QRectF contentRect(rect.x() + 70 + atMsgWidth, rect.y() + rect.height()/2 + 7,
                       contentWidth + 5 , contentF.height() + 5);
    painter->drawText(contentRect, Qt::AlignTop, content);
    painter->restore();
    painter->save();
    // 时间戳
    painter->setPen(QPen(select ? StyleDefine::instance().getNavTimeSelectFontColor() : StyleDefine::instance().getNavTimeFontColor()));
    painter->setFont(_timeFont);
    painter->drawText(QRect(rect.x() + 65, rect.y() + 20, rect.width() - 70, rect.height())
            , Qt::AlignRight, time);
    painter->restore();
    painter->save();
    // 头像
    painter->setRenderHints(QPainter::Antialiasing,true);
    QPixmap pixmap = QTalk::qimage::instance().loadPixmap(headPath, true, true, HEAD_WIDTH * dpi);
    if(pixmap.isNull())
    {
        if(chattype == QTalk::Enum::GroupChat)
        {
#ifdef _STARTALK
            headPath = ":/QTalk/image1/StarTalk_defaultGroup.png";
#else
            headPath = ":/QTalk/image1/defaultGroupHead.png";
#endif
        }
        else
        {
#ifdef _STARTALK
            headPath = ":/QTalk/image1/StarTalk_defaultHead.png";
#else
            headPath = ":/QTalk/image1/headPortrait.png";
#endif
        }
        pixmap = QTalk::qimage::instance().loadPixmap(headPath, true, true, HEAD_WIDTH * dpi);
    }
    if(!isOnline)
    {
        pixmap = QTalk::qimage::instance().generateGreyPixmap(pixmap);
    }
    {
        QPainterPath path;
        QRect headRect(rect.x() + 20, rect.y() + 12, HEAD_WIDTH, HEAD_WIDTH);
        painter->setPen(Qt::NoPen);
        painter->setBrush(QColor(240, 240, 240, 200));
        painter->drawEllipse(headRect);
        path.addEllipse(headRect);
        painter->setClipPath(path);
        int w = pixmap.width() / dpi;
        int h = pixmap.height() / dpi;
        painter->drawPixmap((HEAD_WIDTH - w) / 2 + headRect.x(),
                (HEAD_WIDTH - h) / 2 + headRect.y(), w, h, pixmap);
        //
        painter->fillPath(path, QTalk::StyleDefine::instance().getHeadPhotoMaskColor());
    }
    painter->restore();
    painter->save();
    // 未读消息提示
    if(unreadCount > 0)
    {
        QPen pen(QColor(255, 108, 86));
        painter->setRenderHint(QPainter::Antialiasing, true);
        painter->setPen(pen);
        painter->setBrush(QBrush(QColor(255, 75, 62)));

        if(unNotice)
        {
            QRect unreadRect(rect.right() - 17, rect.y() + rect.height()/2 + 10, 8, 8);
            painter->drawRoundRect(unreadRect, 99, 99);
        }
        else
        {
            QRect unreadRect(rect.right() - 32, rect.y() + rect.height()/2 + 6, 20, 13);
            painter->drawRoundRect(unreadRect, 80, 90);
            painter->setPen(QPen(StyleDefine::instance().getNavUnReadFontColor()));
            painter->setFont(_unreadFont);
            painter->drawText(unreadRect,  Qt::AlignCenter, strUnreadCount);
        }
    }
    //
    painter->restore();
}

