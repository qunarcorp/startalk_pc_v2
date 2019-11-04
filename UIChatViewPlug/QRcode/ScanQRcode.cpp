//
// Created by QITMAC000260 on 2019-04-16.
//

#include "ScanQRcode.h"
#include <QVBoxLayout>
#include <QTimer>
#include <QDateTime>
#include <QDesktopWidget>
#include <QScreen>
#include <QApplication>
#include <QEvent>
#include <QDesktopServices>
#include "QRcode.h"
#include "ScanMainFrm.h"
#include "TipButton.h"
#include "../ChatViewMainPanel.h"
#include "../../WebService/WebService.h"
#include "../../include/Line.h"
#include "../../UICom/uicom.h"
#include "../../CustomUi/QtMessageBox.h"
#include "../../Platform/AppSetting.h"

extern ChatViewMainPanel *g_pMainPanel;
ScanQRcode::ScanQRcode(QRcode* parent )
    :QFrame(parent), _pQRcode(parent)
{
    //
    _pScanFrm = new ScanMainFrm(this);
    _pScanFrm->setContentsMargins(60, 50, 60, 50);

    auto* mainLay = new QVBoxLayout(this);
    mainLay->setMargin(0);
    mainLay->addWidget(new Line(Qt::Horizontal, this), 1);
    mainLay->addWidget(_pScanFrm, 380);

    _pTimer = new QTimer(this);
    _pTimer->setInterval(3000);
    connect(_pTimer, &QTimer::timeout, this, &ScanQRcode::onScan);
    connect(this, &ScanQRcode::sgScanSuccess, this, &ScanQRcode::onScanSuccess, Qt::QueuedConnection);
}

ScanQRcode::~ScanQRcode() {

}

/**
 * 扫描处理
 */
void ScanQRcode::onScan()
{
    QPoint pos = mapToGlobal(_pScanFrm->geometry().topLeft());
#ifdef _LINUX
    QScreen *screen = nullptr;
    auto lstScreens = QApplication::screens();
    for(QScreen* tmps : lstScreens)
    {
        if(tmps->geometry().contains(pos))
        {
            screen = tmps;
            break;
        }
    }

#else
    QScreen *screen = QApplication::screenAt(pos);
#endif
    auto* desktop = QApplication::desktop();
    WId id = desktop->winId();
    QPixmap pix = screen->grabWindow(id,
            pos.x() + 50, pos.y() + 50,
            _pScanFrm->width() - 100, _pScanFrm->height() - 100);
    scanPixmap(pix);
}

/**
 * event时间 -> timer 处理
 * @param e
 * @return
 */
bool ScanQRcode::event(QEvent* e)
{
    if(e->type() == QEvent::Show)
    {
        if(!_pTimer->isActive())
            _pTimer->start();

        _pScanFrm->start();
    }
    else if(e->type() == QEvent::Hide)
    {
        if(_pTimer->isActive())
            _pTimer->stop();

        _pScanFrm->stop();
    }

    return QFrame::event(e);
}

void ScanQRcode::onScanSuccess(const QString& ret)
{
    if(_pTimer && _pTimer->isActive())
        _pTimer->stop();
    QUrl url(ret);
    if(ret.startsWith("qtalk://group?id="))
    {
        QString groupId = ret.section("id=", 1, 1);
        std::string selfId = g_pMainPanel->getSelfUserId();
        //
        QtMessageBox::information(g_pMainPanel, tr("提醒"), QString(tr("即将加入群聊%1")).arg(groupId.section("@", 0, 0)));
        //
        std::vector<std::string> members;
        members.push_back(selfId);
        g_pMainPanel->getMessageManager()->addGroupMember(members, groupId.toStdString());
    }
    else
    {
        if(ret.startsWith("http") ||
           ret.startsWith("www"))
        {
            int ret_btn =QtMessageBox::question(g_pMainPanel, tr("提醒"), QString(tr("是否使用浏览器打开 %1 ?")).arg(ret));
            if(QtMessageBox::EM_BUTTON_YES == ret_btn)
            {
                if(AppSetting::instance().getOpenOaLinkWithAppBrowser())
                    WebService::loadUrl(url);
                else
                    QDesktopServices::openUrl(url);
            }
        }
        else
        {
            QtMessageBox::information(g_pMainPanel, tr("扫一扫结果"), ret);
        }
    }

    _pQRcode->setVisible(false);
}

void ScanQRcode::scanPixmap(const QPixmap& pix, bool flag)
{
    QPointer<ScanQRcode> pThis(this);
    std::thread([pThis, pix, flag](){
        QImage img = pix.toImage();
        int width = 0, height = 0;
        if(!pThis) return;
        if(flag)
        {
            width = pix.width();
            height = pix.height();
        }
        else
        {

            width = pThis->_pScanFrm->width();
            height = pThis->_pScanFrm->height();
        }
        QString ret = pThis->_qzxing.decodeImage(img, width, height, true);
        if(!pThis) return;
        if(!ret.isEmpty())
        {
            emit pThis->sgScanSuccess(ret);
        }
        else if(flag)
        {
            emit g_pMainPanel->sgShowInfoMessageBox(tr("无法识别二维码"));
        }
    }).detach();
}