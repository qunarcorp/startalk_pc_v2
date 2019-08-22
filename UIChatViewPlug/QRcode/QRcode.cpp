//
// Created by QITMAC000260 on 2019-04-16.
//

#include "QRcode.h"
#include <QStackedWidget>
#include <QHBoxLayout>
#include <QPushButton>
#include "ScanQRcode.h"
#include "MakeQRcode.h"

QRcode::QRcode(QWidget* parent)
    :UShadowDialog(parent, true, false)
{
    Qt::WindowFlags flags = Qt::X11BypassWindowManagerHint | Qt::WindowStaysOnTopHint;
    //setWindowFlags(windowFlags() | flags);

    //
    auto * closeBtn = new QPushButton(this);
    closeBtn->setToolTip(tr("关闭"));
    auto* titleLay = new QHBoxLayout;
#ifdef _MACOS
    closeBtn->setFixedSize(12, 12);
    closeBtn->setObjectName("gmCloseBtn");
    titleLay->addWidget(closeBtn);
    titleLay->addItem(new QSpacerItem(10, 10, QSizePolicy::Expanding));
    titleLay->setContentsMargins(8, 8, 20, 0);
#else
    closeBtn->setFixedSize(30, 30);
    closeBtn->setObjectName("gwCloseBtn");
    titleLay->setMargin(0);
    titleLay->addItem(new QSpacerItem(10, 10, QSizePolicy::Expanding));
    titleLay->addWidget(closeBtn);
#endif
    //
    scanBtn = new TipButton("扫一扫", em_wnd_scan,this);
    makeBtn = new TipButton("生成二维码", em_wnd_make,this);
    auto* btnLay = new QHBoxLayout;
    btnLay->setSpacing(30);
    btnLay->addItem(new QSpacerItem(10, 10, QSizePolicy::Expanding));
    btnLay->addWidget(scanBtn);
    btnLay->addWidget(makeBtn);
    btnLay->addItem(new QSpacerItem(10, 10, QSizePolicy::Expanding));

    //
    QFrame* topFrm = new QFrame(this);
    topFrm->setFixedHeight(90);
    auto* topLay = new QVBoxLayout(topFrm);
    topLay->setMargin(0);
    topLay->addLayout(titleLay, 0);
    topLay->addLayout(btnLay, 1);
    topLay->setAlignment(btnLay, Qt::AlignBottom);
    //
    _pScanQRcode = new ScanQRcode(this);
    _pMakeQRcode = new MakeQRcode(this);

    QFrame* mainFrm = new QFrame(this);
    mainFrm->setObjectName("QRcode");
    auto* mainLay = new QVBoxLayout(mainFrm);
    mainLay->addWidget(topFrm);
    mainLay->addWidget(_pScanQRcode);
    mainLay->addWidget(_pMakeQRcode);
    //
    auto* layout = new QHBoxLayout(_pCenternWgt);
    layout->addWidget(mainFrm);
    //
    layout->setMargin(0);
    layout->setSpacing(0);
    setMoverAble(true);

    onChangeWnd(em_wnd_scan);

    connect(closeBtn, &QPushButton::clicked, [this](){
        this->setVisible(false);
    });
    connect(makeBtn, &TipButton::clicked, this, &QRcode::onChangeWnd, Qt::QueuedConnection);
    connect(scanBtn, &TipButton::clicked, this, &QRcode::onChangeWnd, Qt::QueuedConnection);
#ifdef _MACOS
    macAdjustWindows();
#endif
}

QRcode::~QRcode() {

}

void QRcode::onChangeWnd(int id)
{
    _pScanQRcode->setVisible(em_wnd_scan == id);
    _pMakeQRcode->setVisible(em_wnd_make == id);
    scanBtn->setCheckState(em_wnd_scan == id);
    makeBtn->setCheckState(em_wnd_make == id);

    if(em_wnd_scan == id)
        resize(380, 450);
    else if(em_wnd_make == id)
        resize(380, 480);
    else{}
}