//
// Created by QITMAC000260 on 2019/10/21.
//

#include "MedalDetailWnd.h"
#include "../../include/Line.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QToolButton>

MedalDetailWnd::MedalDetailWnd(QWidget *parent)
    :QFrame(parent)
{
    setFixedWidth(385);

    _pTitleLabel = new QLabel(this);
    _pTitleLabel->setObjectName("MedalDetailWnd_TitleLabel");
    _pTitleLabel->setAlignment(Qt::AlignCenter);
    auto* closeBtn = new QToolButton(this);

    _topFrm = new QFrame(this);
    _topFrm->setObjectName("MedalDetailWnd_TitleFrm");
    auto topLay = new QHBoxLayout(_topFrm);
#ifdef _MACOS
    closeBtn->setFixedSize(12, 12);
    closeBtn->setObjectName("gmCloseBtn");
    topLay->addWidget(closeBtn);
    topLay->addWidget(_pTitleLabel);
#else
	topLay->setContentsMargins(30, 0, 0, 0);
    topLay->addWidget(_pTitleLabel);
    topLay->addWidget(closeBtn);
    closeBtn->setFixedSize(30, 30);
    closeBtn->setObjectName("gwCloseBtn");
#endif
    auto* backBtn = new QToolButton(this);
    backBtn->setFixedSize(20, 20);
    backBtn->setObjectName("BackBtn");
    auto* backBtnLay = new QHBoxLayout();
    backBtnLay->setContentsMargins(10, 0, 10, 0);
    backBtnLay->addWidget(backBtn);
    backBtnLay->setAlignment(backBtn, Qt::AlignLeft);
    // body
    _pImageView = new UserMedalItem(this);
    _pTipWgt = new MedalTip(this);
    _pTipWgt->setContentsMargins(0, 10, 0, 10);
    //
    moreBtn = new UserHeadWgt(tr("更多"), "", true,this);
    moreBtn->setFixedSize(30, 30);
    _userLay = new QHBoxLayout;
    _userLay->setContentsMargins(0, 0, 0, 0);
    _userLay->setSpacing(15);
    _userLay->addItem(new QSpacerItem(1, 1, QSizePolicy::Expanding));
    _userLay->addWidget(moreBtn);
    usersCountLabel = new QLabel( this);
    auto* usersLay = new QVBoxLayout();
    usersLay->setContentsMargins(0, 30, 0, 50);
    usersLay->setSpacing(16);
    usersLay->addWidget(new Line(Qt::Horizontal, this));
    usersLay->addWidget(usersCountLabel);
    usersLay->addLayout(_userLay);
    usersLay->addWidget(new Line(Qt::Horizontal, this));
    //
    wearBtn = new QPushButton(tr("佩戴"), this);
    wearBtn->setObjectName("wearMedalBtn");
    unloadBtn = new QPushButton(tr("卸下"), this);
    unloadBtn->setObjectName("unloadMedalBtn");
    wearTipLabel = new QLabel( this);
    wearTipLabel->setObjectName("wearMedalTipLabel");
    wearTipLabel->setAlignment(Qt::AlignCenter);
    //
    auto* bodyFrm = new QFrame(this);
    auto *bodyLay = new QVBoxLayout(bodyFrm);
    bodyLay->setContentsMargins(20, 10, 20, 30);
    bodyLay->setSpacing(0);
    bodyLay->addWidget(_pImageView);
    bodyLay->addItem(new QSpacerItem(10, 20, QSizePolicy::Fixed, QSizePolicy::Fixed));
    bodyLay->addWidget(_pTipWgt);
    bodyLay->addLayout(usersLay);
    bodyLay->addItem(new QSpacerItem(10, 1, QSizePolicy::Fixed, QSizePolicy::Expanding));
    bodyLay->addWidget(wearBtn);
    bodyLay->addWidget(unloadBtn);
    bodyLay->addItem(new QSpacerItem(10, 20, QSizePolicy::Fixed, QSizePolicy::Fixed));
    bodyLay->addWidget(wearTipLabel);
    bodyLay->setAlignment(_pImageView, Qt::AlignHCenter);
    bodyLay->setAlignment(wearBtn, Qt::AlignHCenter);
    bodyLay->setAlignment(unloadBtn, Qt::AlignHCenter);
    //
    auto* mainFrm = new QFrame(this);
    mainFrm->setObjectName("MedalDetailWnd_MainFrm");
    auto mainLay = new QVBoxLayout(mainFrm);
    mainLay->setMargin(0);
    mainLay->setSpacing(10);
    mainLay->addWidget(_topFrm);
    mainLay->addLayout(backBtnLay);
    mainLay->addWidget(bodyFrm);
    mainLay->setStretch(0, 0);
    mainLay->setStretch(1, 0);
    mainLay->setStretch(2, 1);

    auto* lay = new QHBoxLayout(this);
    lay->setMargin(0);
    lay->addWidget(mainFrm);

    connect(closeBtn, &QToolButton::clicked, [parent](){parent->setVisible(false);});
    connect(backBtn, &QToolButton::clicked, this, &MedalDetailWnd::sgShowBack);
    connect(this, &MedalDetailWnd::sgUpdateUsers, this, &MedalDetailWnd::onUpdateUsers);
    connect(moreBtn, &UserHeadWgt::clicked, [this](){
        emit sgShowUserList(_metalUsers);
    });

    connect(wearBtn, &QPushButton::clicked, [this](){
        emit sgModifyStatus(_medalId, true);
    });

    connect(unloadBtn, &QPushButton::clicked, [this](){
        emit sgModifyStatus(_medalId, false);
    });
}

MedalDetailWnd::~MedalDetailWnd() = default;


void MedalDetailWnd::showMedalDetail(const QTalk::Entity::ImMedalList &medalInfo, bool isSelf, int status) {
    // title
    for (auto* h : _userHeadWnds)
    {
        _userLay->removeWidget(h);
        h->setVisible(false);
    }

    _pTitleLabel->setText(medalInfo.medalName.data());
    _pImageView->setUserMedal(medalInfo, status, true);
    _pTipWgt->showTips(medalInfo.obtainCondition.data());
    _medalId = medalInfo.medalId;

    if(isSelf && status > 0)
    {
        unloadBtn->setVisible(true);
        wearBtn->setVisible(true);
        wearTipLabel->setVisible(true);
        if((status & 0x02) > 0) {
            wearBtn->setVisible(false);
            wearTipLabel->setText(tr("卸下后勋章将不再展示在姓名后"));
            return;
        }
        else if((status & 0x01) > 0) {
            unloadBtn->setVisible(false);
            wearTipLabel->setText(tr("佩戴后勋章将展示在姓名后"));
            return;
        }
    }
    unloadBtn->setVisible(false);
    wearBtn->setVisible(false);
    wearTipLabel->setVisible(false);
}

void MedalDetailWnd::onUpdateUsers() {
    auto size = _metalUsers.size();
    usersCountLabel->setText(tr("%1 位小驼已获得:").arg(_metalUsers.size()));
    moreBtn->setVisible(size > 6);

    auto index = 0;
    for (const auto& u : _metalUsers)
    {
        auto* wgt = new UserHeadWgt(u.userName.data(), u.userHead.data());
        wgt->setFixedSize(30, 30);
        _userLay->insertWidget(index, wgt);
        _userHeadWnds.insert(wgt);

        if(++index > 6)
            break;
    }
}

void MedalDetailWnd::setMedalUsers(int id, const std::vector<QTalk::StMedalUser> &metalUsers) {
    QMutexLocker locker(&_mutex);
    if(id == _medalId)
    {
        _metalUsers = metalUsers;
        emit sgUpdateUsers();
    }
}

void MedalDetailWnd::onModifySuccess(int id, bool isWear) {
    if(id != _medalId)
        return;

    unloadBtn->setVisible(isWear);
    wearBtn->setVisible(!isWear);
    wearTipLabel->setText(isWear ? tr("佩戴后勋章将展示在姓名后") : tr("卸下后勋章将不再展示在姓名后"));
}
