//
// Created by QITMAC000260 on 2019-04-12.
//

#include "SelectUserWnd.h"
#include <QPushButton>
#include <QButtonGroup>
#include <thread>
#include <QFile>
#include <QFileInfo>
#include <QEvent>
#include "../include/Line.h"
#include "ChatViewMainPanel.h"
#include "../Platform/Platform.h"
#include "../UICom/qimage/qimage.h"
#include "../Platform/dbPlatForm.h"
#include "../UICom/StyleDefine.h"

extern ChatViewMainPanel *g_pMainPanel;
#define HEAD_WIDTH 22

SelectUserWndSort::SelectUserWndSort(QObject *parent)
    : QSortFilterProxyModel(parent)
{

}

bool SelectUserWndSort::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const {

    QModelIndex index =  sourceModel()->index(source_row, 0, source_parent);
    if(!index.isValid())
        return false;

    bool ret = index.data(EM_DATATYPE_CHECK).toBool();
    if(!ret)
    {
        //
        QString xmppId = index.data(EM_DATATYPE_XMPPID).toString().section("@", 0, 0).toLower();
        QString realJid = index.data(EM_DATATYPE_REALJID).toString().section("@", 0, 0).toLower();
        QString name = index.data(EM_DATATYPE_NAME).toString().toLower();
        QString searchKey = index.data(EM_DATATYPE_SEARCHKEY).toString().section("@", 0, 0).toLower();
        //
        ret = xmppId.contains(filterRegExp()) ||
                   realJid.contains(filterRegExp()) ||
                   searchKey.contains(filterRegExp()) ||
                   name.contains(filterRegExp());
    }

    return ret;
}

SelectUserWndDelegate::SelectUserWndDelegate(QWidget *parent)
    : QStyledItemDelegate(parent)
{
    _nameFont.setPixelSize(13);
    _nameFont.setWeight(400);
}

void
SelectUserWndDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const {
    QStyledItemDelegate::paint(painter, option, index);

    painter->save();
    painter->setRenderHint(QPainter::TextAntialiasing);

    if (option.state & QStyle::State_Selected)
    {
        painter->fillRect(option.rect, QTalk::StyleDefine::instance().getNavSelectColor());
    }
    else
    {
        painter->fillRect(option.rect, QTalk::StyleDefine::instance().getNavNormalColor());
    }

    QRect rect = option.rect;

    QString name = index.data(EM_DATATYPE_NAME).toString();
    QString headPath = index.data(EM_DATATYPE_HEAD).toString();
    QString id = index.data(EM_DATATYPE_XMPPID).toString();
    int chatType = index.data(EM_DATATYPE_CHATTYPE).toInt();
    bool isCheck = index.data(EM_DATATYPE_CHECK).toBool();

    QFontMetricsF nf(_nameFont);
    if(name.isEmpty()) {
        name = id.section("@", 0, 0);
    }

    name = nf.elidedText(name, Qt::ElideRight, 200);

    painter->setPen(QTalk::StyleDefine::instance().getNavNameFontColor());
    painter->setFont(_nameFont);
    painter->drawText(QRect(rect.x() + 43, rect.y(), rect.width() - 43 - 30, rect.height()), Qt::AlignVCenter, name);
    painter->setRenderHints(QPainter::Antialiasing,true);

    QFileInfo headFileInfo(headPath);
    if(!headFileInfo.exists() || headFileInfo.isDir())
    {
        if(chatType == QTalk::Enum::GroupChat)
        {

#ifdef _STARTALK
            headPath = ":/QTalk/image1/StarTalk_defaultGroup.png";
#else
            headPath = ":/QTalk/image1/defaultGroupHead.png";
#endif
        }
        else
        {
#ifdef _STARTALK
            headPath = ":/QTalk/image1/StarTalk_defaultHead.png";
#else
            headPath = ":/QTalk/image1/headPortrait.png";
#endif
        }
    }

    if(!QFile(headPath).isOpen())
    {
        int dpi = QTalk::qimage::instance().dpi();
        QPixmap pixmap = QTalk::qimage::instance().loadPixmap(headPath, true, true, HEAD_WIDTH * dpi);
        QPainterPath path;
        QRect headRect(rect.x() + 13, rect.y() + 8, HEAD_WIDTH, HEAD_WIDTH);
        path.addEllipse(headRect);
        painter->setClipPath(path);
        painter->drawPixmap(headRect, pixmap);
    }

    painter->restore();
    painter->save();

    // check state
    QString checkIcon;
    if(isCheck)
        checkIcon = ":/chatview/image1/toolBar/checkbox_checked.png";
    else
        checkIcon = ":/chatview/image1/toolBar/checkbox_unchecked.png";

    QRect iconRect(rect.right() - 40, rect.top() + 10, 20, 20);
    painter->drawPixmap(iconRect, QPixmap(checkIcon));

    painter->restore();
}

QSize SelectUserWndDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const {
    QSize size = QStyledItemDelegate::sizeHint(option, index);
    return {size.width(), 40};
}

bool SelectUserWndDelegate::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option,
                                        const QModelIndex &index) {

    if(event->type() == QEvent::MouseButtonPress)
    {
        bool isCheck = model->data(index, EM_DATATYPE_CHECK).toBool();
        model->setData(index, !isCheck, EM_DATATYPE_CHECK);
        emit itemCheckChanged(index);
    }
    else if(event->type() == QEvent::MouseButtonDblClick)
    {
        emit itemDbClicked(index);
    }

    return QStyledItemDelegate::editorEvent(event, model, option, index);
}


SelectUserWnd::SelectUserWnd(QWidget* parent)
    : UShadowDialog(parent, true, true)
    , _pSearchEdit(nullptr)
    , _pListView(nullptr)
    , _pModel(nullptr)
    , _pBtnGroup(nullptr)
    , _loop(nullptr)
{
    initUi();
    _loop = new QEventLoop(this);
}

SelectUserWnd::~SelectUserWnd() = default;

void SelectUserWnd::initUi()
{
    //
    setFixedSize(330, 500);
    // title
    QFrame *titleFrm = new QFrame(this);
    titleFrm->setObjectName("titleFrm");
    QLabel *titleLabel = new QLabel(tr("选择会话"), this);
    titleLabel->setObjectName("titleLabel");
    auto titleLay = new QHBoxLayout(titleFrm);
    auto * closeBtn = new QPushButton(this);
    closeBtn->setToolTip(tr("关闭"));
#ifdef _MACOS
    closeBtn->setFixedSize(12, 12);
    closeBtn->setObjectName("gmCloseBtn");
    titleLay->addWidget(closeBtn);
    titleLay->addWidget(titleLabel);
    titleLay->setContentsMargins(8, 0, 20, 0);
#else
    closeBtn->setFixedSize(30, 30);
    closeBtn->setObjectName("gwCloseBtn");
    titleLay->addWidget(titleLabel);
    titleLay->setMargin(0);
    titleLay->addWidget(closeBtn);
#endif
    titleFrm->setFixedHeight(40);
    titleLabel->setAlignment(Qt::AlignCenter);
    // search
    QFrame* searchFrm = new QFrame(this);
    searchFrm->setFixedHeight(50);
    _pSearchEdit = new Search_Edit(this);
    _pSearchEdit->setFixedHeight(30);
    searchFrm->setObjectName("Select_SearchFrm");
    auto searchLay = new QHBoxLayout(searchFrm);
    searchLay->setContentsMargins(20, 10, 20, 10);
    searchLay->addWidget(_pSearchEdit);
    //
    auto* recentBtn = new QPushButton(tr("最近"),this);
    auto* ContactsBtn = new QPushButton(tr("联系人"),this);
    auto* groupChatBtn = new QPushButton(tr("群聊"),this);
    recentBtn->setFixedHeight(32);
    ContactsBtn->setFixedHeight(32);
    groupChatBtn->setFixedHeight(32);
    recentBtn->setObjectName("Select_HeaderBtn");
    ContactsBtn->setObjectName("Select_HeaderBtn");
    groupChatBtn->setObjectName("Select_HeaderBtn");
    recentBtn->setCheckable(true);
    ContactsBtn->setCheckable(true);
    groupChatBtn->setCheckable(true);
    recentBtn->setChecked(true);
    _pBtnGroup = new QButtonGroup(this);
    _pBtnGroup->addButton(recentBtn, 0);
    _pBtnGroup->addButton(ContactsBtn, 1);
    _pBtnGroup->addButton(groupChatBtn, 2);
    QFrame* btnFrm = new QFrame(this);
    btnFrm->setObjectName("btnFrm");
    auto* btnLay = new QHBoxLayout(btnFrm);
    btnLay->addWidget(recentBtn);
    btnLay->addWidget(ContactsBtn);
    btnLay->addWidget(groupChatBtn);
    //
    QFrame* bodyFrm = new QFrame(this);
    bodyFrm->setObjectName("bodyFrm");
    _pListView = new QListView(this);
    _pSortModel =  new SelectUserWndSort(this);
    _pModel = new QStandardItemModel(this);
    _pSortModel->setSourceModel(_pModel);
    _pListView->setModel(_pSortModel);
    _pListView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    auto selectUserWndDelegate = new SelectUserWndDelegate(this);
    _pListView->setItemDelegate(selectUserWndDelegate);
    auto* bodyLay = new QHBoxLayout(bodyFrm);
    bodyLay->setMargin(5);
    bodyLay->addWidget(_pListView);
    //
    QFrame* bottomFrm = new QFrame(this);
    bottomFrm->setFixedHeight(60);
    bottomFrm->setObjectName("bottomFrm");
    QPushButton* cancelBtn = new QPushButton(tr("取消"), this);
    QPushButton* okBtn = new QPushButton(tr("确定"), this);
    cancelBtn->setObjectName("cancelBtn");
    okBtn->setObjectName("sendBtn");
    auto* bottomLay = new QHBoxLayout(bottomFrm);
    bottomLay->addItem(new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Fixed));
    bottomLay->addWidget(cancelBtn);
    bottomLay->addWidget(okBtn);
    //
    auto lay = new QVBoxLayout(_pCenternWgt);
    lay->setSpacing(0);
    lay->addWidget(titleFrm, 0);
    lay->addWidget(searchFrm, 0);
    lay->addWidget(new Line(), 0);
    lay->addWidget(btnFrm, 0);
    lay->addWidget(bodyFrm, 1);
    lay->addWidget(bottomFrm, 0);
    //
    setMoverAble(true);
    //
    connect(closeBtn, &QPushButton::clicked, [this](){this->close();});
    connect(cancelBtn, &QPushButton::clicked, [this](){this->close();});
    connect(okBtn, &QPushButton::clicked, [this](){
        _evtRet = 1;
        this->close();
    });
    connect(_pBtnGroup, SIGNAL(buttonClicked(int)), this, SLOT(onBtnGroupClicked(int)));

    connect(this, &SelectUserWnd::sgUpdateUi, this, &SelectUserWnd::updateUi);
    connect(selectUserWndDelegate, &SelectUserWndDelegate::itemCheckChanged, this, &SelectUserWnd::onItemCheckChanged);
    connect(selectUserWndDelegate, &SelectUserWndDelegate::itemDbClicked, this, &SelectUserWnd::onItemDbClicked);
    connect(_pSearchEdit, &Search_Edit::textChanged, [this](const QString& text){
        _searchQueue->push(text.toStdString());
    });
    //
    std::function<int(STLazyQueue<std::string > *)> func
            = [this](STLazyQueue<std::string > *queue)->int
            {
                int count = 0;
                std::string key = queue->tail();
                while (!queue->empty()) {
                    queue->pop();
                    count++;
                }

                if(g_pMainPanel)
                {
                    _pSortModel->setFilterRegExp(QString::fromStdString(key).toLower());
                }
                return count;
            };
    _searchQueue = new STLazyQueue<std::string>(200, func);
}

void SelectUserWnd::reset()
{
    _pSearchEdit->clear();
    recentSessions.clear();
    _selectedIds.clear();
    _pModel->clear();
    _type = EM_TYPE_RECENT;
    _pBtnGroup->buttonClicked(EM_TYPE_RECENT);
    //
    QPointer<SelectUserWnd> pThis(this);
    std::thread([pThis](){
        if(g_pMainPanel)
        {
            // recent
            g_pMainPanel->searchLocalSession(EM_TYPE_RECENT, "");
            // group
            std::vector<QTalk::Entity::ImGroupInfo> groups;
            dbPlatForm::instance().getAllGroup(groups);

            if(!pThis) return;

            QMutexLocker locker(&pThis->_mutex);
            for(const auto& group : groups)
            {
                QTalk::StShareSession sess;
                sess.xmppId = group.GroupId;
                sess.headUrl = group.HeaderSrc;
                sess.name = group.Name;
                sess.realJid = group.GroupId;
                sess.chatType = QTalk::Enum::GroupChat;
                if(pThis)
                    pThis->groupSessions.push_back(sess);
            }
        }
    }).detach();
}

void SelectUserWnd::onBtnGroupClicked(int tab)
{
    if(_type == tab)
        return;
    _type = tab;
    //
    emit sgUpdateUi();
}

void SelectUserWnd::setSessions(int type, const std::vector<QTalk::StShareSession> &sessions)
{
    QMutexLocker locker(&_mutex);
    switch (type)
    {
        case SelectUserWnd::EM_TYPE_RECENT:
            recentSessions = sessions;
            break;
        case SelectUserWnd::EM_TYPE_CONTACTS:
            contactsSessions = sessions;
            break;
        case SelectUserWnd::EM_TYPE_GROUPCHAT:
            groupSessions = sessions;
            break;
        default:
            break;
    }

    emit sgUpdateUi();
}

void SelectUserWnd::updateUi()
{
    QMutexLocker locker(&_mutex);
    std::vector<QTalk::StShareSession> sessions;
    switch (_type)
    {
        case SelectUserWnd::EM_TYPE_RECENT:
            sessions = recentSessions;
            break;
        case SelectUserWnd::EM_TYPE_CONTACTS:
            sessions = contactsSessions;
            break;
        case SelectUserWnd::EM_TYPE_GROUPCHAT:
            sessions = groupSessions;
            break;
        default:
            break;
    }

    _pModel->clear();
    for(const auto& sess : sessions)
    {
        auto* item = new QStandardItem;
        item->setData(sess.chatType, EM_DATATYPE_CHATTYPE);
        item->setData(sess.xmppId.data(), EM_DATATYPE_XMPPID);
        item->setData(sess.realJid.data(), EM_DATATYPE_REALJID);
        item->setData(sess.name.data(), EM_DATATYPE_NAME);
        item->setData(QTalk::GetHeadPathByUrl(sess.headUrl).data(), EM_DATATYPE_HEAD);
        item->setData(false, EM_DATATYPE_CHECK);
        item->setData(sess.searchKey.data(),EM_DATATYPE_SEARCHKEY);
        _pModel->appendRow(item);
    }
}

/**
 *
 * @param index
 */
void SelectUserWnd::onItemCheckChanged(const QModelIndex& index)
{
    QString xmppId = index.data(EM_DATATYPE_XMPPID).toString();
    QString realJid = index.data(EM_DATATYPE_REALJID).toString();
    int chatType = index.data(EM_DATATYPE_CHATTYPE).toInt();
    bool isChecked = index.data(EM_DATATYPE_CHECK).toBool();

    QTalk::Entity::UID uid(xmppId, realJid);
    if(isChecked)
        _selectedIds[uid] = chatType;
    else
        _selectedIds.erase(uid);
}

void SelectUserWnd::closeEvent(QCloseEvent* e)
{
    _loop->quit();
    UShadowDialog::closeEvent(e);
}

/**
 *
 * @param index
 */
void SelectUserWnd::onItemDbClicked(const QModelIndex &index)
{
    QString xmppId = index.data(EM_DATATYPE_XMPPID).toString();
    QString realJid = index.data(EM_DATATYPE_REALJID).toString();
    int chatType = index.data(EM_DATATYPE_CHATTYPE).toInt();

    QTalk::Entity::UID uid(xmppId, realJid);
    _selectedIds.clear();
    _selectedIds[uid] = chatType;
    //
    _evtRet = 1;
    this->close();
}
