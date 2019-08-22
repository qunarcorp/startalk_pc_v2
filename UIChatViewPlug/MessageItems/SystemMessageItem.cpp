//
// Created by lihaibin on 2019-06-12.
//

#include "SystemMessageItem.h"

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

SystemMessageItem::SystemMessageItem(const QTalk::Entity::ImMessageInfo &msgInfo,
                                     QWidget *parent):MessageItemBase(msgInfo,parent),
                                                      titleLabel(Q_NULLPTR),
                                                      promptLabel(Q_NULLPTR)
{
    init();
}
SystemMessageItem::~SystemMessageItem(){

}

void SystemMessageItem::loadUrl(const QTalk::Entity::ImMessageInfo& msgInfo) {
    QJsonDocument jsonDocument = QJsonDocument::fromJson(msgInfo.Content.data());
    if(jsonDocument.isNull()){
        jsonDocument = QJsonDocument::fromJson(msgInfo.ExtendedInfo.data());
    }
    if (!jsonDocument.isNull()) {
        QJsonObject jsonObject = jsonDocument.object();
        QString linkUrl = jsonObject.value("operation_url").toString();

        MapCookie cookies;
        cookies["ckey"] = QString::fromStdString(Platform::instance().getClientAuthKey());
        std::string qvt = Platform::instance().getQvt();
        if(!qvt.empty()){
            cJSON *qvtJson = cJSON_GetObjectItem(cJSON_Parse(qvt.data()),"data");
            std::string qcookie = cJSON_GetObjectItem(qvtJson,"qcookie")->valuestring;
            std::string vcookie = cJSON_GetObjectItem(qvtJson,"vcookie")->valuestring;
            std::string tcookie = cJSON_GetObjectItem(qvtJson,"tcookie")->valuestring;
            cJSON_Delete(qvtJson);
            cookies["_q"] = QString::fromStdString(qcookie);
            cookies["_v"] = QString::fromStdString(vcookie);
            cookies["_t"] = QString::fromStdString(tcookie);
        }
        WebService::loadUrl(QUrl(linkUrl), false, cookies);
    }
}

void SystemMessageItem::init() {
    initLayout();
}

void SystemMessageItem::initLayout() {
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

void SystemMessageItem::initSendLayout() {
    auto *mainLay = new QHBoxLayout(this);
    mainLay->setContentsMargins(_mainMargin);
    mainLay->setSpacing(_mainSpacing);
    mainLay->addWidget(_btnShareCheck);
    auto *horizontalSpacer = new QSpacerItem(40, 1, QSizePolicy::Expanding, QSizePolicy::Fixed);
    mainLay->addItem(horizontalSpacer);
    auto *rightLay = new QVBoxLayout;
    rightLay->setContentsMargins(_rightMargin);
    mainLay->addLayout(rightLay);
    if (!_contentFrm) {
        _contentFrm = new QFrame(this);
    }
    _contentFrm->setObjectName("messSendContentFrm");
    _contentFrm->setFixedWidth(_contentSize.width());
    //_contentFrm->setStyleSheet("QFrame{background:#f5f5f5;border-width:1px;border-color:#eaeaea;border-radius:5px;}");
    //
    auto* tmpLay = new QHBoxLayout;
    tmpLay->setMargin(0);
    tmpLay->setSpacing(5);
    if(nullptr == _sending)
    {
        _sending = new HeadPhotoLab(":/chatview/image1/messageItem/loading.gif", 10, false, false, true, this);
        tmpLay->addItem(new QSpacerItem(10, 10, QSizePolicy::Expanding));
        tmpLay->addWidget(_sending);
        bool startMovie = (0 == _msgInfo.ReadedTag && 0 == _msgInfo.State);
        _sending->setVisible(startMovie);
        if(startMovie)
            _sending->startMovie();
    }
    if(nullptr != _resending)
    {
        tmpLay->addWidget(_resending);
        _resending->setVisible(false);
    }
    tmpLay->addWidget(_contentFrm);
    tmpLay->setAlignment(_contentFrm, Qt::AlignRight);
    rightLay->addLayout(tmpLay);
    if (nullptr != _readStateLabel) {
        auto *rsLay = new QHBoxLayout;
        rsLay->addItem(new QSpacerItem(10, 10, QSizePolicy::Expanding));
        rsLay->setMargin(0);
        rsLay->addWidget(_readStateLabel);
        rightLay->addLayout(rsLay);
    }
    mainLay->setStretch(0, 1);
    mainLay->setStretch(1, 0);

    initContentLayout();
}

void SystemMessageItem::initReceiveLayout() {
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
    _contentFrm->setObjectName("messReceiveContentFrm");
    _contentFrm->setFixedWidth(_contentSize.width());
    _contentFrm->setStyleSheet("QFrame{background:#f5f5f5;border-width:1px;border-color:#eaeaea;border-radius:5px;}");
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

void SystemMessageItem::initContentLayout() {
    QJsonDocument jsonDocument = QJsonDocument::fromJson(_msgInfo.Content.data());
    if(jsonDocument.isNull()){
        jsonDocument = QJsonDocument::fromJson( _msgInfo.ExtendedInfo.data());
    }
    if(!jsonDocument.isNull()){
        auto *vBox = new QVBoxLayout;
        vBox->setSpacing(20);
        _contentFrm->setLayout(vBox);

        titleLabel = new QLabel;
        titleLabel->setObjectName("titleLab");
        titleLabel->setWordWrap(true);
        titleLabel->setFixedWidth(_contentSize.width() - 20);
        titleLabel->setStyleSheet("QLabel{font-size:14px;color:#212121;}");
        titleLabel->adjustSize();

        contentLabel = new QLabel;
        contentLabel->setObjectName("contentLabel");
        contentLabel->setWordWrap(true);
        contentLabel->setFixedWidth(_contentSize.width() - 20);
        contentLabel->setStyleSheet("QLabel{font-size:12px;color:#666666;}");

        promptLabel = new QLabel;
        promptLabel->setObjectName("promptLabel");
        promptLabel->setWordWrap(true);
        promptLabel->setFixedWidth(_contentSize.width() - 20);
        promptLabel->setStyleSheet("QLabel{font-size:12px;color:#f4343d;}");
        promptLabel->adjustSize();

        QJsonObject jsonObject = jsonDocument.object();
        if(!jsonObject.empty()){
            titleLabel->setText(jsonObject.value("title").toString());
            QString prompt = jsonObject.value("prompt").toString();
            promptLabel->setText(prompt);

            QJsonArray contents = jsonObject.value("content").toArray();
            QString ctext = "";
            for(int i = 0;i<contents.size();i++){
                QJsonObject content = contents.at(i).toObject();
                ctext.append(content.value("sub_title").toString());
                ctext.append(":");
                ctext.append(content.value("sub_content").toString());
                ctext.append("\n");
            }
            contentLabel->setText(ctext);
            contentLabel->adjustSize();
        }

        vBox->addWidget(titleLabel,0);
        vBox->addWidget(contentLabel,1);
        vBox->addWidget(promptLabel,0);

        _contentFrm->setFixedSize(10 + 10 + promptLabel->width(), 20 + 40 + titleLabel->height() + promptLabel->height() + contentLabel->height());
    }
}

QSize SystemMessageItem::itemWdtSize() {
    int height = qMax(_mainMargin.top() + _nameLabHeight + _mainSpacing + _contentFrm->height() + _mainMargin.bottom(),
                      _headPixSize.height()); // 头像和文本取大的
    int width = _contentFrm->width();
    if(nullptr != _readStateLabel)
    {
        height += 12;
    }
    return {width, height + 8};
}

void SystemMessageItem::resizeEvent(QResizeEvent *event) {

}

void SystemMessageItem::mousePressEvent(QMouseEvent *event) {
    if(event->button() == Qt::LeftButton && _contentFrm->geometry().contains(event->pos()))
        loadUrl(_msgInfo);
    QFrame::mousePressEvent(event);
}