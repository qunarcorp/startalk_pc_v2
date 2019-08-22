//
// Created by cc on 18-12-17.
//

#include "ListItemView.h"
#include <QHBoxLayout>
#include <QPainter>
#include <QFile>
#include <QEvent>
#include <QtWidgets/QListView>
#include "AddressBookPanel.h"
#include "../UICom/qimage/qimage.h"
#include "../UICom/StyleDefine.h"

#define HEAD_WIDTH 22

/**/
using namespace QTalk;
ListItemDelegate::ListItemDelegate(QObject *parent )
    : QStyledItemDelegate(parent)
{

}

ListItemDelegate::~ListItemDelegate() {

}

QSize ListItemDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const {
    return {option.widget->width(), 40};
}

void ListItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const {
    QStyledItemDelegate::paint(painter, option, index);

    painter->save();
    painter->setRenderHint(QPainter::TextAntialiasing);

    if (option.state & QStyle::State_Selected)
    {
        painter->fillRect(option.rect, StyleDefine::instance().getNavSelectColor());
    }
    else
    {
        painter->fillRect(option.rect, StyleDefine::instance().getNavNormalColor());
    }

    QRect rect = option.rect;

    QString strText = index.data(EM_DATA_TYPE_TEXT).toString();
    painter->setPen(StyleDefine::instance().getAdrNameFontColor());
    painter->drawText(QRect(rect.x() + 65, rect.y(), rect.width() - 65, rect.height()), Qt::AlignVCenter, strText);
    QString headPath = index.data(EM_DATA_TYPE_ICON).toString();
    painter->setRenderHints(QPainter::Antialiasing, true);
    if(!QFile(headPath).isOpen())
    {
        int dpi = QTalk::qimage::instance().dpi();
        QPixmap pixmap = QTalk::qimage::instance().loadPixmap(headPath, true, true, HEAD_WIDTH * dpi, HEAD_WIDTH * dpi);
        QPainterPath path;
        QRect headRect(rect.x() + 30, rect.y() + 8, HEAD_WIDTH, HEAD_WIDTH);
        path.addEllipse(headRect);
        painter->setClipPath(path);
        painter->drawPixmap(headRect, pixmap);
    }
    painter->restore();
}

bool ListItemDelegate::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option,
                                   const QModelIndex &index) {

    if(event->type() == QEvent::MouseButtonPress)
    {
        QString id = model->data(index, EM_DATA_TYPE_ID).toString();
        QUInt32 type = model->data(index, EM_DATA_TYPE_TYPE).toUInt();
        emit itemClicked(id, type);
    }
    return QStyledItemDelegate::editorEvent(event, model, option, index);
}

/**/
ListItemView::ListItemView(AddressBookPanel* _panel, QWidget *parent)
    :QFrame(parent)
    , _pLstView(nullptr)
    , _pModel(nullptr)
    , _pDelegate(nullptr)
    , _mainPanel(_panel)
{
    setObjectName("ListItemView");

    _pLstView = new QListView(this);
    _pLstView->setObjectName("ListItemViewListView");
    _pModel = new QStandardItemModel(this);
    _pDelegate = new ListItemDelegate(this);

    _pLstView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    _pLstView->setFrameShape(QFrame::NoFrame);
    //_pLstView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    _pLstView->setModel(_pModel);
    _pLstView->setItemDelegate(_pDelegate);
    _pLstView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    _pLstView->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    _pLstView->setResizeMode(QListView::Adjust);

    auto* layout = new QHBoxLayout(this);
    layout->setMargin(0);
    layout->setSpacing(0);
    layout->addWidget(_pLstView);

    connect(_pDelegate, &ListItemDelegate::itemClicked, _mainPanel, &AddressBookPanel::onListItemClicked);
   // connect(this, &ListItemView::addItemSignal, this, &ListItemView::addItem);
  //  connect(this, &ListItemView::removeItemSignal, this, &ListItemView::removeItem);

}

ListItemView::~ListItemView()
= default;

void ListItemView::addItem(const QString &id, const QUInt8& type, const QString &icon, const QString& name)
{
    if(!_items.contains(id))
    {
        auto item = new QStandardItem;
        item->setData(id, EM_DATA_TYPE_ID);
        item->setData(icon, EM_DATA_TYPE_ICON);
        item->setData(name, EM_DATA_TYPE_TEXT);
        item->setData(type, EM_DATA_TYPE_TYPE);

        _pModel->appendRow(item);

        QMutexLocker locker(&_mutex);
        _items[id] = item;

        _pLstView->update();
    }
}

void ListItemView::removeItem(const QString &id)
{
    if(_items.contains(id))
    {
        //QMutexLocker locker(&_mutex);
        QStandardItem *item = _items[id];
        _pModel->removeRow(item->row());
        _items.remove(id);
        _pLstView->update();
    }
}

void ListItemView::resetHeight(int parentH, int FixedH)
{
    int itemH = _items.size() * 40;
    int maxH = parentH - FixedH;
    if(itemH > maxH)
        this->setFixedHeight(maxH);
    else
        this->setFixedHeight(itemH);
}

void ListItemView::clearSelection()
{
    if(_pLstView)
    {
        _pLstView->clearSelection();
    }
}
