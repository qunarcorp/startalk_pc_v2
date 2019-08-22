//
// Created by cc on 2019-02-28.
//

#include "CodeItem.h"

#include "ChatViewMainPanel.h"
#include "../CustomUi/HeadPhotoLab.h"

#include <QSpacerItem>
#include <QHBoxLayout>
#include <QJsonDocument>
#include <QDesktopServices>
#include <QMouseEvent>
#include <QUrl>
#include "../../Platform/Platform.h"
#include "../../UICom/qimage/qimage.h"
#include "../../QtUtil/Utils/Log.h"

extern ChatViewMainPanel *g_pMainPanel;
CodeItem::CodeItem(const QTalk::Entity::ImMessageInfo &msgInfo, QWidget *parent) :
        MessageItemBase(msgInfo, parent),
        _iconLab(Q_NULLPTR),
        _contentLab(Q_NULLPTR),
        _titleLab(nullptr){
    init();
}

void CodeItem::init() {
    initLayout();
}

void CodeItem::initLayout() {

    _contentSize = QSize(370, 160);
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

QSize CodeItem::itemWdtSize() {
    int height = qMax(_mainMargin.top() + _nameLabHeight + _mainSpacing + _contentFrm->height() + _mainMargin.bottom(),
                      _headPixSize.height()); // 头像和文本取大的
    int width = _contentFrm->width();
    if(nullptr != _readStateLabel)
    {
        height += 12;
    }
    return {width, height + 8};
}

/**
  * @函数名
  * @功能描述
  * @参数
  * @author cc
  * @date 2018.10.19
  */
void CodeItem::initSendLayout() {
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

/**
  * @函数名
  * @功能描述
  * @参数
  * @author cc
  * @date 2018.10.19
  */
void CodeItem::initReceiveLayout() {
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

/**
 *
 */
void CodeItem::initContentLayout() {

    _contentFrm->installEventFilter(this);
    QJsonDocument jsonDocument = QJsonDocument::fromJson(_msgInfo.ExtendedInfo.data());
    QJsonObject jsonObject = jsonDocument.object();
    _codeStyle = jsonObject.value("CodeStyle").toString();
    _codeLanguage = jsonObject.value("CodeType").toString();
    _code = jsonObject.value("Code").toString();

    auto * lay = new QHBoxLayout(_contentFrm);
    lay->setMargin(12);

    QFrame* leftFrm = new QFrame(this);
    leftFrm->setObjectName("CodeLeftFrm");
    QFrame* rightFrm = new QFrame(this);
    rightFrm->setObjectName("CodeRightFrm");
    lay->setSpacing(0);
    lay->addWidget(leftFrm, 0);
    lay->addWidget(rightFrm, 1);

    auto* leftLay = new QVBoxLayout(leftFrm);

    _iconLab = new QLabel(this);
    _iconLab->setFixedSize(40, 40);
    auto pix = QTalk::qimage::instance().loadPixmap(":/chatview/image1/messageItem/code.png", true, true, 40, 40);
    _iconLab->setPixmap(pix);
    leftLay->addWidget(_iconLab);
    leftLay->addItem(new QSpacerItem(10, 10, QSizePolicy::Fixed, QSizePolicy::Expanding));

    auto* rightLay = new QVBoxLayout(rightFrm);
    rightLay->setSpacing(5);
    _titleLab = new QLabel("代码片段", this);
    _titleLab->setObjectName("CodeTitle");

    _contentLab = new QLabel(this);
    _contentLab->setWordWrap(true);
    _contentLab->setObjectName("CodeContent");
    QString text = QString::fromStdString(_msgInfo.Content);
    _contentLab->setText(text.left(200));
    _contentLab->setMaximumHeight(90);
    _contentLab->adjustSize();

    auto* bottomLab = new QLabel("查看更多", this);
    bottomLab->setObjectName("CodeBottomLab");
    rightLay->addWidget(_titleLab, 0);
    rightLay->addWidget(_contentLab, 1);
    rightLay->addWidget(bottomLab, 0);
    rightLay->setAlignment(bottomLab, Qt::AlignRight);

    rightFrm->adjustSize();

    _contentFrm->setFixedHeight(rightFrm->height() + 10 + 24);
}

void CodeItem::mousePressEvent(QMouseEvent *event) {

    if(event->button() == Qt::LeftButton && _contentFrm->geometry().contains(event->pos()))
    {
        if(g_pMainPanel)
        {
            g_pMainPanel->showShowCodeWnd(_codeStyle, _codeLanguage, _code);
        }
    }
    QFrame::mousePressEvent(event);
}

bool CodeItem::eventFilter(QObject* o, QEvent* e)
{
    if(o == _contentFrm)
    {
        if(e->type() == QEvent::Enter)
        {
            setCursor(Qt::PointingHandCursor);
        }
        else if(e->type() == QEvent::Leave)
        {
            setCursor(Qt::ArrowCursor);
        }
    }

    return MessageItemBase::eventFilter(o, e);
}
