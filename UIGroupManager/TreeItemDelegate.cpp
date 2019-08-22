//
// Created by cc on 18-11-13.
//

#include "TreeItemDelegate.h"
#include "../CustomUi/HeadPhotoLab.h"
#include "../UICom/qimage/qimage.h"
#include "../UICom/StyleDefine.h"
#include <QTreeView>
#include <iostream>
#include <QApplication>
#include <QMouseEvent>
#include <QAbstractItemModel>

TreeItemDelegate::TreeItemDelegate(QWidget *parent)
    : QStyledItemDelegate(parent), _parentWgt(parent)
{

}

TreeItemDelegate::~TreeItemDelegate()
= default;

void TreeItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyledItemDelegate::paint(painter, option, index);
    painter->save();
    painter->setRenderHint(QPainter::TextAntialiasing);
    painter->fillRect(option.rect, QTalk::StyleDefine::instance().getGroupManagerNormalColor());
    ///
    int rowType =  index.data(EM_STAFF_DATATYPE_ROW_TYPE).toInt();

    int radius = 15;
    if(rowType == EM_ROW_TYPE_SUBTITLE)
        radius = 11;
    else if(rowType == EM_ROW_TYPE_ITEM)
        radius = 10;

    QRect rect = option.rect;

    QString strText = index.data(EM_STAFF_DATATYPE_TEXT).toString();
    QString iconPath = index.data(EM_STAFF_DATATYPE_ICONPATH).toString();

    QPixmap icon = QTalk::qimage::instance().loadCirclePixmap(iconPath, radius * QTalk::qimage::instance().dpi());
    painter->setRenderHints(QPainter::Antialiasing, true);
    painter->setRenderHints(QPainter::SmoothPixmapTransform, true);
    painter->drawPixmap(QRect(rect.x(), rect.y() + (rect.height() - radius * 2) / 2, radius * 2, radius * 2), icon);
    painter->setPen(QTalk::StyleDefine::instance().getGroupManagerNormalFontColor());
    painter->drawText(QRect(rect.x() + radius * 2 + 2, rect.y(),
            rect.width() - radius * 2 - 2 - 40, rect.height()), Qt::AlignVCenter, strText);

    QString picPath;
    QRect pixRect = QRect(rect.right() - 40, rect.y() + (rect.height() - radius * 2) / 2, 20, 20);;
    if(rowType == EM_ROW_TYPE_ITEM)
    {
        bool isChecked = index.data(EM_DATATYPE_CHECKSTATE).toBool();
        picPath = isChecked ? ":/GroupManager/image1/checkbox_checked.png"
                            : ":/GroupManager/image1/checkbox_unchecked.png";
    }
    else
    {
        bool isChecked = index.data(EM_STAFF_DATATYPE_EXTEND).toBool();
        picPath = isChecked ? ":/GroupManager/image1/collapse.png"
                            : ":/GroupManager/image1/extend.png";
    }
    QPixmap checkIcon = QTalk::qimage::instance().loadCirclePixmap(picPath,
            20 * QTalk::qimage::instance().dpi());
    painter->drawPixmap(pixRect, checkIcon);

    painter->restore();
}

QSize TreeItemDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    const QSize& size = QStyledItemDelegate::sizeHint(option, index);

    int rowType = index.data(EM_STAFF_DATATYPE_ROW_TYPE).toInt();

    if(rowType == EM_ROW_TYPE_TITLE)
        return {size.width(), 60};
    else
        return {size.width(), 40};
}

bool TreeItemDelegate::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option,
                                   const QModelIndex &index)
{
    if(event->type() == QEvent::MouseButtonPress)
    {
        int rowType = index.data(EM_STAFF_DATATYPE_ROW_TYPE).toInt();
        if(EM_ROW_TYPE_ITEM == rowType)
        {
            bool isChecked = model->data(index, EM_DATATYPE_CHECKSTATE).toBool();
            model->setData(index, !isChecked, EM_DATATYPE_CHECKSTATE);
            emit itemChanged(index);
        }
        else
        {
            bool extend = model->data(index, EM_STAFF_DATATYPE_EXTEND).toBool();
            model->setData(index, !extend, EM_STAFF_DATATYPE_EXTEND);
            emit itemChanged(index);
        }
    }
    else if(event->type() == QEvent::MouseButtonDblClick)
    {
        emit sgItemDbClicked(index);
    }
    return QStyledItemDelegate::editorEvent(event, model, option, index);
}
