#include <utility>

#include <utility>

//
// Created by cc on 18-12-17.
//

#include "OaMainWgt.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPixmap>
#include <QDesktopServices>
#include <QUrl>
#include <QHeaderView>
#include "../Platform/Platform.h"
#include "OADefine.h"
#include "../WebService/WebService.h"
#include "../UICom/qimage/qimage.h"
#include "../UICom/StyleDefine.h"

using namespace QTalk;
CellWgtItem::CellWgtItem(QString name, QString icon, QString  url, QWidget *parent)
    :QFrame(parent), _name(std::move(name)), _icon(std::move(icon)), _url(std::move(url))
{
    setMinimumSize(100, 100);
    setFrameShape(QFrame::NoFrame);
}

CellWgtItem::~CellWgtItem()
{

}

/**
 *
 * @param e
 */
void CellWgtItem::mousePressEvent(QMouseEvent* e)
{
    QUrl url(_url);
    if(AppSetting::instance().getOpenOaLinkWithAppBrowser())
    {
        MapCookie cookie;
        cookie["q_u"] = Platform::instance().getSelfUserId().data();
        WebService::loadUrl(url, true, cookie);
    }
    else
    {
        QDesktopServices::openUrl(url);
    }
    QFrame::mousePressEvent(e);
}

/**
 *
 * @param e
 */
void CellWgtItem::paintEvent(QPaintEvent* e)
{
    QPainter painter(this);
    std::string localPath = QTalk::getOAIconPath(_icon.toStdString());
    QPixmap pixmap = QTalk::qimage::instance().loadPixmap(QString::fromLocal8Bit(localPath.data()), false, true, 40, 40);

    painter.setRenderHints(QPainter::Antialiasing, true);
    painter.setRenderHints(QPainter::SmoothPixmapTransform, true);
    painter.drawPixmap( (width() - 40) / 2, (height() - 70) / 2, 40, 40, pixmap);
    painter.setPen(QPen(StyleDefine::instance().getAdrNameFontColor()));
    painter.drawText(0, 55, width(), (height() - 50) / 2, Qt::AlignCenter, _name);
    return QFrame::paintEvent(e);
}

/**
 *
 */
using namespace OA;
OaMainWgt::OaMainWgt(const int& id, QString name, const std::vector<StMember>& members, QWidget* parent)
    : QFrame(parent), _id(id), _members(members), _name(std::move(name))
{
    initUi();
}

OaMainWgt::~OaMainWgt()
{

}

/**
 *
 */
void OaMainWgt::initUi()
{
    setObjectName("OaMainWgt");
    //
    QFrame* topFrm = new QFrame(this);
    topFrm->setObjectName("OaMainWgt_topFrm");
    auto* topLay = new QHBoxLayout(topFrm);
    topLay->setMargin(0);
    topLay->setSpacing(0);
    QLabel* titleLabel = new QLabel(_name, this);
    titleLabel->setContentsMargins(10, 0, 0, 0);
    titleLabel->setObjectName("OaMainWgt_titleLab");
    titleLabel->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);
    topLay->addWidget(titleLabel);
    titleLabel->setFixedHeight(50);
    //
    QFrame* mainFrm = new QFrame(this);
    _pTabWgt = new QTableWidget();
    _pTabWgt->setFrameShape(QFrame::NoFrame);
    _pTabWgt->horizontalHeader()->setVisible(false);
    _pTabWgt->verticalHeader()->setVisible(false);
    _pTabWgt->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    _pTabWgt->setShowGrid(false);
    _pTabWgt->setFocusPolicy(Qt::NoFocus);
    auto* mainLay = new QHBoxLayout(mainFrm);
    mainLay->setMargin(0);
    mainLay->setSpacing(0);
    mainLay->addWidget(_pTabWgt);
    //
    auto* layout = new QVBoxLayout(this);
    layout->setMargin(0);
    layout->setSpacing(0);
    layout->addWidget(topFrm);
    layout->addWidget(mainFrm);
    //
    _pTabWgt->setFrameShape(QFrame::NoFrame);
    //
    _pTabWgt->setColumnCount(6);
    //
    int tmpRow = 0, tmpColumn = 0;
    auto it = _members.begin();
    for(; it != _members.end(); it++)
    {
        if (_pTabWgt->rowCount() != tmpRow + 1)
        {
            _pTabWgt->insertRow(tmpRow);
            _pTabWgt->setRowHeight(tmpRow, 100);
        }
        CellWgtItem* wgt = new CellWgtItem(QString::fromStdString(it->memberName),
                QString::fromStdString(it->memberIcon), QString::fromStdString(it->memberAction), this);
        _pTabWgt->setCellWidget(tmpRow, tmpColumn, wgt);

        if (++tmpColumn == 6)
        {
            tmpColumn = 0;
            tmpRow++;
        }
    }
}
