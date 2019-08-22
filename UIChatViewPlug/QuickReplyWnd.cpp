//
// Created by lihaibin on 2019-07-03.
//

#include <QHBoxLayout>
#include "QuickReplyWnd.h"
//#include "../LogicManager/LogicManager.h"
#include "ChatViewMainPanel.h"
#include <QScrollBar>
#include <QToolButton>
#include "ChatViewMainPanel.h"

extern ChatViewMainPanel *g_pMainPanel;
QuickReplyWnd::QuickReplyWnd(QWidget *parent)
        : UShadowDialog(parent, true, true)
{

#ifndef _WINDOWS
    setWindowFlag(Qt::Popup);
#endif // !_WINDOWS

    QFrame* titleFrm = new QFrame(this);
    titleFrm->setObjectName("titleFrm");
    titleFrm->setFixedHeight(50);

    QLabel* titleLabel = new QLabel(tr("快捷回复"), this);
    titleLabel->setAlignment(Qt::AlignCenter);

    auto * titleLay = new QHBoxLayout(titleFrm);
    titleLay->setSpacing(0);

    auto * closeBtn = new QToolButton();

#ifdef _MACOS
    closeBtn->setFixedSize(10, 10);
    titleLay->addWidget(closeBtn);
    closeBtn->setObjectName("gmCloseBtn");
#else
    closeBtn->setFixedSize(20, 20);
    closeBtn->setObjectName("gwCloseBtn");
    titleLay->addWidget(closeBtn);
#endif
    titleLay->addWidget(titleLabel);

    QFrame* middleFrm = new QFrame(this);
    auto* middlelay = new QHBoxLayout(middleFrm);
    _tagListWidget = new QListWidget(this);
    _tagListWidget->setFixedWidth(200);
    _tagListWidget->setFrameShape(QFrame::NoFrame);
    _tagListWidget->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    _tagListWidget->verticalScrollBar()->setSingleStep(15);
    _tagListWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    _tagListWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
//    _tagListWidget->installEventFilter(this);

    QFrame * line = new QFrame();
    line->setFrameShape(QFrame::VLine);
    line->setFrameShadow(QFrame::Sunken);

    _contentListWidget = new QListWidget(this);
    _contentListWidget->setFixedWidth(400);
    _contentListWidget->setFrameShape(QFrame::NoFrame);
    _contentListWidget->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    _contentListWidget->verticalScrollBar()->setSingleStep(15);
    _contentListWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    _contentListWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
//    _contentListWidget->installEventFilter(this);

    middlelay->addWidget(_tagListWidget);
    middlelay->addWidget(line);
    middlelay->addWidget(_contentListWidget);
    middlelay->setSpacing(0);
    middlelay->setMargin(0);

    QFrame* mainFrm = new QFrame(this);
    auto* mainlay = new QVBoxLayout(mainFrm);
    mainlay->setMargin(0);
    mainlay->setSpacing(0);
    mainlay->addWidget(titleFrm);
    mainlay->addWidget(middleFrm);


    auto* lay = new QVBoxLayout(_pCenternWgt);
    lay->setMargin(0);
    lay->addWidget(mainFrm);
    lay->setSpacing(0);

    setMoverAble(true, titleFrm);
    setFixedSize(600, 400);

    addTagItem();

    connect(closeBtn, &QToolButton::clicked, [this](){this->setVisible(false);});

    QObject::connect(_tagListWidget,SIGNAL(itemClicked(QListWidgetItem*)),this,SLOT(singleclicked(QListWidgetItem*)));
    QObject::connect(_contentListWidget,SIGNAL(itemDoubleClicked(QListWidgetItem*)),this,SLOT(doubleclicked(QListWidgetItem*)));
}

QuickReplyWnd::~QuickReplyWnd()
{

}

void QuickReplyWnd::addTagItem() {
    std::vector<QTalk::Entity::ImQRgroup> groups;
    if(g_pMainPanel ){
        g_pMainPanel->getMessageManager()->getQuickGroups(groups);
    }
    int id = -1;
    for(QTalk::Entity::ImQRgroup group :groups){
        QListWidgetItem *item = new QListWidgetItem;
        if(id == -1){
            id = group.sid;
        }
        item->setText(QString::fromStdString(group.groupname));
        item->setWhatsThis(QString::number(group.sid));
        _tagListWidget->addItem(item);
    }
    if(id != -1)
        addContentItem(id);
}

void QuickReplyWnd::addContentItem(int id) {
    _contentListWidget->clear();
    std::vector<QTalk::Entity::IMQRContent> contents;
    if(g_pMainPanel ){
        g_pMainPanel->getMessageManager()->getQuickContentByGroup(contents,id);
    }
    for(QTalk::Entity::IMQRContent content :contents){
        QListWidgetItem *item = new QListWidgetItem;
        item->setText(QString::fromStdString(content.content));
        _contentListWidget->addItem(item);
    }
}

void QuickReplyWnd::singleclicked(QListWidgetItem *item) {
    int sid = item->whatsThis().toInt();
    addContentItem(sid);
}

void QuickReplyWnd::doubleclicked(QListWidgetItem *item) {
    QString text = item->text();
    emit sendQuickReply(text.toStdString());
}