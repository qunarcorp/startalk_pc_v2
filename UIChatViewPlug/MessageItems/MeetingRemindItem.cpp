//
// Created by lihaibin on 2019-06-20.
//

#include "MeetingRemindItem.h"

#include "../CustomUi/HeadPhotoLab.h"
#include "../../WebService/WebService.h"

#include <QSpacerItem>
#include <QHBoxLayout>
#include <QJsonDocument>
#include <QDesktopServices>
#include <QMouseEvent>
#include <QUrl>
#include "../../Platform/Platform.h"
#include "../../UICom/qimage/qimage.h"
#include "../../Platform/NavigationManager.h"
#include <QTextEdit>
#include <QJsonArray>

MeetingRemindItem::MeetingRemindItem(const QTalk::Entity::ImMessageInfo &msgInfo,
                                     QWidget *parent):MessageItemBase(msgInfo,parent),
                                                      contentLabel(Q_NULLPTR)
{
    init();
}
MeetingRemindItem::~MeetingRemindItem(){

}

void MeetingRemindItem::loadUrl(const QTalk::Entity::ImMessageInfo& msgInfo) {
    QJsonDocument jsonDocument = QJsonDocument::fromJson(msgInfo.ExtendedInfo.data());
    if (!jsonDocument.isNull()) {
        QJsonObject jsonObject = jsonDocument.object();
        QString linkUrl = jsonObject.value("url").toString();
        QString id = jsonObject.value("params").toObject().value("id").toString();
        linkUrl.append("username=");
        linkUrl.append(QString::fromStdString(Platform::instance().getSelfUserId()));
        linkUrl.append("&meeting_id=");
        linkUrl.append(id);

        MapCookie cookies;
        cookies["ckey"] = QString::fromStdString(Platform::instance().getClientAuthKey());
        WebService::loadUrl(QUrl(linkUrl), false, cookies);
    }
}

void MeetingRemindItem::init() {
    initLayout();
}

void MeetingRemindItem::initLayout() {
    _contentSize = QSize(350, 0);
    _mainMargin = QMargins(15, 0, 20, 0);
    _mainSpacing = 10;
    if (QTalk::Entity::MessageDirectionSent == _msgDirection) {
        _headPixSize = QSize(0, 0);
        _nameLabHeight = 0;
        _leftMargin = QMargins(0, 0, 0, 0);
        _rightMargin = QMargins(0, 0, 0, 0);
        _leftSpacing = 0;
        _rightSpacing = 0;
        initSendLayout();
    } else if (QTalk::Entity::MessageDirectionReceive == _msgDirection) {
        _headPixSize = QSize(28, 28);
        _nameLabHeight = 16;
        _leftMargin = QMargins(0, 10, 0, 0);
        _rightMargin = QMargins(0, 0, 0, 0);
        _leftSpacing = 0;
        _rightSpacing = 4;
        initReceiveLayout();
    }
    if (QTalk::Enum::ChatType::GroupChat != _msgInfo.ChatType) {
        _nameLabHeight = 0;
    }
    setContentsMargins(0, 5, 0, 5);
}

void MeetingRemindItem::initSendLayout() {

}

void MeetingRemindItem::initReceiveLayout() {
    auto *mainLay = new QHBoxLayout(this);
    mainLay->setContentsMargins(_mainMargin);
    mainLay->setSpacing(_mainSpacing);
    mainLay->addWidget(_btnShareCheck);
    auto *leftLay = new QVBoxLayout;
    leftLay->setContentsMargins(_leftMargin);
    leftLay->setSpacing(_leftSpacing);
    mainLay->addLayout(leftLay);
    if (!_headLab) {
        _headLab = new HeadPhotoLab;
    }
    _headLab->setFixedSize(_headPixSize);
    _headLab->setHead(QString::fromStdString(_msgInfo.HeadSrc), HEAD_RADIUS);
    _headLab->installEventFilter(this);
    leftLay->addWidget(_headLab);
    auto *vSpacer = new QSpacerItem(1, 1, QSizePolicy::Fixed, QSizePolicy::Expanding);
    leftLay->addItem(vSpacer);

    leftLay->setStretch(0, 0);
    leftLay->setStretch(1, 1);

    auto *rightLay = new QVBoxLayout;
    rightLay->setContentsMargins(_rightMargin);
    rightLay->setSpacing(_rightSpacing);
    mainLay->addLayout(rightLay);
    if (QTalk::Enum::ChatType::GroupChat == _msgInfo.ChatType
        && QTalk::Entity::MessageDirectionReceive == _msgInfo.Direction ) {
        rightLay->addWidget(_nameLab);
    }
    if (!_contentFrm) {
        _contentFrm = new QFrame(this);
    }
    _contentFrm->setObjectName("MeetingRemindItem");
    _contentFrm->setFixedWidth(_contentSize.width());
//    _contentFrm->setStyleSheet("");
    rightLay->addWidget(_contentFrm);
    rightLay->setStretch(0, 0);
    rightLay->setStretch(1, 1);

    auto *horizontalSpacer = new QSpacerItem(40, 1, QSizePolicy::Expanding, QSizePolicy::Fixed);
    mainLay->addItem(horizontalSpacer);
    if (QTalk::Enum::ChatType::GroupChat == _msgInfo.ChatType) {
        mainLay->setStretch(0, 0);
        mainLay->setStretch(1, 0);
        mainLay->setStretch(2, 1);
    } else {
        mainLay->setStretch(0, 0);
        mainLay->setStretch(1, 1);
    }

    initContentLayout();
}

void MeetingRemindItem::initContentLayout() {
    QJsonDocument jsonDocument = QJsonDocument::fromJson(_msgInfo.ExtendedInfo.data());
    if(jsonDocument.isNull()){

    } else{

        auto *vBox = new QVBoxLayout;
        vBox->setSpacing(20);
        _contentFrm->setLayout(vBox);

        contentLabel = new QLabel;
        contentLabel->setObjectName("contentLabel");
        contentLabel->setWordWrap(true);
        contentLabel->setFixedWidth(_contentSize.width() - 20);
//        contentLabel->setStyleSheet("QLabel{font-size:14px;color:#666666;}");

        QJsonObject jsonObject = jsonDocument.object();
        if(!jsonObject.empty()){
            QJsonArray contents = jsonObject.value("keyValues").toArray();
            QString ctext = "您收到一个行程邀请";
            ctext.append("\n\n");
            for(int i = 0;i<contents.size();i++){
                QJsonObject content = contents.at(i).toObject();
                QStringList keys = content.keys();
                QStringListIterator strIterator(keys);
                while (strIterator.hasNext()){
                    QString key = strIterator.next();
                    ctext.append(key);
                    ctext.append(":");
                    ctext.append(content.value(key).toString());
                    ctext.append("\n");
                }
            }
            contentLabel->setText(ctext);
            contentLabel->adjustSize();
        }

        clickLabel = new QLabel;
        clickLabel->setObjectName("clickLabel");
        clickLabel->setFixedWidth(_contentSize.width() - 20);
        clickLabel->setStyleSheet("QLabel{font-size:14px;color:#15b0f9;}");
        clickLabel->setText("点击查看详情>>");

        vBox->addWidget(contentLabel);
        vBox->addWidget(clickLabel);

        _contentFrm->setFixedSize(10 + 10 + contentLabel->width(), 20 + 40 + contentLabel->height());
    }
}

QSize MeetingRemindItem::itemWdtSize() {
    int height = qMax(_mainMargin.top() + _nameLabHeight + _mainSpacing + _contentFrm->height() + _mainMargin.bottom(),
                      _headPixSize.height()); // 头像和文本取大的
    int width = _contentFrm->width();
    if(nullptr != _readStateLabel)
    {
        height += 12;
    }
    return {width, height + 8};
}

void MeetingRemindItem::resizeEvent(QResizeEvent *event) {

}

void MeetingRemindItem::mousePressEvent(QMouseEvent *event) {
    if(event->button() == Qt::LeftButton && _contentFrm->geometry().contains(event->pos()))
        loadUrl(_msgInfo);
    QFrame::mousePressEvent(event);
}