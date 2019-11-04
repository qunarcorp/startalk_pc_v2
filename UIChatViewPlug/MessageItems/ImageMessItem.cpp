#include "ImageMessItem.h"
#include "../CustomUi/HeadPhotoLab.h"
#include <QHBoxLayout>
#include <QSpacerItem>
#include <QDebug>
#include <QMovie>
#include <QMouseEvent>
#include <QFileInfo>
#include "../GIFManager.h"
#include "../ChatViewMainPanel.h"
#include "../../QtUtil/Utils/Log.h"
#include "../../UICom/qimage/qimage.h"
#include "../../Platform/Platform.h"
#include "ChatUtil.h"

extern ChatViewMainPanel *g_pMainPanel;

ImageMessItem::ImageMessItem(const QTalk::Entity::ImMessageInfo &msgInfo,
                             const QString &link,
                             const QString &path,
                             const QSize &size,
                             QWidget *parent) :
        MessageItemBase(msgInfo, parent),
        _imageLab(nullptr),
        _movie(nullptr),
        _imageLink(link),
        _imagePath(path),
        _size(size),
        _isGIF(false){
    init();
    setImage();
    _moveTimer.setSingleShot(true);
    _moveTimer.setInterval(1000);
    connect(&_moveTimer, SIGNAL(timeout()), this, SLOT(onMoveTimer()));
}

//
ImageMessItem::~ImageMessItem() {

}


/**
  * @函数名
  * @功能描述
  * @参数
  * @date 2018.10.17
  */
QSize ImageMessItem::itemWdtSize() {
    int height = qMax(_mainMargin.top() + _nameLabHeight + _mainSpacing + _contentFrm->height() + _mainMargin.bottom(),
                      _headPixSize.height()); // 头像和文本取大的
    int width = _contentFrm->width();
    if(nullptr != _readStateLabel)
    {
        height += 12;
    }
    return QSize(width, height + 8);
}

/**
  * @函数名
  * @功能描述
  * @参数
  * @date 2018.10.21
  */
void ImageMessItem::resizeEvent(QResizeEvent *event) {
    if (_movie)
        _movie->stop();
    _moveTimer.start();
}

/**
  * @函数名
  * @功能描述
  * @参数
  * @date 2018.10.17
  */
void ImageMessItem::init() {
//    this->setFrameShape(QFrame::NoFrame);
    initLayout();
}

/**
  * @函数名
  * @功能描述
  * @参数
  * @date 2018.10.17
  */
void ImageMessItem::initLayout() {
    _sizeMaxPix = QSize(200, 200);
    _mainMargin = QMargins(15, 0, 25, 0);
    _contentMargin = QMargins(5, 5, 5, 5);
    _mainSpacing = 10;
    _contentSpacing = 0;
    if (QTalk::Entity::MessageDirectionSent == _msgDirection) {
        _headPixSize = QSize(0, 0);
        _nameLabHeight = 0;
        _leftMargin = QMargins(0, 0, 0, 0);
        _rightMargin = QMargins(0, 0, 0, 0);
        _leftSpacing = 0;
        _rightSpacing = 0;
        initSendLayout();
    } else //if (QTalk::Entity::MessageDirectionReceive == _msgDirection)
    {
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
  * @date 2018.10.17
  */
void ImageMessItem::initSendLayout() {
    QHBoxLayout *mainLay = new QHBoxLayout(this);
    mainLay->setContentsMargins(_mainMargin);
    mainLay->setSpacing(_mainSpacing);
    mainLay->addWidget(_btnShareCheck);
    mainLay->addItem(new QSpacerItem(40, 1, QSizePolicy::Expanding));
    QVBoxLayout *rightLay = new QVBoxLayout;
    rightLay->setContentsMargins(_rightMargin);
    mainLay->addLayout(rightLay);
    if (nullptr == _contentFrm) {
        _contentFrm = new QFrame(this);
    }
    _contentFrm->setObjectName("messSendContentFrm");
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
        tmpLay->addWidget(_resending);
        _resending->setVisible(false);
    }
    tmpLay->addWidget(_contentFrm);
    tmpLay->setAlignment(_contentFrm, Qt::AlignRight);
    rightLay->addLayout(tmpLay);
    auto *contentLay = new QVBoxLayout(_contentFrm);
    contentLay->setContentsMargins(_contentMargin);
    rightLay->setSpacing(_rightSpacing);
    if (nullptr == _imageLab) {
        _imageLab = new QLabel;
    }
    contentLay->addWidget(_imageLab);
    contentLay->setSpacing(_contentSpacing);
    if (nullptr != _readStateLabel) {
        QHBoxLayout *rsLay = new QHBoxLayout;
        rsLay->addItem(new QSpacerItem(10, 10, QSizePolicy::Expanding));
        rsLay->setMargin(0);
        rsLay->addWidget(_readStateLabel);
        _readStateLabel->setContentsMargins(0, 10, 0, 0);
        rightLay->addLayout(rsLay);
    }
    mainLay->setStretch(0, 1);
    mainLay->setStretch(1, 0);
}

/**
  * @函数名
  * @功能描述
  * @参数
  * @date 2018.10.17
  */
void ImageMessItem::initReceiveLayout() {
    QHBoxLayout *mainLay = new QHBoxLayout(this);
    mainLay->setContentsMargins(_mainMargin);
    mainLay->setSpacing(_mainSpacing);
    mainLay->addWidget(_btnShareCheck);
    QVBoxLayout *leftLay = new QVBoxLayout;
    leftLay->setContentsMargins(_leftMargin);
    leftLay->setSpacing(_leftSpacing);
    mainLay->addLayout(leftLay);
    if (nullptr == _headLab) {
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
    rightLay->addWidget(_contentFrm);
    rightLay->setStretch(0, 0);
    rightLay->setStretch(1, 1);

    auto *contentLay = new QVBoxLayout;
    contentLay->setContentsMargins(_contentMargin);
    _contentFrm->setLayout(contentLay);

    if (!_imageLab) {
        _imageLab = new QLabel;
    }
    contentLay->addWidget(_imageLab);
    contentLay->setSpacing(_contentSpacing);
    auto* horizontalSpacer = new QSpacerItem(40, 1, QSizePolicy::Expanding, QSizePolicy::Fixed);
    mainLay->addItem(horizontalSpacer);
    if (QTalk::Enum::ChatType::GroupChat == _msgInfo.ChatType) {
        mainLay->setStretch(0, 0);
        mainLay->setStretch(1, 0);
        mainLay->setStretch(2, 1);
    } else {
        mainLay->setStretch(0, 0);
        mainLay->setStretch(1, 1);
    }
}

/**
  * @函数名
  * @功能描述
  * @参数
  * @date 2018.10.17
  */
void ImageMessItem::setImage() {
    if (_imagePath.isEmpty() || !QFile::exists(_imagePath)) {
        //warn_log("load head failed, use default picture-> imagePath:{0}, realMessage:{1}", _imagePath,
          //       _msgInfo.Content);

        _imagePath = ":/chatview/image1/defaultImage.png";
        QPixmap image = QTalk::qimage::instance().loadPixmap(_imagePath, true, true, 80, 80);
        _imageLab->setPixmap(image);
        _imageLab->setFixedSize(image.size());
    } else {
        QString suffix = QTalk::qimage::instance().getRealImageSuffix(_imagePath);
        if ("GIF" == suffix) {
            QPixmap image = QTalk::qimage::instance().loadPixmap(_imagePath, false);
            if (image.isNull()) {
                _imagePath = "";
                setImage();
                return;
            }
            _isGIF = true;
            _imageLab->setFixedSize(_size);
        } else {
            QPixmap pixmap = QTalk::qimage::instance().loadPixmap(_imagePath, false);
            if (pixmap.isNull()) {
                _imagePath = "";
                setImage();
                return;
            }

            QPixmap npixmap = pixmap.scaled(_size.width(), _size.height(), Qt::KeepAspectRatio,
                                            Qt::SmoothTransformation);
            _imageLab->setPixmap(npixmap);
            _imageLab->setFixedSize(_size);
        }
    }
    _contentFrm->setFixedSize(_imageLab->width() + _contentMargin.left() + _contentMargin.right(),
                              _imageLab->height() + _contentMargin.top() + _contentMargin.bottom());
    //    this->update();
}

/**
  * @函数名
  * @功能描述
  * @参数
  * @date 2018.10.21
  */
void ImageMessItem::onMoveTimer() {
    if (_movie)
        _movie->start();
}

void ImageMessItem::mousePressEvent(QMouseEvent *e) {
    if (e->button() == Qt::LeftButton && _contentFrm->geometry().contains(mapFromGlobal(QCursor::pos()))) {
        const QString &userId = _strUserId;
        const QString &messageId = QString::fromStdString(msgInfo().MsgId);
        const QString &msgContent = QString::fromStdString(msgInfo().Content);
        emit g_pMainPanel->showChatPicture(messageId, msgContent, 0);
    }
    QFrame::mousePressEvent(e);
}

bool ImageMessItem::event(QEvent* e)
{
    if(e->type() == QEvent::Show && nullptr == _movie)
    {
        if(_isGIF)
        {
            _movie = g_pMainPanel->gifManager->getMovie(_imagePath);
            _movie->setSpeed(80);
            _movie->setScaledSize(_size);
            _movie->start();
            _imageLab->setMovie(_movie);
        }

    }
    else if(e->type() == QEvent::Hide && _movie)
    {
        if(_isGIF)
        {
            g_pMainPanel->gifManager->removeMovie(_movie);
            _movie = nullptr;
        }
    }
    return QFrame::event(e);
}

void ImageMessItem::onImageDownloaded()
{
    _imagePath = QTalk::GetImagePathByUrl(_imageLink.toStdString()).data();
    qreal w = 0, h = 0;
    QTalk::Image::scaImageSizeByPath(_imagePath, w, h);
    _size = QSizeF(w, h).toSize();

    setImage();
    // 刷新一下
    if(isVisible())
    {
        setVisible(false);
        setVisible(true);
    }
    emit sgItemChanged();
}