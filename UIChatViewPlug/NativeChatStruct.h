#ifndef _NATIVECHATSTRUCT_H_
#define _NATIVECHATSTRUCT_H_

#include <QColor>
#include <QFont>
#include <QString>
#include "../QtUtil/Enum/im_enum.h"
#include "../include/CommonDefine.h"

enum MsgMask
{
	EM_MSGMASK_SENDING,
	EM_MSGMASK_UNREAD,
	EM_MSGMASK_READ,
};

// 宽高信息
struct DstItemWH
{
	//
	const int nRadius          = 3;	 // 圆角
	const int nMarginLeft      = 5;	 // 左边距
	const int nMarginRight     = 20; // 右边距
	const int nMarginTopBottom = 9; //  上下边距
	const int nIconWidth       = 30; // 头像大小
	const int nSpace           = 6;	 //  通用间隔值
	const int nLodingWidth     = 20; //  加载动画大小
	const int nLineSpace       = 2; //   行间距
	
	int nContentHeight   = 0;	   //
	int nContentWidth    = 0;	   //
};

// 
struct PainterInfo
{
	QFont  font;
	QColor penColor;
	QColor backColor;
};

class QVBoxLayout;
class StatusWgt;
class ChatMainWgt;
class GroupChatSidebar;
class ToolWgt;
class InputWgt;
struct StSubWgt
{
	QVBoxLayout*      layout;

	StatusWgt*        statusWgt;
	ChatMainWgt*      chatMainWgt;
	GroupChatSidebar* groupSidebar;
	ToolWgt*          toolWgt;
	InputWgt*         inputWgt;

	StSubWgt()
		: layout(nullptr), statusWgt(nullptr), chatMainWgt(nullptr)
		, groupSidebar(nullptr), toolWgt(nullptr), inputWgt(nullptr)
	{

	}
};

struct StTextMessage
{
    enum Type {EM_TINVALID, EM_TEXT, EM_IMAGE, EM_EMOTICON, EM_LINK, EM_ATMSG};

    QString content;
    Type    type;
    QString imageLink;
    double  imageWidth{};
    double  imageHeight{};

    explicit StTextMessage(Type t = EM_TINVALID) : type(t){}
};

#endif//_NATIVECHATSTRUCT_H_
