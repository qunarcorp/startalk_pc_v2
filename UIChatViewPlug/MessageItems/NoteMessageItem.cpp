//
// Created by lihaibin on 2019-06-05.
//

#include "NoteMessageItem.h"

#include "ChatViewMainPanel.h"
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
#include <QTextEdit>

extern ChatViewMainPanel *g_pMainPanel;

NoteMessageItem::NoteMessageItem(const QTalk::Entity::ImMessageInfo &msgInfo,QWidget *parent):
        MessageItemBase(msgInfo,parent),
        titleLabel(Q_NULLPTR),
        iconLabel(Q_NULLPTR),
        priceLabel(Q_NULLPTR)
{
    init();
}
NoteMessageItem::~NoteMessageItem(){

}

void loadUrl(const QTalk::Entity::ImMessageInfo& msgInfo)
{

    QJsonDocument jsonDocument = QJsonDocument::fromJson(msgInfo.ExtendedInfo.data());
    if(jsonDocument.isNull()){
        jsonDocument = QJsonDocument::fromJson(msgInfo.Content.data());
    }
    if (!jsonDocument.isNull()) {
        QJsonObject jsonObject = jsonDocument.object();
        QJsonObject data = jsonObject.value("data").toObject();
        QString linkUrl = data.value("webDtlUrl").toString();
        QDesktopServices::openUrl(QUrl(linkUrl));
    }
}

void NoteMessageItem::init() {
    initLayout();
}

void NoteMessageItem::initLayout() {
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

void NoteMessageItem::initContentLayout() {
    QJsonDocument jsonDocument = QJsonDocument::fromJson(_msgInfo.ExtendedInfo.data());
    if(jsonDocument.isNull()){
        jsonDocument = QJsonDocument::fromJson(_msgInfo.Content.data());
    }

    if(!jsonDocument.isNull()){
        auto *hBox = new QHBoxLayout;
        hBox->setSpacing(5);
        _contentFrm->setLayout(hBox);

        iconLabel = new QLabel(this);
        iconLabel->setFixedSize(80, 40);
        iconLabel->setAlignment(Qt::AlignmentFlag::AlignTop);

        auto *vBox = new QVBoxLayout;
        titleLabel = new QLabel;
        titleLabel->setObjectName("titleLab");
        titleLabel->setWordWrap(true);
        titleLabel->setFixedWidth(_contentSize.width() - 20);
        titleLabel->setStyleSheet("QLabel{font-size:14px;color:#212121;}");
        titleLabel->adjustSize();

        tagLabel = new QLabel;
        tagLabel->setObjectName("tagLab");
        tagLabel->setFixedWidth(_contentSize.width() - 20);
        tagLabel->setStyleSheet("QLabel{font-size:12px;color:#666666;}");
        tagLabel->adjustSize();

        priceLabel = new QLabel;
        priceLabel->setObjectName("priceLab");
        priceLabel->setStyleSheet("QLabel{font-size:14px;color:#f4343d;}");
        priceLabel->adjustSize();
        priceLabel->setAlignment(Qt::AlignmentFlag::AlignBottom);

        QString imgUrl;
        QJsonObject jsonObject = jsonDocument.object();
        QJsonObject data = jsonObject.value("data").toObject();
        if(!data.empty()){
            imgUrl = data.value("imageUrl").toString();
            titleLabel->setText(data.value("title").toString());
            tagLabel->setText(data.value("tag").toString());
            priceLabel->setText(data.value("price").toString());
        }

        connect(this, &NoteMessageItem::sgDownloadedIcon, this, &NoteMessageItem::setIcon, Qt::QueuedConnection);
        QString placeHolder = ":/chatview/image1/defaultShareIcon.png";
        QPixmap defaultPix = QTalk::qimage::instance().loadPixmap(placeHolder, true, true, 80, 40);
        if (imgUrl.isEmpty()) {
            iconLabel->setPixmap(defaultPix);
        } else {
            std::string imgPath = QTalk::GetImagePathByUrl(imgUrl.toStdString());
            if(QFile::exists(imgPath.data()))
            {
                iconLabel->setPixmap(QTalk::qimage::instance().loadPixmap(imgPath.data(), false, true, 80, 40));
            }
            else
            {
                iconLabel->setPixmap(defaultPix);
                QPointer<NoteMessageItem> pThis(this);
                std::thread([pThis, imgUrl](){
                    std::string downloadFile = g_pMainPanel->getMessageManager()->getLocalFilePath(imgUrl.toStdString());
                    if(pThis && !downloadFile.empty())
                            emit pThis->sgDownloadedIcon(QString::fromStdString(downloadFile));
                }).detach();
            }
        }

        vBox->addWidget(titleLabel);
        vBox->addWidget(tagLabel);
        vBox->addWidget(priceLabel);

        hBox->addWidget(iconLabel);
        hBox->addLayout(vBox);

        _contentFrm->setFixedSize(10 + 10 + titleLabel->width() + iconLabel->width(), 15 + 15 + titleLabel->height() + tagLabel->height() + iconLabel->height());
    }
}

void NoteMessageItem::initSendLayout() {
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
//        _sending->setHead(":/chatview/image1/messageItem/loading.gif", 15, false, true, true);
        tmpLay->addItem(new QSpacerItem(10, 10, QSizePolicy::Expanding));
        tmpLay->addWidget(_sending);
        bool startMovie = (0 == _msgInfo.ReadedTag && 0 == _msgInfo.State);
        _sending->setVisible(startMovie);
        if(startMovie)
            _sending->startMovie();
    }
    if(nullptr != _resending)
    {
//        _resending = new HeadPhotoLab;
//        _resending->setHead(":/chatview/image1/error.png", 8, false, false, true);
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

void NoteMessageItem::initReceiveLayout() {
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
        auto* nameLay = new QHBoxLayout;
        nameLay->setMargin(0);
        nameLay->setSpacing(5);
        nameLay->addWidget(_nameLab);
        nameLay->addWidget(_medalWgt);
        rightLay->addLayout(nameLay);
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

QSize NoteMessageItem::itemWdtSize() {
    int height = qMax(_mainMargin.top() + _nameLabHeight + _mainSpacing + _contentFrm->height() + _mainMargin.bottom(),
                      _headPixSize.height()); // 头像和文本取大的
    int width = _contentFrm->width();
    if(nullptr != _readStateLabel)
    {
        height += 12;
    }
    return {width, height + 8};
}

void NoteMessageItem::resizeEvent(QResizeEvent *event) {

}

void NoteMessageItem::mousePressEvent(QMouseEvent *event) {
    if(event->button() == Qt::LeftButton && _contentFrm->geometry().contains(event->pos()))
        loadUrl(_msgInfo);
    QFrame::mousePressEvent(event);
}

void NoteMessageItem::setIcon(const QString &iconPath) {
    QPixmap pixmap = QTalk::qimage::instance().loadPixmap(iconPath, false, true, 80, 40);
    iconLabel->setPixmap(pixmap);
}