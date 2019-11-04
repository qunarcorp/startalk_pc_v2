//
// Created by admin on 2019-02-22.
//

#include "AppSetting.h"
#include "../QtUtil/lib/cjson/cJSON.h"
#include "../QtUtil/lib/cjson/cJSON_inc.h"

AppSetting* AppSetting::_appSetting = nullptr;

AppSetting::AppSetting()
: _newMsgAudioNotify(true)
, _newMsgTipWindowNotify(true)
, _phoneAnyReceive(false)
, _openWithAppBrowser(true)
, _hotCutEnable(true)
, _sessionShowTopAndUnread(false) //会话列表紧显示未读和置顶
, _sessionListLimitEnable(false)  //是否开启会话列表加载的会话数
, _sessionListLimit(false)        //会话列表最大会话数
, _sendMsgType(AppSetting::SendMsgKey::Enter)      //发送消息类型
, _sessionAutoRecover(false)      //是否开启会话自动回收
, _sessionAutoRecoverCount(500) //同时存在的会话数
, _msgHistoryLimitEnable(false)   //是否开启会话框消息条数限制
, _msgHistoryLimit(500)         //会话框历史消息条数
, _msgFontSetEnable(false)        //是否开启消息字号设置
, _msgFontSize(AppSetting::MsgFontSize::Normal)      //字号
, _leaveCheckEnable(false)
, _leaveMinute(5)
, _autoReplyMsg("不在办公室")
, _addFriendMode(AppSetting::AddFriendMode::AllAgree)
, _themeMode(1)
, _autoStartUp(false)            // 开机自启动
, _exitMainClose(false)        // 关闭会话框，退出程序
, _groupDestroyNotify(true)   // 群组销毁提示
, _shockWindow(true)          // 窗口震动
, _recordLogEnable(false)      // 是否记录日志
, _escCloseMain(false)         // ESC是否退出主窗口
, _showGroupMembers(true)     // 是否展示群成员列表
, _switchChatScrollToBottom(false) //切换会话是否滚动到最低下
, _autoLoginEnable(false)            // 是否自动登录
, _openOaWithAppBrowser(true)
, _screentShotHideWndFlag(false)
, _showSendMessageBtn(false)
, _strongWarn(true)
, _showMoodFlag(true)
, _autoReplyPreset(true)
, _supportNativeMessagePrompt(false)
, _useNativeMessagePrompt(false)
, _autoDeleteSession(true)
{

}

void AppSetting::initAppSetting(const std::string& setting)
{
	cJSON* obj = cJSON_Parse(setting.data());
	if (obj)
	{
        std::lock_guard<QTalk::util::spin_mutex> lock(sm);
		_newMsgAudioNotify = (bool)QTalk::JSON::cJSON_SafeGetBoolValue(obj, "NEWMSGAUDIO", _newMsgAudioNotify);
		_newMsgAudioPath = QTalk::JSON::cJSON_SafeGetStringValue(obj, "NEWMSGAUDIOPATH", _newMsgAudioPath.data());
		_newMsgTipWindowNotify = (bool)QTalk::JSON::cJSON_SafeGetBoolValue(obj, "NEWMSGTIPWINDOW", _newMsgTipWindowNotify);
		_screenshotKey = QTalk::JSON::cJSON_SafeGetStringValue(obj, "SCREENSHOT", _screenshotKey.data());
		_wakeWndKey = QTalk::JSON::cJSON_SafeGetStringValue(obj, "WAKEWND", _wakeWndKey.data());
		_openWithAppBrowser = (bool)QTalk::JSON::cJSON_SafeGetBoolValue(obj, "OPENWITHAPPBROWSER", _openWithAppBrowser);
		_openOaWithAppBrowser = (bool)QTalk::JSON::cJSON_SafeGetBoolValue(obj, "OPENOAWITHAPPBROWSER", _openOaWithAppBrowser);
        _screentShotHideWndFlag = (bool)QTalk::JSON::cJSON_SafeGetBoolValue(obj, "SCREENTSHOTHIDEWNDFLAG", _screentShotHideWndFlag);
        _showSendMessageBtn = (bool)QTalk::JSON::cJSON_SafeGetBoolValue(obj, "SHOWSENDMESSAGEBTN", _showSendMessageBtn);
        _strongWarn = (bool)QTalk::JSON::cJSON_SafeGetBoolValue(obj, "STRONGWARN", _strongWarn);
        _hotCutEnable = (bool)QTalk::JSON::cJSON_SafeGetBoolValue(obj, "HOTCUTENABLE", _hotCutEnable);
        _sendMessageKey = QTalk::JSON::cJSON_SafeGetStringValue(obj, "SENDMESSAGEKEY", "Enter");
        _showMoodFlag = (bool)QTalk::JSON::cJSON_SafeGetBoolValue(obj, "SHOWMOODFLAG", true);
        _autoStartUp = (bool)QTalk::JSON::cJSON_SafeGetBoolValue(obj, "AUTOSTARTUP", false);
        _leaveCheckEnable = (bool)QTalk::JSON::cJSON_SafeGetBoolValue(obj, "LEAVECHECKENABLE", false);
        _autoReplyPreset = (bool)QTalk::JSON::cJSON_SafeGetBoolValue(obj, "AUTOREPLYPRESET", true);
        _autoReplyMsg = QTalk::JSON::cJSON_SafeGetStringValue(obj, "AUTOREPLYMSG");
        _autoReplyCusMsg = QTalk::JSON::cJSON_SafeGetStringValue(obj, "AUTOREPLYCUSMSG");
        _leaveMinute = QTalk::JSON::cJSON_SafeGetIntValue(obj, "LEAVEMINUTE", 5);
        _useNativeMessagePrompt = QTalk::JSON::cJSON_SafeGetBoolValue(obj, "USENATIVEMESSAGEPROMPT", false);
        _autoDeleteSession = QTalk::JSON::cJSON_SafeGetBoolValue(obj, "AUTODELETESESSION", true);

		cJSON_Delete(obj);
	}
}

AppSetting & AppSetting::instance()
{
	if (nullptr == _appSetting)
	{
		static AppSetting appSetting;
		_appSetting = &appSetting;
	}
	return *_appSetting;
	
}

// 消息通知
bool  AppSetting::getNewMsgAudioNotify() {
    std::lock_guard<QTalk::util::spin_mutex> lock(sm);
    return _newMsgAudioNotify;
} //新消息声音提醒

void AppSetting::setNewMsgAudioNotify(bool flag) {
    std::lock_guard<QTalk::util::spin_mutex> lock(sm);
    _newMsgAudioNotify = flag;
}

std::string AppSetting::getgetNewMsgAudioPath()
{
	return _newMsgAudioPath;
}

void AppSetting::setgetNewMsgAudioPath(const std::string & path)
{
	_newMsgAudioPath = path;
}

bool AppSetting::getNewMsgTipWindowNotify() {
    std::lock_guard<QTalk::util::spin_mutex> lock(sm);
    return _newMsgTipWindowNotify;
}    //新消息提示窗

void AppSetting::setNewMsgTipWindowNotify(bool flag) {
    std::lock_guard<QTalk::util::spin_mutex> lock(sm);
    _newMsgTipWindowNotify = flag;
}

bool AppSetting::getPhoneAnyReceive() {
    std::lock_guard<QTalk::util::spin_mutex> lock(sm);
    return _phoneAnyReceive;
}       //手机端随时接收推送

void AppSetting::setPhoneAnyReceive(bool flag) {
    std::lock_guard<QTalk::util::spin_mutex> lock(sm);
    _phoneAnyReceive = flag;
}

void AppSetting::setStrongWarnFlag(bool flag)
{
    std::lock_guard<QTalk::util::spin_mutex> lock(sm);
    _strongWarn = flag;
}

bool AppSetting::getStrongWarnFlag()
{
    std::lock_guard<QTalk::util::spin_mutex> lock(sm);
    return _strongWarn;
}

// 热键设置
bool AppSetting::getHotCutEnable() {
    std::lock_guard<QTalk::util::spin_mutex> lock(sm);
    return _hotCutEnable;
}          //热键是否开启

void AppSetting::setHotCutEnable(bool flag) {
    std::lock_guard<QTalk::util::spin_mutex> lock(sm);
    _hotCutEnable = flag;
}

std::string  AppSetting::getScreenshotHotKey() {
    std::lock_guard<QTalk::util::spin_mutex> lock(sm);
    return _screenshotKey;
}        //截图快捷键

void AppSetting::setScreenshotHotKey(const std::string& hotKey) {
    std::lock_guard<QTalk::util::spin_mutex> lock(sm);
    _screenshotKey = hotKey;
}

std::string  AppSetting::getWakeWndHotKey() {
    std::lock_guard<QTalk::util::spin_mutex> lock(sm);
    return _wakeWndKey;
}

void AppSetting::setWakeWndHotKey(const std::string& hotKey) {
    std::lock_guard<QTalk::util::spin_mutex> lock(sm);
    _wakeWndKey = hotKey;
}

int  AppSetting::getSearchUserFlag() {
    return _searchUserFlag;
}        //搜索快捷键

void AppSetting::setSearchUserFlag(int flagValue) {
    _searchUserFlag = flagValue;
}

int  AppSetting::getSearchUserKey() {
    return _searchUserKey;
}

void AppSetting::setSearchUserKey(int keyValue) {
    _searchUserKey = keyValue;
}

// 会话设置
bool AppSetting::getSessionShowTopAndUnread() {
    return _sessionShowTopAndUnread;
} //会话列表紧显示未读和置顶

void AppSetting::setSessionShowTopAndUnread(bool flag) {
    _sessionShowTopAndUnread = flag;
}

bool AppSetting::getSessionListLimitEnable() {
    return _sessionListLimitEnable;
}  //是否开启会话列表加载的会话数

void AppSetting::setSessionListLimitEnable(bool flag) {
    _sessionListLimitEnable = flag;
}

int  AppSetting::getSessionListLimit() {
    return _sessionListLimit;
}        //会话列表最大会话数

void AppSetting::setSessionListLimit(int limit){
    _sessionListLimit = limit;
}

AppSetting::SendMsgKey AppSetting::getSendMsgType() {
    return _sendMsgType;
}      //发送消息类型

void AppSetting::setSendMsgType(AppSetting::SendMsgKey key) {
    _sendMsgType = key;
}

bool AppSetting::getSessionAutoRecover() {
    return _sessionAutoRecover;
}      //是否开启会话自动回收

void AppSetting::setSessionAutoRecover(bool flag) {
    _sessionAutoRecover = flag;
}

int  AppSetting::getSessionAutoRecoverCount() {
    return _sessionAutoRecoverCount;
} //同时存在的会话数

void AppSetting::setSessionAutoRecoverCount(int count) {
    _sessionAutoRecoverCount = count;
}

bool AppSetting::getMsgHistoryLimitEnable() {
    return _msgHistoryLimitEnable;
}   //是否开启会话框消息条数限制

void AppSetting::setMsgHistoryLimitEnable(bool flag) {
    _msgHistoryLimitEnable = flag;
}

int  AppSetting::getMsgHistoryLimit() {
    return _msgHistoryLimit;
}         //会话框历史消息条数

void AppSetting::setMsgHistoryLimit(int limit) {
    _msgHistoryLimit = limit;
}

bool AppSetting::getMsgFontSetEnable() {
    return _msgFontSetEnable;
}        //是否开启消息字号设置

void AppSetting::setMsgFontSetEnable(bool flag) {
    _msgFontSetEnable = flag;
}

AppSetting::MsgFontSize AppSetting::getMsgFontSize() {
    return _msgFontSize;
}      //字号

void AppSetting::setMsgFontSize(AppSetting::MsgFontSize size) {
    _msgFontSize = size;
}

// 自动回复
bool AppSetting::getLeaveCheckEnable() {
    std::lock_guard<QTalk::util::spin_mutex> lock(sm);
    return _leaveCheckEnable;
}

void AppSetting::setLeaveCheckEnable(bool flag) {
    std::lock_guard<QTalk::util::spin_mutex> lock(sm);
    _leaveCheckEnable = flag;
}

int AppSetting::getLeaveMinute() {
    std::lock_guard<QTalk::util::spin_mutex> lock(sm);
    return _leaveMinute;
}

void AppSetting::setLeaveMinute(int minute) {
    std::lock_guard<QTalk::util::spin_mutex> lock(sm);
    _leaveMinute = minute;
}

bool AppSetting::getAutoReplyPreset() {
    std::lock_guard<QTalk::util::spin_mutex> lock(sm);
    return _autoReplyPreset;
}

void AppSetting::setAutoReplyPreset(bool flag) {
    std::lock_guard<QTalk::util::spin_mutex> lock(sm);
    _autoReplyPreset = flag;
}

std::string AppSetting::getAutoReplyMsg() {
    std::lock_guard<QTalk::util::spin_mutex> lock(sm);
    return _autoReplyMsg;
}

void AppSetting::setAutoReplyMsg(std::string msg) {
    std::lock_guard<QTalk::util::spin_mutex> lock(sm);
    _autoReplyMsg = std::move(msg);
}

std::string AppSetting::getAutoReplyCusMsg() {
    std::lock_guard<QTalk::util::spin_mutex> lock(sm);
    return _autoReplyCusMsg;
}

void AppSetting::setAutoReplyCusMsg(std::string msg) {
    std::lock_guard<QTalk::util::spin_mutex> lock(sm);
    _autoReplyCusMsg = std::move(msg);
}

// 文件目录
std::string AppSetting::getUserDirectory() {
    std::lock_guard<QTalk::util::spin_mutex> lock(sm);
    return _userDirectory;
}

void AppSetting::setUserDirectory(const std::string& userDirectory) {
    std::lock_guard<QTalk::util::spin_mutex> lock(sm);
    _userDirectory = userDirectory;
}

std::string AppSetting::getFileSaveDirectory() {
    std::lock_guard<QTalk::util::spin_mutex> lock(sm);
    return _fileSaveDirectory;
}

void AppSetting::setFileSaveDirectory(const std::string& fileSaveDirectory) {
    std::lock_guard<QTalk::util::spin_mutex> lock(sm);
    _fileSaveDirectory = fileSaveDirectory;
}

// 好友权限
AppSetting::AddFriendMode AppSetting::getAddFriendMode() {
    return _addFriendMode;
}

void AppSetting::setAddFriendMode(AddFriendMode mode) {
    _addFriendMode = mode;
}

std::string AppSetting::getQuestion() {
    return _question;
}

void AppSetting::setQuestion(std::string question) {
    _question = question;
}

std::string AppSetting::getAnswer() {
    return _answer;
}

void AppSetting::setAnswer(std::string answer) {
    _answer = answer;
}

// 皮肤设置
int AppSetting::getThemeMode() {
    std::lock_guard<QTalk::util::spin_mutex> lock(sm);
    return _themeMode;
}

void AppSetting::setThemeMode(int mode) {
    std::lock_guard<QTalk::util::spin_mutex> lock(sm);
    _themeMode = mode;
}

std::string AppSetting::getFont() {
    std::lock_guard<QTalk::util::spin_mutex> lock(sm);
    return _font;
}

void AppSetting::setFont(const std::string& font) {
    std::lock_guard<QTalk::util::spin_mutex> lock(sm);
    _font = font;
}

// 其他设置
bool AppSetting::getSelfStart() {
    std::lock_guard<QTalk::util::spin_mutex> lock(sm);
    return _autoStartUp;
}

// 开机自启动
void AppSetting::setSelfStart(bool flag) {
    std::lock_guard<QTalk::util::spin_mutex> lock(sm);
    _autoStartUp = flag;
}

//
void AppSetting::setLogLevel(int level)
{
    std::lock_guard<QTalk::util::spin_mutex> lock(sm);
    _logLevel = level;
}

//
int AppSetting::getLogLevel()
{
    std::lock_guard<QTalk::util::spin_mutex> lock(sm);
    return _logLevel;
}


bool AppSetting::getExitMainClose() {
    return _exitMainClose;
}        // 关闭会话框，退出程序

void AppSetting::setExitMainClose(bool flag) {
    _exitMainClose = flag;
}

bool AppSetting::getGroupDestroyNotify() {
    return _groupDestroyNotify;
}   // 群组销毁提示

void AppSetting::setGroupDestroyNotify(bool flag) {
    _groupDestroyNotify = flag;
}

bool AppSetting::getShockWindow() {
    return _shockWindow;
}          // 窗口震动

void AppSetting::setShockWindow(bool flag) {
    _shockWindow = flag;
}

bool AppSetting::getRecordLogEnable() {
    return _recordLogEnable;
}      // 是否记录日志

void AppSetting::setRecordLogEnable(bool flag) {
    _recordLogEnable = flag;
}

bool AppSetting::getESCCloseMain() {
    return _escCloseMain;
}         // ESC是否退出主窗口

void AppSetting::setESCCloseMain(bool flag) {
    _escCloseMain = flag;
}

bool AppSetting::getShowGroupMembers() {
    return _showGroupMembers;
}     // 是否展示群成员列表

void AppSetting::setShowGroupMembers(bool flag) {
    _showGroupMembers = flag;
}

bool AppSetting::getSwitchChatScrollToBottom() {
    return _switchChatScrollToBottom;
} //切换会话是否滚动到最低下

void AppSetting::setSwitchChatScrollToBottom(bool flag) {
    _switchChatScrollToBottom = flag;
}

bool AppSetting::getAutoLoginEnable() {
    std::lock_guard<QTalk::util::spin_mutex> lock(sm);
    return _autoLoginEnable;
}            // 是否自动登录

void AppSetting::setAutoLoginEnable(bool flag) {
    std::lock_guard<QTalk::util::spin_mutex> lock(sm);
    _autoLoginEnable = flag;
}

bool AppSetting::getOpenLinkWithAppBrowser()
{
    std::lock_guard<QTalk::util::spin_mutex> lock(sm);
    return _openWithAppBrowser;
}

void AppSetting::setOpenLinkWithAppBrowser(bool flag) {
    std::lock_guard<QTalk::util::spin_mutex> lock(sm);
    _openWithAppBrowser = flag;
}

bool AppSetting::getOpenOaLinkWithAppBrowser() {
    std::lock_guard<QTalk::util::spin_mutex> lock(sm);
    return _openOaWithAppBrowser;
}

void AppSetting::setOpenOaLinkWithAppBrowser(bool flag) {
    std::lock_guard<QTalk::util::spin_mutex> lock(sm);
    _openOaWithAppBrowser = flag;
}

void AppSetting::setScreentShotHideWndFlag(bool hide) {
    std::lock_guard<QTalk::util::spin_mutex> lock(sm);
    _screentShotHideWndFlag = hide;
}

bool AppSetting::getScreentShotHideWndFlag() {
    std::lock_guard<QTalk::util::spin_mutex> lock(sm);
    return _screentShotHideWndFlag;
}

void AppSetting::setShowSendMessageBtnFlag(bool flag) {
    std::lock_guard<QTalk::util::spin_mutex> lock(sm);
    _showSendMessageBtn = flag;
}

bool AppSetting::getShowSendMessageBtnFlag() {
    std::lock_guard<QTalk::util::spin_mutex> lock(sm);
    return _showSendMessageBtn;
}

void AppSetting::setSendMessageKey(std::string key)
{
    std::lock_guard<QTalk::util::spin_mutex> lock(sm);
    _sendMessageKey = std::move(key);
}

std::string AppSetting::getSendMessageKey()
{
    std::lock_guard<QTalk::util::spin_mutex> lock(sm);
    return _sendMessageKey;
}

void AppSetting::setShowMoodFlag(bool flag)
{
    std::lock_guard<QTalk::util::spin_mutex> lock(sm);
    _showMoodFlag = flag;
}

bool AppSetting::getShowMoodFlag() {
    std::lock_guard<QTalk::util::spin_mutex> lock(sm);
    return _showMoodFlag;
}

void AppSetting::setAutoDeleteSession(bool autoDelete)
{
    std::lock_guard<QTalk::util::spin_mutex> lock(sm);
    _autoDeleteSession = autoDelete;
}

bool AppSetting::getAutoDeleteSession()
{
    std::lock_guard<QTalk::util::spin_mutex> lock(sm);
    return _autoDeleteSession;
}

void AppSetting::setShowQuanTool(bool show) {
    std::lock_guard<QTalk::util::spin_mutex> lock(sm);
    _showQuanWnd = show;
}

bool AppSetting::getShowQuanTool() {
    std::lock_guard<QTalk::util::spin_mutex> lock(sm);
    return _showQuanWnd;
}

std::string AppSetting::getCoEdit() {
    std::lock_guard<QTalk::util::spin_mutex> lock(sm);
    return _coEditor;
}

void AppSetting::setCoEdit(const std::string& edit) {
    std::lock_guard<QTalk::util::spin_mutex> lock(sm);
    _coEditor = edit;
}

/**
 *
 * @return
 */
std::string AppSetting::saveAppSetting() {

    std::string ret;
    std::lock_guard<QTalk::util::spin_mutex> lock(sm);
	cJSON* obj = cJSON_CreateObject();
	cJSON_AddBoolToObject(obj, "NEWMSGAUDIO", _newMsgAudioNotify);
	cJSON_AddStringToObject(obj, "NEWMSGAUDIOPATH", _newMsgAudioPath.data());
	cJSON_AddBoolToObject(obj, "NEWMSGTIPWINDOW", _newMsgTipWindowNotify);
	cJSON_AddStringToObject(obj, "SCREENSHOT", _screenshotKey.data());
	cJSON_AddStringToObject(obj, "WAKEWND", _wakeWndKey.data());
	cJSON_AddBoolToObject(obj, "OPENWITHAPPBROWSER", _openWithAppBrowser);
	cJSON_AddBoolToObject(obj, "OPENOAWITHAPPBROWSER", _openOaWithAppBrowser);
	cJSON_AddBoolToObject(obj, "SCREENTSHOTHIDEWNDFLAG", _screentShotHideWndFlag);
	cJSON_AddBoolToObject(obj, "SHOWSENDMESSAGEBTN", _showSendMessageBtn);
	cJSON_AddBoolToObject(obj, "STRONGWARN", _strongWarn);
	cJSON_AddBoolToObject(obj, "HOTCUTENABLE", _hotCutEnable);
    cJSON_AddStringToObject(obj, "SENDMESSAGEKEY", _sendMessageKey.data());
    cJSON_AddBoolToObject(obj, "SHOWMOODFLAG", _showMoodFlag);
    cJSON_AddBoolToObject(obj, "AUTOSTARTUP", _autoStartUp);
    cJSON_AddBoolToObject(obj, "LEAVECHECKENABLE", _leaveCheckEnable);
    cJSON_AddBoolToObject(obj, "AUTOREPLYPRESET", _autoReplyPreset);
    cJSON_AddStringToObject(obj, "AUTOREPLYMSG", _autoReplyMsg.data());
    cJSON_AddStringToObject(obj, "AUTOREPLYCUSMSG", _autoReplyCusMsg.data());
    cJSON_AddNumberToObject(obj, "LEAVEMINUTE", _leaveMinute);
    cJSON_AddBoolToObject(obj, "USENATIVEMESSAGEPROMPT", _useNativeMessagePrompt);
    cJSON_AddBoolToObject(obj, "AUTODELETESESSION", _autoDeleteSession);

	ret = QTalk::JSON::cJSON_to_string(obj);
	cJSON_Delete(obj);
    return ret;
}
