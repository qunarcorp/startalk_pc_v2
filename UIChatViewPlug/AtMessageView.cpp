//
// Created by QITMAC000260 on 2018/11/26.
//

#include "AtMessageView.h"
#include <QPainter>
#include <QHBoxLayout>
#include <QEvent>
#include <QKeyEvent>
#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include "InputWgt.h"
#include "../CustomUi/HeadPhotoLab.h"
#include "../Platform/Platform.h"
#include "../QtUtil/Entity/JID.h"
#include "../UICom/qimage/qimage.h"

#include "../UICom/StyleDefine.h"

enum {
    ITEM_DATE_NAME = Qt::UserRole,
    ITEM_DATA_ICON,
    ITEM_DATA_USERID,
    ITEM_DATA_XMPPID,
    ITEM_DATA_SEARCHINDEX
};

//
int AtSortModel::filterCount = 0;
AtSortModel::AtSortModel(QObject *parent)
        : QSortFilterProxyModel(parent) {


}

AtSortModel::~AtSortModel() {

}

bool AtSortModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const {

    QModelIndex index = sourceModel()->index(source_row, 0, source_parent);

    bool ret = index.data(ITEM_DATE_NAME).toString().contains(filterRegExp()) ||
              index.data(ITEM_DATA_SEARCHINDEX).toString().contains(filterRegExp()) ||
               index.data(ITEM_DATA_USERID).toString().contains(filterRegExp());

    if(ret)
    {
        filterCount++;
    }

    return ret;
}


//
AtItemDelegate::AtItemDelegate(QObject *parent)
        : QStyledItemDelegate(parent) {

}

AtItemDelegate::~AtItemDelegate()
{

}

void AtItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const {
    QStyledItemDelegate::paint(painter, option, index);

    painter->save();
    painter->setRenderHint(QPainter::TextAntialiasing);

    if (option.state & QStyle::State_Selected)
    {
        painter->fillRect(option.rect, QTalk::StyleDefine::instance().getNavSelectColor());
    }
    else
    {
        painter->fillRect(option.rect, QTalk::StyleDefine::instance().getNavNormalColor());
    }

	QString headPath = index.data(ITEM_DATA_ICON).toString();
	QRect rect = option.rect;
	painter->setPen(QTalk::StyleDefine::instance().getNavNameFontColor());
    painter->drawText(QRect(rect.x() + 35, rect.y(), rect.width() - 35, rect.height()), Qt::AlignLeft | Qt::AlignVCenter,
            index.data(ITEM_DATE_NAME).toString());

    QFileInfo headInfo(headPath);
    if(!headInfo.exists() || headInfo.isDir())
    {
#ifdef _STARTALK
        headPath = ":/QTalk/image1/StarTalk_defaultHead.png";
#else
        headPath = ":/QTalk/image1/headPortrait.png";
#endif
    }
	if (!QFile(headPath).isOpen())
	{
        int dpi = QTalk::qimage::instance().dpi();
		QPixmap pixmap = QTalk::qimage::instance().loadPixmap(headPath, true, true, 24 * dpi);
		QPainterPath path;
		QRect headRect(rect.x() + 5, rect.y() + 3, 24, 24);
		path.addEllipse(headRect);
		painter->setClipPath(path);
		painter->drawPixmap(headRect, pixmap);
	}
    painter->restore();

}

QSize AtItemDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const {

    const QSize& size = QStyledItemDelegate::sizeHint(option, index);
    return QSize(size.width(), 30);
}

bool AtItemDelegate::editorEvent(QEvent *e, QAbstractItemModel *model,
	const QStyleOptionViewItem &option, const QModelIndex &index)
{
	if (e->type() == QEvent::MouseButtonPress)
	{
		QString name = index.data(ITEM_DATE_NAME).toString();
		QString xmppId = index.data(ITEM_DATA_XMPPID).toString();
		emit itemClicked(xmppId, name);
	}
	return QStyledItemDelegate::editorEvent(e, model, option, index);
}


AtMessageView::AtMessageView(InputWgt *inputWgt)
        : QFrame(inputWgt), _pInputWgt(inputWgt) {

    Qt::WindowFlags flags = Qt::Tool | Qt::WindowContextHelpButtonHint | Qt::FramelessWindowHint
                            | Qt::WindowFullscreenButtonHint | Qt::WindowCloseButtonHint | Qt::WindowTitleHint| Qt::X11BypassWindowManagerHint | Qt::WindowStaysOnTopHint;
    setWindowFlags(flags);

	this->setAttribute(Qt::WA_ShowWithoutActivating, true);
	this->setAttribute(Qt::WA_X11DoNotAcceptFocus, true);
//    this->setAttribute(Qt::WA_TranslucentBackground, true);

    setFixedWidth(250);
    this->setObjectName("AtMessageView");
    _atView = new QListView;
    _atView->setFrameShape(QFrame::NoFrame);
    _atView->setObjectName("AtMessageViewListView");
    _atView->setEditTriggers(QAbstractItemView::NoEditTriggers);
	_atView->installEventFilter(this);
    _atView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    _atSrcModel = new QStandardItemModel;
    _atModel = new AtSortModel();
    _atItemDelegate = new AtItemDelegate;
    _atModel->setSourceModel(_atSrcModel);
    _atModel->setFilterRole(ITEM_DATE_NAME);
    _atView->setModel(_atModel);
    _atView->setItemDelegate(_atItemDelegate);
    _atView->setContentsMargins(10, 10, 10, 10);
    //
//    QFrame* mainFrm = new QFrame(this);
//    mainFrm->setObjectName("AtViewMainFrm");
//    auto * mainLay = new QHBoxLayout(mainFrm);
//    mainLay->setMargin(0);
//    mainLay->addWidget(_atView);
    //
    auto * layout = new QHBoxLayout(this);
    layout->setMargin(0);
    layout->addWidget(_atView);

	connect(_atItemDelegate, &AtItemDelegate::itemClicked, this, &AtMessageView::confirmAtUser);
}

AtMessageView::~AtMessageView()
{

}

void AtMessageView::addItem(const QString &icon, const QString& xmppId, const QString &name, const QString& searchIndex) {

    if(_items.contains(xmppId))
    {
        auto *item = _items[xmppId];
        item->setData(name, ITEM_DATE_NAME);
        item->setData(QString::fromStdString(QTalk::Entity::JID(xmppId.toStdString().c_str()).username()), ITEM_DATA_USERID);
        item->setData(xmppId, ITEM_DATA_XMPPID);
        item->setData(searchIndex, ITEM_DATA_SEARCHINDEX);
        QString headSrc = QString::fromStdString(QTalk::GetHeadPathByUrl(icon.toStdString()));
        item->setData(headSrc, ITEM_DATA_ICON);
        return;
    }
    else
    {
        auto * item = new QStandardItem;
        item->setData(name, ITEM_DATE_NAME);
        item->setData(QString::fromStdString(QTalk::Entity::JID(xmppId.toStdString().c_str()).username()), ITEM_DATA_USERID);
        item->setData(xmppId, ITEM_DATA_XMPPID);
        item->setData(searchIndex, ITEM_DATA_SEARCHINDEX);

        QString headSrc = QString::fromStdString(QTalk::GetHeadPathByUrl(icon.toStdString()));

        item->setData(headSrc, ITEM_DATA_ICON);
        _items[xmppId] = item;
        _atSrcModel->appendRow(item);
    }
}

bool AtMessageView::eventFilter(QObject *o, QEvent *e) {

	if(e->type() == QEvent::KeyPress)
	{
		auto* keyEvt = static_cast<QKeyEvent*>(e);

		if(nullptr != keyEvt)
		{
			if(keyEvt == QKeySequence::Cut)
			{
				this->setVisible(false);
			}

			switch (keyEvt->key())
			{
				case Qt::Key_Enter:
				case Qt::Key_Return:
				{
					QModelIndex index = _atView->currentIndex();
					QString name = index.data(ITEM_DATE_NAME).toString();
					QString xmppId = index.data(ITEM_DATA_XMPPID).toString();
					confirmAtUser(xmppId, name);
					return false;
				}
				case Qt::Key_Up:
				{
					QModelIndex index = _atView->currentIndex();
					if(index.row() != 0)
						_atView->setCurrentIndex(_atModel->index(index.row() - 1, 0));
					return false;
				}
				case Qt::Key_Down:
				{
					QModelIndex index = _atView->currentIndex();
					if (index.row() != _atModel->rowCount() - 1)
						_atView->setCurrentIndex(_atModel->index(index.row() + 1, 0));
					return false;
				}
				default:
				{
					break;
				}
			}

			return true;
		}
	}
	else if (e->type() == QEvent::Show)
    {
	    if(!_matched)
        {
            _pos = _pInputWgt->textCursor().position() - 1;
            _atModel->setFilterRegExp("");
        } else
            _matched = false;
		_atView->setCurrentIndex(_atModel->index(0, 0));
    }

    return QWidget::eventFilter(o, e);
}

void AtMessageView::updateFilter() {
    if(_pInputWgt->textCursor().position() <= _pos)
    {
        this->setVisible(false);
    }
//    _count += changedSize;
    auto now_pos = _pInputWgt->textCursor().position();
    QString text = _pInputWgt->toPlainText().mid(_pos + 1, now_pos - 1);

    _atModel->filterCount = 0;
    _atModel->setFilterRegExp(text);
    if(_atModel->filterCount == 0)
    {
        this->setVisible(false);
    }
}

void AtMessageView::confirmAtUser(const QString& xmppId, const QString& name) {

    QModelIndex index = _atView->currentIndex();
    this->setVisible(false);
    _pInputWgt->setFocus();
    _pInputWgt->insertAt(_pos, name, xmppId);
}

/**
 *
 * @param memberId
 */
void AtMessageView::deleteItem(const QString &memberId)
{
    QMutexLocker locker(&_mutex);
    if(_items.contains(memberId))
    {
        QStandardItem* item = _items[memberId];
        _atSrcModel->removeRow(item->row());
        _items.remove(memberId);
    }
}

/**
 *
 * @param member
 */
void AtMessageView::updateGroupMemberInfo(const std::vector<QTalk::StUserCard> &members)
{
    for(const auto& it : members)
    {
        QString xmppid = QString::fromStdString(it.xmppId);

        std::string name = it.nickName;
        if(name.empty())
            name = it.userName;
        if(name.empty())
            name = QTalk::Entity::JID(it.xmppId.data()).username();

        if(_items.contains(xmppid))
        {
            auto *item = _items[xmppid];
            item->setData(QString::fromStdString(name), ITEM_DATE_NAME);
            QString headSrc = QString::fromStdString(QTalk::GetHeadPathByUrl(it.headerSrc));
            item->setData(headSrc, ITEM_DATA_ICON);
        }
    }
}

bool AtMessageView::match(const QString &str) {
    _atModel->filterCount = 0;
    _atModel->setFilterRegExp(str);
    if(_atModel->filterCount == 0)
    {
        _atModel->setFilterRegExp("");
        _matched = false;
        return false;
    }
    else
    {
        _matched = true;
        _pos = _pInputWgt->textCursor().position() - str.size() - 1;
        return true;
    }
}

//
///**
//  * @函数名
//  * @功能描述
//  * @参数
//     void
//  * @author   cc
//  * @date     2018/10/17
//  */
//void AtMessageView::focusOutEvent(QFocusEvent *e) {
//    this->setVisible(false);
//    return QWidget::focusOutEvent(e);
//}
//
///**
//  * @函数名
//  * @功能描述
//  * @参数
//     void
//  * @author   cc
//  * @date     2018/10/17
//  */
//void AtMessageView::mousePressEvent(QMouseEvent *e) {
//    QPoint pos = e->pos();
//    if (!realContentsRect().contains(pos)) {
//        this->setVisible(false);
//    }
//    return QDialog::mousePressEvent(e);
//}

