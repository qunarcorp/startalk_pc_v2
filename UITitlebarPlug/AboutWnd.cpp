//
// Created by QITMAC000260 on 2019-02-13.
//

#include "AboutWnd.h"
#include <QVBoxLayout>
#include "../Platform/Platform.h"
#include <QToolButton>

AboutWnd::AboutWnd(QWidget * parent)
    : UShadowDialog(parent, true, false)
    , _pVersionLabel(nullptr)
{
    initUi();
}

AboutWnd::~AboutWnd() {
    if(nullptr != _pVersionLabel)
    {
        delete _pVersionLabel;
        _pVersionLabel = nullptr;
    }
}

void AboutWnd::initUi()
{
    this->setFixedSize(484, 336);
    setMoverAble(true);
    //
    QFrame* titleFrm = new QFrame(this);
    auto * titleLay = new QHBoxLayout(titleFrm);
    titleLay->setContentsMargins(10, 10, 10, 10);
    auto * closeBtn = new QToolButton();

#ifdef _MACOS
    closeBtn->setFixedSize(10, 10);
    titleLay->addWidget(closeBtn);
    closeBtn->setObjectName("gmCloseBtn");
    titleLay->addItem(new QSpacerItem(10, 10, QSizePolicy::Expanding));
#else
    closeBtn->setFixedSize(20, 20);
    titleLay->addItem(new QSpacerItem(10, 10, QSizePolicy::Expanding));
    closeBtn->setObjectName("gwCloseBtn");
    titleLay->addWidget(closeBtn);
#endif
    //
    QFrame* mainFrm = new QFrame(this);

#if defined(_ATALK)
    mainFrm->setObjectName("ATalkAboutMainFrm");
#elif defined(_STARTALK)
    mainFrm->setObjectName("AboutMainFrm");
    this->setStyleSheet("background:url(:/title/image1/aboutStarTalk.png);");
#elif defined(_QCHAT)
    mainFrm->setObjectName("AboutMainFrm");
    this->setStyleSheet("background:url(:/title/image1/aboutQchat.png);");
#else
    mainFrm->setObjectName("AboutMainFrm");
    this->setStyleSheet("background:url(:/title/image1/aboutQtalk.png);");
#endif
    auto * mainLay = new QVBoxLayout(mainFrm);
    mainLay->setContentsMargins(0, 0, 0, 20);
    //
    std::string version = Platform::instance().getGlobalVersion();
    _pVersionLabel = new QLabel(QString("Version: (%1)").arg(version.data()), this);
    _pCopyrightLabel = new QLabel(" Copyright @2017 Qunar.com", this);
    _pVersionLabel->setObjectName("AboutVersionLabel");
    _pCopyrightLabel->setObjectName("AboutCopyrightLabel");
    mainLay->addWidget(titleFrm);
    mainLay->addItem(new QSpacerItem(10, 10, QSizePolicy::Fixed, QSizePolicy::Expanding));
    mainLay->addWidget(_pVersionLabel);
    mainLay->addWidget(_pCopyrightLabel);
    mainLay->setAlignment(_pVersionLabel, Qt::AlignHCenter);
    mainLay->setAlignment(_pCopyrightLabel, Qt::AlignHCenter);
    auto* lay = new QVBoxLayout(_pCenternWgt);
    lay->setMargin(0);
    lay->addWidget(mainFrm);

    connect(closeBtn, &QToolButton::clicked, [this](){this->setVisible(false);});
}

