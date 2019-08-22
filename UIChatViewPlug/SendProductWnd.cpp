//
// Created by lihaibin on 2019-06-27.
//

#include "SendProductWnd.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>

SendProductWnd::SendProductWnd(QWidget* parent)
        : UShadowDialog(parent, true, true)
{

    QFrame* titleFrm = new QFrame(this);
    titleFrm->setObjectName("titleFrm");
    QLabel* titleLabel = new QLabel(tr("产品卡片JSON："), this);

    auto * titleLay = new QHBoxLayout(titleFrm);
    titleLay->addWidget(titleLabel);

    _pEidt = new QTextEdit(this);
    _pEidt->setObjectName("TextEdit");
    _pEidt->setContentsMargins(9, 6, 9, 6);

    QPushButton* cancelBtn = new QPushButton(tr("取消"));
    QPushButton* okBtn = new QPushButton(tr("确定"));
    QFrame* btnFrm = new QFrame(this);
    btnFrm->setObjectName("bodyFrm");
    auto* btnFLay = new QHBoxLayout(btnFrm);
    btnFLay->addItem(new QSpacerItem(10, 10, QSizePolicy::Expanding));
    btnFLay->addWidget(cancelBtn);
    btnFLay->addWidget(okBtn);
    cancelBtn->setObjectName("cancelButton");
    okBtn->setObjectName("makeSureButton");
    cancelBtn->setFixedSize(72, 32);
    okBtn->setFixedSize(72, 32);

    QFrame* mainFrm = new QFrame(this);
    auto* mainlay = new QVBoxLayout(mainFrm);
    mainlay->setMargin(0);
    mainlay->setSpacing(0);
    mainlay->addWidget(titleFrm);
    mainlay->addWidget(_pEidt);
    mainlay->addWidget(btnFrm);

    auto* lay = new QVBoxLayout(_pCenternWgt);
    lay->setMargin(0);
    lay->addWidget(mainFrm);

    connect(cancelBtn, &QPushButton::clicked, [this](){
        this->setVisible(false);
    });
    connect(okBtn, &QPushButton::clicked, [this](){
        QString text = _pEidt->toPlainText();
        emit sendJsonSig(text);
        this->setVisible(false);
    });

    setMoverAble(true, titleFrm);
    setFixedSize(300, 500);
}

SendProductWnd::~SendProductWnd() {

}

/**
 *
 */
void SendProductWnd::clear() {
    _pEidt->clear();
}