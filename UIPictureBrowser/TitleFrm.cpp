//
// Created by QITMAC000260 on 2018/11/16.
//

#include "TitleFrm.h"
#include "PictureBrowser.h"
#include "../include/Line.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QToolButton>
#include <QSplitter>
#include <QMouseEvent>
#include "../UICom/uicom.h"

TitleFrm::TitleFrm(PictureBrowser* pPicBrowser)
    :QFrame(), _pPicBrowser(pPicBrowser)
{
    initUi();
}

TitleFrm::~TitleFrm() {

}

void TitleFrm::initUi()
{
    this->setObjectName("TitleFrame");
    this->setFixedHeight(50);

    _pTurnBeforeBtn = new QToolButton();
    _pTurnNextBtn = new QToolButton();
    _pEnlargeBtn = new QToolButton();
    _pNarrowBtn = new QToolButton();
    _pOne2OneBtn = new QToolButton();
    _pRotateBtn = new QToolButton();
    _pSaveAsBtn = new QToolButton();
    _pMinBtn = new QToolButton();
    _pMaxBtn = new QToolButton();
    _pRestBtn = new QToolButton();
    _pCloseBtn = new QToolButton();

    _pTurnBeforeBtn->setFixedSize(30, 30);
    _pTurnNextBtn->setFixedSize(30, 30);
    _pEnlargeBtn->setFixedSize(30, 30);
    _pNarrowBtn->setFixedSize(30, 30);
    _pOne2OneBtn->setFixedSize(30, 30);
    _pRotateBtn->setFixedSize(30, 30);
    _pSaveAsBtn->setFixedSize(30, 30);
    _pMinBtn->setFixedSize(30, 30);
    _pMaxBtn->setFixedSize(30, 30);
    _pRestBtn->setFixedSize(30, 30);
    _pCloseBtn->setFixedSize(30, 30);

    _pTurnBeforeBtn->setObjectName("TurnLeftBtn");
    _pTurnNextBtn->setObjectName("TurnRightBtn");
    _pEnlargeBtn->setObjectName("EnlargeBtn");
    _pNarrowBtn->setObjectName("NarrowBtn");
    _pOne2OneBtn->setObjectName("One2OneBtn");
    _pRotateBtn->setObjectName("RotateBtn");
    _pSaveAsBtn->setObjectName("SaveAsBtn");
    _pMinBtn->setObjectName("MinBtn");
    _pMaxBtn->setObjectName("MaxBtn");
    _pRestBtn->setObjectName("RestBtn");

    _pTurnBeforeBtn->setToolTip(tr("上一张"));
    _pTurnNextBtn->setToolTip(tr("下一张"));
    _pEnlargeBtn->setToolTip(tr("放大"));
    _pNarrowBtn->setToolTip(tr("缩小"));
    _pOne2OneBtn->setToolTip(tr("1:1显示"));
    _pRotateBtn->setToolTip(tr("旋转"));
    _pSaveAsBtn->setToolTip(tr("另存为"));
    _pMinBtn->setToolTip(tr("最小化"));
    _pMaxBtn->setToolTip(tr("最大化"));
    _pRestBtn->setToolTip(tr("恢复窗口"));
    _pCloseBtn->setToolTip(tr("关闭"));

	_pMinBtn->setVisible(false);
	_pMaxBtn->setVisible(false);
	_pRestBtn->setVisible(false);
	_pRestBtn->setVisible(false);

    auto *topLayout = new QHBoxLayout();
#ifdef _MACOS
    _pCloseBtn->setFixedSize(12, 12);
    _pCloseBtn->setObjectName("gmCloseBtn");
    topLayout->addItem(new QSpacerItem(8, 1, QSizePolicy::Fixed, QSizePolicy::Fixed));
    topLayout->addWidget(_pCloseBtn);
    topLayout->addItem(new QSpacerItem(8, 1, QSizePolicy::Fixed, QSizePolicy::Fixed));
    topLayout->addWidget(new Line(Qt::Vertical));
#endif
    topLayout->addWidget(_pTurnBeforeBtn);
    topLayout->addWidget(_pTurnNextBtn);
    //topLayout->addWidget(new Line());
    topLayout->addWidget(_pEnlargeBtn);
    topLayout->addWidget(_pNarrowBtn);
    topLayout->addWidget(_pOne2OneBtn);
    topLayout->addWidget(new Line(Qt::Vertical));
    topLayout->addWidget(_pRotateBtn);
    topLayout->addWidget(_pSaveAsBtn);
    topLayout->addSpacerItem(new QSpacerItem(10, 10, QSizePolicy::Expanding));
    topLayout->addWidget(_pMinBtn);
    topLayout->addWidget(_pMaxBtn);
    topLayout->addWidget(_pRestBtn);
#ifndef _MACOS
    _pCloseBtn->setObjectName("gwCloseBtn");
    topLayout->addWidget(_pCloseBtn);
#endif
    setLayout(topLayout);
    // todo
    _pMinBtn->setVisible(false);
    _pMaxBtn->setVisible(false);
    _pRestBtn->setVisible(false);

    connect(_pTurnBeforeBtn, &QToolButton::clicked, _pPicBrowser, &PictureBrowser::turnBefore);
    connect(_pTurnNextBtn, &QToolButton::clicked, _pPicBrowser, &PictureBrowser::turnNext);

    connect(_pCloseBtn, &QToolButton::clicked, _pPicBrowser, [this](){
        _pPicBrowser->setVisible(false);
    });
    connect(_pMinBtn, &QToolButton::clicked, [this]()
    {
        if(nullptr != _pPicBrowser)
        {
            _pPicBrowser->showMinimized();
        }
    });
    connect(_pMaxBtn, &QToolButton::clicked, [this]()
    {
        if(nullptr != _pPicBrowser)
        {
            _pMaxBtn->setVisible(false);
            _pRestBtn->setVisible(true);
            _pPicBrowser->showMaximized();
        }
    });
    connect(_pRestBtn, &QToolButton::clicked, [this]()
    {
        if(nullptr != _pPicBrowser)
        {
            _pMaxBtn->setVisible(true);
            _pRestBtn->setVisible(false);
            _pPicBrowser->showNormal();
        }
    });

    connect(_pEnlargeBtn, &QToolButton::clicked, [this]()
    {
        emit _pPicBrowser->enlargeSignal();
    });

    connect(_pNarrowBtn, &QToolButton::clicked, [this]()
    {
        emit _pPicBrowser->narrowSignal();
    });
    connect(_pOne2OneBtn, &QToolButton::clicked, [this]()
    {
        emit _pPicBrowser->one2oneSiganl();
    });
    connect(_pRotateBtn, &QToolButton::clicked, [this]()
    {
        emit _pPicBrowser->rotateSiganl();
    });
    connect(_pSaveAsBtn, &QToolButton::clicked, [this]()
    {
        emit _pPicBrowser->saveAsSignal();
    });

}

void TitleFrm::setBeforeBtnEnable(bool enable) {
    _pTurnBeforeBtn->setEnabled(enable);
}

void TitleFrm::setNextBtnEnable(bool enable) {

    _pTurnNextBtn->setEnabled(enable);
}

/**
 *
 * @param e
 * @return
 */
bool TitleFrm::event(QEvent *e) {
   /* if(e->type() == QEvent::MouseButtonDblClick)
    {
        if(_pPicBrowser->isMaximized())
            _pPicBrowser->showNormal();
        else
            _pPicBrowser->showMaximized();
    }*/
    return QFrame::event(e);
}
