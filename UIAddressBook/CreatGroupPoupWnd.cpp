//
// Created by cc on 18-12-21.
//

#include "CreatGroupPoupWnd.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QMouseEvent>

CreatGroupPoupWnd::CreatGroupPoupWnd(QWidget* parent)
    : UShadowDialog(parent, true)
{
    setFixedWidth(300);
    //
	QLabel* labelTitle = new QLabel(QString::fromLocal8Bit("创建群聊"));
    labelTitle->setContentsMargins(10, 0, 0, 0);
    _pLineEdit = new QLineEdit(this);
    _pWarningLabel = new QLabel(this);
    labelTitle->setAlignment(Qt::AlignVCenter);
    labelTitle->setObjectName("CreatGroupTitle");
    //
    _pTitleFrm = new QFrame(this);
    _pTitleFrm->setFixedHeight(40);
    _pTitleFrm->setObjectName("CreatGroupTitleFrm");
    auto * titleLay = new QHBoxLayout(_pTitleFrm);
    titleLay->setMargin(5);
    titleLay->setSpacing(0);
    titleLay->addWidget(labelTitle);
    //
    _pLineEdit->setContentsMargins(20, 0, 20, 0);
    _pWarningLabel->setContentsMargins(20, 0, 20, 0);
    _pLineEdit->setObjectName("CreatGroupName");
    //
    _pLineEdit->setPlaceholderText(QString::fromLocal8Bit("请输入群名称"));

    _pWarningLabel->setObjectName("CreatGroupWarning");
    //
    QPushButton* btnYes = new QPushButton(QString::fromLocal8Bit("确定"));
    QPushButton* btnNo = new QPushButton(QString::fromLocal8Bit("取消"));
    btnYes->setFixedWidth(70);
    btnNo->setFixedWidth(70);
    btnYes->setObjectName("CreatGroupYesBtn");
    btnNo->setObjectName("CreatGroupNoBtn");
    auto * bottomLay = new QHBoxLayout;
    bottomLay->setContentsMargins(10, 10, 20, 10);
    bottomLay->addItem(new QSpacerItem(10, 10, QSizePolicy::Expanding));
    bottomLay->addWidget(btnYes);
    bottomLay->addWidget(btnNo);
    //
    QFrame *mainFrm = new QFrame(this);
    mainFrm->setObjectName("mainFrm");
    auto * layout = new QVBoxLayout(mainFrm);
    layout->setMargin(4);
    layout->setSpacing(10);
    layout->addWidget(_pTitleFrm);
    layout->addWidget(_pLineEdit);
    layout->addWidget(_pWarningLabel);
    layout->addLayout(bottomLay);
    //
    auto* lay = new QHBoxLayout(_pCenternWgt);
    lay->setMargin(0);
    lay->setSpacing(0);
    lay->addWidget(mainFrm);

    setMoverAble(true, _pTitleFrm);

    connect(btnYes, &QPushButton::clicked, [this]()
    {
        emit sgCreatGroupSignal();
        this->close();
    });

    connect(btnNo, &QPushButton::clicked, [this]()
    {
        this->close();
    });
}

CreatGroupPoupWnd::~CreatGroupPoupWnd()
{

}

QString CreatGroupPoupWnd::getGroupName() {
    return _pLineEdit->text();
}

void CreatGroupPoupWnd::setCunt(int count)
{
    if(count > 50)
    {
        _pWarningLabel->setText(QString::fromLocal8Bit("群成员超过50人, 当前人数: %1").arg(count));
        _pWarningLabel->setVisible(true);
    }
    else
        _pWarningLabel->setVisible(false);
}
