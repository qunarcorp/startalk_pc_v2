//
// Created by lihaibin on 2019-07-08.
//
#include "TriggerConfig.h"

TriggerConfig::TriggerConfig(qtalk::sqlite::database *sqlDb) :
        _sqlDb(sqlDb) {

}

/**
 * update immessage readtag字段后 session未读数trigger
 * @return
 */
bool TriggerConfig::createUnreadUpdateTrigger() {
    if (!_sqlDb) {
        return false;
    }
    std::string sql = "CREATE TRIGGER if not exists sessionlist_unread_update "
                      "after update of ReadedTag on IM_Message "
                      "for each row begin update IM_SessionList set UnreadCount = "
                      "case when (new.ReadedTag & 2) =2 and old.ReadedTag & 2 <>2 "
                      "then (case when UnreadCount >0 then (unreadcount -1) else 0 end ) "
                      "when (new.ReadedTag & 2) <>2 and old.ReadedTag & 2 =2 "
                      "then (UnreadCount + 1) "
                      "else UnreadCount "
                      "end "
                      "where XmppId = new.XmppId and RealJid = new.realjid and new.'from' <> (select value from IM_Cache_Data where key ='USER_ID' and type = 6); end";
    qtalk::sqlite::statement query(*_sqlDb, sql);
    return query.executeStep();
}

/**
 * insert immessage session未读数trigger
 * @return
 */
bool TriggerConfig::createUnreadInserttrigger() {
    if (!_sqlDb) {
        return false;
    }
    std::string sql = "CREATE TRIGGER if not exists  sessionlist_unread_insert "
                      "after insert on IM_Message "
                      "for each row begin update IM_SessionList set UnreadCount = "
                      "case when (new.ReadedTag & 2 )<>2 "
                      "then (UnreadCount+1) "
                      "else UnreadCount "
                      "end "
                      "where XmppId = new.XmppId and RealJid = new.realjid and new.'from' <> (select value from IM_Cache_Data where key ='USER_ID' and type = 6); end";
    qtalk::sqlite::statement query(*_sqlDb, sql);
    return query.executeStep();
}