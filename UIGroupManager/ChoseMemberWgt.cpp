//
// Created by cc on 18-11-12.
//

#include "ChoseMemberWgt.h"
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QListWidgetItem>
#include <QPainter>
#include <QMouseEvent>
#include "../CustomUi/HeadPhotoLab.h"
#include "../UICom/qimage/qimage.h"
#include "../UICom/StyleDefine.h"

ChoseItemDelegate::ChoseItemDelegate(QWidget *parent)
    : QStyledItemDelegate(parent)
{

}

void ChoseItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyledItemDelegate::paint(painter, option, index);
    painter->save();
    painter->setRenderHint(QPainter::TextAntialiasing);

    QRect rect = option.rect;

    QRect btnRect = QRect(rect.right() - 30, rect.y() + 10, 20, 20);
    QString closeBtn;
    closeBtn = ":/GroupManager/image1/delete_normal.png";
//    if ((option.state & QStyle::State_Selected) || (option.state & QStyle::State_MouseOver))
//    {
        painter->fillRect(option.rect, QTalk::StyleDefine::instance().getGroupManagerNormalColor());
//        closeBtn = ":/GroupManager/image1/delete_horver.png";
//    }
//    else
//    {
//        painter->fillRect(option.rect, QTalk::StyleDefine::instance().getGroupManagerNormalColor());
//    }

    painter->drawPixmap(btnRect, QPixmap(closeBtn));
//    if(index.column() == ITEM_COLUMN_CONTENT)
    {
        QString head = index.data(EM_ITEM_DATA_HEAD).toString();
        QString text = index.data(EM_ITEM_DATA_NAME).toString();
        QPixmap headPix = QTalk::qimage::instance().loadCirclePixmap(head, 11 * QTalk::qimage::instance().dpi());
        painter->setRenderHints(QPainter::Antialiasing, true);
        painter->setRenderHints(QPainter::SmoothPixmapTransform, true);
        painter->drawPixmap(rect.x() + 9, rect.y() + 9, 22, 22, headPix);

        QFont font;
        font.setPixelSize(13);
        font.setWeight(400);
        painter->setPen(QPen(QColor(51,51,51)));
        painter->setFont(font);
        painter->setPen(QTalk::StyleDefine::instance().getGroupManagerNormalFontColor());
        painter->drawText(rect.x() + 40, rect.y(), rect.width() - 40, rect.height(), Qt::AlignVCenter, text);
    }

    painter->restore();
}

QSize ChoseItemDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    const QSize& size = QStyledItemDelegate::sizeHint(option, index);

    return {size.width(), 40};
}

bool ChoseItemDelegate::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option,
                                   const QModelIndex &index)
{
    if(event->type() == QEvent::MouseButtonPress) {

        QRect rect = option.rect;
        rect = QRect(rect.right() - 30, rect.y() + 10, 20, 20);

        auto * e = (QMouseEvent*)event;
        if(rect.contains(e->pos()))
        {
            QString id = index.data(EM_ITEM_DATA_XMPPID).toString();
            emit removeItem(id);
        }
    }
    return QStyledItemDelegate::editorEvent(event, model, option, index);
}

