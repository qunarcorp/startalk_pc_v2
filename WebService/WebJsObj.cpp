

#include "WebJsObj.h"
#include <QDateTime>
#include <QDebug>
#include "../Platform/Platform.h"

WebJsObj::WebJsObj(QObject *o)
        : QObject(o) {

}

WebJsObj::~WebJsObj()
= default;

void WebJsObj::getCkey() {
    QString jsCommend = QString("sgsetCkey('%1');").arg(Platform::instance().getClientAuthKey().data());
    emit runScript(jsCommend);
}

void WebJsObj::getUserId() {
    QString jsCommend = QString("sgsetUserId('%1');").arg(Platform::instance().getSelfUserId().data());
    emit runScript(jsCommend);
}

void WebJsObj::getMyNick() {
    QString jsCommend = QString("sgsetMyNick('%1')").arg(Platform::instance().getSelfUserId().data());
    emit runScript(jsCommend);
}

void WebJsObj::getRoomID() {
    QString jsCommend = QString("sgsetRoomID('%1')").arg("6d1c13880eb94c8591f744dd41c92de8@conference.ejabhost1");
    emit runScript(jsCommend);
}

void WebJsObj::getTopic() {

}

void WebJsObj::getStartTime() {
    QString jsCommend = QString("sgsetStartTime(%1)").arg(QDateTime::currentMSecsSinceEpoch());
    emit runScript(jsCommend);
}

void WebJsObj::close_video_room(const QString &roomid) {
    emit closeGroupRoom();
}

void WebJsObj::jsgotoVideoRoom(const QString &roomjid) {

}

void WebJsObj::getNickByUserId(const QString &id) {
    QString jsCommend = QString("sgsetNickByUserId('%1', '%2');").arg(id, id);
    emit runScript(jsCommend);
}

void WebJsObj::enableFullScreen(const QString &roomid) {
    emit sgFullScreen();
}

void WebJsObj::cancelFullScreen(const QString &roomid) {
    emit sgFullScreen();
}

void WebJsObj::sendSignal(const QString &json, const QString &id) {
    emit sgSendSignal(json, id);
    qDebug() << "sendSignal" << json << id;
}

void WebJsObj::StartVideoSound(const QString &id) {
    qDebug() << "sendSignal" << id;
}

void WebJsObj::StopVideoSound(const QString &id) {
    qDebug() << "StopVideoSound" << id;
}

void WebJsObj::WriteLocalLog(const QString &id, const QString &log) {
    qDebug() << "WriteLocalLog" << id << log;
}

void WebJsObj::closeRtcWindow(const QString &id, long long occupied_time) {
    emit sgCloseRtcWindow(id, occupied_time);
}