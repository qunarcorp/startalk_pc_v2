//
// Created by QITMAC000260 on 2019/08/29.
//

#include "HotLineTipItem.h"
#include <QHBoxLayout>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include "../CustomUi/LinkButton.h"
#include "../ChatViewMainPanel.h"

extern ChatViewMainPanel *g_pMainPanel;
HotLineTipItem::HotLineTipItem(const QTalk::Entity::ImMessageInfo& info, QWidget* parent)
    :QFrame(parent)
{
    auto* mainFrm = new QFrame(this);
    mainFrm->setObjectName("messReceiveContentFrm");
    auto* mainLay = new QHBoxLayout(mainFrm);
    mainLay->setMargin(3);
    mainLay->setSpacing(1);
    //
    QJsonDocument jsonDocument = QJsonDocument::fromJson(info.ExtendedInfo.data());
    if (!jsonDocument.isNull()) {
        QJsonObject jsonObject = jsonDocument.object();
        QJsonArray hints = jsonObject.value("hints").toArray();
        auto it = hints.begin();
        while (it != hints.end())
        {
            auto hint = it->toObject();
            auto event = hint.value("event").toObject();
            //
            QString text = hint.value("text").toString();
            QString type = event.value("type").toString();
            //
            if( "postInterface" == type) {

                QString url = event.value("url").toString();
                auto params = event.value("params").toObject();
                QJsonDocument paramDoc;
                paramDoc.setObject(params);
                QByteArray strParams = paramDoc.toJson(QJsonDocument::Compact);

                auto* link = new LinkButton(text, this);
                mainLay->addWidget(link);

                connect(link, &LinkButton::clicked, [url, strParams](){
                    if(g_pMainPanel)
                        g_pMainPanel->postInterface(url.toStdString(), strParams.data());
                });

            }  else if("text" == type) {
                auto* label = new QLabel(text, this);
                mainLay->addWidget(label);
            }

            it++;
        }
    }
    else
    {
        auto* label = new QLabel(_msgInfo.Content.data(), this);
        mainLay->addWidget(label);
    }
    //
    auto* lay = new QHBoxLayout(this);
    lay->setMargin(2);
    lay->addItem(new QSpacerItem(10, 10, QSizePolicy::Expanding));
    lay->addWidget(mainFrm);
    lay->addItem(new QSpacerItem(10, 10, QSizePolicy::Expanding));

    mainFrm->setFixedHeight(30);
}

HotLineTipItem::~HotLineTipItem() = default;