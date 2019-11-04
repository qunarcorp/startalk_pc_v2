//
// Created by cc on 19-1-20.
//

#include "DropMenu.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QEvent>
#include <QAction>
#include <QComboBox>
#include <QMouseEvent>
#include <QProcess>
#include <QApplication>
#include "../QtUtil/Utils/Log.h"
#include "../include/Line.h"

DropMenu::DropMenu(QWidget *parent)
    : UShadowDialog(parent, true)
{
    initUi();
}

DropMenu::~DropMenu() = default;

void DropMenu::initUi()
{
    setFixedWidth(180);
    setMinimumHeight(100);
#ifndef _WINDOWS
    setWindowFlag(Qt::Popup);
#endif // !_WINDOWS
    //
    _pNameLabel = new QLabel(this);
    _pNameLabel->setObjectName("DropMenu_Name");
    _headLabel = new HeadPhotoLab;

    //
    comboBox = new ComboBox(this);
    comboBox->setObjectName("userStatusCombox");
    comboBox->addItem(tr("在线"));
    comboBox->addItem(tr("繁忙"));
    comboBox->addItem(tr("离开"));

    _menu = new QMenu(this);
    _menu->setFixedWidth(80);
    _menu->setAttribute(Qt::WA_TranslucentBackground, true);
    auto* actOnline = new QAction(tr("在线"), _menu);
    auto* actBusy = new QAction(tr("繁忙"), _menu);
    auto* actAway = new QAction(tr("离开"), _menu);
    _menu->addAction(actOnline);
    _menu->addAction(actBusy);
    _menu->addAction(actAway);
    actOnline->setData("online");
    actBusy->setData("busy");
    actAway->setData("away");

    auto *topLeftLay = new QVBoxLayout;
    topLeftLay->setMargin(0);
    topLeftLay->setSpacing(8);
    topLeftLay->addWidget(_pNameLabel);
    topLeftLay->addWidget(comboBox);

    auto * topLay = new QHBoxLayout;
    topLay->setMargin(0);
    topLay->setSpacing(6);
    topLay->setContentsMargins(0, 0 ,15, 10);
    topLay->addLayout(topLeftLay);
    topLay->addWidget(_headLabel);
    //
    _pUserCardLabel = new ActionLabel(tr("个人资料"));
    _pSettingLabel = new ActionLabel(tr("系统设置"));
    _pLogoutLabel = new ActionLabel(tr("退出登录"));
    _pSysQuitLabel = new ActionLabel(tr("系统退出"));
    _pAboutLabel = new ActionLabel(tr("关于"));

    auto* mainFrm = new QFrame(this);
    mainFrm->setObjectName("DropMenu");
    auto * lay = new QVBoxLayout(mainFrm);
    lay->setSpacing(2);
    lay->setContentsMargins(0, 10, 0, 10);
    lay->addLayout(topLay);
    lay->addWidget(new Line);
    lay->addWidget(_pUserCardLabel);
    lay->addWidget(_pSettingLabel);
    lay->addWidget(new Line);
    lay->addWidget(_pLogoutLabel);
    lay->addWidget(_pSysQuitLabel);
    lay->addWidget(new Line);
    lay->addWidget(_pAboutLabel);
    //
    auto* mainLay = new QVBoxLayout(_pCenternWgt);
    mainLay->setMargin(0);
    mainLay->addWidget(mainFrm);
//    _pSettingLabel->setVisible(false);
    connect(_pUserCardLabel, &ActionLabel::clicked, this, &DropMenu::showSelfUserCard);
    connect(_pSysQuitLabel, &ActionLabel::clicked, this, &DropMenu::sysQuit);
    connect(_pAboutLabel, &ActionLabel::clicked, this, &DropMenu::sgShowAboutWnd);
    connect(_pSettingLabel, &ActionLabel::clicked, this, &DropMenu::sgShowSystemSetting);
    connect(_pLogoutLabel, &ActionLabel::clicked, [](){
        // 重启应用
        QString program = QApplication::applicationFilePath();
        QStringList arguments;
        arguments << "AUTO_LOGIN=OFF";
        QProcess::startDetached(program, arguments);
        exit(0);
    });

    connect(_pUserCardLabel, &ActionLabel::clicked, [this](){setVisible(false);});
    connect(_pSysQuitLabel, &ActionLabel::clicked, [this](){setVisible(false);});
    connect(_pLogoutLabel, &ActionLabel::clicked, [this](){setVisible(false);});
    connect(_pSettingLabel, &ActionLabel::clicked, [this](){setVisible(false);});
    connect(_pAboutLabel, &ActionLabel::clicked, [this](){
        setVisible(false);
    });

    connect(comboBox, &ComboBox::clicked, [this](){
        QPoint pos = this->geometry().topLeft();
        _menu->move(pos.x() - 80, pos.y() + 50);
        _menu->exec();
    });

    connect(actOnline, &QAction::triggered, this, &DropMenu::switchUserStatus);
    connect(actBusy, &QAction::triggered, this, &DropMenu::switchUserStatus);
    connect(actAway, &QAction::triggered, this, &DropMenu::switchUserStatus);
}

void DropMenu::switchUserStatus()
{
    auto *act = (QAction*) sender();
    QString val = act->data().toString();
    //
    emit sgSwitchUserStatus(val);
}

void DropMenu::setName(const QString& name) {
    _pNameLabel->setText(name);
    _pNameLabel->setToolTip(name);
}

void DropMenu::setHead(const QString& headPath) {
    _headLabel->setHead(headPath, 17, false, true);
}

/**
 *
 * @param status
 */
void DropMenu::onSwitchUserStatusRet(const QString& status)
{
    for(const auto* act : _menu->actions())
    {
        QString val = act->data().toString();
        if(val == status)
        {
            comboBox->setText(act->text());
            break;
        }
    }
}