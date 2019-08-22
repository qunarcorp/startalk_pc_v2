//
// Created by admin on 2019-03-01.
//

#include "VideoMessageItem.h"
#include "../CustomUi/HeadPhotoLab.h"
#include <QJsonDocument>
#include <QHBoxLayout>
#include <QStackedLayout>
#include <QMouseEvent>
#include <QPainter>
#include "../../Platform/Platform.h"
#include "../../UICom/qimage/qimage.h"
#include <QTime>
#include <ChatUtil.h>
#include <QFileInfo>
#include <QApplication>
#include "../ChatViewMainPanel.h"
#include "../../CustomUi/QtMessageBox.h"
#include "../../UICom/StyleDefine.h"
#include "../../WebService/WebService.h"

extern ChatViewMainPanel *g_pMainPanel;
class VideoMaskFrame : public QFrame {
public:
   explicit VideoMaskFrame(int direction = 0,QFrame *parent = Q_NULLPTR)
   :QFrame(parent), _direction(direction){
        this->setAttribute(Qt::WA_TranslucentBackground,true);
    };
public:
    void setImage(const QString& path)
    {
        _imgPath = path;
        update();
    }

    void setCurValue(int val )
    {
        _cur_val = val;
        update();
    }

    void setDownload(bool downloaded)
    {
        _downloaded = downloaded;
        update();
    }

private:
    int _direction;
    QString _imgPath;
    int _cur_val = 0;
    bool _downloaded = true;

protected:
    void paintEvent(QPaintEvent *event) override {
        QPainter painter(this);
        painter.setRenderHints(QPainter::Antialiasing, true);
        painter.setRenderHints(QPainter::SmoothPixmapTransform, true);
        QRectF rect = QRect(0, 0, this->width(), this->height());

        QPainterPath path;
        path.addRoundedRect(rect, 6, 6);
        painter.setClipPath(path);
        if(!_imgPath.isEmpty())
        {
            QPixmap pix = QTalk::qimage::instance().loadPixmap(_imgPath, false);
            painter.drawPixmap(0, 0, this->width(), this->height(), pix);
        }

        painter.save();
        painter.restore();

        if(!_downloaded)
        {
            double degrees = _cur_val * double(360.0 / 100.0);
            qreal radius = 15;
            QRectF prect(width() / 2 - radius, height() / 2 - radius, 2 * radius, 2 * radius);
            //
            painter.setPen(Qt::NoPen);
            painter.setBrush(QColor(240,246,245, 200));
            painter.drawEllipse(prect);
            //
            painter.setBrush(QTalk::StyleDefine::instance().getFileProcessBarLine());
            painter.setPen(Qt::NoPen);
            painter.drawEllipse(prect);
            //
            QPainterPath dataPath;
            dataPath.setFillRule(Qt::WindingFill);
            dataPath.moveTo(prect.center());
            dataPath.arcTo(prect, 90, - degrees);
            dataPath.lineTo(prect.center());
            painter.setBrush(QColor(255, 255, 255));
            painter.setPen(Qt::NoPen);
            painter.drawPath(dataPath);
        }
        QFrame::paintEvent(event);
    }
};

VideoMessageItem::VideoMessageItem(const QTalk::Entity::ImMessageInfo &msgInfo, QWidget *parent)
        : MessageItemBase(msgInfo, parent),
          _videoImgLab(nullptr),
          _contentLab(nullptr) {
    this->init();
}

QSize VideoMessageItem::itemWdtSize() {
    int height = qMax(_mainMargin.top() + _nameLabHeight + _mainSpacing + _contentFrm->height() + _mainMargin.bottom(),
                      _headPixSize.height()); // 头像和文本取大的
    int width = _contentFrm->width();
    if(nullptr != _readStateLabel)
    {
        height += 12;
    }
    return {width, height + 8};
}

void VideoMessageItem::init() {
    initLayout();
}

void VideoMessageItem::initLayout() {

    _contentSize = QSize(271, 84);
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

/**
  * @函数名
  * @功能描述
  * @参数
  * @author cc
  * @date 2018.10.19
  */
void VideoMessageItem::initSendLayout() {
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
//    _contentFrm->setObjectName("messSendContentFrm");
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
        rsLay->setContentsMargins(0,0,10,0);
        rsLay->addWidget(_readStateLabel);
        rightLay->addLayout(rsLay);
    }
    mainLay->setStretch(0, 1);
    mainLay->setStretch(1, 0);

    initContentLayout();
}

/**
  * @函数名
  * @功能描述
  * @参数
  * @author cc
  * @date 2018.10.19
  */
void VideoMessageItem::initReceiveLayout() {
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
//    _contentFrm->setObjectName("messReceiveContentFrm");
    _contentFrm->setFixedWidth(_contentSize.width());
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

void VideoMessageItem::initContentLayout() {

    QJsonDocument jsonDocument = QJsonDocument::fromJson(_msgInfo.ExtendedInfo.data());
    if (jsonDocument.isNull()) {

        auto *vBox = new QVBoxLayout;
        vBox->setSpacing(5);
        _contentFrm->setLayout(vBox);

        _contentLab = new QLabel(this);
        _contentLab->setObjectName("contentLab");
        _contentLab->setWordWrap(true);
        _contentLab->setFixedWidth(_contentSize.width() - 20);
        _contentLab->setText(QString::fromStdString(_msgInfo.Content));
        _contentLab->adjustSize();
        vBox->addWidget(_contentLab);

        _contentFrm->setFixedSize(10 + 10 + _contentLab->width(), 10 + 10 + _contentLab->height());

    } else {
        QJsonObject jsonObject = jsonDocument.object();
        QString thumbUrl = jsonObject.value("ThumbUrl").toString();
        double width = jsonObject.value("Width").toString().toInt();
        double height = jsonObject.value("Height").toString().toInt();
        int duration = jsonObject.value("Duration").toString().toInt();
        if (duration <= 0) {
            duration = jsonObject.value("Duration").toString().toInt();
        }

        QString fileSize = jsonObject.value("FileSize").toString();
        QTalk::Image::scaImageSize(width, height);
        contentSize = QSizeF(width, height).toSize();
        if (_videoImgLab == nullptr) {
            auto *cBox = new QHBoxLayout;
            cBox->setMargin(0);
            _contentFrm->setLayout(cBox);

            maskFrame = new VideoMaskFrame(_msgInfo.Direction);
            maskFrame->setFixedSize(contentSize);
            cBox->addWidget(maskFrame);

            auto *vBox = new QVBoxLayout;
            vBox->setAlignment(Qt::AlignmentFlag::AlignCenter);
            maskFrame->setLayout(vBox);

            QFrame *btnContentFrame = new QFrame(this);
            btnContentFrame->setFixedSize(46, 36);
            vBox->addWidget(btnContentFrame);

            auto *vBox1 = new QVBoxLayout;
            vBox1->setContentsMargins(10, 0, 0, 0);
            vBox1->setSpacing(0);
            btnContentFrame->setLayout(vBox1);

            QFrame *btnFrame = new QFrame(this);
            btnFrame->setFixedSize(36, 36);
            btnFrame->setObjectName("BtnBgFrame");
            vBox1->addWidget(btnFrame);

            auto *vBox2 = new QVBoxLayout;
            vBox2->setAlignment(Qt::AlignmentFlag::AlignCenter);
            vBox2->setMargin(0);
            vBox2->setSpacing(0);
            btnFrame->setLayout(vBox2);

            btnLable = new QLabel(this);
            btnLable->setAttribute(Qt::WA_TranslucentBackground, true);
            QPixmap p(":/chatview/image1/messageItem/video_play_icon.png");
            btnLable->setPixmap(p);
            btnLable->setFixedSize(32, 32);
            vBox2->addWidget(btnLable);

            connect(this, &VideoMessageItem::sgDownloadedIcon, this, &VideoMessageItem::setIcon, Qt::QueuedConnection);
            QString placeHolder = ":/chatview/image1/defaultShareIcon.png";
            if (thumbUrl.isEmpty()) {
                maskFrame->setImage(placeHolder);
            } else {
                std::string imgPath = QTalk::GetImagePathByUrl(thumbUrl.toStdString());
                if(QFile::exists(imgPath.data()))
                {
                    maskFrame->setImage(imgPath.data());
                }
                else
                {
                    maskFrame->setImage(placeHolder);
                    std::thread([this, thumbUrl](){
                        std::string downloadFile = g_pMainPanel->getMessageManager()->getLocalFilePath(thumbUrl.toStdString());
                        if(!downloadFile.empty())
                                emit sgDownloadedIcon(QString::fromStdString(downloadFile));
                    }).detach();
                }
            }

            QString strDuration;
            if (duration < 3600)
                strDuration = QTime::fromMSecsSinceStartOfDay(duration * 1000).toString("mm:ss");
            else
                strDuration = QTime::fromMSecsSinceStartOfDay(duration * 1000).toString("hh:mm:ss");
            if (duration > 0) {

            }
        }

        _contentFrm->setFixedSize(contentSize);
    }
}

/**
 *
 */
void VideoMessageItem::playVideo() {

    QJsonDocument jsonDocument = QJsonDocument::fromJson(_msgInfo.ExtendedInfo.data());
    if (!jsonDocument.isNull()) {
        QJsonObject jsonObject = jsonDocument.object();
        QString videoUrl = jsonObject.value("FileUrl").toString();
        int width = jsonObject.value("Width").toInt(0);
        int height = jsonObject.value("Height").toInt(0);
        if (width == 0 || height == 0) {
            width = jsonObject.value("Width").toString().toInt();
            height = jsonObject.value("Height").toString().toInt();
        }
        //
        if(jsonObject.contains("newVideo") && jsonObject.value("newVideo").toBool())
        {
#ifdef _WINDOWS
            QDesktopServices::openUrl(QUrl(videoUrl));
#else
            WebService::loadUrl(QUrl(videoUrl), false);
#endif
        }
        else
        {
            //
            if(videoUrl.isEmpty())
                QtMessageBox::warning(g_pMainPanel, "警告", "无效的视频文件!");
            else
            {
                //
                localVideo = QTalk::getVideoPathByUrl(videoUrl.toStdString()).data();
                //
                QFileInfo info(localVideo);
                if(!info.exists() || info.isDir())
                {
                    btnLable->setVisible(false);
                    maskFrame->setDownload(false);
                    g_pMainPanel->getMessageManager()->sendDownLoadFile(localVideo.toStdString(),
                                                                        videoUrl.toStdString(), _msgInfo.MsgId);
                }
                else
                {
#ifdef _WINDOWS
                    QDesktopServices::openUrl(QUrl::fromLocalFile(localVideo));
#else
                    WebService::loadUrl(QUrl::fromLocalFile(localVideo), false);
#endif
//                    emit sgPlayVideo(localVideo, width, height);
                }
            }
        }
    }

}

void VideoMessageItem::mousePressEvent(QMouseEvent *event) {

    if (event->button() == Qt::LeftButton && _contentFrm->geometry().contains(event->pos())) {
        playVideo();
    }
    QFrame::mousePressEvent(event);
}

void VideoMessageItem::setIcon(const QString &iconPath)
{
    if(maskFrame)
        maskFrame->setImage(iconPath);
}

void VideoMessageItem::setProcess(double speed, double dtotal, double dnow, double utotal, double unow) {

    double process = 0;
    if (dtotal > 0) {
        process = (dnow * 100.0) / dtotal;
    } else if (utotal > 0) {
        process = (unow * 100.0) / utotal;
    } else {
        return;
    }

    if(abs((int) process - 100) < 0.001)
    {
        maskFrame->setDownload(true);
        //
        QFileInfo info(localVideo);
        while (!info.exists() || info.isDir())
        {
            QApplication::processEvents();
        }
        playVideo();
        btnLable->setVisible(true);
    }

    if (maskFrame) {
        maskFrame->setCurValue(process);
    }
}