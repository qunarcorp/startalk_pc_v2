#include "FileSendReceiveMessItem.h"
#include "../CustomUi/HeadPhotoLab.h"
#include <QHBoxLayout>
#include <QSpacerItem>
#include <QDebug>
#include <QMovie>
#include <QFileInfo>
#include <QFileIconProvider>
#include <qevent.h>
#include <QProcess>
#include <QFileDialog>
#include <QDesktopServices>
#include <QCryptographicHash>
#include <thread>
#include "../../CustomUi/QtMessageBox.h"
#include "../ChatViewMainPanel.h"
#include "../MessageManager.h"
#include "../Platform/Platform.h"
#include "CustomMenu.h"
#include "../../UICom/qimage/qimage.h"
#include "../../CustomUi/LiteMessageBox.h"
#include "../../QtUtil/Utils/Log.h"
#include "../../QtUtil/lib/MacHelper.h"
#include "../../QtUtil/Utils/utils.h"
#include <math.h>

#ifdef _WINDOWS
#include<windows.h>
#else

#include <unistd.h>

#endif

#define FILEICON_AUDIO ":/QTalk/image1/file_type/audio.png"
#define FILEICON_EXCEL ":/QTalk/image1/file_type/excel.png"
#define FILEICON_PDF ":/QTalk/image1/file_type/pdf.png"
#define FILEICON_PPT ":/QTalk/image1/file_type/ppt.png"
#define FILEICON_PROCESS ":/QTalk/image1/file_type/process.png"
#define FILEICON_TEXT ":/QTalk/image1/file_type/text.png"
#define FILEICON_UNKNOWN ":/QTalk/image1/file_type/unknown.png"
#define FILEICON_VIDEO ":/QTalk/image1/file_type/video.png"
#define FILEICON_WORD ":/QTalk/image1/file_type/word.png"
#define FILEICON_ZIP ":/QTalk/image1/file_type/zip.png"

#define compare_doule_Equal(x, y) (abs(x - y) < 0.00001)

extern ChatViewMainPanel *g_pMainPanel;

FileSendReceiveMessItem::FileSendReceiveMessItem(const QTalk::Entity::ImMessageInfo &msgInfo, QWidget *parent) :
        MessageItemBase(msgInfo, parent),
        _contentTopFrm(Q_NULLPTR),
        _contentButtomFrm(Q_NULLPTR),
        _contentButtomFrmMessLab(Q_NULLPTR),
        _contentButtomFrmDownLoadBtn(Q_NULLPTR),
        _contentButtomFrmMenuBtn(Q_NULLPTR),
        _contentButtomFrmProgressBar(Q_NULLPTR),
        _contentButtomFrmOPenFileBtn(Q_NULLPTR),
        _contentTopFrmIconLab(Q_NULLPTR),
        _contentTopFrmFileNameLab(Q_NULLPTR),
        _contentTopFrmFileSizeLab(Q_NULLPTR),
        _downLoadMenu(Q_NULLPTR),
        _saveAsAct(Q_NULLPTR),
        isDownLoad(false),
        isUpLoad(false),
        _openDir(false),
        _openFileAct(nullptr),
        _openFile(false) {

    init();
    setData();

    if (QTalk::Entity::MessageDirectionSent == _msgDirection) {
        judgeFileIsUpLoad();
    } else if (QTalk::Entity::MessageDirectionReceive == _msgDirection) {
        judgeFileIsDownLoad();
    }

}

/**
  * @函数名
  * @功能描述
  * @参数
  * @author cc
  * @date 2018.10.19
  */
QSize FileSendReceiveMessItem::itemWdtSize() {
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
  * @date 2018.10.22
  */
void FileSendReceiveMessItem::setProcess(double speed, double dtotal, double dnow, double utotal, double unow) {

    // 发送文件切换会话处理
    if(QTalk::Entity::MessageDirectionSent == _msgDirection && utotal > 0)
    {
        if(_resending && _resending->isVisible())
            _resending->setVisible(false);

        if(_sending && !_sending->isVisible())
        {
            _sending->startMovie();
            _sending->setVisible(true);
        }
    }

    double process = 0;
    if (dtotal > 0) {
        process = (dnow * 100.0) / dtotal;
    } else if (utotal > 0) {
        process = (unow * 100.0) / utotal;
    } else {
        return;
    }

    if (_contentButtomFrmProgressBar) {
        _contentButtomFrmProgressBar->setCurValue(process);
    }
    if (_contentButtomFrmMessLab) {
        QString unit = " B/S";
        if (speed > 1024 * 1024 * 1024) {
            unit = " G/S";
            speed /= 1024 * 1024 * 1024;
        } else if (speed > 1024 * 1024) {
            unit = " M/S";
            speed /= 1024 * 1024;
        } else if (speed > 1024) {
            unit = " kB/S";
            speed /= 1024;
        }
        QString sspeed = QString::number(speed, 10, 2) + unit;
        _contentButtomFrmMessLab->setText(sspeed);
    }
    if (compare_doule_Equal(100, process) && _contentButtomFrmMessLab) //
    {
        isDownLoad = true;
        isUpLoad = true;

        if (QTalk::Entity::MessageDirectionSent == _msgDirection) {
            _contentButtomFrmMessLab->setText(tr("上传成功"));
            _contentButtomFrmProgressBar->hide();
            _contentButtomFrmOPenFileBtn->show();
        } else if (QTalk::Entity::MessageDirectionReceive == _msgDirection) {
            _contentButtomFrmMessLab->setText(tr("已下载"));
            _contentButtomFrmDownLoadBtn->hide();
            _contentButtomFrmMenuBtn->show();
            _contentButtomFrmProgressBar->hide();
            _contentButtomFrmOPenFileBtn->show();
        }

        QPointer<FileSendReceiveMessItem> pThis(this);
        std::thread([pThis](){
            int i = 0;
            if(!pThis) return;

            QString filePath(pThis->getLocalFilePath());
            while (!QFile::exists(filePath) && i < 1000) {
#ifdef _WINDOWS
                Sleep(100);
#else
                usleep(100000);
#endif
                ++i;
            }

            if(pThis && !filePath.isEmpty())
                g_pMainPanel->makeFileLink(filePath, pThis->_msgInfo.FileMd5.data());

            if (pThis && (pThis->_openDir || pThis->_openFile)) {
                if(!filePath.isEmpty())
                {
                    if (pThis && pThis->_openDir)
                        emit pThis->sgOpenDir();
                    else if (pThis && pThis->_openFile)
                        emit pThis->sgOpenFile(false);
                }

                if(pThis)
                {
                    pThis->_openDir = false;
                    pThis->_openFile = false;
                }
            }

        }).detach();
    }
}

/**
  * @函数名
  * @功能描述
  * @参数
  * @author cc
  * @date 2018.10.17
  */
void FileSendReceiveMessItem::init()
{
    this->setFrameShape(QFrame::NoFrame);
    initFileIconMap();
    initLayout();
    initMenu();
}

/**
  * @函数名
  * @功能描述
  * @参数
  * @author cc
  * @date 2018.10.22
  */
void FileSendReceiveMessItem::initFileIconMap() {
    _fileIcons.insert("mp3", FILEICON_AUDIO);
    _fileIcons.insert("txt", FILEICON_TEXT);
    _fileIcons.insert("pdf", FILEICON_PDF);
    _fileIcons.insert("ppt", FILEICON_PPT);
    _fileIcons.insert("pptx", FILEICON_PPT);
    _fileIcons.insert("doc", FILEICON_WORD);
    _fileIcons.insert("docx", FILEICON_WORD);
    _fileIcons.insert("xls", FILEICON_EXCEL);
    _fileIcons.insert("xlsx", FILEICON_EXCEL);
    _fileIcons.insert("rar", FILEICON_ZIP);
    _fileIcons.insert("zip", FILEICON_ZIP);
    _fileIcons.insert("json", FILEICON_TEXT);
}

/**
  * @函数名
  * @功能描述
  * @参数
  * @author cc
  * @date 2018.10.22
  */
void FileSendReceiveMessItem::initMenu() {
    _saveAsAct = new QAction(tr("另存为"), this);
    _openFileAct = new QAction(tr("打开文件"), this);
    _downLoadMenu = new QMenu(this);
    _downLoadMenu->setAttribute(Qt::WA_TranslucentBackground, true);
    _downLoadMenu->setFixedWidth(84);
    _downLoadMenu->addAction(_openFileAct);
    _downLoadMenu->addAction(_saveAsAct);
    connect(_saveAsAct, SIGNAL(triggered(bool)), this, SLOT(onSaveAsAct()));
    connect(_openFileAct, &QAction::triggered, this, &FileSendReceiveMessItem::onOpenFile);
    connect(this, &FileSendReceiveMessItem::sgOpenFile, this, &FileSendReceiveMessItem::onOpenFile, Qt::QueuedConnection);
}

/**
  * @函数名
  * @功能描述
  * @参数
  * @author cc
  * @date 2018.10.19
  */
void FileSendReceiveMessItem::initLayout() {
    _contentSize = QSize(251, 104);
    _sizeMaxPix = QSize(200, 200);
    _mainMargin = QMargins(15, 0, 25, 0);
    _contentMargin = QMargins(0, 0, 0, 0);
    _mainSpacing = 10;
    _contentSpacing = 0;
    _contentButtomFrmHeight = 40;
    _btnSzie = QSize(30, 30);
    _contentTopFrmHlayMargin = QMargins(12, 0, 25, 0);
    _contentButtomFrmHlayMargin = QMargins(10, 0, 10, 0);
    _contentTopFrmIconLabSize = QSize(40, 40);
    _contentTopFrmFileNameLabHeight = 18;
    _contentTopFrmFileSizeLabHeight = 16;
    _contentTopFrmHlaySpacing = 19;
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
void FileSendReceiveMessItem::initSendLayout() {
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
    _contentFrm->setFixedSize(_contentSize);

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

    initSendContentLayout();
}

/**
  * @函数名
  * @功能描述
  * @参数
  * @author cc
  * @date 2018.10.19
  */
void FileSendReceiveMessItem::initReceiveLayout() {
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
    _contentFrm->setFixedSize(_contentSize);
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

    initReceiveContentLayout();
}

/**
  * @函数名
  * @功能描述
  * @参数
  * @author cc
  * @date 2018.10.19
  */
void FileSendReceiveMessItem::initSendContentLayout() {
    auto *contentLay = new QVBoxLayout;
    contentLay->setContentsMargins(_contentMargin);
    contentLay->setSpacing(_contentSpacing);
    _contentFrm->setLayout(contentLay);
    if (!_contentTopFrm) {
        _contentTopFrm = new QFrame(this);
    }
    _contentTopFrm->setObjectName("messSendContentTopFrm");
    if (!_contentButtomFrm) {
        _contentButtomFrm = new QFrame(this);
    }
    _contentButtomFrm->setObjectName("messSendContentBottomFrm");
    _contentButtomFrm->setFixedHeight(_contentButtomFrmHeight);
    contentLay->addWidget(_contentTopFrm);
    contentLay->addWidget(_contentButtomFrm);
    contentLay->setStretch(0, 1);
    contentLay->setStretch(1, 0);
    initSendContentTopFrmLayout();
    initSendContentButtomFrmLayout();
}

/**
  * @函数名
  * @功能描述
  * @参数
  * @author cc
  * @date 2018.10.19
  */
void FileSendReceiveMessItem::initReceiveContentLayout() {
    auto *contentLay = new QVBoxLayout;
    contentLay->setContentsMargins(_contentMargin);
    contentLay->setSpacing(_contentSpacing);
    _contentFrm->setLayout(contentLay);
    if (!_contentTopFrm) {
        _contentTopFrm = new QFrame(this);
    }
    _contentTopFrm->setObjectName("messReceiveContentTopFrm");
    if (!_contentButtomFrm) {
        _contentButtomFrm = new QFrame(this);
    }
    _contentButtomFrm->setObjectName("messReceiveContentBottomFrm");
    _contentButtomFrm->setFixedHeight(_contentButtomFrmHeight);
    contentLay->addWidget(_contentTopFrm);
    contentLay->addWidget(_contentButtomFrm);
    contentLay->setStretch(0, 1);
    contentLay->setStretch(1, 0);
    initReceiveContentTopFrmLayout();
    initReceiveContentButtomFrmLayout();
}

/**
  * @函数名
  * @功能描述
  * @参数
  * @author cc
  * @date 2018.10.21
  */
void FileSendReceiveMessItem::initSendContentTopFrmLayout() {
    auto *contentTopFrmHlay = new QHBoxLayout;
    contentTopFrmHlay->setContentsMargins(_contentTopFrmHlayMargin);
    contentTopFrmHlay->setSpacing(_contentTopFrmHlaySpacing);
    _contentTopFrm->setLayout(contentTopFrmHlay);
    if (!_contentTopFrmIconLab) {
        _contentTopFrmIconLab = new HeadPhotoLab;
        _contentTopFrmIconLab->setObjectName("contentTopFrmIconLab");
    }
    _contentTopFrmIconLab->setFixedSize(_contentTopFrmIconLabSize);
    contentTopFrmHlay->addWidget(_contentTopFrmIconLab);
    auto *contentTopRightFrmHlay = new QVBoxLayout;
    contentTopRightFrmHlay->setContentsMargins(0, 14, 0, 12);
    contentTopRightFrmHlay->setSpacing(8);
    contentTopFrmHlay->addLayout(contentTopRightFrmHlay);
    if (!_contentTopFrmFileNameLab) {
        _contentTopFrmFileNameLab = new QLabel;
    }
    _contentTopFrmFileNameLab->setObjectName("contentTopFrmFileNameLab");
    _contentTopFrmFileNameLab->setFixedHeight(_contentTopFrmFileNameLabHeight);
    contentTopRightFrmHlay->addWidget(_contentTopFrmFileNameLab);
    if (!_contentTopFrmFileSizeLab) {
        _contentTopFrmFileSizeLab = new QLabel;
    }
    _contentTopFrmFileSizeLab->setObjectName("contentTopFrmFileSizeLab");
    _contentTopFrmFileNameLab->setFixedHeight(_contentTopFrmFileSizeLabHeight);
    contentTopRightFrmHlay->addWidget(_contentTopFrmFileSizeLab);
    contentTopFrmHlay->setStretch(0, 0);
    contentTopFrmHlay->setStretch(1, 1);
}

/**
  * @函数名
  * @功能描述
  * @参数
  * @author cc
  * @date 2018.10.21
  */
void FileSendReceiveMessItem::initSendContentButtomFrmLayout() {
    auto *contentButtomFrmHlay = new QHBoxLayout;
    contentButtomFrmHlay->setContentsMargins(_contentButtomFrmHlayMargin);
    _contentButtomFrm->setLayout(contentButtomFrmHlay);
    if (!_contentButtomFrmMessLab) {
        _contentButtomFrmMessLab = new QLabel;
    }
    _contentButtomFrmMessLab->setObjectName("contentButtomFrmMessLab");
    _contentButtomFrmMessLab->setText(tr("正在发送"));
    contentButtomFrmHlay->addWidget(_contentButtomFrmMessLab);

    if (!_contentButtomFrmProgressBar) {
        _contentButtomFrmProgressBar = new FileRoundProgressBar;
    }
    _contentButtomFrmProgressBar->setObjectName("contentButtomFrmProgressBar");
    _contentButtomFrmProgressBar->setFixedSize(_btnSzie);

    if (!_contentButtomFrmOPenFileBtn) {
        _contentButtomFrmOPenFileBtn = new QPushButton;
    }
    _contentButtomFrmOPenFileBtn->setObjectName("contentButtomFrmOPenFileBtn");
    _contentButtomFrmOPenFileBtn->setFixedSize(_btnSzie);
    contentButtomFrmHlay->addWidget(_contentButtomFrmOPenFileBtn);
    contentButtomFrmHlay->addWidget(_contentButtomFrmProgressBar);

    contentButtomFrmHlay->addWidget(_contentButtomFrmOPenFileBtn);
    contentButtomFrmHlay->setStretch(0, 1);
    contentButtomFrmHlay->setStretch(1, 0);
    contentButtomFrmHlay->setStretch(2, 0);
    _contentButtomFrmOPenFileBtn->hide();
    connect(_contentButtomFrmOPenFileBtn, SIGNAL(clicked(bool)),
            this, SLOT(onOpenFilePath()));
}

/**
  * @函数名
  * @功能描述
  * @参数
  * @author cc
  * @date 2018.10.19
  */
void FileSendReceiveMessItem::initReceiveContentTopFrmLayout() {
    auto *contentTopFrmHlay = new QHBoxLayout;
    contentTopFrmHlay->setContentsMargins(_contentTopFrmHlayMargin);
    contentTopFrmHlay->setSpacing(_contentTopFrmHlaySpacing);
    _contentTopFrm->setLayout(contentTopFrmHlay);
    if (!_contentTopFrmIconLab) {
        _contentTopFrmIconLab = new HeadPhotoLab;
        _contentTopFrmIconLab->setObjectName("contentTopFrmIconLab");
    }
    _contentTopFrmIconLab->setFixedSize(_contentTopFrmIconLabSize);
    contentTopFrmHlay->addWidget(_contentTopFrmIconLab);
    auto *contentTopRightFrmHlay = new QVBoxLayout;
    contentTopRightFrmHlay->setContentsMargins(0, 10, 0, 10);
    contentTopRightFrmHlay->setSpacing(6);
    contentTopFrmHlay->addLayout(contentTopRightFrmHlay);
    if (!_contentTopFrmFileNameLab) {
        _contentTopFrmFileNameLab = new QLabel;
    }
    _contentTopFrmFileNameLab->setObjectName("contentTopFrmFileNameLab");
    _contentTopFrmFileNameLab->setFixedHeight(_contentTopFrmFileNameLabHeight);
    contentTopRightFrmHlay->addWidget(_contentTopFrmFileNameLab);
    if (!_contentTopFrmFileSizeLab) {
        _contentTopFrmFileSizeLab = new QLabel;
    }
    _contentTopFrmFileSizeLab->setObjectName("contentTopFrmFileSizeLab");
    _contentTopFrmFileSizeLab->setFixedHeight(_contentTopFrmFileSizeLabHeight);
    contentTopRightFrmHlay->addWidget(_contentTopFrmFileSizeLab);
    contentTopFrmHlay->setStretch(0, 0);
    contentTopFrmHlay->setStretch(1, 1);
}

/**
  * @函数名
  * @功能描述
  * @参数
  * @author cc
  * @date 2018.10.19
  */
void FileSendReceiveMessItem::initReceiveContentButtomFrmLayout() {
    auto *contentButtomFrmHlay = new QHBoxLayout;
    contentButtomFrmHlay->setContentsMargins(_contentButtomFrmHlayMargin);
    _contentButtomFrm->setLayout(contentButtomFrmHlay);
    if (!_contentButtomFrmMessLab) {
        _contentButtomFrmMessLab = new QLabel;
    }
    _contentButtomFrmMessLab->setObjectName("contentButtomFrmMessLab");
    _contentButtomFrmMessLab->setText(tr("等待接收"));
    contentButtomFrmHlay->addWidget(_contentButtomFrmMessLab);
    if (!_contentButtomFrmDownLoadBtn) {
        _contentButtomFrmDownLoadBtn = new QPushButton;
    }
    _contentButtomFrmDownLoadBtn->setObjectName("contentButtomFrmDownLoadBtn");
    _contentButtomFrmDownLoadBtn->setFixedSize(_btnSzie);
    contentButtomFrmHlay->addWidget(_contentButtomFrmDownLoadBtn);

    if (!_contentButtomFrmProgressBar) {
        _contentButtomFrmProgressBar = new FileRoundProgressBar;
    }
    _contentButtomFrmProgressBar->setObjectName("contentButtomFrmProgressBar");
    _contentButtomFrmProgressBar->setFixedSize(_btnSzie);
    contentButtomFrmHlay->addWidget(_contentButtomFrmProgressBar);
    if (!_contentButtomFrmOPenFileBtn) {
        _contentButtomFrmOPenFileBtn = new QPushButton;
    }
    _contentButtomFrmOPenFileBtn->setObjectName("contentButtomFrmOPenFileBtn");
    _contentButtomFrmOPenFileBtn->setFixedSize(_btnSzie);
    contentButtomFrmHlay->addWidget(_contentButtomFrmOPenFileBtn);

    if (!_contentButtomFrmMenuBtn) {
        _contentButtomFrmMenuBtn = new QToolButton;
    }
    _contentButtomFrmMenuBtn->setObjectName("contentButtomFrmMenuBtn");
    _contentButtomFrmMenuBtn->setFixedSize(_btnSzie);
    contentButtomFrmHlay->addWidget(_contentButtomFrmMenuBtn);

    contentButtomFrmHlay->setStretch(0, 1);
    contentButtomFrmHlay->setStretch(1, 0);
    contentButtomFrmHlay->setStretch(2, 0);
    contentButtomFrmHlay->setStretch(3, 0);
    contentButtomFrmHlay->setStretch(4, 0);

    _contentButtomFrmProgressBar->hide();
    _contentButtomFrmOPenFileBtn->hide();
    connect(_contentButtomFrmDownLoadBtn, SIGNAL(clicked(bool)),
            this, SLOT(onDownLoadClicked()));
    connect(_contentButtomFrmOPenFileBtn, SIGNAL(clicked(bool)),
            this, SLOT(onOpenFilePath()));
    connect(_contentButtomFrmMenuBtn, SIGNAL(clicked(bool)),
            this, SLOT(onMenuBtnChicked()));
    connect(this, SIGNAL(sgOpenDir()),
            this, SLOT(onOpenFilePath()));

}

/**
  * @函数名
  * @功能描述
  * @参数
  * @author cc
  * @date 2018.10.22
  */
void FileSendReceiveMessItem::setData() {
    if (_contentTopFrmFileNameLab) {
        _contentTopFrmFileNameLab->setText(QString::fromStdString(_msgInfo.FileName));
    }
    if (_contentTopFrmFileSizeLab) {
        _contentTopFrmFileSizeLab->setText(QString::fromStdString(_msgInfo.FileSize));
    }

    QFileInfo fileinfo(QString::fromStdString(_msgInfo.FileName));
    QString suffix = fileinfo.suffix().toLower();
    if (!_fileIcons.value(suffix).isEmpty()) {
        _contentTopFrmIconLab->setHead(_fileIcons.value(suffix), _contentTopFrmIconLabSize.width() / 2, false, false, true);
    } else {
        _contentTopFrmIconLab->setHead(FILEICON_UNKNOWN, _contentTopFrmIconLabSize.width() / 2, false, false, true);
    }
    //
    _contentTopFrmFileNameLab->setToolTip(QString::fromStdString(_msgInfo.FileName));
}

/**
  * @函数名
  * @功能描述 判断文件是否已下载或者发送成功
  * @参数
  * @author cc
  * @date 2018.10.25
  */
void FileSendReceiveMessItem::judgeFileIsDownLoad()
{
    auto path = getLocalFilePath();
    if (!path.isEmpty() && QFileInfo(path).size() > 0) {
        _contentButtomFrmMessLab->setText(tr("已下载"));
        _contentButtomFrmDownLoadBtn->hide();
        _contentButtomFrmMenuBtn->show();
        _contentButtomFrmProgressBar->hide();
        _contentButtomFrmOPenFileBtn->show();

        isDownLoad = true;
    } else {
        isDownLoad = false;
    }
}

/**
  * @函数名
  * @功能描述 判断文件是否已上传
  * @参数
  * @author cc
  * @date 2018.10.25
  */
void FileSendReceiveMessItem::judgeFileIsUpLoad() {
    if (!_msgInfo.FileUrl.empty()) {
        isUpLoad = true;
        _contentButtomFrmMessLab->setText(tr("上传成功"));
        _contentButtomFrmProgressBar->hide();
        _contentButtomFrmOPenFileBtn->show();
    } else {
        _contentButtomFrmMessLab->setText(tr("正在处理"));
        isUpLoad = false;
    }
}

/**
  * @函数名
  * @功能描述
  * @参数
  * @author cc
  * @date 2018.10.22
  */
void FileSendReceiveMessItem::sendDownLoadFile(const std::string &strLocalPath, const std::string &strUri) {
    if (g_pMainPanel && g_pMainPanel->getMessageManager()) {
        g_pMainPanel->getMessageManager()->sendDownLoadFile(strLocalPath, strUri, _msgInfo.MsgId);
    }
}

/**
  * @函数名
  * @功能描述
  * @参数
  * @author cc
  * @date 2018.10.22
  */
void FileSendReceiveMessItem::downLoadFile()
{
    QPointer<FileSendReceiveMessItem> pThis(this);

    if(!pThis) return;

    if (_contentButtomFrmDownLoadBtn)
        _contentButtomFrmDownLoadBtn->hide();
    if (_contentButtomFrmMenuBtn)
        _contentButtomFrmMenuBtn->hide();

    _contentButtomFrmProgressBar->show();
    _contentButtomFrmOPenFileBtn->hide();

    std::string localPath = g_pMainPanel->getFileMsgLocalPath(_msgInfo.MsgId);
    std::string netPath = _msgInfo.FileUrl;
    if (localPath.empty()) {
        QString fileName = _msgInfo.FileName.data();
        fileName.replace(QRegExp("[\"|\\<|\\>|\\“|\\”|\\、|\\╲|\\/|\\\\|\\:|\\*|\\?]"), "");
        std::ostringstream src;
        src << AppSetting::instance().getFileSaveDirectory()
            << "/"
            << fileName.toStdString();
        localPath = src.str();
    }
    // 文件名处理
    QString qLocalFilePath = QString::fromStdString(localPath);
    QFileInfo qFileInfo = QFileInfo(qLocalFilePath);
    if (QFile::exists(qLocalFilePath)) {
        QString baseName = qLocalFilePath.left(qLocalFilePath.lastIndexOf("."));
        QString suffix = qFileInfo.suffix();
        //
        int i = 0;
        //
        do {
            qLocalFilePath = QString("%1(%2).%3").arg(baseName).arg(++i).arg(suffix);
        } while (QFile::exists(qLocalFilePath));
        //
    }
    // 占个坑
    {
        QFile file(qLocalFilePath);
        file.open(QIODevice::WriteOnly);
        file.close();
    }
    //下载文件
    localPath = qLocalFilePath.toStdString();
    sendDownLoadFile(localPath, netPath);
    // 回写配置文件
    g_pMainPanel->setFileMsgLocalPath(_msgInfo.MsgId, localPath);
}

/**
  * @函数名
  * @功能描述
  * @参数
  * @author cc
  * @date 2018.10.25
  */
QString FileSendReceiveMessItem::getLocalFilePath()
{
    QString fullFileName = QString::fromStdString(g_pMainPanel->getFileMsgLocalPath(_msgInfo.MsgId));
	if(fullFileName.isEmpty() || !QFile::exists(fullFileName))
    {
	    QString linkFile = g_pMainPanel->getFileLink(_msgInfo.FileMd5.data());
#ifdef _WINDOWS
		linkFile.append(".lnk");
#endif
	    QFileInfo linkFileInfo(linkFile);
		if (linkFileInfo.exists() && !linkFileInfo.canonicalFilePath().isEmpty())
#ifdef _WINDOWS
			return linkFileInfo.symLinkTarget();
#else
			return linkFileInfo.canonicalFilePath();
#endif // _WINDOWS
        else
            return QString();
    }
	else
    {
        return fullFileName;
    }
}

/**
  * @函数名
  * @功能描述
  * @参数
  * @author cc
  * @date 2018.10.21
  */
void FileSendReceiveMessItem::onDownLoadClicked() {
    downLoadFile();
}

/**
  * @函数名
  * @功能描述
  * @参数
  * @author cc
  * @date 2018.10.22
  */
void FileSendReceiveMessItem::onMenuBtnChicked() {
    if (_downLoadMenu && _contentButtomFrmMenuBtn) {
        QPoint pos1 = QPoint(_contentButtomFrmMenuBtn->rect().center().x(), _contentButtomFrmMenuBtn->rect().bottom());
        QPoint pos = _contentButtomFrmMenuBtn->mapToGlobal(pos1);
        pos = QPoint(pos.x() - (_downLoadMenu->width() / 2), pos.y() + 2);
        _downLoadMenu->exec(pos);
    }
}

/**
  * @函数名
  * @功能描述
  * @参数
  * @author cc
  * @date 2018.10.22
  */
void FileSendReceiveMessItem::onSaveAsAct() {

    QString filePath = getLocalFilePath();
    QFileInfo fileInfo(filePath);

	QString suffix = QFileInfo(_msgInfo.FileName.data()).suffix();
	QString historyPath = QString::fromStdString(Platform::instance().getHistoryDir());
	QString fileName = QFileDialog::getSaveFileName(this,
		tr("另存为"),
		historyPath + "/" + QString::fromStdString(_msgInfo.FileName),
		tr("File (*.%1)").arg(suffix));
	if (fileName.isEmpty())
		return;

    if(!filePath.isEmpty() && fileInfo.exists())
    {
        std::string fileMd5 = QTalk::utils::getFileMd5(filePath.toLocal8Bit().data());
        if(_msgInfo.FileMd5 == fileMd5)
        {
            Platform::instance().setHistoryDir(QFileInfo(fileName).absoluteDir().absolutePath().toStdString());

            QFile::copy(filePath, fileName);
            LiteMessageBox::success(QString(tr("文件已另存为:%1")).arg(fileName));
            return;
        }
    }
    // 下载文件
	g_pMainPanel->setFileMsgLocalPath(_msgInfo.MsgId, fileName.toStdString());
    downLoadFile();
}

/**
  * @函数名
  * @功能描述
  * @参数
  * @author cc
  * @date 2018.10.22
  */
void FileSendReceiveMessItem::onOpenFilePath()
{
    QString fileName = getLocalFilePath();
    QFileInfo info(fileName);
    if (fileName.isEmpty() || !info.exists()) {
        int ret = QtMessageBox::question(this, tr("提醒"), tr("未找到本地文件, 是否下载?"));
        if (ret == QtMessageBox::EM_BUTTON_NO) {
            return;
        } else {
            _openDir = true;
            g_pMainPanel->pool().enqueue(std::bind(&FileSendReceiveMessItem::downLoadFile, this));
        }
    } else {

        std::thread([info](){

#if defined(_WINDOWS)
            QStringList params;
        QString path = info.absoluteFilePath();
        if (!QFileInfo(path).isDir()) {
            params << QLatin1String("/select,");
            params << QDir::toNativeSeparators(path);
            QProcess::startDetached(QLatin1String("explorer.exe"), params);
        }
#elif defined(_MACOS)
            QStringList scriptArgs;
            QDesktopServices::openUrl(QUrl::fromLocalFile(info.absolutePath()));
            scriptArgs << QLatin1String("-e")
                       << QString::fromLatin1(R"(tell application "Finder" to reveal POSIX file "%1")")
                               .arg(info.absoluteFilePath());
            QProcess::execute(QLatin1String("/usr/bin/osascript"), scriptArgs);
#else
            QDesktopServices::openUrl(QUrl::fromLocalFile(info.absolutePath()));
#endif
        }).detach();

    }
}

void FileSendReceiveMessItem::onOpenFile(bool) {
    QString fileName = getLocalFilePath();

    if (!QFileInfo::exists(fileName)) {
        int ret = QtMessageBox::question(this, tr("提醒"), tr("未找到本地文件, 是否下载?"));
        if (ret == QtMessageBox::EM_BUTTON_NO) {
            return;
        } else {
            _openFile = true;
            g_pMainPanel->pool().enqueue(std::bind(&FileSendReceiveMessItem::downLoadFile, this));
        }
    } else {
        QDesktopServices::openUrl(QUrl::fromLocalFile(fileName));
    }
}