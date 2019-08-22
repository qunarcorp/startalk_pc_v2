#ifndef IM_MESSAGE_H
#define IM_MESSAGE_H
#include <string>
#include "../include/CommonDefine.h"
#include "limits.h"
/**
 * @brief The ImMessageInfo struct 消息信息结构体
 * @author
 * @date 2018.9.25
 */
namespace QTalk {
	namespace Entity {

		enum MessageDirection {
			MessageDirectionSent = 1,
			MessageDirectionReceive = 0,
		};

		enum MessageType {
            MessageTypeEmpty = INT_MIN,
            MessageTypePNote                =-11,
            MessageTypeRevoke               =-1,
            MessageTypeText                 =1,
            MessageTypeVoice                =2,
            MessageTypePhoto                =3,
            MessageTypeSogouIcon            =4,
            MessageTypeFile                 =5,
            MessageTypeTopic                =6,        //(群公告)
            MessageTypeRichText             =7,        //（富文本）
            MessageTypeActionRichText       =8,        //(富文本)
            MessageTypeReply                =9,        //(群回复)
            MessageTypeShock                =10,       //(震动)
            MessageTypeNote                =11,
            MessageTypeGroupAt              =12,       //(群消息At)
            MessageTypeMarkdown             =13,//(markdown)
            MessageTypeExtText              =14,       //文本消息，带extinfo
            MessageTypeGroupNotify          =15,       //(通知类消息)
            MessageTypeLocalShare           =16,       //(位置共享)
            MessageTypeWebRTCAudio          =20,
            MessageTypeWebRTCVidio          =21,
            MessageTypeImageNew             = 30,      //(发送图片，单张，无背景用)
            MessageTypeSmallVideo           =32,       //(小视频)
            MessageTypeRobotAnswer          =47,      //(机器人答案)
            MessageTypeSourceCode           =64,       //(源代码)
            MessageTypeTime                 =101,      //(时间戳，本地使用)
            MessageTypeBurnAfterRead        =128,      //(阅后即焚)
            MessageTypeCardShare            =256,      //(分享名片)
            MessageTypeMeetingRemind        =257,      //会议室提醒
            MessageTypeEncrypt              =404,      //加密消息
            MessageTypeActivity             =511,
            MessageTypeRedPack              =512,      //(红包)
            MessageTypeAA                   =513,      //  (AA)
            MessageTypeCommonTrdInfo        =666,      //(通用气泡)
			MessageTypeCommonTrdInfoV2     = 667,
            MessageTypeCommonProductInfo    =888,      //,
            MessageTypeTransChatToCustomer = 1001,
            MessageTypeTransChatToCustomer_Feedback = 1003,
            MessageTypeTransChatToCustomerService = 1002,
            MessageTypeTransChatToCustomerService_Feedback = 1004,
            MessageTypeRedPackInfo          =1024,     // (红包消息通知)
            MessageTypeAAInfo               =1025,     //(AA消息通知)
            MessageTypeConsult              =2001,     //(微信抢单)
            MessageTypeConsultResult        =2002,     //(微信抢单结果)
            MessageTypeGrabMenuVcard        =2003,     //(抢单卡片)
            MessageTypeGrabMenuResult       =2004,     //(抢单结果)
            MessageTypeQCZhongbao           =2005,     //(众包消息)
            MessageTypeMicroTourGuide       =3001,     //(微导游抢单)
            MessageTypeProduct              =4096,     //(产品信息)
            WebRTC_MsgType_VideoMeeting     =5001,
            MessageTypeShareLocation        =8192,     // (位置共享)
            MessageTypeTransNormal          =10081,    //(普通透传消息)
            WebRTC_MsgType_Live             =65501,     //(直播消息)
            WebRTC_MsgType_Video            =65535,    //音视频
            MessageTypeRobotQuestionList    =65536,    //机器人问题列表
            MessageTypeRobotTurnToUser      =65537,    //机器人转人工提示
            MessageTypeFlightRemind         =65550,    //机票航班动态提醒消息
            WebRTC_MsgType_Audio            =131072,
            MessageTypeNotice               =134217728,//(通知类信息)
            MessageTypeSystem               =268435456,
            MediaTypeSystemLY               =268435457,
		};

		struct ImMessageInfo
		{
			ImMessageInfo()
			{
				MsgId = "";
				XmppId = "";
				ChatType = 0;
				Platform = 0;
				From = "";
				To = "";
				Content = "";
				Type = 0;
				State = 0;
				Direction = 0;
				LastUpdateTime = 0;
				MessageRaw = "";
				RealJid = "";
				ExtendedInfo = "";
				BackupInfo = "";
				ExtendedFlag = "";
				BackupInfo = "";
				RealFrom = "";
				RealTo = "";
				SendJid = "";

				// 消息发送人的扩展信息
				UserName = "";
				HeadSrc = "";
				// 文件相关的
				FileName = "";
				FileSize = "";
				FileMd5 = "";
				FileUrl = "";
			}

			std::string MsgId;
			std::string XmppId;
			int         ChatType;
			int         Platform;
			std::string From;
			std::string To;
			std::string Content;
			int         Type;
			int         ReadedTag{};
			int         State;
			int         Direction;
			QInt64      LastUpdateTime;
			std::string MessageRaw;
			std::string RealJid;
			std::string ExtendedInfo;
			std::string BackupInfo;
			std::string ExtendedFlag;
			std::string RealFrom;
			std::string RealTo;
			std::string SendJid;

			// 消息发送人的扩展信息
			std::string UserName;
			std::string HeadSrc;
			// 文件相关的
			std::string FileName;
			std::string FileSize;
			std::string FileMd5;
			std::string FileUrl;
			// 自动回复标志
			bool AutoReply = false;
		};
	}
}
#endif // IM_MESSAGE_H