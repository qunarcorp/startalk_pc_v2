//
// Created by lihaibin on 2019-06-18.
//
#include <QVBoxLayout>
#include "SessionTransferWnd.h"
#include "../UITitlebarPlug/SystemSettingWnd.h"
#include "../CustomUi/LiteMessageBox.h"
#include <QPushButton>
SessionTransferWnd::SessionTransferWnd(QWidget *parent)
        : UShadowDialog(parent, true,true)
{
    initUI();
}

SessionTransferWnd::~SessionTransferWnd() {

}

void SessionTransferWnd::initUI() {

    //
    _pTitleFrm = new QFrame(this);
    _pTitleFrm->setObjectName("titleFrm");

    QLabel* labelTitle = new QLabel(tr("会话转移"),this);
    labelTitle->setAlignment(Qt::AlignCenter);

    auto * titleLay = new QHBoxLayout(_pTitleFrm);
    titleLay->setMargin(10);
    titleLay->setSpacing(10);
    titleLay->addWidget(labelTitle);

    QLabel* serviceLabel = new QLabel(tr("选择要转移的客服"),this);
    serviceLabel->setContentsMargins(10,0,0,0);
    serviceLabel->setStyleSheet("QLabel{font-size:14px;color:#666666;}");

    auto* seatFrm = new QFrame(this);
    _pSeats = new QVBoxLayout(seatFrm);
    _pSeats->setMargin(10);
    _pSeats->setSpacing(10);
    _pButtonGroup = new QButtonGroup(_pSeats);

    //
    _pEdit = new QTextEdit(this);
    _pEdit->setContentsMargins(20, 0, 20, 0);
    _pEdit->setObjectName("TransferName");
    _pEdit->setPlaceholderText(tr("请输入转移原因"));
    //
    QPushButton* btnYes = new QPushButton(tr("确定"));
    QPushButton* btnNo = new QPushButton(tr("取消"));
    btnYes->setFixedWidth(70);
    btnNo->setFixedWidth(70);
    btnYes->setObjectName("TransferYesBtn");
    btnNo->setObjectName("TransferNoBtn");

    QFrame* btnFrm = new QFrame(this);
    btnFrm->setObjectName("bodyFrm");

    auto * bottomLay = new QHBoxLayout(btnFrm);
    bottomLay->setContentsMargins(10, 10, 20, 10);
    bottomLay->addWidget(btnYes);
    bottomLay->addItem(new QSpacerItem(10, 10, QSizePolicy::Fixed));
    bottomLay->addWidget(btnNo);
    bottomLay->setMargin(10);
    bottomLay->setSpacing(10);
    //
    QFrame *mainFrm = new QFrame(this);
    auto * layout = new QVBoxLayout(mainFrm);
    layout->setMargin(0);
    layout->setSpacing(0);
    layout->addWidget(_pTitleFrm);
    layout->addWidget(serviceLabel);
    layout->addWidget(seatFrm);
    layout->addWidget(_pEdit);
    layout->addWidget(btnFrm);
    //
    auto* lay = new QHBoxLayout(_pCenternWgt);
    lay->setMargin(0);
    lay->setSpacing(0);
    lay->addWidget(mainFrm);

    this->setStyleSheet("background:rgba(250,250,250,1);");

    setFixedSize(400,500);

    setMoverAble(true, _pTitleFrm);

    connect(btnYes, &QPushButton::clicked, [this]()
    {
        std::string reason = _pEdit->toPlainText().toStdString();
        if(newCsrName.empty()){
            LiteMessageBox::failed(tr("请选择要转移的客服!"), 2000);
            return;
        }
        emit sessiontransfer(newCsrName,reason);
        this->close();
    });

    connect(btnNo, &QPushButton::clicked, [this]()
    {

        this->close();
    });
}

void SessionTransferWnd::showSeats(std::vector<QTalk::Entity::ImTransfer> transfers) {
    if(isHaveShow)
        return;
    int i = 0;
    for(QTalk::Entity::ImTransfer transfer : transfers){
        SettingCheckBox* radioButton = new SettingCheckBox(transfer.nickName.c_str());
        radioButton->setWhatsThis(QString::fromStdString(transfer.newCsrName));
        _pButtonGroup->addButton(radioButton,i++);
        _pSeats->addWidget(radioButton);
    }
    connect(_pButtonGroup, SIGNAL(buttonToggled(int,bool)), this, SLOT(operatingModeButtonsToggled(int,bool)));
    isHaveShow = true;
}

void SessionTransferWnd::operatingModeButtonsToggled(int id, bool status) {
    if(status){
        QString jid = _pButtonGroup->button(id)->whatsThis();
        newCsrName = jid.toStdString();
    }
}