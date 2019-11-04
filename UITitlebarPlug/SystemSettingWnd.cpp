#include <utility>

//
// Created by admin on 2019-02-19.
//

#include "SystemSettingWnd.h"

#include <QHBoxLayout>
#include <QGridLayout>
#include <QToolButton>
#include <QListWidget>
#include <QCheckBox>
#include <QLineEdit>
#include <QTextEdit>
#include <QScrollBar>
#include <QComboBox>
#include <QRadioButton>
#include <QPushButton>
#include <QSpinBox>
#include <QFileDialog>
#include <QDesktopServices>
#include <QJsonDocument>
#include <QJsonArray>
#include <QButtonGroup>
#include <QApplication>
#include <QDate>
#include "../include/Line.h"
#include "DropMenu.h"
#include "../Platform/Platform.h"
#include "../CustomUi/QtMessageBox.h"
#include "SKRecorder.h"
#include "../QtUtil/Utils/Log.h"

#include <QProcess>
#include <QFontDatabase>
#include <QDebug>
#include <QSettings>

SystemSettingWnd::SystemSettingWnd(TitlebarMsgManager *messageManager,QWidget *parent)
        :_pMessageManager(messageManager),
        UShadowDialog(parent, true) {

    initUi();
}

SystemSettingWnd::~SystemSettingWnd() = default;

void SystemSettingWnd::initUi()
{
    this->setFixedSize(648,573);
    auto mainLay = new QVBoxLayout(_pCenternWgt);
    mainLay->setMargin(0);
    mainLay->setSpacing(0);
    //
    //
    QFrame* titleFrm = new QFrame(this);
    titleFrm->setObjectName("SettingTitleFrm");
    mainLay->addWidget(titleFrm);
    setMoverAble(true, titleFrm);

    auto * titleLay = new QHBoxLayout(titleFrm);
    titleLay->setMargin(10);
    auto * closeBtn = new QToolButton();
    QLabel *titleLbl = new QLabel(tr("系统设置"));
    titleLbl->setObjectName("SettingTitleLabel");
    titleLbl->setAlignment(Qt::AlignmentFlag::AlignCenter);

#ifdef _MACOS
    closeBtn->setFixedSize(10, 10);
    titleLay->addWidget(closeBtn);
    closeBtn->setObjectName("gmCloseBtn");
    titleLay->addWidget(titleLbl);
    titleLay->addItem(new QSpacerItem(12, 12));
#else
    titleLay->addWidget(titleLbl);
    closeBtn->setFixedSize(20, 20);
    titleLay->addItem(new QSpacerItem(10, 10, QSizePolicy::Expanding));
    closeBtn->setObjectName("gwCloseBtn");
    titleLay->addWidget(closeBtn);
#endif
    connect(closeBtn, &QToolButton::clicked, [this](){this->setVisible(false);});

    auto contentFrm = new QFrame(this);
    contentFrm->setObjectName("SettingContentFrm");
    mainLay->addWidget(contentFrm);

    auto *colLay = new QHBoxLayout(contentFrm);
    colLay->setSpacing(0);
    colLay->setMargin(0);

    auto navFrm = new QFrame(this);
    navFrm->setObjectName("NavFrm");
    colLay->addWidget(navFrm);
    _pNavlayout = new QVBoxLayout(navFrm);
    _pNavlayout->setAlignment(Qt::AlignmentFlag::AlignHCenter);
    _pNavlayout->setMargin(0);
    _pNavlayout->setSpacing(0);

    colLay->addWidget(new Line(Qt::Vertical));

    auto settingFrm = new QFrame(this);
    settingFrm->setObjectName("SettingFrm");
    colLay->addWidget(settingFrm);
    auto *Settinglayout = new QVBoxLayout(settingFrm);
    Settinglayout->setSpacing(0);
    Settinglayout->setMargin(0);

    //
    _settingListWidget = new QListWidget(this);
    _settingListWidget->setFixedWidth(452);
    _settingListWidget->setFrameShape(QFrame::NoFrame);
    _settingListWidget->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    _settingListWidget->verticalScrollBar()->setSingleStep(15);
    _settingListWidget->setObjectName("SettingListWidget");
    _settingListWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    _settingListWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    _settingListWidget->installEventFilter(this);
    Settinglayout->addWidget(_settingListWidget);
    //
    for(int index = EM_SETTING_MESSAGE;
        index <= EM_SETTING_SUGGESTION; index++)
    {
        initSetting(index);
    }
    _pNavlayout->addItem(new QSpacerItem(1, 1, QSizePolicy::Fixed, QSizePolicy::Expanding));
#ifdef _MACOS
    macAdjustWindows();
#endif
}

void SystemSettingWnd::initSetting(int type)
{
    QString text, objName;
    auto itemFrm = new QFrame(this);
    itemFrm->setObjectName("fileDirectoryFrm");

    auto *vlayout = new QVBoxLayout(itemFrm);
    vlayout->setSpacing(10);
    vlayout->setContentsMargins(30,30,30,1);

    QLabel *titleLbl = new QLabel(this);
    titleLbl->setObjectName("SettingTitleLbl");
    vlayout->addWidget(titleLbl);

    itemFrm->setFixedSize(450, 300);
    switch(type)
    {
        case EM_SETTING_MESSAGE:
        {
            text = tr("消息设置");
            objName = "msgNotifyLbl";

            initMsgNotify(vlayout);
            break;
        }
        case EM_SETTING_HOTKEY:
        {
            text = tr("热键设置");
            objName = "hotCutLbl";
            initHotCut(vlayout);
            break;
        }
        case EM_SETTING_SESSION:
        {
            text = tr("会话设置");
            objName = "sessionLbl";
            initSession(vlayout);
            break;
        }
        case EM_SETTING_AUTOREPLY:
        {
            text = tr("自动回复");
            objName = "sessionLbl";
            initAutoReply(vlayout);
            break;
        }
        case EM_SETTING_FOLDER:
        {
            text = tr("文件设置");
            objName = "autoReplyLbl";
            initFolder(vlayout);
            break;
        }
        case EM_SETTING_FRIEND:
        {
            text = tr("好友验证");
            objName = "friendAuthorityLbl";
            initFriendAuthority(vlayout);
            break;
        }
        case EM_SETTING_FONT:
        {
            text = tr("外观设置");
            objName = "fontSettingLbl";
            initFontSetting(vlayout);
            break;
        }
        case EM_SETTING_OTHERSETTING:
        {
            text = tr("其他设置");
            objName = "otherSettingLbl";
            initOtherSetting(vlayout);
            break;
        }
        case EM_SETTING_VERSION:
        {
            text = tr("版本信息");
            objName = "versionInfoLbl";
            initVersionInfo(vlayout);
            break;
        }
        case EM_SETTING_SUGGESTION:
        {
            text = tr("意见反馈");
            objName = "feedbackLbl";
            itemFrm->setFixedHeight(500);
            initFeedback(vlayout);
            break;
        }
#ifdef _QCHAT
        case EM_SETTING_SEAT:
        {
            text = tr("服务状态");
            objName = "seatInfoLbl";
            initSeat(vlayout);
            break;
        }
#endif
        default:
            return;
    }
    //
    titleLbl->setText(text);

    auto *actLabel = new ActionLabel(text, this);
    actLabel->setContentsMargins(25, 0, 0, 0);
    actLabel->setCheckAble(true);
    actLabel->setFixedSize(176, 52);
    actLabel->setObjectName(objName);
    _pNavlayout->addWidget(actLabel);

    auto *settingItem = new QListWidgetItem(_settingListWidget);
    settingItem->setSizeHint(itemFrm->frameSize());
    _settingListWidget->addItem(settingItem);
    _settingListWidget->setItemWidget(settingItem, itemFrm);

    _mapSettingItems[settingItem] = actLabel;
    actLabel->setCheckState(_mapSettingItems.size() == 1);
    connect(actLabel, &ActionLabel::clicked, [this,  actLabel, settingItem](){
        _settingListWidget->scrollToItem(settingItem, QAbstractItemView::PositionAtTop);
        for(const auto lab : _mapSettingItems.values())
        {
            lab->setCheckState(lab == actLabel);
        }
    });
}

/**
 *
 * @param o
 * @param e
 * @return
 */
bool SystemSettingWnd::eventFilter(QObject *o, QEvent *e)
{
    if(o == _settingListWidget)
    {
        if (e->type() == QEvent::Wheel)
        {
            QListWidgetItem *currentItem = _settingListWidget->itemAt(_settingListWidget->contentsRect().topLeft());
            if(_mapSettingItems.contains(currentItem))
            {
                emit _mapSettingItems[currentItem]->clicked();
            }
        }
    }

    return QDialog::eventFilter(o,e);
}

void SystemSettingWnd::hideEvent(QHideEvent* e)
{
	emit saveConfig();
	QWidget::hideEvent(e);
}

void SystemSettingWnd::initMsgNotify(QVBoxLayout* vlayout) {

	SettingCheckBox *msgAudioCK = new SettingCheckBox(tr("新消息声音提醒"), AppSetting::instance().getNewMsgAudioNotify(), this);
	SettingCheckBox *nativeNotify = new SettingCheckBox(tr("使用系统提示窗"), AppSetting::instance().getUseNativeMessagePrompt(), this);
	SettingCheckBox *msgNotifyCK = new SettingCheckBox(tr("显示新消息提示窗"), AppSetting::instance().getNewMsgTipWindowNotify(), this);
	SettingCheckBox *msgPhonePushCK = new SettingCheckBox(tr("手机端随时接收推送"), AppSetting::instance().getPhoneAnyReceive(), this);
	SettingCheckBox *strongWarn = new SettingCheckBox(tr("消息强提醒"), AppSetting::instance().getStrongWarnFlag(), this);
    nativeNotify->setEnabled(AppSetting::instance().getNativeMessagePromptEnable());

#ifdef _MACOS
    nativeNotify->setVisible(false);
#endif

	auto* msgAudioLay = new QHBoxLayout(this);
	auto* changeSoundBtn = new QPushButton(tr("更换提示音"));
	changeSoundBtn->setObjectName("SettingBtn");
    msgAudioLay->addWidget(msgAudioCK);
    msgAudioLay->addWidget(changeSoundBtn);
    msgAudioLay->setAlignment(changeSoundBtn, Qt::AlignRight);

    vlayout->addLayout(msgAudioLay);
    vlayout->addWidget(nativeNotify);
    vlayout->addWidget(msgNotifyCK);
    vlayout->addWidget(msgPhonePushCK);
    vlayout->addWidget(strongWarn);
    vlayout->addItem(new QSpacerItem(10, 10, QSizePolicy::Fixed, QSizePolicy::Expanding));
    vlayout->addWidget(new Line(Qt::Horizontal, this));

    msgPhonePushCK->setVisible(false);

    connect(msgNotifyCK, &QCheckBox::stateChanged,[](int state){
        AppSetting::instance().setNewMsgTipWindowNotify(state == Qt::Checked);
    });
    connect(msgAudioCK, &QCheckBox::stateChanged,[](int state){
        AppSetting::instance().setNewMsgAudioNotify(state == Qt::Checked);
    });
    connect(msgPhonePushCK, &QCheckBox::stateChanged,[](int state){
        AppSetting::instance().setPhoneAnyReceive(state==Qt::Checked);
    });
    connect(strongWarn, &QCheckBox::stateChanged,[](int state){
        AppSetting::instance().setStrongWarnFlag(state==Qt::Checked);
    });
    connect(nativeNotify, &QCheckBox::stateChanged,[](int state){
        AppSetting::instance().setUseNativeMessagePrompt(state==Qt::Checked);
    });

    connect(changeSoundBtn, &QPushButton::clicked, this, &SystemSettingWnd::msgSoundChanged);
}

void SystemSettingWnd::initHotCut(QVBoxLayout* vlayout) {

    auto *hotCutEnableCK = new SettingCheckBox(tr("启用热键"), AppSetting::instance().getHotCutEnable(), this);
    vlayout->addWidget(hotCutEnableCK);

	QKeySequence screenShot;
	QKeySequence wakeWnd;

	std::string screenShotHotKey =  AppSetting::instance().getScreenshotHotKey();
	std::string wakeUpHotKey =  AppSetting::instance().getWakeWndHotKey();

	if (screenShotHotKey.empty())
	{
		screenShot = QKeySequence(Qt::Key_A|Qt::AltModifier);
		screenShotHotKey = screenShot.toString(QKeySequence::NativeText).toStdString();
		AppSetting::instance().setScreenshotHotKey(screenShotHotKey);
	}
	else 
		screenShot = QKeySequence(QString::fromStdString(screenShotHotKey), QKeySequence::NativeText);

	if (wakeUpHotKey.empty())
	{
		wakeWnd = QKeySequence(Qt::Key_W | Qt::AltModifier);
		wakeUpHotKey = wakeWnd.toString(QKeySequence::NativeText).toStdString();
		AppSetting::instance().setWakeWndHotKey(wakeUpHotKey);
	}
	else 
		wakeWnd = QKeySequence(QString::fromStdString(wakeUpHotKey), QKeySequence::NativeText);

    SKRecorder* screenItem = addSortCutItem(tr("屏幕捕捉"), screenShot, vlayout, [this](const QKeySequence& keys){
        std::string hotKey = keys.toString(QKeySequence::NativeText).toStdString();
        AppSetting::instance().setScreenshotHotKey(hotKey);
        emit sgUpdateHotKey();
    });
    SKRecorder* wakeUpItem = addSortCutItem(tr("快速唤起"), wakeWnd, vlayout,[this](const QKeySequence& keys){
        std::string hotKey = keys.toString(QKeySequence::NativeText).toStdString();
        AppSetting::instance().setWakeWndHotKey(hotKey);
        emit sgUpdateHotKey();
    });

    vlayout->addItem(new QSpacerItem(10, 10, QSizePolicy::Fixed, QSizePolicy::Expanding));
    vlayout->addWidget(new Line(Qt::Horizontal, this));

    connect(hotCutEnableCK, &QCheckBox::stateChanged,[screenItem, wakeUpItem](int state){
        bool isOk = state==Qt::Checked;
        AppSetting::instance().setHotCutEnable(isOk);
        screenItem->setEnabled(isOk);
        wakeUpItem->setEnabled(isOk);
    });
}

/**
 *
 * @param text
 * @param defaultVal
 * @param layout
 */
SKRecorder* SystemSettingWnd::addSortCutItem(const QString& text, const QKeySequence& keys,
        QVBoxLayout* layout, std::function<void(const QKeySequence& keys)> callback)
{
    auto *itemLay = new QHBoxLayout;
    itemLay->setMargin(0);
    itemLay->setSpacing(20);

    layout->addLayout(itemLay);

    QLabel *tLabel = new QLabel(text, this);
    tLabel->setObjectName("SettingTLabel");
    itemLay->addWidget(tLabel);

    auto *cutEdit = new SKRecorder(this,300,32);
    cutEdit->setShorCut(keys);
    cutEdit->setKeyChangeEvent(std::move(callback));
    itemLay->addWidget(cutEdit);

    itemLay->setStretch(0, 0);
    itemLay->setStretch(1, 1);

    return cutEdit;
}

void SystemSettingWnd::initSession(QVBoxLayout* vlayout) {

    QLabel* sendMessageLabel = new QLabel(tr("发送消息"), this);
    sendMessageLabel->setObjectName("sendMessageLabel");
    auto* sendMessageBox = new NoSlidingHandoverComboBox(this);
    sendMessageBox->addItem(QKeySequence(Qt::Key_Enter).toString(QKeySequence::NativeText));
    sendMessageBox->addItem(QKeySequence(Qt::Key_Enter | Qt::ShiftModifier).toString(QKeySequence::NativeText));
    sendMessageBox->addItem(QKeySequence(Qt::Key_Enter | Qt::AltModifier).toString(QKeySequence::NativeText));
    sendMessageBox->addItem(QKeySequence(Qt::Key_Enter | Qt::ControlModifier).toString(QKeySequence::NativeText));

    auto* sendMessageLay = new QHBoxLayout;
    sendMessageLay->addWidget(sendMessageLabel);
    sendMessageLay->addWidget(sendMessageBox);
    sendMessageLabel->setFixedWidth(100);
    //
    SettingCheckBox *showMood = new SettingCheckBox(tr("群会话显示签名"), AppSetting::instance().getShowMoodFlag(), this);
    SettingCheckBox *autoDeleteSession = new SettingCheckBox(tr("切换会话时自动销毁旧会话"), AppSetting::instance().getAutoDeleteSession(), this);
    //
    vlayout->addLayout(sendMessageLay);
    vlayout->addWidget(showMood);
    vlayout->addWidget(autoDeleteSession);
    showMood->setVisible(false);
    vlayout->addItem(new QSpacerItem(10, 10, QSizePolicy::Minimum, QSizePolicy::Expanding));
    vlayout->addWidget(new Line(Qt::Horizontal, this));

    std::string sendMessageKey = AppSetting::instance().getSendMessageKey();
    int index = sendMessageBox->findText(sendMessageKey.data());
    if(index != -1)
    {
        sendMessageBox->setCurrentIndex(index);
    }
    connect(sendMessageBox, &QComboBox::currentTextChanged, [this](const QString &key){
        AppSetting::instance().setSendMessageKey(key.toStdString());
    });
    connect(showMood, &QCheckBox::stateChanged,[](int state){
        bool isOk = state==Qt::Checked;
        AppSetting::instance().setShowMoodFlag(isOk);
    });

    connect(autoDeleteSession, &QCheckBox::stateChanged,[](int state){
        bool isOk = state==Qt::Checked;
        AppSetting::instance().setAutoDeleteSession(isOk);
    });
}

/**
 *
 * @return
 */
void SystemSettingWnd::initAutoReply(QVBoxLayout* vlayout) {

    { // 离开电脑
        auto *hLay = new QHBoxLayout;
        hLay->setContentsMargins(0, 10, 0, 0);
        hLay->setSpacing(5);
        hLay->setAlignment(Qt::AlignmentFlag::AlignLeft);
        vlayout->addLayout(hLay);

        auto *leverCK = new SettingCheckBox("", AppSetting::instance().getLeaveCheckEnable(), this);
        hLay->addWidget(leverCK);

        connect(leverCK, &QCheckBox::stateChanged,[](int state){
            AppSetting::instance().setLeaveCheckEnable(state==Qt::Checked);
        });

        QLabel *descLb = new QLabel(tr("离开电脑"), this);
        descLb->setObjectName("SettingDescLab");
        hLay->addWidget(descLb);
        auto *secondEdit = new QSpinBox(this);
        secondEdit->setMinimum(1);
        secondEdit->setMaximum(60 * 24);
        hLay->addWidget(secondEdit);
        descLb = new QLabel(tr("分钟 push消息到手机端，并自动回复"));
        descLb->setObjectName("SettingDescLab");
        hLay->addWidget(descLb);

        int lev = AppSetting::instance().getLeaveMinute();
        if(lev <= 1)
            lev = 1;
        secondEdit->setValue(lev);

        connect(secondEdit, SIGNAL(valueChanged(int)), this, SLOT(onLeveMinuteChanged(int)));
    }
    {
        auto *hLay = new QHBoxLayout;
        hLay->setAlignment(Qt::AlignmentFlag::AlignLeft);
        vlayout->addLayout(hLay);

        auto * presetCK = new QRadioButton(tr("预设"), this);
        hLay->addWidget(presetCK);

        _pAutoPeplyCombobox = new NoSlidingHandoverComboBox(this);
        _pAutoPeplyCombobox->addItem(tr("不在办公室"));
        _pAutoPeplyCombobox->addItem(tr("目前正忙"));
        _pAutoPeplyCombobox->setObjectName("SettingCBox");
        hLay->addWidget(_pAutoPeplyCombobox);

        auto * customTextCK = new QRadioButton(tr("自定义"), this);
        vlayout->addWidget(customTextCK);

        hLay = new QHBoxLayout;
        vlayout->addLayout(hLay);

        _pAutoReplyEdit = new QTextEdit(this);
        _pAutoReplyEdit->setAcceptRichText(false);
        _pAutoReplyEdit->setFixedSize(356,70);
        _pAutoReplyEdit->setAcceptRichText(false);
        hLay->addWidget(_pAutoReplyEdit);

        auto* group = new QButtonGroup(this);
        group->addButton(presetCK, 1);
        group->addButton(customTextCK, 0);

        connect(group, SIGNAL(buttonClicked(int)), this, SLOT(onAutoReplyClicked(int)));

        bool isAutoReplyPreset = AppSetting::instance().getAutoReplyPreset();
        presetCK->setChecked(isAutoReplyPreset);
        customTextCK->setChecked(!isAutoReplyPreset);
        //
        std::string autoReplyMessage = AppSetting::instance().getAutoReplyCusMsg();
        if(autoReplyMessage.empty())
            autoReplyMessage = _pAutoPeplyCombobox->currentText().toStdString();
        _pAutoReplyEdit->setText(autoReplyMessage.data());

        connect(_pAutoReplyEdit, &QTextEdit::textChanged, this, &SystemSettingWnd::setAutoReplyMessage);
        setAutoReplyMessage();
    }

    vlayout->addItem(new QSpacerItem(10, 30, QSizePolicy::Minimum, QSizePolicy::Expanding));
    vlayout->addWidget(new Line(Qt::Horizontal, this));
}

/**
 *
 */
void SystemSettingWnd::onAutoReplyClicked(int id)
{
    AppSetting::instance().setAutoReplyPreset(id);
    _pAutoReplyEdit->setReadOnly(id);
    setAutoReplyMessage();
}

/**
 *
 */
void SystemSettingWnd::setAutoReplyMessage() {

    bool isAutoReplyPreset = AppSetting::instance().getAutoReplyPreset();

    if(isAutoReplyPreset)
    {
        QString text = _pAutoPeplyCombobox->currentText();
        AppSetting::instance().setAutoReplyMsg(text.toStdString());
    }
    else
    {
        QString text = _pAutoReplyEdit->toPlainText();
        AppSetting::instance().setAutoReplyMsg(text.toStdString());
        AppSetting::instance().setAutoReplyCusMsg(text.toStdString());
    }
}

/**
 *
 * @param minute
 */
void SystemSettingWnd::onLeveMinuteChanged(int minute)
{
    AppSetting::instance().setLeaveMinute(minute);
}

/**
 * 文件夹设置
 * @return
 */
void SystemSettingWnd::initFolder(QVBoxLayout* vlayout) {

     // 用户目录
    auto func([this, vlayout](int type) {

        auto *userDirLay = new QHBoxLayout;
        userDirLay->setSpacing(20);
        userDirLay->setMargin(0);
        vlayout->addLayout(userDirLay);

        auto* tmpLab = new QLabel(this);
        tmpLab->setObjectName("SettingTLabel");
        userDirLay->addWidget(tmpLab);

        auto *fileDirEdit = new QLineEdit(this);
        fileDirEdit->setReadOnly(true);
        fileDirEdit->setObjectName("SettingLineEdit");
        userDirLay->addWidget(fileDirEdit);

        auto *userDirBtnLay = new QHBoxLayout;
        vlayout->addLayout(userDirBtnLay);

        auto *changeSaveFileBtn = new QPushButton;
        changeSaveFileBtn->setFixedHeight(24);
        changeSaveFileBtn->setText(tr("变更文件夹"));
        changeSaveFileBtn->setObjectName("SettingBtn");

        auto *openFileDirBtn = new QPushButton(tr("打开文件夹"), this);
        openFileDirBtn->setFixedHeight(24);
        openFileDirBtn->setObjectName("SettingBtn");

        userDirBtnLay->addItem(new QSpacerItem(10, 10, QSizePolicy::Expanding));
        userDirBtnLay->addWidget(changeSaveFileBtn);
        userDirBtnLay->addWidget(openFileDirBtn);

        QString editText;
        switch (type)
        {
            case EM_USER_FOLDER:
            {
                tmpLab->setText(tr("用户目录"));
                editText = QString::fromStdString(AppSetting::instance().getUserDirectory());
                break;
            }
            case EM_FILE_FOLDER:
            {
                tmpLab->setText(tr("文件目录"));
                editText = QString::fromStdString(AppSetting::instance().getFileSaveDirectory());
                break;
            }
            default:
                break;
        }

        QFontMetrics f(fileDirEdit->font());
        fileDirEdit->setText(f.elidedText(editText, Qt::ElideMiddle, 320));

        connect(changeSaveFileBtn, &QPushButton::clicked, [this, type, fileDirEdit](){

            std::string path;
            switch (type)
            {
                case EM_USER_FOLDER:
                {
                    int ret = QtMessageBox::question(this, tr("重要提示"), "<font color=red>重新设置用户目录会丢失部分设置, 是否继续 ?</font>");
                    if(ret != QtMessageBox::EM_BUTTON_YES)
                    {
                        return;
                    }
                    path = AppSetting::instance().getUserDirectory();
                    break;
                }
                case EM_FILE_FOLDER:
                    path = AppSetting::instance().getFileSaveDirectory();
                    break;
                default:
                    break;
            }
            QString folderPath = QFileDialog::getExistingDirectory(this, tr("请选择一个文件夹"), QString::fromStdString(path));
            if(!folderPath.isEmpty())
            {
                QFontMetrics ff(fileDirEdit->font());
                fileDirEdit->setText(ff.elidedText(folderPath, Qt::ElideMiddle, 320));
                switch (type)
                {
                    case EM_USER_FOLDER:
                    {
                        AppSetting::instance().setUserDirectory(folderPath.toStdString());
                        QtMessageBox::information(this, tr("友情提示"), tr("部分目录重启后生效"));
                        break;
                    }
                    case EM_FILE_FOLDER:
                        AppSetting::instance().setFileSaveDirectory(folderPath.toStdString());
                        break;
                    default:
                        break;
                }
            }
        });

        connect(openFileDirBtn, &QPushButton::clicked, [changeSaveFileBtn, fileDirEdit, type](){

            std::string path;
            switch (type)
            {
                case EM_USER_FOLDER:
                    path = AppSetting::instance().getUserDirectory();
                    break;
                case EM_FILE_FOLDER:
                    path = AppSetting::instance().getFileSaveDirectory();
                    break;
                default:
                    break;
            }
            QDesktopServices::openUrl(QUrl::fromLocalFile(QString::fromStdString(path)));
        });
    });

    func(EM_USER_FOLDER);
    func(EM_FILE_FOLDER);
    vlayout->addItem(new QSpacerItem(10, 10, QSizePolicy::Fixed, QSizePolicy::Expanding));

    Line *capLine = new Line(Qt::Horizontal, this);
    vlayout->addWidget(capLine);
}

void SystemSettingWnd::initFriendAuthority(QVBoxLayout* vlayout) {

    vlayout->addItem(new QSpacerItem(10, 15, QSizePolicy::Expanding));

    { // 权限类型

        auto *friendModeLay = new QHBoxLayout;
        friendModeLay->setSpacing(0);
        friendModeLay->setMargin(0);
        vlayout->addLayout(friendModeLay);

        QLabel *tLabel = new QLabel;
        tLabel->setFixedWidth(74);
        tLabel->setText(tr("权限类型"));
        tLabel->setObjectName("SettingTLabel");
        friendModeLay->addWidget(tLabel);

        auto *friendModeCB = new NoSlidingHandoverComboBox(this);
        friendModeCB->addItem(tr("全部接受"));
        friendModeCB->addItem("500");
        friendModeCB->addItem("1000");
        friendModeCB->addItem("2000");
        friendModeCB->setObjectName("SettingCBox");
        friendModeLay->addWidget(friendModeCB);

        friendModeLay->addItem(new QSpacerItem(10, 10, QSizePolicy::Fixed, QSizePolicy::Expanding));
    }

    { // 问题

        auto *questionLay = new QHBoxLayout;
        questionLay->setSpacing(0);
        questionLay->setMargin(0);
        vlayout->addLayout(questionLay);

        QLabel *titleLbl = new QLabel;
        titleLbl->setFixedWidth(74);
        titleLbl->setFixedHeight(13);
        titleLbl->setText(tr("问题"));
        titleLbl->setStyleSheet("font-size:13px;font-family:PingFang-SC-Regular;font-weight:400;color:#666666;");
        questionLay->addWidget(titleLbl);

        auto *questionEdit = new QLineEdit;
        questionEdit->setFixedHeight(30);
        questionEdit->setText("C:\\Users\\suuiduofud");
        questionEdit->setObjectName("SettingLineEdit");
        questionLay->addWidget(questionEdit);
    }

    { // 答案
        auto *anwserLay = new QHBoxLayout;
        anwserLay->setSpacing(0);
        anwserLay->setMargin(0);
        vlayout->addLayout(anwserLay);

        QLabel *titleLbl = new QLabel;
        titleLbl->setFixedWidth(74);
        titleLbl->setFixedHeight(13);
        titleLbl->setText(tr("答案"));
        titleLbl->setStyleSheet("font-size:13px;font-family:PingFang-SC-Regular;font-weight:400;color:#666666;");
        anwserLay->addWidget(titleLbl);

        auto *anwserEdit = new QLineEdit;
        anwserEdit->setFixedHeight(30);
        anwserEdit->setPlaceholderText(tr("请输入问题"));
        anwserEdit->setObjectName("SettingLineEdit");
        anwserLay->addWidget(anwserEdit);
    }

    vlayout->addItem(new QSpacerItem(10, 10, QSizePolicy::Fixed, QSizePolicy::Expanding));

    Line *capLine = new Line(Qt::Horizontal, this);
    vlayout->addWidget(capLine);
}

void SystemSettingWnd::initFontSetting(QVBoxLayout* vlayout) {

    blackTheme = new SettingCheckBox(tr("暗黑模式"), false, this);
    int mode = AppSetting::instance().getThemeMode();
    blackTheme->setChecked(2 == mode);
    vlayout->addWidget(blackTheme);
    //
    QFontDatabase fontDatabase;
    QStringList families = fontDatabase.families();
    QString defaultFont = QApplication::font().family();
    //
    auto* fontLabel = new QLabel(tr("字体选择"), this);
    auto* fontCombox = new NoSlidingHandoverComboBox(this);
    fontCombox->addItems(families);
    //
    std::string sysFont = AppSetting::instance().getFont();
    if(sysFont.empty())
    {
        sysFont = defaultFont.toStdString();
        AppSetting::instance().setFont(sysFont);
    }
    if(families.contains(sysFont.data()))
        fontCombox->setCurrentText(sysFont.data());

    auto* fontLay = new QHBoxLayout();
    fontLay->addWidget(fontLabel);
    fontLay->addWidget(fontCombox);
    fontLay->addItem(new QSpacerItem(10, 10, QSizePolicy::Expanding));
    vlayout->addLayout(fontLay);
    //
    auto* scaleEnable = new SettingCheckBox(tr("高分屏适配(重启客户端生效)"), false, this);
//    auto *scaleEdit = new QSpinBox(this);
//    scaleEdit->setMinimum(50);
//    scaleEdit->setMaximum(200);
//    scaleEdit->setSingleStep(10);
//    scaleEdit->setSuffix(" %");
//    scaleEdit->setValue(100);
//    auto* scaleLabel = new QLabel(tr("画面缩放"), this);
//    auto* scaleLay = new QHBoxLayout();
//    scaleLay->addWidget(scaleLabel);
//    scaleLay->addWidget(scaleEdit);
//    scaleLay->addItem(new QSpacerItem(10, 10, QSizePolicy::Expanding));
//    vlayout->addWidget(new Line);
    vlayout->addWidget(scaleEnable);
//    vlayout->addLayout(scaleLay);

    // 缩放因子
    QSettings settings(QSettings::NativeFormat, QSettings::UserScope,
#ifdef _STARTALK
        "qunar.com", "startalk");
#else
        "qunar.com", "qtalk");
#endif
//    QString scale_factor = settings.value("QT_SCALE_FACTOR").toByteArray();
//    if(!scale_factor.isEmpty())
//    {
//        int scale = scale_factor.toDouble();
//        scaleEdit->setValue(scale);
//    }

    bool scale_enable = settings.value("QT_SCALE_ENABLE").toBool();
    scaleEnable->setChecked(scale_enable);
//    scaleEdit->setEnabled(scale_enable);

    connect(scaleEnable, &QCheckBox::stateChanged, [this](int state) {
        bool check = state == Qt::Checked;
        QSettings settings(QSettings::NativeFormat, QSettings::UserScope,
#ifdef _STARTALK
                "qunar.com", "startalk");
#else
                           "qunar.com", "qtalk");
#endif
        settings.setValue("QT_SCALE_ENABLE", check);
//        scaleEdit->setEnabled(check);
    });
//    connect(scaleEdit, SIGNAL(valueChanged(int)), this, SLOT(onScaleFactorChanged(int)));
    //
    vlayout->addItem(new QSpacerItem(10, 10, QSizePolicy::Fixed, QSizePolicy::Expanding));
    vlayout->addWidget(new Line(Qt::Horizontal, this));

    connect(blackTheme, &QCheckBox::stateChanged, [this](int state) {
        bool check = state == Qt::Checked;
        AppSetting::instance().setThemeMode(check ? 2 : 1);
    });

    connect(fontCombox, &QComboBox::currentTextChanged, [](const QString& text){
        if(!text.isEmpty())
            AppSetting::instance().setFont(text.toStdString());
    });

}

void SystemSettingWnd::onScaleFactorChanged(int value) {
    QSettings settings(QSettings::NativeFormat, QSettings::UserScope,
#ifdef _STARTALK
            "qunar.com", "startalk");
#else
                       "qunar.com", "qtalk");
#endif
    settings.setValue("QT_SCALE_FACTOR", QString::number(value).toUtf8());
}

#ifdef _WINDOWS
#include <QSettings>
void appAutoRun(bool bAutoRun)
{
    QSettings reg("HKEY_CURRENT_USER\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run",QSettings::NativeFormat);

    if (bAutoRun)
    {
        QString strAppPath=QDir::toNativeSeparators(QCoreApplication::applicationFilePath());
        reg.setValue("wirtepad",strAppPath);
    }
    else
    {
        reg.setValue("wirtepad","");
    }

}
#endif

void SystemSettingWnd::initOtherSetting(QVBoxLayout* vlayout) {

    auto *autoLoginCK = new SettingCheckBox(tr("自动登录"), AppSetting::instance().getAutoLoginEnable(), this);
    vlayout->addWidget(autoLoginCK);

#ifdef _WINDOWS
    auto *autoStartUp = new SettingCheckBox(tr("开机自启动"), AppSetting::instance().getSelfStart(), this);
    vlayout->addWidget(autoStartUp);

    connect(autoStartUp, &QCheckBox::stateChanged, [this](int state){
        appAutoRun(state == Qt::Checked);
        AppSetting::instance().setSelfStart(state == Qt::Checked);
    });
#endif

    auto *openLink = new SettingCheckBox(tr("使用内置浏览器打开对话中的链接"), AppSetting::instance().getOpenLinkWithAppBrowser(), this);
    vlayout->addWidget(openLink);

    auto *openOaLink = new SettingCheckBox(tr("使用内置浏览器打开OA链接"), AppSetting::instance().getOpenOaLinkWithAppBrowser(), this);
    vlayout->addWidget(openOaLink);

    auto *showSendMessageWnd = new SettingCheckBox(tr("显示发送按钮"), AppSetting::instance().getShowSendMessageBtnFlag(), this);
    vlayout->addWidget(showSendMessageWnd);

    auto *testChannel = new SettingCheckBox(tr("内测通道"), AppSetting::instance().getTestchannel() - 1, this);
    vlayout->addWidget(testChannel);

    auto *logLevel = new SettingCheckBox(tr("记录INFO日志"), false, this);
    vlayout->addWidget(logLevel);

    int level = AppSetting::instance().getLogLevel();
    logLevel->setChecked(1 == level);

    vlayout->addItem(new QSpacerItem(10, 10, QSizePolicy::Fixed, QSizePolicy::Expanding));
    vlayout->addWidget(new Line(Qt::Horizontal, this));

    connect(autoLoginCK, &QCheckBox::stateChanged, [this](int state){
        emit sgSetAutoLogin(state == Qt::Checked);
    });
    connect(openLink, &QCheckBox::stateChanged, [](int state){
        AppSetting::instance().setOpenLinkWithAppBrowser(state == Qt::Checked);
    });
    connect(openOaLink, &QCheckBox::stateChanged, [](int state){
        AppSetting::instance().setOpenOaLinkWithAppBrowser(state == Qt::Checked);
    });
    connect(showSendMessageWnd, &QCheckBox::stateChanged, [this](int state){
        AppSetting::instance().setShowSendMessageBtnFlag(state == Qt::Checked);
    });

    connect(testChannel, &QCheckBox::stateChanged, [this](int state){
        int testChannel = state == Qt::Checked;
        testChannel += 1;
        AppSetting::instance().setTestchannel(testChannel);
    });

    connect(logLevel, &QCheckBox::stateChanged, [this](int state){
        int level = 0;
        if(state == Qt::Checked)
            level = 1;
        else
            level = 2;
        QTalk::logger::setLevel(level);
        AppSetting::instance().setLogLevel(level);
    });

}

void deleteDictionary(const QString& dirPath)
{
    QFileInfo info(dirPath);
    if(info.isDir())
    {
        QDir baseDir(dirPath);
        auto infoList = baseDir.entryInfoList(QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot);
        for(const auto& tmpInfo : infoList)
        {
            if(tmpInfo.isDir())
            {
                deleteDictionary(tmpInfo.absoluteFilePath());
                baseDir.rmpath(tmpInfo.absoluteFilePath());
            }
            else {
                QFile::remove(tmpInfo.absoluteFilePath());
            }
        }
    }
    else
    {
        QFile::remove(dirPath);
    }
}

void deleteHistoryLogs()
{
    QString logBasePath = QString("%1/logs").arg(Platform::instance().getAppdataRoamingPath().data());

    QDir baseDir(logBasePath);
    auto infoList = baseDir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);
    for(const auto& info : infoList)
    {
        QString dirName = info.baseName();
        auto dirDate = QDate::fromString(dirName, "yyyy-MM-dd");
        auto now = QDate::currentDate();
        if(dirDate.addDays(5) <= now)
        {
            deleteDictionary(info.absoluteFilePath());
            baseDir.rmpath(info.absoluteFilePath());
        }
    }
}

void SystemSettingWnd::initVersionInfo(QVBoxLayout* vlayout) {

    QLabel *versionLbl = new QLabel(tr("当前版本:"), this);
    versionLbl->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    vlayout->addWidget(versionLbl);

    { // 检查更新
        auto *versionLay = new QHBoxLayout;
        versionLay->setSpacing(10);
        versionLay->setContentsMargins(65,0,0,0);
        vlayout->addLayout(versionLay);

        auto *versionLab = new QLabel(this);
        versionLay->addWidget(versionLab);

        std::string clientVersion = Platform::instance().getGlobalVersion();
        QString currentVersion = QString("%1 ( %2 )").arg(clientVersion.data()).arg(Platform::instance().getClientNumVerison());
        versionLab->setText(currentVersion);

        auto *build_time_label = new QLabel(this);
        build_time_label->setContentsMargins(65,0,0,0);
        vlayout->addWidget(build_time_label);

        std::string build_time = Platform::instance().get_build_date_time();
        build_time_label->setText(QString("build at: %1").arg(build_time.data()));

        auto *checkUpdateBtn = new QPushButton(tr("检查更新"));
        checkUpdateBtn->setFixedSize(80,24);
        checkUpdateBtn->setObjectName("SettingBtn");
        versionLay->addWidget(checkUpdateBtn);

        versionLay->addItem(new QSpacerItem(1, 1, QSizePolicy::Expanding));
        checkUpdateBtn->setVisible(false);
        connect(checkUpdateBtn, &QPushButton::clicked, this, &SystemSettingWnd::sgCheckUpdate);
    }

    QLabel *repairLbl = new QLabel(tr("便捷维护:"), this);
    vlayout->addWidget(repairLbl);

    { // 便捷维护
        auto *repairLay = new QHBoxLayout;
        repairLay->setAlignment(Qt::AlignmentFlag::AlignLeft);
        repairLay->setSpacing(30);
        repairLay->setContentsMargins(65,0,0,0);
        vlayout->addLayout(repairLay);

        auto *repairBtn = new QPushButton(this);
        repairBtn->setFixedHeight(24);
        repairBtn->setText(tr("清除缓存"));
        repairBtn->setObjectName("SettingBtn");
        repairLay->addWidget(repairBtn);

        auto *clearLogs = new QPushButton(this);
        clearLogs->setFixedHeight(24);
        clearLogs->setText(tr("清除日志"));
        clearLogs->setObjectName("SettingBtn");
        repairLay->addWidget(clearLogs);

        connect(repairBtn, &QPushButton::clicked, [this](){
            emit sgClearSystemCache();
        });

        connect(clearLogs, &QPushButton::clicked, [this](){
            deleteHistoryLogs();
            QtMessageBox::information(this, tr("提醒"), tr("日志清理完毕"));
//            emit sgClearSystemCache();
        });
    }

    vlayout->addItem(new QSpacerItem(10, 10, QSizePolicy::Fixed, QSizePolicy::Expanding));
    vlayout->addWidget(new Line(Qt::Horizontal,this));
}

/**
 * 意见反馈
 * @return
 */
void SystemSettingWnd::initFeedback(QVBoxLayout* vlayout) {

    QFrame *feedbackInputFrm = new QFrame(this);
    vlayout->addWidget(feedbackInputFrm);

    auto *vInputLay = new QVBoxLayout(feedbackInputFrm);
    vInputLay->setMargin(0);

    auto *inputEdit = new QTextEdit(this);
    inputEdit->setAcceptRichText(false);
    inputEdit->setPlaceholderText(tr("请输入反馈描述"));
    inputEdit->setObjectName("FeedbackEdit");
    inputEdit->setFixedHeight(100);
    vInputLay->addWidget(inputEdit);

     // 提交按钮
    auto *hLayout = new QHBoxLayout;
    hLayout->setAlignment(Qt::AlignmentFlag::AlignRight);
    vInputLay->addLayout(hLayout);
    auto *submitBtn = new QPushButton(tr("提交"), this);
    submitBtn->setObjectName("SubmitBtn");
    submitBtn->setFixedSize(72,32);
    hLayout->addWidget(submitBtn);

    connect(submitBtn, &QPushButton::clicked, [this, inputEdit](){
        QString strText = inputEdit->toPlainText();
        emit sgFeedbackLog(strText);
        inputEdit->clear();
        this->setVisible(false);
    });


    vlayout->addItem(new QSpacerItem(10, 10, QSizePolicy::Fixed, QSizePolicy::Expanding));
}

void SystemSettingWnd::initSeat(QVBoxLayout *vlayout) {
//    QFrame *seatFrame = new QFrame(this);
//    vlayout->addWidget(seatFrame);
//
//    auto *vSeatLay = new QVBoxLayout(seatFrame);
//    vSeatLay->setMargin(0);

    std::string seats = Platform::instance().getSeats();
    if(!seats.empty()){
        QJsonDocument jsonDocument = QJsonDocument::fromJson(seats.data());
        if (!jsonDocument.isNull()){
            QJsonObject jsonObject = jsonDocument.object();
            QJsonArray datas = jsonObject.value("data").toArray();
            for(int i = 0;i<datas.size();i++){
                QJsonObject seat = datas.at(i).toObject();

                QLabel* title = new QLabel;
                title->setText(tr("店铺名：") + seat.value("sname").toString());

                vlayout->addWidget(title);

                int sid = seat.value("sid").toInt() * 10;

                QButtonGroup* group = new QButtonGroup(vlayout);
                SettingCheckBox* radioButton1 = new SettingCheckBox(tr("标准模式 (在线时才接收咨询，默认)"));
                SettingCheckBox* radioButton2 = new SettingCheckBox(tr("超人模式 (不在线也接受咨询)"));
                SettingCheckBox* radioButton3 = new SettingCheckBox(tr("勿扰模式 (在线也不接收咨询)"));
                group->addButton(radioButton1,sid + 0);
                group->addButton(radioButton2,sid + 4);
                group->addButton(radioButton3,sid + 1);
                vlayout->addWidget(radioButton1);
                vlayout->addWidget(radioButton2);
                vlayout->addWidget(radioButton3);

                int st = seat.value("st").toInt(0);
                if(st == 4){
                    radioButton2->setChecked(true);
                } else if(st == 1){
                    radioButton3->setChecked(true);
                } else{
                    radioButton1->setChecked(true);
                }
                connect(group, SIGNAL(buttonToggled(int,bool)), this, SLOT(operatingModeButtonsToggled(int,bool)));
            }
        }
    }
}

void SystemSettingWnd::operatingModeButtonsToggled(int id, bool status)
{
    if(status){
        int seat = id % 10;
        int sid = id / 10;
        if(_pMessageManager){
            _pMessageManager->setServiceSeat(sid,seat);
        }
    }
}