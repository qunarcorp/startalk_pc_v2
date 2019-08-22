//
// Created by admin on 2019-02-19.
//

#ifndef QTALK_V2_SYSTEMSETTINGWND_H
#define QTALK_V2_SYSTEMSETTINGWND_H

#include "../CustomUi/UShadowWnd.h"
#include <QMap>
#include <QCheckBox>
#include "../Platform/AppSetting.h"
#include "MessageManager.h"

class QVBoxLayout;
class QListWidget;
class ActionLabel;
class QListWidgetItem;
class SKRecorder;
class QTextEdit;
class QComboBox;
class SettingCheckBox : public QCheckBox
{
public:
    explicit SettingCheckBox(const QString& text = QString(), bool defaultVal = false, QWidget* parent = nullptr)
        :QCheckBox(text, parent)
    {
        setChecked(defaultVal);
        setObjectName("SettingCheckBox");
    }
    ~SettingCheckBox() override = default;

};

class SystemSettingWnd  : public UShadowDialog{
    Q_OBJECT
public:
    explicit SystemSettingWnd(TitlebarMsgManager *messageManager,QWidget * parent = nullptr);
    ~SystemSettingWnd() override;

signals:
    void sgSetAutoLogin(bool);
    void sgFeedbackLog(const QString&);
    void sgCheckUpdate();
    void sgUpdateHotKey();
	void saveConfig();
    void sgClearSystemCache();
    void msgSoundChanged();

protected:
    bool eventFilter(QObject* o, QEvent* e) override;
	void hideEvent(QHideEvent* e) override;

private slots:
    void operatingModeButtonsToggled(int, bool);

private:
    void initUi();
    void initSetting(int type);

private:
    void initMsgNotify(QVBoxLayout* itemLay); //消息通知UI
    void initHotCut(QVBoxLayout* itemLay); //热键设置
    void initSession(QVBoxLayout* itemLay); //会话设置
    void initAutoReply(QVBoxLayout* itemLay); //自动回复
    void initFolder(QVBoxLayout* itemLay); //文件目录
    void initFriendAuthority(QVBoxLayout* itemLay); //好友权限
    void initFontSetting(QVBoxLayout* itemLay); //字体设置
    void initOtherSetting(QVBoxLayout* itemLay); //其他设置
    void initVersionInfo(QVBoxLayout* itemLay); //版本信息
    void initFeedback(QVBoxLayout* itemLay); //意见反馈
    void initSeat(QVBoxLayout* itemLay);//Qchat坐席状态设置

private:
    SKRecorder* addSortCutItem(const QString& text, const QKeySequence& keys, QVBoxLayout* layout,std::function<void(const QKeySequence& keys)> callback);

    private slots:
    void setAutoReplyMessage();
    void onAutoReplyClicked(int);
    void onLeveMinuteChanged(int);

private:
    QListWidget *_settingListWidget;
    QVBoxLayout * _pNavlayout;
    QMap<QListWidgetItem*, ActionLabel*> _mapSettingItems;

private:
    QTextEdit* _pAutoReplyEdit;
    QComboBox* _pAutoPeplyCombobox;

    SettingCheckBox* blackTheme;

private:
    TitlebarMsgManager *_pMessageManager;

private:

    enum settingType
    {
        EM_SETTING_MESSAGE,
        EM_SETTING_HOTKEY,
        EM_SETTING_AUTOREPLY,
        EM_SETTING_SESSION,
        EM_SETTING_FOLDER,

#ifdef _QCHAT
        EM_SETTING_SEAT,
#endif
        EM_SETTING_FONT,
        EM_SETTING_OTHERSETTING,
        EM_SETTING_VERSION,
        EM_SETTING_SUGGESTION,
        // todo 暂时不显示

        EM_SETTING_FRIEND,
    };

    enum {EM_USER_FOLDER, EM_FILE_FOLDER};
};


#endif //QTALK_V2_SYSTEMSETTINGWND_H
