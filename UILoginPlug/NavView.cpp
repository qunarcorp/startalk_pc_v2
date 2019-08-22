//
// Created by cc on 2018-12-26.
//

#include "NavView.h"
#include <QSplitter>
#include <QHBoxLayout>
#include <QPainter>
#include <QModelIndex>
#include <QGridLayout>
#include <QEvent>
#include <QtGui/QList>
#include <QUrlQuery>
#include <QDesktopServices>

#include "../qzxing/QZXing.h"
#include "../UICom/StyleDefine.h"

NavItemDelegate::NavItemDelegate()
{

}

NavItemDelegate::~NavItemDelegate()
{

}

void NavItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyledItemDelegate::paint(painter, option, index);
    painter->save();
    // 背景色
    QRect rect = option.rect;
    QPen pen;
    if (option.state & QStyle::State_Selected)
    {
        painter->fillRect(rect, QTalk::StyleDefine::instance().getDropSelectColor());
        painter->fillRect(QRect(rect.x(), rect.y(), 5, rect.height()), QColor(0, 202, 190));
        pen.setColor(QTalk::StyleDefine::instance().getDropSelectFontColor());
    }
    else
    {
        painter->fillRect(rect, QTalk::StyleDefine::instance().getDropNormalColor());
        pen.setColor(QTalk::StyleDefine::instance().getDropNormalFontColor());
    }
    //
    QString text = index.data(ITEM_DATA_NAME).toString();
    bool    isSelected = index.data(ITEM_DATA_CHECKED).toBool();
    //
    painter->setPen(pen);
    QRect textRect(rect.x() + 30, rect.y(), rect.width(), rect.height());
    painter->drawText(textRect, Qt::AlignVCenter, index.data(ITEM_DATA_NAME).toString());

    if(isSelected)
    {
        painter->drawPixmap(rect.right() - 30, rect.y() + 15, 20, 20, QPixmap(":/login/image1/checkbox_checked.png"));
    }
    //
    painter->restore();
}

QSize NavItemDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const {
    QSize size = QStyledItemDelegate::sizeHint(option, index);
    return {size.width(), 50};
}

bool NavItemDelegate::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option,
                                  const QModelIndex &index) {
    if(event->type() == QEvent::MouseButtonPress)
    {
        QString name = index.data(ITEM_DATA_NAME).toString();
        emit itemClicked(name);
    }
    return QStyledItemDelegate::editorEvent(event, model, option, index);
}

/**
 *
 */
NavMainView::NavMainView(const StNav& nav, QWidget* parnet)
    :QFrame(parnet), _nav(nav)
{
    setObjectName("NavMainView");

    _pQRCodeLabel = new QLabel(this);
    _pQRCodeLabel->setObjectName("QRCodeLabel");
    _pNameEdit = new QLineEdit(this);
    _pHostEdit = new QLineEdit(this);
    _pAddressEdit = new QTextEdit(this);
    _pAddressEdit->setAcceptRichText(false);
    //
    QLabel* nameLabel = new QLabel("名称");
    QLabel* hostLabel = new QLabel("域名");
    QLabel* addrLabel = new QLabel("导航地址");
    //
    _pNavDetail = new LinkButton("导航详情");
    //
    _pChoseBtn = new QPushButton("选择");
    _pDeleteBtn = new QPushButton("删除");
    _pDeleteBtn->setObjectName("DeleteBtn");
    _pChoseBtn->setObjectName("ChoseBtn");
    _pChoseBtn->setFixedHeight(30);
//    _pChoseBtn->setVisible(false);
    //
    _pNameEdit->setReadOnly(true);
    _pHostEdit->setReadOnly(true);
    _pAddressEdit->setFixedHeight(60);
    _pDeleteBtn->setFixedSize(70, 30);
    _pAddressEdit->installEventFilter(this);

    auto* layout = new QGridLayout(this);
    layout->setContentsMargins(0, 40, 40, 0);
    layout->setHorizontalSpacing(15);
    layout->setVerticalSpacing(15);
    layout->addWidget(_pQRCodeLabel, 0, 1, Qt::AlignLeft);
    layout->addWidget(nameLabel, 1, 0, Qt::AlignRight);
    layout->addWidget(_pNameEdit, 1, 1, 1, 2, Qt::AlignLeft);
    layout->addWidget(hostLabel, 2, 0, Qt::AlignRight);
    layout->addWidget(_pHostEdit, 2, 1, 1, 2, Qt::AlignLeft);
    layout->addWidget(addrLabel, 3, 0, Qt::AlignRight| Qt::AlignTop);
    layout->addWidget(_pAddressEdit, 3, 1, 1, 2, Qt::AlignLeft | Qt::AlignTop);

    layout->addWidget(_pNavDetail, 4, 1);

    layout->addWidget(_pChoseBtn, 5, 1);
    layout->addWidget(_pDeleteBtn, 5, 2);
    //
    QImage qrCode = QZXing::encodeData(nav.url,
            QZXing::EncoderFormat_QR_CODE, {128, 128});

    _pQRCodeLabel->setPixmap(QPixmap::fromImage(qrCode));
    _pNameEdit->setText(nav.name);
    _pHostEdit->setText(nav.domain);
    _pAddressEdit->setText(nav.url);

    connect(_pChoseBtn, &QPushButton::clicked, [this]()
    {
        emit selectSignal(_nav.name);
    });
    connect(_pDeleteBtn, &QPushButton::clicked, [this]()
    {
        emit deleteSignal(_nav.name);
    });
    connect(_pNavDetail, &LinkButton::clicked, [this]()
    {
        QUrl url(_nav.url);
        QUrlQuery query;
        query.addQueryItem("p", "pc");
        url.setQuery(query);

        QDesktopServices::openUrl(url);
    });
    //emit navDebugChanged(_nav.name, _nav.debug);
}

NavMainView::~NavMainView()
{

}

void NavMainView::setSelectState(bool selected)
{
    _pChoseBtn->setText(selected ? "已选择" : "选择");
    _pChoseBtn->setEnabled(!selected);
    _pDeleteBtn->setEnabled(!selected);
}

/**
 *
 */
bool NavMainView::eventFilter(QObject *o, QEvent *e)
{
    if(o == _pAddressEdit && e->type() == QEvent::FocusOut)
    {
        QString url = _pAddressEdit->toPlainText().trimmed();
        if(_nav.url != url)
        {
            emit navAddrChanged(_nav.name, url);
        }
        else
        {
            _pAddressEdit->setText(url);
        }
    }
    return QObject::eventFilter(o, e);
}

/**
 *
 * @param wgt
 */
NavView::NavView(QMap<QString, StNav>& mapNav, QString& defalutNav, QWidget *wgt)
    :QFrame(wgt), _defaultName(defalutNav), _mapNav(mapNav)
{
    //
    QFrame* leftFrm = new QFrame(this);
    auto* leftLay = new QVBoxLayout(leftFrm);
    leftLay->setMargin(0);
    //
    _itemView = new QListView;
    _itemModel = new QStandardItemModel;
    _itemDelegate = new NavItemDelegate;
    _itemView->setModel(_itemModel);
    _itemView->setItemDelegate(_itemDelegate);
    //
    _itemView->setFrameShape(QFrame::NoFrame);
    _itemView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    //
    _addBtn = new QPushButton("添加");
    _addBtn->setObjectName("NavView_addbtn");
    _addBtn->setFixedSize(128, 30);
    leftLay->addWidget(_itemView);
    leftLay->addWidget(_addBtn);
    leftLay->setAlignment(_addBtn, Qt::AlignHCenter);
    //
    _itemStackWgt = new QStackedWidget;
    _itemStackWgt->setMinimumWidth(400);
    //
    QSplitter* mainSplitter = new QSplitter();
    mainSplitter->setHandleWidth(1);
    mainSplitter->addWidget(leftFrm);
    mainSplitter->addWidget(_itemStackWgt);
    mainSplitter->setStretchFactor(0, 176);
    mainSplitter->setStretchFactor(1, 451);
    mainSplitter->setCollapsible(0, false);
    mainSplitter->setCollapsible(1, false);

    auto* lay = new QHBoxLayout(this);
    lay->setContentsMargins(0, 1, 0, 17);
    lay->addWidget(mainSplitter);
    //
    _pAddNavWnd = new AddNavWnd(this);

    connect(this, &NavView::addItemSignal, this, &NavView::addItem);
    connect(_itemDelegate, &NavItemDelegate::itemClicked, this, &NavView::onItemClicked);
    connect(_addBtn, &QPushButton::clicked, [this](){
        _pAddNavWnd->resetWnd();
        _pAddNavWnd->showCenter(true, this);
    });

    connect(_pAddNavWnd, &AddNavWnd::addNavSinal, this, &NavView::addNavSinal);
}

NavView::~NavView()
{

}

/**
 * 添加item
 * @param stNav
 */
void NavView::addItem(const StNav &stNav)
{
    auto* item = new QStandardItem;
    item->setData(stNav.name, ITEM_DATA_NAME);
    item->setData(stNav.name == _defaultName, ITEM_DATA_CHECKED);
    _itemModel->appendRow(item);

    _mapListItem[stNav.name] = item;
    auto* view = new NavMainView(stNav);
    view->setSelectState(stNav.name == _defaultName);
    _itemStackWgt->addWidget(view);
    _mapMainView[stNav.name] = view;

    connect(view, &NavMainView::selectSignal, this, &NavView::onSelectChange, Qt::QueuedConnection);
    connect(view, &NavMainView::deleteSignal, this, &NavView::onDeleteItem, Qt::QueuedConnection);
    connect(view, &NavMainView::navAddrChanged, this, &NavView::onNavAddrChanged, Qt::QueuedConnection);
    connect(view, &NavMainView::navDebugChanged, this, &NavView::onNavDebugChanged, Qt::QueuedConnection);

//    if(_itemModel->rowCount() == 1)
    {
        _itemView->setCurrentIndex(item->index());
        _itemStackWgt->setCurrentWidget(view);
    }
}

/**
 *
 * @param name
 */
void NavView::onItemClicked(const QString& name)
{
    if(_mapMainView.contains(name) && _mapListItem.contains(name))
    {
        _itemStackWgt->setCurrentWidget(_mapMainView[name]);
        //
        QModelIndex index = _mapListItem[name]->index();
        _itemView->setCurrentIndex(index);
    }
}

/**
 *
 * @param name
 */
void NavView::onSelectChange(const QString& name)
{
    _defaultName = name;
    emit saveConfSignal();

    for (NavMainView* view : _mapMainView.values())
    {
        view->setSelectState(view->_nav.name == _defaultName);
    }
    //
    for (QStandardItem* item : _mapListItem.values()) {
        QString itemName = item->data(ITEM_DATA_NAME).toString();
        item->setData(itemName == _defaultName, ITEM_DATA_CHECKED);
    }
}

/**
 * 删除item 都在主线程操作 不加锁
 * @param name
 */
void NavView::onDeleteItem(const QString& name)
{
    //
    if(_mapMainView.contains(name))
    {
        NavMainView* view = _mapMainView[name];
        delete view;
        _mapMainView.remove(name);
    }
    if(_mapListItem.contains(name))
    {
        QStandardItem* item = _mapListItem[name];
        _itemModel->removeRow(item->row());
        _mapListItem.remove(name);
        _itemView->update();
    }
    //
    _mapNav.remove(name);
    // 选择
    QModelIndex index = _itemModel->index(0, 0);
    QString itemName = index.data(ITEM_DATA_NAME).toString();
    _itemView->setCurrentIndex(index);
    _itemStackWgt->setCurrentWidget(_mapMainView.value(itemName));
    //
    emit saveConfSignal();
}

/**
 * 导航地址被修改
 * @param name
 * @param addr
 */
void NavView::onNavAddrChanged(const QString& name, const QString& addr)
{
    if(_mapNav.contains(name))
    {
        _mapNav[name].url = addr;
        QUrl qUrl(addr);
        QUrlQuery query(qUrl.query());
        if(query.hasQueryItem("debug"))
            _mapNav[name].debug = query.queryItemValue("debug") == "true";
        else
            _mapNav[name].debug = false;
    }
    //
    emit saveConfSignal();
}

void NavView::onNavDebugChanged(const QString &name, bool debug)
{
    if(_mapNav.contains(name))
    {
        _mapNav[name].debug = debug;
    }
    //
    emit saveConfSignal();
}

/**
 *
 * @param name
 * @return
 */
bool NavView::checkName(const QString& name)
{
    bool ret = _mapNav.contains(name);
    return ret;
}