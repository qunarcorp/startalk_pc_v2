//
// Created by cc on 18-11-9.
//

#include "TipMessageItem.h"
#include <QLabel>
#include <QHBoxLayout>
#include "../../WebService/WebService.h"
#include "../../Platform/Platform.h"

TipMessageItem::TipMessageItem(QWidget *parent)
    //:MessageItemBase(msgInfo, parent)
{
    _pLabel = new QLabel;
    _pLabel->setObjectName("TipLabel");

    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->setMargin(0);
    layout->addSpacerItem(new QSpacerItem(10, 10, QSizePolicy::Expanding));
    layout->addWidget(_pLabel);
    layout->addSpacerItem(new QSpacerItem(10, 10, QSizePolicy::Expanding));

    //绑定超链接与信号槽
    connect(_pLabel, SIGNAL(linkActivated(QString)), this, SLOT(openURL(QString)));
}

TipMessageItem::~TipMessageItem()
{

}

void TipMessageItem::openURL(QString url) {
    MapCookie cookies;
    cookies["ckey"] = QString::fromStdString(Platform::instance().getClientAuthKey());
#ifdef _QCHAT
    std::string qvt = Platform::instance().getQvt();
    if(!qvt.empty()){
        cJSON *qvtJson = cJSON_GetObjectItem(cJSON_Parse(qvt.data()),"data");
        std::string qcookie = cJSON_GetObjectItem(qvtJson,"qcookie")->valuestring;
        std::string vcookie = cJSON_GetObjectItem(qvtJson,"vcookie")->valuestring;
        std::string tcookie = cJSON_GetObjectItem(qvtJson,"tcookie")->valuestring;
        cJSON_Delete(qvtJson);
        cookies["_q"] = QString::fromStdString(qcookie);
        cookies["_v"] = QString::fromStdString(vcookie);
        cookies["_t"] = QString::fromStdString(tcookie);
    }
    WebService::loadUrl(QUrl(url), false, cookies);
#else
    WebService::loadUrl(QUrl(url), false, cookies);
#endif

}

void TipMessageItem::setText(const QString &text)
{
    _strText = text;
    _pLabel->setText(text);
}

QString TipMessageItem::getText()
{
    return _strText;
}

void TipMessageItem::resizeEvent(QResizeEvent *e)
{
    _pLabel->hide();
    _pLabel->show();
    QWidget::resizeEvent(e);
}
