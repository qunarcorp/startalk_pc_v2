//
// Created by QITMAC000260 on 2019/10/21.
//

#include "UserMedalWnd.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QToolButton>
#include <QFile>
#include <QDateTime>
#include <QHeaderView>
#include <QScrollBar>
#include <QDebug>
#include "../../CustomUi/HeadPhotoLab.h"
#include "../../Platform/Platform.h"
#include "../../Platform/dbPlatForm.h"
#include "../../UICom/qimage/qimage.h"

//
UserMedalTip::UserMedalTip(int count, QWidget* parent)
        :QFrame(parent), _user_medal_size(count)
{
    setFixedSize(44, 61);
}

void UserMedalTip::paintEvent(QPaintEvent * e)
{
    QPixmap pixmap(":/CardManager/image1/MadelBg.png");
    QPainter painter(this);
    painter.drawPixmap(0, 0, this->width(), this->height(), pixmap);
    //
    QString count = QString::number(_user_medal_size);
    QFont font;
    font.setBold(true);
    font.setPixelSize(20);
    painter.setPen(QColor(136,92,83));
    painter.setFont(font);
    int fw = QFontMetricsF(font).width(count) + 1;
    int fh = QFontMetricsF(font).height() + 1;
    QRect countRect((this->width() - fw) / 2, 5, fw, fh);
    painter.drawText(countRect, count);
    //
    font.setBold(false);
    font.setPixelSize(11);
    painter.setFont(font);
    QString text = tr("勋章");
    fw = QFontMetricsF(font).width(text) + 1;
    fh = QFontMetricsF(font).height() + 1;
    QRect textRect((this->width() - fw) / 2, 30, fw, fh);
    painter.drawText(textRect, text);
}

// --------------
UserMedalWnd::~UserMedalWnd() = default;

UserMedalWnd::UserMedalWnd(const QString& userName,
                           const QString& headPath,
                           const std::set<QTalk::StUserMedal> &user_medals, QWidget *parent)
                           : QFrame(parent)
{
    //
    setFixedWidth(385);
    this->setMinimumHeight(540);
    //
    auto* closeBtn = new QToolButton(this);
    auto* headLabel = new HeadPhotoLab();
    headLabel->setParent(this);
    if (QFile::exists(headPath))
    {
        headLabel->setHead(headPath, 30, false, true);
    }
    else
    {
#ifdef _STARTALK
        QString defaultHead = ":/QTalk/image1/StarTalk_defaultHead.png";
#else
        QString defaultHead = ":/QTalk/image1/headPortrait.png";
#endif
        headLabel->setHead(defaultHead, 30);
    }

    auto* nameLabel = new QLabel(userName, this);
    nameLabel->setContentsMargins(0, 18, 0, 0);
    nameLabel->setObjectName("UserMedalWnd_NameLabel");

    auto* medalBgItem = new UserMedalTip(user_medals.size(), this);

    _topFrm = new QFrame(this);
    _topFrm->setObjectName("UserMedalWnd_topFrm");
    _topFrm->setFixedHeight(140);

    auto* topLay = new QHBoxLayout(_topFrm);
    
    auto* topLeftLay = new QVBoxLayout;
    topLay->addLayout(topLeftLay);
    topLeftLay->setContentsMargins(2, 12, 12, 0);
#ifdef _MACOS
	topLay->setContentsMargins(10, 0, 12, 0);
    closeBtn->setFixedSize(12, 12);
    closeBtn->setObjectName("gmCloseBtn");
    topLeftLay->addWidget(closeBtn);
#endif

    auto* info_lay = new QHBoxLayout();
    info_lay->setContentsMargins(20, 10, 25, 35);
    info_lay->setSpacing(16);
    info_lay->addWidget(headLabel);
    info_lay->addWidget(nameLabel);
    info_lay->setAlignment(nameLabel, Qt::AlignTop);
    topLeftLay->addLayout(info_lay);
    //
    topLay->addWidget(medalBgItem);
    topLay->setAlignment(medalBgItem, Qt::AlignTop);
#ifndef _MACOS
	topLay->setContentsMargins(10, 0, 0, 0);
    topLay->addWidget(closeBtn);
	topLay->setAlignment(closeBtn, Qt::AlignTop);
    closeBtn->setFixedSize(30, 30);
    closeBtn->setObjectName("gwCloseBtn");
#endif
//    setMoverAble(true, topFrm);

    auto* bodyFrm = new QFrame(this);
    bodyFrm->setObjectName("UserMedalWnd_bodyFrm");
    auto* bodyLay = new QHBoxLayout(bodyFrm);
    bodyLay->setContentsMargins(25, 20, 22, 20);
    _pMainWgt = new QTableWidget(this);
    _pMainWgt->setObjectName("UserMedalWnd_TableWgt");
    bodyLay->addWidget(_pMainWgt);
//    bodyLay->setAlignment(_pMainWgt, Qt::AlignHCenter);
    _pMainWgt->setAutoScroll(false);
    _pMainWgt->setFrameShape(QFrame::NoFrame);
    _pMainWgt->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    _pMainWgt->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    _pMainWgt->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    _pMainWgt->horizontalHeader()->setVisible(false);
    _pMainWgt->verticalHeader()->setVisible(false);
    _pMainWgt->setShowGrid(false);
    _pMainWgt->setWindowFlags(Qt::FramelessWindowHint);
    _pMainWgt->setColumnCount(3);
    _pMainWgt->setFocusPolicy(Qt::NoFocus);
    _pMainWgt->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    _pMainWgt->verticalScrollBar()->setSingleStep(15);
    //
    init_medals(user_medals);

    auto* mainFrm = new QFrame(this);
    mainFrm->setObjectName("UserMedalWnd_MainFrm");
    auto mainLay = new QVBoxLayout(mainFrm);
    mainLay->setMargin(0);
    mainLay->addWidget(_topFrm);
    mainLay->addWidget(bodyFrm);

    auto* lay = new QHBoxLayout(this);
    lay->setMargin(0);
    lay->addWidget(mainFrm);

    connect(closeBtn, &QToolButton::clicked, [parent](){parent->setVisible(false);});
    connect(_pMainWgt, &QTableWidget::cellPressed, [this](int row, int col){
        QWidget* wgt = _pMainWgt->cellWidget(row, col);
        auto* itemWgt = qobject_cast<UserMedalItem*>(wgt);
        if(itemWgt && itemWgt->_medalId >= 0)
            emit sgShowMedalDetail(itemWgt->_medalId);
    });
}

//
void init_medal_wgt(QTableWidget* wgt,
                    const std::set<StMedalInfo> &medals)
{
    int row = 0, col = 0;
    for(const auto& m : medals)
    {
        if (wgt->rowCount() != row + 1)
        {
            wgt->insertRow(row);
            wgt->setRowHeight(row, 170);
        }

        auto id = m.im_medal.medalId;
        auto status = m.medal_status;
        //
        auto* itemWgt = new UserMedalItem();
        itemWgt->setUserMedal(m.im_medal, status, false);
        itemWgt->setContentsMargins(10, 15, 10, 15);
        wgt->setCellWidget(row, col, itemWgt);

        if(++col >= 3)
        {
            row++;
            col = 0;
        }
    }
    //
    {
        if (wgt->rowCount() != row + 1)
        {
            wgt->insertRow(row);
            wgt->setRowHeight(row, 170);
        }
        auto* itemWgt = new UserMedalItem();
        itemWgt->setContentsMargins(10, 15, 10, 15);
        itemWgt->setComingSoon();
        wgt->setCellWidget(row, col, itemWgt);
    }
}

//
void UserMedalWnd::init_medals(const std::set<QTalk::StUserMedal> &user_medals) {
    _pMainWgt->clear();
    _pMainWgt->setRowCount(0);
    //
    std::vector<QTalk::Entity::ImMedalList> medals;
    dbPlatForm::instance().getAllMedals(medals);
    // sort
    std::set<StMedalInfo> sort_medals;
    for(const auto& imMedalList : medals)
    {
        StMedalInfo info;
        info.im_medal = imMedalList;

        auto itFind = std::find_if(user_medals.begin(), user_medals.end(), [imMedalList](const QTalk::StUserMedal& info){
            return info.medalId == imMedalList.medalId;
        });
        if(itFind != user_medals.end())
            info.medal_status = itFind->medalStatus;

        sort_medals.insert(info);
    }
    //
    init_medal_wgt(_pMainWgt, sort_medals);
}