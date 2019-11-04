#include <utility>

#include "EmojiMessItem.h"
#include <QHBoxLayout>
#include <QSpacerItem>
#include <QDebug>
#include <QMovie>
#include <QFile>
#include <QFileInfo>
#include "../CustomUi/HeadPhotoLab.h"
#include "../../QtUtil/Utils/Log.h"
#include "../../UICom/qimage/qimage.h"
#include "../ChatViewMainPanel.h"
#include "../GIFManager.h"
#include "ChatUtil.h"

extern ChatViewMainPanel *g_pMainPanel;
EmojiMessItem::EmojiMessItem(const QTalk::Entity::ImMessageInfo &msgInfo,
                             QString path,
                             const QSizeF &size,
                             QWidget *parent) :
        MessageItemBase(msgInfo, parent),
        _imageLab(nullptr),
        _movie(nullptr),
        _imagePath(std::move(path)),
        _size(size),
        _isGIF(false){
    init();
    setImage();
    _moveTimer.setSingleShot(true);
    _moveTimer.setInterval(1000);
    connect(&_moveTimer, SIGNAL(timeout()), this, SLOT(onMoveTimer()));
}

/**
  * @函数名
  * @功能描述
  * @参数
  * @date 2018.10.17
  */
QSize EmojiMessItem::itemWdtSize() {
    int height = qMax(_mainMargin.top() + _nameLabHeight + _mainSpacing + _contentFrm->height() + _mainMargin.bottom(),
                      _headPixSize.height()); // 头像和文本取大的
    int width = _contentFrm->width();
    return {width, height + 8};
}

/**
  * @函数名
  * @功能描述
  * @参数
  * @date 2018.10.19
  */
void EmojiMessItem::resizeEvent(QResizeEvent *event) {
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
void EmojiMessItem::init() {
    this->setFrameShape(QFrame::NoFrame);
    initLayout();
}

/**
  * @函数名
  * @功能描述
  * @参数
  * @date 2018.10.17
  */
void EmojiMessItem::initLayout() {
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
    this->setContentsMargins(0, 0, 0, 5);
}

/**
  * @函数名
  * @功能描述
  * @参数
  * @date 2018.10.17
  */
void EmojiMessItem::initSendLayout() {
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

    auto *contentLay = new QVBoxLayout;
    contentLay->setContentsMargins(_contentMargin);
    rightLay->setSpacing(_rightSpacing);
    _contentFrm->setLayout(contentLay);
    if (!_imageLab) {
        _imageLab = new QLabel;
    }
    contentLay->addWidget(_imageLab);
    contentLay->setSpacing(_contentSpacing);
    if (nullptr != _readStateLabel) {
        auto *rsLay = new QHBoxLayout;
        rsLay->addItem(new QSpacerItem(10, 10, QSizePolicy::Expanding));
        rightLay->setMargin(0);
        rsLay->addWidget(_readStateLabel);
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
void EmojiMessItem::initReceiveLayout() {
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
}

/**
  * @函数名
  * @功能描述
  * @参数
  * @date 2018.10.17
  */
void EmojiMessItem::setImage() {
    if (_imagePath.isEmpty() || !QFile::exists(_imagePath)) {
        //warn_log("load head failed, use default picture-> imagePath:{0}, realMessage:{1}", _imagePath,
        //         _msgInfo.Content);

        _imagePath = ":/chatview/image1/defaultImage.png";
        QPixmap image = QTalk::qimage::instance().loadPixmap(_imagePath, true, true, 80, 80);
        _imageLab->setPixmap(image);
        _imageLab->setFixedSize(image.size());
    } else {
        QString suffix = QTalk::qimage::instance().getRealImageSuffix(_imagePath).toUpper();
        if ("GIF" == suffix) {
            QPixmap image = QTalk::qimage::instance().loadPixmap(_imagePath, true);
            if (image.isNull()) {
                _imagePath = "";
                setImage();
                return;
            }
            _isGIF = true;
//            _movie = new QMovie(_imagePath, QByteArray(), _imageLab);
//            _movie->setSpeed(80);
//            _movie->setScaledSize(_size.toSize());
//            _movie->start();
//            _imageLab->setMovie(_movie);
            _imageLab->setFixedSize(_size.toSize());
        } else {
            QPixmap pixmap = QTalk::qimage::instance().loadPixmap(_imagePath, true);
            if (pixmap.isNull()) {
                _imagePath = "";
                setImage();
                return;
            }

            QPixmap npixmap = pixmap.scaled(_size.width(), _size.height(), Qt::KeepAspectRatio,
                                            Qt::SmoothTransformation);
            _imageLab->setPixmap(npixmap);
            _imageLab->setFixedSize(_size.toSize());
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
  * @date 2018.10.19
  */
void EmojiMessItem::onMoveTimer() {
    if (_movie)
        _movie->start();
}

void EmojiMessItem::onImageDownloaded(const QString& path)
{
    _imagePath = path;
    //
    _size = QPixmap(path).size();
    qreal w = 0, h = 0;
    QTalk::Image::scaImageSizeByPath(_imagePath, w, h);
    _size = {w, h};
    //
    setImage();
    // 刷新一下
    if(isVisible())
    {
        setVisible(false);
        setVisible(true);
    }

    emit sgItemChanged();
}

bool EmojiMessItem::event(QEvent* e)
{
    if(e->type() == QEvent::Show && nullptr == _movie)
    {
        if(_isGIF)
        {
            _movie = g_pMainPanel->gifManager->getMovie(_imagePath);
            _movie->setSpeed(80);
            _movie->setScaledSize(_size.toSize());
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