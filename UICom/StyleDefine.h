//
// Created by QITMAC000260 on 2019-07-15.
//

#ifndef QTALK_V2_STYLEDEFINE_H
#define QTALK_V2_STYLEDEFINE_H

#include "uicom_global.h"
#include <QColor>

/**
* @description: StyleDefine
* @author: cc
* @create: 2019-07-15 19:37
**/
namespace QTalk {

    class UICOMSHARED_EXPORT StyleDefine {
    private:
        static StyleDefine* _defines;
    public:
        static StyleDefine& instance();

    public:
        //
        inline QColor getNavNormalColor() { return _nav_normal_color; };
        inline QColor getNavSelectColor() { return _nav_select_color; };
        inline QColor getNavTopColor() { return _nav_top_color; };
        inline QColor getNavTipColor() { return _nav_tip_color; };
        //
        inline QColor getNavNameSelectFontColor() { return _nav_name_font_select_color; };
        inline QColor getNavContentSelectFontColor() { return _nav_content_font_select_color; };
        inline QColor getNavTimeSelectFontColor() { return _nav_time_font_select_color; };

        inline QColor getNavNameFontColor() { return _nav_name_font_color; };
        inline QColor getNavContentFontColor() { return _nav_content_font_color; };
        inline QColor getNavTimeFontColor() { return _nav_time_font_color; };

        inline QColor getNavAtFontColor() { return _nav_at_font_color; };
        inline QColor getNavUnReadFontColor() { return _nav_unread_font_color; };
        //
        inline QColor getChatGroupNormalColor() { return _chat_group_normal_color; };
        inline QColor getChatGroupFontColor() { return _chat_group_font_color; };
        //
        inline QColor getAdrNormalColor() { return _adr_normal_color; };
        inline QColor getAdrSelectColor() { return _adr_select_color; };
        inline QColor getAdrNameFontColor() { return _adr_name_font_color; };
        //
        inline QColor getDropNormalColor() { return _drop_normal_color; };
        inline QColor getDropSelectColor() { return _drop_select_color; };
        inline QColor getDropNormalFontColor() { return _drop_normal_font_color; };
        inline QColor getDropSelectFontColor() { return _drop_select_font_color; };
        //
        inline QString getLinkUrl() { return _link_url_color; };
        // local search
        inline QColor getLocalSearchTimeFontColor() { return _local_search_time_font_color; }
        // at block
        inline QColor getAtBlockFontColor() { return _at_block_font_color; }
        inline QColor getEmoSelelctIconColor() { return _emo_select_icon_color; }
        //
        inline QColor getGroupCardGroupMemberNormalColor() { return _group_card_group_member_normal_color;}
        //
        inline QColor getGroupManagerNormalColor() { return _group_manager_normal_color; }
        inline QColor getGroupManagerNormalFontColor() { return _group_manager_normal_font_color; }

        inline QColor getTitleSearchNormalColor() {return _title_search_normal_color;}
        inline QColor getTitleSearchSelectColor() {return _title_search_select_color;}
        //
        inline QColor getQuoteBlockBackgroundColor() { return _quote_block_background_color;}
        inline QColor getQuoteBlockTipColor() {return _quote_block_tip_color;}
        inline QColor getQuoteBlockFontColor() {return _quote_block_font_color;}

        inline QColor getFileProcessBarBackground() { return _file_process_bar_background; }
        inline QColor getFileProcessBarLine() { return _file_process_bar_line ; }

        inline QColor getHeadPhotoMaskColor() { return _head_photo_mask_color;}

    public:
        inline void setNavNormalColor(const QColor& color) {  _nav_normal_color = color; };
        inline void setNavSelectColor(const QColor& color) {  _nav_select_color = color; };
        inline void setNavTopColor(const QColor& color) {  _nav_top_color = color; };
        inline void setNavTipColor(const QColor& color) {  _nav_tip_color = color; };
        inline void setNavNameFontColor(const QColor& color) {  _nav_name_font_color = color; };
        inline void setNavContentFontColor(const QColor& color) {  _nav_content_font_color = color; };
        inline void setNavTimeFontColor(const QColor& color) {  _nav_time_font_color = color; };

        inline void setNavNameSelectFontColor(const QColor& color) {  _nav_name_font_select_color = color; };
        inline void setNavContentSelectFontColor(const QColor& color) {  _nav_content_font_select_color = color; };
        inline void setNavTimeSelectFontColor(const QColor& color) {  _nav_time_font_select_color = color; };

        inline void setNavAtFontColor(const QColor& color) {  _nav_at_font_color = color; };
        inline void setNavUnReadFontColor(const QColor& color) {  _nav_unread_font_color = color; };
        inline void setChatGroupNormalColor(const QColor& color) {  _chat_group_normal_color = color; };
        inline void setChatGroupFontColor(const QColor& color) {  _chat_group_font_color = color; };
        inline void setAdrNormalColor(const QColor& color) {  _adr_normal_color = color; };
        inline void setAdrSelectColor(const QColor& color) {  _adr_select_color = color; };
        inline void setAdrNameFontColor(const QColor& color) {  _adr_name_font_color = color; };
        inline void setDropNormalColor(const QColor& color) { _drop_normal_color = color; };
        inline void setDropSelectColor(const QColor& color) { _drop_select_color = color; };
        inline void setDropNormalFontColor(const QColor& color) { _drop_normal_font_color = color; };
        inline void setDropSelectFontColor(const QColor& color) { _drop_select_font_color = color; };
        inline void setLinkUrl(const QString& color) { _link_url_color = color; };
        // local search
        inline void setLocalSearchTimeFontColor(const QColor& color) { _local_search_time_font_color = color; }
        // at block
        inline void setAtBlockFontColor(const QColor& color) {_at_block_font_color = color;}
        //
        inline void setEmoSelectIconColor(const QColor &color) {_emo_select_icon_color = color;}
        //
        inline void setGroupCardGroupMemberNormalColor(const QColor &color) {_group_card_group_member_normal_color = color;}
        //
        inline void setGroupManagerNormalColor(const QColor& color) {_group_manager_normal_color = color; }
        inline void setGroupManagerNormalFontColor(const QColor& color) {_group_manager_normal_font_color = color; }
        //
        inline void setTitleSearchNormalColor(const QColor& color) {_title_search_normal_color = color;}
        inline void setTitleSearchSelectColor(const QColor& color) {_title_search_select_color = color;}
        //
        inline void setQuoteBlockBackgroundColor(const QColor& color) {_quote_block_background_color = color;}
        inline void setQuoteBlockTipColor(const QColor& color) {_quote_block_tip_color = color;}
        inline void setQuoteBlockFontColor(const QColor& color) {_quote_block_font_color = color;}

        inline void setFileProcessBarBackground(const QColor& color) { _file_process_bar_background = color; }
        inline void setFileProcessBarLine(const QColor& color) { _file_process_bar_line = color; }

        inline void setHeadPhotoMaskColor(const QColor& color) { _head_photo_mask_color = color; }

    private:
        // 会话列表
        QColor _nav_normal_color = {255, 255, 255};
        QColor _nav_select_color = {213, 242, 240};
        QColor _nav_top_color    = {242, 242, 242};
        QColor _nav_tip_color    = {176, 228, 225};
        QColor _nav_name_font_color     = {51, 51, 51};
        QColor _nav_content_font_color  = {153, 153, 153};
        QColor _nav_time_font_color     = {153, 153, 153};
        QColor _nav_at_font_color       = {255, 78, 63};
        QColor _nav_unread_font_color   = {255, 255, 255};
        QColor _nav_name_font_select_color = {51, 51, 51};
        QColor _nav_content_font_select_color = {153, 153, 153};
        QColor _nav_time_font_select_color = {153, 153, 153};
        // 群成员列表
        QColor _chat_group_normal_color = {248, 248, 248};
        QColor _chat_group_font_color = {51, 51, 51};
        // 通讯录列表
        QColor _adr_normal_color = {255, 255, 255};
        QColor _adr_select_color = {213, 242, 240};
        QColor _adr_name_font_color = {51, 51, 51};
        // drop menu
        QColor _drop_normal_color      = {255,255,255};
        QColor _drop_select_color      = {242,242,242};
        QColor _drop_normal_font_color = {51,51,51};
        QColor _drop_select_font_color = {0, 202, 190};
        //
        QString _link_url_color = "32,188,210";
        //
        QColor _local_search_time_font_color = {51, 51, 51};

        QColor _at_block_font_color = {0, 0, 0};

        QColor _emo_select_icon_color = {255, 255, 255, 255};

        QColor _group_card_group_member_normal_color = {255, 255, 255};
        QColor _group_manager_normal_color = {255, 255, 255};
        QColor _group_manager_normal_font_color = {255, 255, 255};
        //
        QColor _title_search_normal_color = {0, 0, 0, 51};
        QColor _title_search_select_color = {255, 255, 255};
        //
        QColor _quote_block_background_color = {240, 255, 255};
        QColor _quote_block_tip_color = {207, 207, 207};
        QColor _quote_block_font_color = Qt::black;

        QColor _file_process_bar_background = {255,255,255};
        QColor _file_process_bar_line = {242,242,242};

        QColor _head_photo_mask_color = {0,0,0,0};
    };
}


#endif //QTALK_V2_STYLEDEFINE_H
