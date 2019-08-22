#include "LocalEmoticon.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QListWidget>
#include <QStackedWidget>
#include <QTableWidget>
#include <QLabel>
#include <QMenu>
#include <QAction>
#include <QPushButton>
#include "EmoPreviewWgt.h"
#include "EmoticonMainWgt.h"
#include "EmoCellWidget.h"
#include "EmoLstItemWgt.h"

//***
LocalEmoticon::LocalEmoticon(QMap<QString, StEmoticon>& mapEmo, QWidget *parent)
	: QWidget(parent), _mapEmo(mapEmo), _pLocalEmoLst(nullptr), _pStackEmoView(nullptr), _pMenu(nullptr)
{
	initUi();
}

LocalEmoticon::~LocalEmoticon()
= default;

/**
  * @函数名   
  * @功能描述 
  * @参数
  * @author   cc
  * @date     2018/10/23
  */
void LocalEmoticon::addEmoticon(const StEmoticon& emoInfo)
{
	auto* emoLstItem = new QListWidgetItem(_pLocalEmoLst);
	auto* wgt = new EmoLstItemWgt(emoInfo.pkgid, emoInfo.iconPath, emoInfo.name);
	emoLstItem->setSizeHint(QSize(155, 50));
	_pLocalEmoLst->setItemWidget(emoLstItem, wgt);

	_mapEmoView[emoInfo.pkgid] = _pStackEmoView->addEmoticon(emoInfo.mapEmoticon, emoInfo.pkgid);
	_mapEmoItemWgt[emoLstItem] = wgt;
}

/**
  * @函数名   
  * @功能描述 
  * @参数
  * @author   cc
  * @date     2018/10/20
  */
void LocalEmoticon::initUi()
{
	// 
	_pLocalEmoLst = new QListWidget;
	_pStackEmoView = new EmoPreviewWgt(6);
	_pLocalEmoLst->setObjectName("LocalEmoLst");
	//
	_pLocalEmoLst->setContextMenuPolicy(Qt::CustomContextMenu);
	_pMenu = new QMenu(_pLocalEmoLst);
	_pMenu->setAttribute(Qt::WA_TranslucentBackground, true);
    QAction *act = new QAction(QStringLiteral("移除表情"), this);
	_pMenu->addAction(act);
	//
	auto* layout = new QHBoxLayout(this);
	layout->setMargin(0);
    layout->setSpacing(0);
	layout->addWidget(_pLocalEmoLst);
	layout->addWidget(_pStackEmoView);
	_pLocalEmoLst->setFixedWidth(180);
	setLayout(layout);
	//
	_pLocalEmoLst->setFrameShape(QFrame::NoFrame);
	_pLocalEmoLst->setFocusPolicy(Qt::NoFocus);
	_pLocalEmoLst->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

	connect(_pLocalEmoLst, &QListWidget::itemClicked, this, &LocalEmoticon::onLocalEmoLstClick);
	connect(act, &QAction::triggered, this, &LocalEmoticon::removeLocalEmo);
	connect(_pLocalEmoLst, &QListWidget::customContextMenuRequested, [this](const QPoint &pos)
	{
        auto *curItem = _pLocalEmoLst->currentItem();
        auto id = _mapEmoItemWgt[curItem]->getPkgId();
        if(DEM_COLLECTION == id)
            return;

		_pMenu->exec(mapToGlobal(pos));
	});
}

/**
  * @函数名   
  * @功能描述 
  * @参数
  * @author   cc
  * @date     2018/10/20
  */
void LocalEmoticon::onLocalEmoLstClick(QListWidgetItem* item)
{
	auto* wgt = (EmoLstItemWgt*)_pLocalEmoLst->itemWidget(item);
	if (wgt)
	{
        for (EmoLstItemWgt* w : _mapEmoItemWgt)
		{
			w->setCheckState(w == wgt);
		}
		QString pkgId = wgt->getPkgId();
		if (_mapEmoView.contains(pkgId) && nullptr != _mapEmoView[pkgId])
		{
			_pStackEmoView->setCurrentWidget(_mapEmoView[pkgId]);
		}
	}
}

/**
  * @函数名   removeLocalEmo
  * @功能描述 
  * @参数
  * @author   cc
  * @date     2018/10/21
  */
void LocalEmoticon::removeLocalEmo()
{
	if (nullptr != _pLocalEmoLst)
	{
		QListWidgetItem* curItem = _pLocalEmoLst->currentItem();
		auto* curWgt = (EmoLstItemWgt*)_pLocalEmoLst->itemWidget(curItem);
		if (nullptr != curWgt)
		{
			QString pkgId = curWgt->getPkgId();
			QString iconPath = curWgt->getIconPath();
			_pLocalEmoLst->removeItemWidget(curItem);
			_pStackEmoView->deleteEmoticon((QTableWidget*)_mapEmoView[pkgId]);

			delete curWgt;
			delete curItem;

			_pLocalEmoLst->setCurrentRow(0);
			_mapEmoItemWgt.remove(curItem);
			_mapEmoView.remove(pkgId);
			_mapEmo.remove(pkgId);

			if (!_mapEmoItemWgt.empty())
			{
				((EmoLstItemWgt*)_pLocalEmoLst->itemWidget(_pLocalEmoLst->item(0)))->setCheckState(true);
			}

			EmoticonMainWgt::getInstance()->removeLocalEmoticon(pkgId, iconPath);
		}
	}
}

void LocalEmoticon::initEmoticon()
{
    for(const auto &emo : _mapEmo)
    {
        addEmoticon(emo);
    }
}

void LocalEmoticon::initCollection(const StEmoticon& emo) {

    auto* emoLstItem = new QListWidgetItem(_pLocalEmoLst);
    auto* wgt = new EmoLstItemWgt(DEM_COLLECTION, emo.iconPath, emo.name);
    emoLstItem->setSizeHint(QSize(155, 50));
    _pLocalEmoLst->setItemWidget(emoLstItem, wgt);

    wgt->setCheckState(true);

    _mapEmoView[DEM_COLLECTION] = _pStackEmoView->addEmoticon(emo.mapEmoticon, emo.pkgid);
    _mapEmoItemWgt[emoLstItem] = wgt;

    // 右键菜单
    auto *menu = new QMenu(this);
    menu->setAttribute(Qt::WA_TranslucentBackground, true);
    auto *act = new QAction(QStringLiteral("移除"), menu);
    menu->addAction(act);
    _mapEmoView[DEM_COLLECTION]->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(_mapEmoView[DEM_COLLECTION], &QTableWidget::customContextMenuRequested, [menu, this](const QPoint &pos){
        menu->exec(QCursor::pos());
    });
    connect(act, &QAction::triggered, [this](){
        auto *tableWgt = _mapEmoView[DEM_COLLECTION];
        int row = tableWgt->currentRow(), column = tableWgt->currentColumn();
        auto* wgt = tableWgt->cellWidget(row, column);
        auto* cellWgt = qobject_cast<EmoCellWidget*>(wgt);
        if(cellWgt)
        {
            emit removeCollection(cellWgt->getShortCut());
        }
    });
}

void LocalEmoticon::updateCollection(const std::map<UnorderMapKey, std::string>& collection)
{
    _pStackEmoView->updateCollection(_mapEmoView[DEM_COLLECTION], collection);
}
