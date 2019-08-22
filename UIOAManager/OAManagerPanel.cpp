#include "OAManagerPanel.h"
#include "../Platform/Platform.h"
#include "../include/Line.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <thread>

OAManagerPanel::OAManagerPanel()
        : QWidget() {
    //
    _pMsgManager = new MessageManager;
    //
    initUi();
    //
    getLayoutData();
}

OAManagerPanel::~OAManagerPanel() {
}

void OAManagerPanel::initUi() {
    setObjectName("OAManagerPanel");

    _pSplitter = new QSplitter(this);
    // left frame
    QFrame *leftFrame = new QFrame(this);
    leftFrame->setObjectName("OAManagerLeftFrm");
    leftFrame->setMinimumWidth(260);
    _pLeftLay = new QVBoxLayout(leftFrame);
    _pLeftLay->setContentsMargins(0, 10, 0, 0);
    _pLeftLay->setSpacing(0);

    // right frame
    QFrame *rightFrm = new QFrame(this);
    rightFrm->setObjectName("OAManagerRightFrm");
    _pRightLay = new QStackedLayout(rightFrm);
    _pRightLay->setSpacing(0);
    _pRightLay->setMargin(0);
    _pEmptyLabel = new QLabel(this);
    _pRightLay->addWidget(_pEmptyLabel);
    _pRightLay->addWidget(new Line());
    // main layout
    _pSplitter->addWidget(leftFrame);
    _pSplitter->addWidget(rightFrm);
    _pSplitter->setHandleWidth(1);
    _pSplitter->setStretchFactor(1, 1);
    _pSplitter->setCollapsible(0, false);
    _pSplitter->setCollapsible(1, false);
    auto *layout = new QHBoxLayout(this);
    layout->setMargin(0);
    layout->setSpacing(0);
    layout->addWidget(_pSplitter);

    connect(this, &OAManagerPanel::updateUiSignal, this, &OAManagerPanel::updateUi);
}

/**
  * @函数名   
  * @功能描述 
  * @参数
  * @author   cc
  * @date     2018/12/16
  */
void OAManagerPanel::onNavItemClicked(int id)
{
    for(OANavigationItem *item : _mapNavItems) {
        item->setSelectState(item->getId() == id);
    }
    if (_mapMainWgt.contains(id))
        _pRightLay->setCurrentWidget(_mapMainWgt[id]);
    else
        _pRightLay->setCurrentWidget(_pEmptyLabel);
}

/**
  * @函数名   
  * @功能描述 
  * @参数
  * @author   cc
  * @date     2018/12/16
  */
void OAManagerPanel::dealNavItem() {

    for(OANavigationItem *item : _mapNavItems) {
        _pLeftLay->addWidget(item);
        connect(item, &OANavigationItem::itemClicked, this, &OAManagerPanel::onNavItemClicked);
    }
}

/**
 *
 */
void OAManagerPanel::getLayoutData() {
    if (_pMsgManager) {
        std::thread t([this]() {
#ifdef _MACOS
            pthread_setname_np("OAManagerPanel::getLayoutData");
#endif
            bool ret = _pMsgManager->getOAUiData(_uidata);
            if (ret) {
                emit updateUiSignal();
            }
        });
        t.detach();
    }
}

/**
 *
 */
void OAManagerPanel::updateUi() {
    auto it = _uidata.cbegin();
    for (; it != _uidata.cend(); it++) {
        int id = it->groupId;
        QString name = QString::fromStdString(it->groupName);
        QString path = QString::fromStdString(QTalk::getOAIconPath(it->groupIcon));

        _mapNavItems[id] = new OANavigationItem(id, name, path, this);
        _mapMainWgt[id] = new OaMainWgt(id, name, it->members, this);

        _pRightLay->addWidget(_mapMainWgt[id]);
        if (_mapNavItems.size() == 1) {
            _mapNavItems[id]->setSelectState(true);
            _pRightLay->setCurrentWidget(_mapMainWgt[id]);
        }
    }

    dealNavItem();
    _pLeftLay->addItem(new QSpacerItem(10, 10, QSizePolicy::Fixed, QSizePolicy::Expanding));
}