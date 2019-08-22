//
// Created by QITMAC000260 on 2019-06-20.
//

#include "SearchView.h"
#include "../UICom/qimage/qimage.h"
#include "../UICom/StyleDefine.h"
#include <QPainter>
#include <QEvent>
#include <QDebug>

SearchItemDelegate::SearchItemDelegate(QWidget *parent)
        : QStyledItemDelegate(parent)
{

}

SearchItemDelegate::~SearchItemDelegate()
= default;

void SearchItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyledItemDelegate::paint(painter, option, index);
    painter->save();
    painter->setRenderHint(QPainter::TextAntialiasing);
    painter->fillRect(option.rect, QTalk::StyleDefine::instance().getGroupManagerNormalColor());
    //
    int radius = 16;
    QRect rect = option.rect;

    QString strText = index.data(EM_SEARCH_DATATYPE_TEXT).toString();
    QString iconPath = index.data(EM_SEARCH_DATATYPE_ICONPATH).toString();
    QString staff = index.data(EM_SEARCH_DATATYPE_STAFF).toString();

    QPixmap icon = QTalk::qimage::instance().loadCirclePixmap(iconPath, radius * QTalk::qimage::instance().dpi());
    painter->setRenderHints(QPainter::Antialiasing, true);
    painter->setRenderHints(QPainter::SmoothPixmapTransform, true);
    painter->drawPixmap(QRect(rect.x(), rect.y() + (rect.height() - radius * 2) / 2, radius * 2, radius * 2), icon);
    painter->setPen(QTalk::StyleDefine::instance().getNavNameFontColor());
    painter->drawText(QRect(rect.x() + radius * 2 + 8, rect.y(),
                            rect.width() - radius * 2 - 2 - 30, rect.height() / 2), Qt::AlignBottom, strText);
    painter->setPen(QTalk::StyleDefine::instance().getNavContentFontColor());
    painter->drawText(QRect(rect.x() + radius * 2 + 8, rect.y() + rect.height() / 2 + 5,
                            rect.width() - radius * 2 - 8  - 50, rect.height() / 2), Qt::AlignTop, staff);

    QRect pixRect = QRect(rect.right() - 40, rect.y() + (rect.height() - 20) / 2, 20, 20);;
    bool isChecked = index.data(EM_SEARCH_DATATYPE_CHECKSTATE).toBool();
    QString picPath = isChecked ? ":/GroupManager/image1/checkbox_checked.png"
                            : ":/GroupManager/image1/checkbox_unchecked.png";

    QPixmap checkIcon = QTalk::qimage::instance().loadCirclePixmap(picPath,
                                                                   10 * QTalk::qimage::instance().dpi());
    painter->drawPixmap(pixRect, checkIcon);

    painter->restore();
}

QSize SearchItemDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    const QSize& size = QStyledItemDelegate::sizeHint(option, index);

    return {size.width(), 56};
}

bool SearchItemDelegate::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option,
                                   const QModelIndex &index)
{
    if(event->type() == QEvent::MouseButtonPress)
    {
        bool isChecked = model->data(index, EM_SEARCH_DATATYPE_CHECKSTATE).toBool();
        model->setData(index, !isChecked, EM_SEARCH_DATATYPE_CHECKSTATE);
        emit itemChanged(index);
    }
    else if(event->type() == QEvent::MouseButtonDblClick)
    {
        emit sgItemDbClicked(index);
    }
    return QStyledItemDelegate::editorEvent(event, model, option, index);
}


/*************/
SearchItemSortModel::SearchItemSortModel(QObject* parent)
    : QSortFilterProxyModel(parent)
{

}

bool SearchItemSortModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const {
    QModelIndex index = sourceModel()->index(source_row, 0, source_parent);

    auto exp = filterRegExp();
    bool ret = false;
    ret |= index.data(EM_SEARCH_DATATYPE_TEXT).toString().toLower().contains(exp);
    ret |= index.data(EM_SEARCH_DATATYPE_USERID).toString().toLower().contains(exp);
    ret |= index.data(EM_SEARCH_DATATYPE_INDEX).toString().toLower().contains(exp);
//    if(ret)
//    {
//        sourceModel()->setData(index, false, EM_SEARCH_DATATYPE_CHECKSTATE);
//    }

    return ret;
}
