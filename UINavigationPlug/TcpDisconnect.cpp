#include "TcpDisconnect.h"
#include <QLabel>
#include <QPixmap>
#include <QHBoxLayout>
#include "NavigationMianPanel.h"


TcpDisconnect::TcpDisconnect(NavigationMianPanel *pMainPanel, QWidget *parent)
        : QFrame(parent), _pMainPanel(pMainPanel), _isRetryConnect(false) {
    setFrameShape(QFrame::NoFrame);
    setObjectName("TcpDisconnectWgt");

    QPixmap pic(":/UINavigationPlug/image1/error.png");
    QLabel *picLabel = new QLabel;
    picLabel->setPixmap(pic);
    _pTextLabel = new QLabel(QStringLiteral("当前网络不可用"));
    auto *layout = new QHBoxLayout(this);
    layout->addSpacerItem(new QSpacerItem(10, 10, QSizePolicy::Expanding));
    layout->addWidget(picLabel);
    layout->addWidget(_pTextLabel);
    layout->addSpacerItem(new QSpacerItem(10, 10, QSizePolicy::Expanding));
    setLayout(layout);
}

TcpDisconnect::~TcpDisconnect()
= default;

/**
  * @函数名   onRetryConnected
  * @功能描述 
  * @参数
  * @author   cc
  * @date     2018/10/24
  */
void TcpDisconnect::onRetryConnected() {
    if (_isRetryConnect) {
        _isRetryConnect = false;
    }
    _pTextLabel->setText(QStringLiteral("当前网络不可用"));
}

void TcpDisconnect::setText(const QString& text)
{
    _pTextLabel->setText(text);
}

/**
  * @函数名   mousePressEvent
  * @功能描述 鼠标点击出发重连
  * @参数
  * @author   cc
  * @date     2018/10/24
  */
void TcpDisconnect::mousePressEvent(QMouseEvent *e) {
    if (_pMainPanel && !_isRetryConnect) {
        _isRetryConnect = true;
        _pTextLabel->setText(QStringLiteral("正在重连"));
//        _pMainPanel->retryToConnect();
    }
    return QFrame::mousePressEvent(e);
}
