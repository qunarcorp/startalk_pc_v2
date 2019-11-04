//
// Created by QITMAC000260 on 2018/11/19.
//

#include "GroupItemDelegate.h"
#include <QDebug>
#include <QPainter>
#include <QFile>
#include "../CustomUi/HeadPhotoLab.h"
#include "../UICom/qimage/qimage.h"
#include "../UICom/StyleDefine.h"

#define HEAD_WIDTH 24

using namespace QTalk;
GroupItemDelegate::GroupItemDelegate(QWidget *parent)
        : QStyledItemDelegate(parent), _parentWgt(parent)
{

}

GroupItemDelegate::~GroupItemDelegate() {

}

void GroupItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyledItemDelegate::paint(painter, option, index);

    painter->save();
    painter->setRenderHint(QPainter::TextAntialiasing);

    painter->fillRect(option.rect, StyleDefine::instance().getChatGroupNormalColor());

    QRect rect = option.rect;

    QString strText = index.data(EM_ITEMDATA_TYPE_MASKNAME).toString();
    if(strText.isEmpty())
        strText = index.data(EM_ITEMDATA_TYPE_USERNAME).toString();
    painter->setPen(QPen(StyleDefine::instance().getChatGroupFontColor()));
    painter->drawText(QRect(rect.x() + 30, rect.y(), rect.width() - 30 - 30, rect.height()), Qt::AlignVCenter, strText);
    QString headPath = index.data(EM_ITEMDATA_TYPE_HEADPATH).toString();
    bool isOnline = index.data(EM_ITEMDATA_TYPE_ISONLINE).toBool();
    painter->setRenderHints(QPainter::Antialiasing,true);
    if(!QFile(headPath).isOpen())
    {
        int dpi = QTalk::qimage::instance().dpi();
        QPixmap pixmap = QTalk::qimage::instance().loadPixmap(headPath, true, true, HEAD_WIDTH * dpi);
        if(!isOnline)
        {
            pixmap = QTalk::qimage::instance().generateGreyPixmap(pixmap);
        }
        QPainterPath path;
        QRect headRect(rect.x() + 1, rect.y() + 8, HEAD_WIDTH, HEAD_WIDTH);
        painter->setPen(Qt::NoPen);
        painter->setBrush(QColor(240, 240, 240, 200));
        painter->drawEllipse(headRect);
        path.addEllipse(headRect);
        painter->setClipPath(path);
        int w = pixmap.width() / dpi;
        int h = pixmap.height() / dpi;
        painter->drawPixmap((HEAD_WIDTH - w) / 2 + headRect.x(),
                            (HEAD_WIDTH - h) / 2 + headRect.y(), w, h, pixmap);

        painter->fillPath(path, QTalk::StyleDefine::instance().getHeadPhotoMaskColor());
    }
    // 画成员权限
    int8_t role = index.data(EM_ITEMDATA_TYPE_USERTYPE).toUInt();
    switch (role) {
        case 1: // 群主
        {
            QPainterPath rolePath;
            QRect roleRect(rect.x() + rect.width() - 23, rect.y() + (rect.height() - 16)/2.0, 16,16);
            rolePath.addEllipse(roleRect);
            painter->setClipPath(rolePath);
            QPixmap icon(":/chatview/image1/GroupChatSidebar/group_creator.png");
            painter->drawPixmap(roleRect,icon);
        }
            break;
        case 2: // 管理员
        {
            QPainterPath rolePath;
            QRect roleRect(rect.x() + rect.width() - 23, rect.y() + (rect.height() - 16)/2.0, 16,16);
            rolePath.addEllipse(roleRect);
            painter->setClipPath(rolePath);
            QPixmap icon(":/chatview/image1/GroupChatSidebar/group_admin.png");
            painter->drawPixmap(roleRect,icon);
        }
            break;
        default:
            break;
    }
    painter->restore();
}

QSize GroupItemDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    const QSize& size = QStyledItemDelegate::sizeHint(option, index);
    return {size.width(), 40};
}