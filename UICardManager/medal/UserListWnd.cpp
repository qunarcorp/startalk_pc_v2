//
// Created by QITMAC000260 on 2019/10/23.
//

#include "UserListWnd.h"
#include "../../Platform/Platform.h"
#include "../../UICom/qimage/qimage.h"
#include <QVBoxLayout>
#include <QPainter>
#include <QToolButton>
#include <QScrollBar>
#include <QFileInfo>

UserItemDelegate::UserItemDelegate(QWidget* parent)
    :QStyledItemDelegate(parent)
{

}

QSize UserItemDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const {
    const QSize& size = QStyledItemDelegate::sizeHint(option, index);
    return {size.width(), 50};
}

void UserItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const {
    if(!index.isValid())
        return;
    painter->save();
    painter->setRenderHint(QPainter::TextAntialiasing);       //
    // 背景色
    QRect rect = option.rect;
    painter->fillRect(rect, QColor(255, 255, 255));

    QString strText = index.data(em_user_name).toString();
    painter->setPen(QColor(51,51,51));
    painter->drawText(QRect(rect.x() + 65, rect.y(), rect.width() - 40, rect.height()), Qt::AlignVCenter, strText);

    painter->restore();
    painter->save();

    QRect headRect(rect.x() + 20, rect.y() + 5, 35, 35);
    QString headPath = index.data(em_user_head).toString();
    painter->setRenderHints(QPainter::Antialiasing, true);
    painter->setRenderHints(QPainter::SmoothPixmapTransform, true);
    painter->setPen(Qt::NoPen);
    painter->setBrush(QColor(244,244,244));
    painter->drawEllipse(headRect);
    if(headPath.isEmpty())
    {
#ifdef _STARTALK
        headPath = ":/QTalk/image1/StarTalk_defaultHead.png";
#else
        headPath = ":/QTalk/image1/headPortrait.png";
#endif
    }
    QPainterPath path;
    path.addEllipse(headRect);
    painter->setClipPath(path);
    auto image = QTalk::qimage::instance().loadPixmap(headPath, false);
    painter->drawPixmap(headRect, image);

    painter->restore();
}


UserListWnd::UserListWnd(QWidget* parent)
    :QFrame(parent)
{
    setFixedWidth(385);
    _pTitleLabel = new QLabel(tr("已点亮的小驼"), this);
    _pTitleLabel->setObjectName("MedalDetailWnd_TitleLabel");
    _pTitleLabel->setAlignment(Qt::AlignCenter);
    auto* closeBtn = new QToolButton(this);

    _topFrm = new QFrame(this);
    _topFrm->setObjectName("MedalDetailWnd_TitleFrm");
    auto topLay = new QHBoxLayout(_topFrm);
#ifdef _MACOS
    closeBtn->setFixedSize(12, 12);
    closeBtn->setObjectName("gmCloseBtn");
    topLay->addWidget(closeBtn);
    topLay->addWidget(_pTitleLabel);
#else
	topLay->setContentsMargins(30, 0, 0, 0);
    topLay->addWidget(_pTitleLabel);
    topLay->addWidget(closeBtn);
    closeBtn->setFixedSize(30, 30);
    closeBtn->setObjectName("gwCloseBtn");
#endif
    auto* backBtn = new QToolButton(this);
    backBtn->setFixedSize(20, 20);
    backBtn->setObjectName("BackBtn");
    auto* backBtnLay = new QHBoxLayout();
    backBtnLay->setContentsMargins(10, 0, 10, 0);
    backBtnLay->addWidget(backBtn);
    backBtnLay->setAlignment(backBtn, Qt::AlignLeft);

    auto* bodyFrm = new QFrame(this);
    auto *bodyLay = new QVBoxLayout(bodyFrm);
    bodyLay->setContentsMargins(0, 0, 0, 6);
    bodyLay->setSpacing(0);
    _userView = new QListView(this);
    _userView->setFrameShape(QFrame::NoFrame);
    _userView->verticalScrollBar()->setVisible(false);
    _userView->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    _userView->verticalScrollBar()->setSingleStep(10);
    _model = new QStandardItemModel(this);
    _userView->setModel(_model);
    _userView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    _userView->setItemDelegate(new UserItemDelegate(this));
    bodyLay->addWidget(_userView);
    //
    auto* mainFrm = new QFrame(this);
    mainFrm->setObjectName("MedalDetailWnd_MainFrm");
    auto mainLay = new QVBoxLayout(mainFrm);
    mainLay->setMargin(0);
    mainLay->setSpacing(10);
    mainLay->addWidget(_topFrm);
    mainLay->addLayout(backBtnLay);
    mainLay->addWidget(bodyFrm);
    mainLay->setStretch(0, 0);
    mainLay->setStretch(1, 0);
    mainLay->setStretch(2, 1);

    auto* lay = new QHBoxLayout(this);
    lay->setMargin(0);
    lay->addWidget(mainFrm);

    connect(closeBtn, &QToolButton::clicked, [parent](){parent->setVisible(false);});
    connect(backBtn, &QToolButton::clicked, this, &UserListWnd::sgShowBack);
}

void UserListWnd::showUserList(const std::vector<QTalk::StMedalUser> &userList) {
    //
    _model->clear();
    //
    for(const auto& user : userList)
    {
        QString tmp = QTalk::GetHeadPathByUrl(user.userHead).data();
        QFileInfo info(tmp);

        auto* item = new QStandardItem;
        item->setData(user.xmppId.data(), em_user_xmppId);
        item->setData(user.userName.data(), em_user_name);
        if(info.exists() && info.isFile())
            item->setData(tmp, em_user_head);

        _model->appendRow(item);
    }
}
