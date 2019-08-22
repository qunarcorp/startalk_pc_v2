//
// Created by admin on 2019-02-22.
//

#ifndef QTALK_V2_APPSETTING_H
#define QTALK_V2_APPSETTING_H

#include <string>
#include "platform_global.h"
#include "../include/Spinlock.h"

class PLATFORMSHARED_EXPORT AppSetting{

public:
    AppSetting() ;

public:

    enum MsgFontSize {
        Normal,
        Big,
        MostBig,
    };

    enum SendMsgKey {
        Enter,
        CtrlEnter,
    };

    enum AddFriendMode {
        AllAgree,           //全部接受
        QuestionAndAnswer,  //需要问答
        NeedAgree,          //需要确认
    };

public:
    std::string saveAppSetting();
	void initAppSetting(const std::string& setting);
    // 消息通知
    bool getNewMsgAudioNotify();     //新消息声音提醒
    void setNewMsgAudioNotify(bool flag);
	std::string getgetNewMsgAudioPath();   //
	void setgetNewMsgAudioPath(const std::string& path);   //
    bool getNewMsgTipWindowNotify();    
    void setNewMsgTipWindowNotify(bool flag);
    bool getPhoneAnyReceive();       //手机端随时接收推送
    void setPhoneAnyReceive(bool flag);
    bool getStrongWarnFlag();
    void setStrongWarnFlag(bool flag);

    // 热键设置
    bool getHotCutEnable();          //热键是否开启
    void setHotCutEnable(bool flag);
    std::string getScreenshotHotKey();        //截图快捷键
    void setScreenshotHotKey(const std::string& hotKey);
    std::string getWakeWndHotKey();        //截图快捷键
    void setWakeWndHotKey(const std::string& hotKey);
    int  getSearchUserFlag();        //搜索快捷键
    void setSearchUserFlag(int flagValue);
    int  getSearchUserKey();
    void setSearchUserKey(int keyValue);

    void setScreentShotHideWndFlag(bool hide);
    bool getScreentShotHideWndFlag();

    // 会话设置
    void setSendMessageKey(std::string key); // 发送快捷键
    std::string getSendMessageKey();
    void setShowMoodFlag(bool flag);
    bool getShowMoodFlag();

    bool getSessionShowTopAndUnread(); //会话列表紧显示未读和置顶
    void setSessionShowTopAndUnread(bool flag);
    bool getSessionListLimitEnable();  //是否开启会话列表加载的会话数
    void setSessionListLimitEnable(bool flag);
    int  getSessionListLimit();        //会话列表最大会话数
    void setSessionListLimit(int limit);
    SendMsgKey getSendMsgType();      //发送消息类型
    void setSendMsgType(SendMsgKey key);
    bool getSessionAutoRecover();      //是否开启会话自动回收
    void setSessionAutoRecover(bool flag);
    int  getSessionAutoRecoverCount(); //同时存在的会话数
    void setSessionAutoRecoverCount(int count);
    bool getMsgHistoryLimitEnable();   //是否开启会话框消息条数限制
    void setMsgHistoryLimitEnable(bool flag);
    int  getMsgHistoryLimit();         //会话框历史消息条数
    void setMsgHistoryLimit(int limit);
    bool getMsgFontSetEnable();        //是否开启消息字号设置
    void setMsgFontSetEnable(bool flag);
    MsgFontSize getMsgFontSize();      //字号
    void setMsgFontSize(MsgFontSize size);
    // 自动回复
    bool getLeaveCheckEnable();
    void setLeaveCheckEnable(bool flag);
    int  getLeaveMinute();
    void setLeaveMinute(int minute);
    bool getAutoReplyPreset();
    void setAutoReplyPreset(bool flag);
    std::string getAutoReplyMsg();
    void setAutoReplyMsg(std::string msg);
    std::string getAutoReplyCusMsg();
    void setAutoReplyCusMsg(std::string msg);
    // 文件目录
    std::string getUserDirectory();
    void setUserDirectory(const std::string& userDirectory);
    std::string getFileSaveDirectory();
    void setFileSaveDirectory(const std::string& fileSaveDirectory);
    // 好友权限
    AddFriendMode getAddFriendMode();
    void setAddFriendMode(AddFriendMode mode);
    std::string getQuestion();
    void setQuestion(std::string question);
    std::string getAnswer();
    void setAnswer(std::string answer);
    // 皮肤设置
    int getThemeMode();
    void setThemeMode(int mode);
    std::string getFont();
    void setFont(const std::string& font);
    // 其他设置
    bool getSelfStart();            // 开机自启动
    void setSelfStart(bool flag);

    void setLogLevel(int level);
    int getLogLevel();

    bool getExitMainClose();        // 关闭会话框，退出程序
    void setExitMainClose(bool flag);
    bool getGroupDestroyNotify();   // 群组销毁提示
    void setGroupDestroyNotify(bool flag);
    bool getShockWindow();          // 窗口震动
    void setShockWindow(bool flag);
    bool getRecordLogEnable();      // 是否记录日志
    void setRecordLogEnable(bool flag);
    bool getESCCloseMain();         // ESC是否退出主窗口
    void setESCCloseMain(bool flag);
    bool getShowGroupMembers();     // 是否展示群成员列表
    void setShowGroupMembers(bool flag);
    bool getSwitchChatScrollToBottom(); //切换会话是否滚动到最低下
    void setSwitchChatScrollToBottom(bool flag);
    bool getAutoLoginEnable();            // 是否自动登录
    void setAutoLoginEnable(bool flag);

    bool getOpenLinkWithAppBrowser(); // 使用内置浏览器打开聊天链接
    void setOpenLinkWithAppBrowser(bool flag);

    bool getOpenOaLinkWithAppBrowser(); // 使用内置浏览器打开OA链接
    void setOpenOaLinkWithAppBrowser(bool flag);

    bool getShowSendMessageBtnFlag(); // 显示发送按钮
    void setShowSendMessageBtnFlag(bool flag);

    int getTestchannel() { return _channel;};
    void setTestchannel(int channel) {_channel = channel;};

private:

    // 消息通知
    bool _newMsgAudioNotify;     //新消息声音提醒
	std::string _newMsgAudioPath;
    bool _newMsgTipWindowNotify;    //新消息声音提醒
    bool _phoneAnyReceive;       //手机端随时接收推送
    bool _strongWarn;

    // 热键设置
    bool _hotCutEnable;          //热键是否开启
    std::string  _screenshotKey; // 截图快捷键
    std::string  _wakeWndKey; // 唤醒屏幕快捷键
    int  _searchUserFlag;        //搜索快捷键
    int  _searchUserKey;
    bool _screentShotHideWndFlag;

    // 会话设置
    std::string _sendMessageKey;
    bool _showMoodFlag;

    bool _sessionShowTopAndUnread; //会话列表紧显示未读和置顶
    bool _sessionListLimitEnable;  //是否开启会话列表加载的会话数
    int  _sessionListLimit;        //会话列表最大会话数
    SendMsgKey  _sendMsgType;      //发送消息类型
    bool _sessionAutoRecover;      //是否开启会话自动回收
    int  _sessionAutoRecoverCount; //同时存在的会话数
    bool _msgHistoryLimitEnable;   //是否开启会话框消息条数限制
    int  _msgHistoryLimit;         //会话框历史消息条数
    bool _msgFontSetEnable;        //是否开启消息字号设置
    MsgFontSize _msgFontSize;      //字号

    // 自动回复
    bool _leaveCheckEnable;
    int  _leaveMinute;
    bool _autoReplyPreset;
    std::string _autoReplyMsg;
    std::string _autoReplyCusMsg;

    // 文件目录
    std::string _userDirectory;
    std::string _fileSaveDirectory;

    // 好友权限
    AddFriendMode _addFriendMode;
    std::string _question;
    std::string _answer;

    // 皮肤设置
    int _themeMode;
    std::string _font;

    // 其他设置
    bool _autoStartUp;            // 开机自启动
    bool _exitMainClose;        // 关闭会话框，退出程序
    bool _groupDestroyNotify;   // 群组销毁提示
    bool _shockWindow;          // 窗口震动
    bool _recordLogEnable;      // 是否记录日志
    bool _escCloseMain;         // ESC是否退出主窗口
    bool _showGroupMembers;     // 是否展示群成员列表
    bool _switchChatScrollToBottom; //切换会话是否滚动到最低下
    bool _autoLoginEnable;            // 是否自动登录
    bool _openWithAppBrowser;
    bool _openOaWithAppBrowser;
    bool _showSendMessageBtn;
    int _channel;
    int _logLevel;

private:
	static AppSetting* _appSetting;
    QTalk::util::spin_mutex sm;

public:
	static AppSetting& instance();
};


#endif //QTALK_V2_APPSETTING_H
