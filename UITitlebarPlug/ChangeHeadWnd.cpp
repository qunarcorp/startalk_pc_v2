//
// Created by QITMAC000260 on 2019-03-02.
//

#include "ChangeHeadWnd.h"
#include "../include/Line.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QFileDialog>
#include "../Platform/Platform.h"
#include "../UICom/qimage/qimage.h"
#include <QMouseEvent>
#include <QApplication>
#include <QClipboard>
#include <QMimeData>

ChangeHeadWnd::ChangeHeadWnd(QWidget* parent )
    :UShadowDialog(parent, true)
    ,_pHeadLab(nullptr)
    ,_bottomFrm(nullptr)
{
    initUi();
}

ChangeHeadWnd::~ChangeHeadWnd() = default;

void ChangeHeadWnd::initUi()
{
    setFixedSize(400, 400);

    // top
    QFrame* topFrm = new QFrame(this);
    topFrm->setObjectName("topFrm");
    topFrm->setFixedHeight(50);
    auto* topLay = new QHBoxLayout(topFrm);
    setMoverAble(true, topFrm);

    _pTitleLab = new QLabel(tr("修改头像"), this);
    _pTitleLab->setObjectName("titleLab");
    auto* closeBtn = new QToolButton(this);
#ifdef _MACOS
    topLay->addWidget(closeBtn);
    closeBtn->setObjectName("gmCloseBtn");
    closeBtn->setFixedSize(12, 12);
    topLay->addWidget(_pTitleLab);
    _pTitleLab->setAlignment(Qt::AlignCenter);
    topLay->setContentsMargins(10, 0, 25, 0);
#else
    topLay->addWidget(_pTitleLab);
    topLay->addWidget(closeBtn);
    closeBtn->setObjectName("gwCloseBtn");
    closeBtn->setFixedSize(20, 20);
	_pTitleLab->setAlignment(Qt::AlignCenter);
    topLay->setContentsMargins(30, 0, 10, 0);
#endif

    // mainFrm
    QFrame* bodyFrm = new QFrame(this);
    bodyFrm->setObjectName("bodyFrm");
    bodyFrm->setFrameShape(QFrame::StyledPanel);
    auto * bodyLay = new QHBoxLayout(bodyFrm);
    bodyLay->setContentsMargins(10, 0, 10, 0);
    _pHeadLab = new HeadPhotoLab();
    bodyLay->addWidget(_pHeadLab);
    // bottom

    _bottomFrm = new QFrame(this);
    _bottomFrm->setObjectName("bottomFrm");
    _bottomFrm->setFixedHeight(50);
    auto* bottomLay = new QHBoxLayout(_bottomFrm);
    auto *changeBtn = new QPushButton(tr("选取头像"), this);
    auto *makeSureBtn = new QPushButton(tr("完成"), this);

    changeBtn->setObjectName("ChangeBtn");
    makeSureBtn->setObjectName("ChangeBtn");

    bottomLay->addItem(new QSpacerItem(10, 10, QSizePolicy::Expanding));
    bottomLay->addWidget(changeBtn);
    bottomLay->addWidget(makeSureBtn);

    // main
    auto * lay = new QVBoxLayout(_pCenternWgt);
    lay->setMargin(0);
    lay->setSpacing(0);
    lay->addWidget(topFrm, 0);
    lay->addWidget(bodyFrm, 1);
    lay->addWidget(new Line(), 0);
    lay->addWidget(_bottomFrm, 0);

    connect(closeBtn, &QToolButton::clicked, [this](){setVisible(false);});
    connect(changeBtn, &QPushButton::clicked, this, &ChangeHeadWnd::showChangeHeadWnd);
    connect(makeSureBtn, &QPushButton::clicked, [this](){
        emit sgChangeHead(_headPath);
        this->setVisible(false);
    });
    //
    _pMenu = new QMenu(this);
    auto* copyImgAct = new QAction(tr("复制图片") , _pMenu);
    _pMenu->addAction(copyImgAct);
    connect(copyImgAct, &QAction::triggered, this, &ChangeHeadWnd::onCopyImage);
}

void ChangeHeadWnd::onChangeHeadWnd(const QString& headPath)
{
    if(headPath.isEmpty())
    {

#ifdef _STARTALK
        _pHeadLab->setHead(":/QTalk/image1/StarTalk_defaultHead.png", 140, false, false, false);
#else
        _pHeadLab->setHead(":/QTalk/image1/headPortrait.png", 140, false, false, false);
#endif
    }
    else
    {
        _pHeadLab->setHead(headPath, 140, false, true, false);
    }
    _headPath = headPath;
    _bottomFrm->setVisible(true);
    _pTitleLab->setText(tr("修改头像"));
    this->adjustSize();
    showCenter(true, nullptr);
}

/**
 *
 */
void ChangeHeadWnd::showChangeHeadWnd()
{
    std::string hisDir = Platform::instance().getHistoryDir();

    QString headPath = QFileDialog::getOpenFileName(this, tr("选择头像文件"),
            QString::fromStdString(hisDir), "image (*.jpg *.jpeg *.png *.bmp *.gif)");

    _headPath = headPath;
    if(!headPath.isEmpty())
    {
        _pHeadLab->setHead(headPath, 140, false, true, false);
    }
}

/**
 *
 * @param headPath
 */
void ChangeHeadWnd::onShowHead(const QString &headPath)
{
    if(headPath.isEmpty())
    {

#ifdef _STARTALK
        _pHeadLab->setHead(":/QTalk/image1/StarTalk_defaultHead.png", 140, false, false, false);
#else
        _pHeadLab->setHead(":/QTalk/image1/headPortrait.png", 140, false, false, false);
#endif
    }
    else
    {
        _headPath = headPath;
        _pHeadLab->setHead(headPath, 140, false, true, false);
    }
    _bottomFrm->setVisible(false);
    _pTitleLab->setText(tr("查看头像"));
    this->adjustSize();
    showCenter(true, nullptr);
}

bool ChangeHeadWnd::event(QEvent *e) {
    if(e->type() == QEvent::MouseButtonRelease)
    {
        auto* evt = (QMouseEvent*)e;
        if(_pMenu && evt->button() == Qt::RightButton)
        {
            _pMenu->exec(QCursor::pos());
        }
    }
    return QWidget::event(e);
}

/**
 *
 */
void ChangeHeadWnd::onCopyImage()
{
    if(!_headPath.isEmpty())
    {
        auto *mimeData = new QMimeData;
        mimeData->setUrls(QList<QUrl>() << QUrl::fromLocalFile(_headPath));
        QApplication::clipboard()->setMimeData(mimeData);
    }
}