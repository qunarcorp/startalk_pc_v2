#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif

#ifndef MAINPANEL_H
#define MAINPANEL_H

#include <QFrame>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QToolButton>
#include "../include/CommonStrcut.h"
#include "SeachEditPanel.h"
#include "SearchEdit.h"
#include "SearchResultPanel.h"
#include "../UICom/UIEntity.h"
#include "../CustomUi/HeadPhotoLab.h"
#include "DropMenu.h"


class MainPanelProperty
{
public:
    MainPanelProperty()
    {
        _mainFrmHeight = 52;
        _searchFrmSize = QSize(198, 30);
        _searchBtnSize = QSize(20, 20);
        _qrCodeBtnSize = QSize(30, 30);
        _sessionBtnSize = QSize(30, 30);
        _contactBtnSize = QSize(30, 30);
        _multifunctionBtnSize = QSize(30, 30);
        _userBtnSize = QSize(36, 36);
        _maxminFrmSize = QSize(120, 52);
#ifdef _MACOS
        _maxminBtnsSize = QSize(12, 12);
#else
        _maxminBtnsSize = QSize(30, 30);
#endif
    }
    int   _mainFrmHeight;
    QSize _searchFrmSize;
    QSize _searchBtnSize;
    QSize _qrCodeBtnSize;
    QSize _sessionBtnSize;
    QSize _contactBtnSize;
    QSize _multifunctionBtnSize;
    QSize _userBtnSize;
    QSize _maxminFrmSize;
    QSize _maxminBtnsSize;
};

class TitlebarMsgManager;
class TitlebarMsgListener;
class SessionBtn;
class ChangeHeadWnd;

class MainPanel : public QFrame
{
    Q_OBJECT

public:
    explicit MainPanel(QWidget *parent = 0);
    ~MainPanel() override;

public:
	void setCtrlWdt(QWidget *wdt);
	void getSelfCard();
	void recvUserCard(const std::vector<QTalk::StUserCard>& userCards);
	void onChangeHeadRet(bool ret, const std::string& locaHead);
    void recvSwitchUserStatus(const std::string&);

public slots:
	void onSwitchFunc(int);
    void updateUnreadCount(int);
    void onShowHeadWnd(const QString& headPath, bool isSelf);
    void onMousePressGolbalPos(const QPoint &gpos);
    void onSwitchUserStatus(const QString&);

protected:
    void mousePressEvent(QMouseEvent *me) override;
    void mouseDoubleClickEvent(QMouseEvent* e) override;
    void mouseMoveEvent(QMouseEvent *me) override;
    void mouseReleaseEvent(QMouseEvent *m) override;
    bool eventFilter(QObject* o, QEvent* e) override;

private:
    void init();
    void connects();
    void adaptiveResolution();

private slots:
    void showSmall();
    void showMaxRestore();
    void onCtrlWdtClose();
    void onAppDeactivated();
    void onSearchResultVisible(const bool & visible);
    void onOpenNewSession(const StSessionInfo &into);
    void setNewHead(const QString& userName, const QString& newHeadPath);
    void onTabGroupClicked(int tab);

signals:
    void sgCurFunChanged(const int &index);
    void showSelfUserCard(const QString&);
    void sgOpenNewSession(const StSessionInfo &into);
    void sgJumpToNewMessage();
    void setHeadSignal(const QString&, const QString&);
    void feedbackLog(const QString&);
    void creatGroup(const QString&);
	void systemQuitSignal();
	void sgCheckUpdate();
	void sgSetAutoLogin(bool);
    void sgUpdateHotKey();
    void sgSaveSysConfig();
    void sgActiveWnd();
    void msgSoundChanged();
    void sgOperator(const QString& desc);
    void sgSwitchUserStatusRet(const QString& status);
    void sgAutoReply(bool);

private:
    QFrame * _leftCorFrm;
    SeachEditPanel * _searchFrm;
    QToolButton * _creatGroupBtn; // 二维码
    SessionBtn * _sessionBtn; // 会话
    QToolButton * _contactBtn; // 联系人
    QToolButton * _multifunctionBtn; // 多功能
    HeadPhotoLab *_userBtn;
    QFrame * _maxminFrm;
    QToolButton * _minimizeBtn;
    QToolButton * _maximizeBtn;
    QToolButton * _restoreBtn;
    QToolButton * _closeBtn;
    QHBoxLayout * _hbox;

    QWidget *_pCtrlWdt;
    MainPanelProperty _property;
    SearchResultPanel *_pSearchResultPanel;
    //
private:
    DropMenu*    _dropMenu;

private:
    AboutWnd* _pAboutWnd;
    SystemSettingWnd* _pSystemSettingWnd;
    ChangeHeadWnd* _pChangeHeadWnd;

private:
	TitlebarMsgManager*  _pMessageManager;
	TitlebarMsgListener* _pMessageListener;

private:
    QPoint chickPos;
    bool _press;

private:
    QString _headPath;
};

#endif // MAINPANEL_H
