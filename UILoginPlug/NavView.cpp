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

int w = 0;
NavItemDelegate::NavItemDelegate()
{
    QFont detailFont;
    detailFont.setPixelSize(13);
    w = QFontMetricsF(detailFont).width(tr("查看详情"));
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
    painter->setPen(QColor(219,219,219));
    painter->fillRect(rect, QTalk::StyleDefine::instance().getDropNormalColor());
    painter->drawRoundedRect(QRect(rect.x(), rect.y() + 7, rect.width(), rect.height() - 14), 7, 7);
    //
    QString text = index.data(ITEM_DATA_NAME).toString();
    QString link = index.data(ITEM_DATA_Link).toString();
    bool isSelected = index.data(ITEM_DATA_CHECKED).toBool();
    //
    QFont nameFont;
    nameFont.setPixelSize(18);
    pen.setColor(QTalk::StyleDefine::instance().getDropNormalFontColor());
    painter->setPen(pen);
    painter->setFont(nameFont);
    QRect nameRect(rect.x() + 50, rect.y() + 20, rect.width() - 100, 30);
    painter->drawText(nameRect, Qt::AlignTop, text);

    QFont textFont;
    textFont.setPixelSize(15);
    pen.setColor(QColor(153,153,153));
    painter->setPen(pen);
    painter->setFont(textFont);
    link = QFontMetricsF(textFont).elidedText(link, Qt::ElideRight, rect.width() - 210);
    QRect textRect(rect.x() + 50, rect.bottom() - 50, rect.width() - 200, 30);
    painter->drawText(textRect, Qt::AlignBottom, link);
    //
    QFont detailFont;
    detailFont.setPixelSize(13);
    pen.setColor(QColor(0,202,190));
    painter->setPen(pen);
    painter->setFont(detailFont);
    QRectF detailRect((int)rect.right() - 30 - w, (int)rect.y(), w, (int)rect.height());
    painter->drawText(detailRect, Qt::AlignVCenter, tr("查看详情"));

    QRect checkRect = {rect.left() + 20, rect.y() + 20, 20, 20};
    if(isSelected)
        painter->drawPixmap(checkRect, QPixmap(":/login/image1/checkbox_checked.png"));
    else
        painter->drawPixmap(checkRect, QPixmap(":/login/image1/checkbox_unchecked.png"));
    //
    painter->restore();
}

QSize NavItemDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const {
    QSize size = QStyledItemDelegate::sizeHint(option, index);
    return {size.width(), 86};
}

bool NavItemDelegate::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option,
                                  const QModelIndex &index) {
    if(event->type() == QEvent::MouseButtonPress)
    {
        QString name = index.data(ITEM_DATA_NAME).toString();
        auto pos = ((QMouseEvent*)event)->pos();
        QRect rect = option.rect;
        QRectF detailRect((int)rect.right() - 30 - w, (int)rect.y(), w, (int)rect.height());
        if(detailRect.contains(pos))
        {
            emit showDetail(name);
        }
        else
        {
//            static QString _name;
//            if(_name != name)
            {
//                _name = name;
                emit itemClicked(name);
            }
        }
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

    //
    auto* backBtn = new QPushButton(tr("返回"), this);
    backBtn->setObjectName("BackBtn");
    auto* topLay = new QHBoxLayout;
    topLay->setMargin(20);
    topLay->addWidget(backBtn);
    topLay->addItem(new QSpacerItem(10, 10, QSizePolicy::Expanding));
    //
    _pQRCodeLabel = new QLabel(this);
    _pQRCodeLabel->setObjectName("QRCodeLabel");
    _pNameEdit = new QLineEdit(this);
    _pHostEdit = new QLineEdit(this);
    _pAddressEdit = new QTextEdit(this);
    _pAddressEdit->setAcceptRichText(false);
    //
    auto* nameLabel = new QLabel(tr("名称"));
    auto* hostLabel = new QLabel(tr("域名"));
    auto* addrLabel = new QLabel(tr("导航地址"));
    //
    _pDeleteBtn = new QPushButton(tr("删除此导航"));
    _pDeleteBtn->setObjectName("DeleteBtn");
    //
    _pHostEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    _pNameEdit->setReadOnly(true);
    _pHostEdit->setReadOnly(true);
    _pAddressEdit->setFixedHeight(60);
    _pAddressEdit->installEventFilter(this);

    auto* layout = new QGridLayout;
    layout->setContentsMargins(0, 0, 40, 40);
    layout->setHorizontalSpacing(15);
    layout->setVerticalSpacing(15);
    layout->addWidget(_pQRCodeLabel, 0, 1, Qt::AlignLeft);
    layout->addWidget(nameLabel, 1, 0, Qt::AlignRight);
    layout->addWidget(_pNameEdit, 1, 1, 1, 2, Qt::AlignLeft);
    layout->addWidget(hostLabel, 2, 0, Qt::AlignRight);
    layout->addWidget(_pHostEdit, 2, 1, 1, 2, Qt::AlignLeft);
    layout->addWidget(addrLabel, 3, 0, Qt::AlignRight| Qt::AlignTop);
    layout->addWidget(_pAddressEdit, 3, 1, 1, 2, Qt::AlignLeft | Qt::AlignTop);

    layout->addWidget(_pDeleteBtn, 5, 2);
    //
    auto* lay = new QVBoxLayout(this);
    lay->setContentsMargins(0, 0, 0, 10);
    lay->addLayout(topLay);
    lay->addLayout(layout);
    //
    QImage qrCode = QZXing::encodeData(nav.url,
            QZXing::EncoderFormat_QR_CODE, {128, 128});

    _pQRCodeLabel->setPixmap(QPixmap::fromImage(qrCode));
    _pNameEdit->setText(nav.name);
    _pHostEdit->setText(nav.domain);
    _pAddressEdit->setText(nav.url);

    connect(_pDeleteBtn, &QPushButton::clicked, [this]()
    {
        emit sgBack();
        emit deleteSignal(_nav.name);
    });

    connect(backBtn, &QPushButton::clicked, this, &NavMainView::sgBack);
}

NavMainView::~NavMainView()
{

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
    _itemView = new QListView;
    _itemModel = new QStandardItemModel;
    _itemDelegate = new NavItemDelegate;
    _itemView->setModel(_itemModel);
    _itemView->setItemDelegate(_itemDelegate);
    //
    _itemView->setFrameShape(QFrame::NoFrame);
    _itemView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    //
    _addBtn = new QPushButton(tr("添加"), this);
    _addBtn->setObjectName("NavView_addbtn");
    _addBtn->setFixedSize(128, 30);
    auto* label = new QLabel(tr("导航选择："), this);
    //
    _itemStackWgt = new QStackedWidget;
    //
    auto* topLay = new QHBoxLayout;
    topLay->setMargin(0);
    topLay->addWidget(label);
    topLay->addItem(new QSpacerItem(10, 10, QSizePolicy::Expanding));
    topLay->addWidget(_addBtn);
    //
    _mainFrm = new QFrame(this);
    auto* lay = new QVBoxLayout(_mainFrm);
    lay->setContentsMargins(24, 10, 24, 12);
    lay->addLayout(topLay);
    lay->addWidget(_itemView);
    //
    _pAddNavWnd = new AddNavWnd(this);
    //
    auto* mainLay = new QHBoxLayout(this);
    mainLay->setMargin(0);
    mainLay->setSpacing(0);
    mainLay->addWidget(_mainFrm);
    mainLay->addWidget(_itemStackWgt);
    _itemStackWgt->setVisible(false);

    connect(this, &NavView::addItemSignal, this, &NavView::addItem);
    connect(_itemDelegate, &NavItemDelegate::itemClicked, this, &NavView::onItemClicked);
    connect(_itemDelegate, &NavItemDelegate::showDetail, this, &NavView::onShowDetail);
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
    item->setData(stNav.url, ITEM_DATA_Link);
    _itemModel->appendRow(item);

    _mapListItem[stNav.name] = item;
    auto* view = new NavMainView(stNav);
    _itemStackWgt->addWidget(view);
    _mapMainView[stNav.name] = view;

//    connect(view, &NavMainView::selectSignal, this, &NavView::onSelectChange, Qt::QueuedConnection);
    connect(view, &NavMainView::deleteSignal, this, &NavView::onDeleteItem, Qt::QueuedConnection);
    connect(view, &NavMainView::navAddrChanged, this, &NavView::onNavAddrChanged, Qt::QueuedConnection);
    connect(view, &NavMainView::navDebugChanged, this, &NavView::onNavDebugChanged, Qt::QueuedConnection);
    connect(view, &NavMainView::sgBack, this, &NavView::onBack, Qt::QueuedConnection);

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
        _itemStackWgt->setCurrentWidget(_mapMainView[name]);

    onSelectChange(name);
    //
    emit sgClose();
}

/**
 *
 * @param name
 */
void NavView::onSelectChange(const QString& name)
{
    _defaultName = name;
    emit saveConfSignal();
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

void NavView::onShowDetail(const QString &name) {
    if(_mapMainView.contains(name) && _mapListItem.contains(name))
        _itemStackWgt->setCurrentWidget(_mapMainView[name]);

    _itemStackWgt->setVisible(true);
    _mainFrm->setVisible(false);
}

void NavView::onBack() {
    _itemStackWgt->setVisible(false);
    _mainFrm->setVisible(true);
}
