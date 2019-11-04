//
// Created by cc on 19-1-6.
//

#ifndef QTALK_V2_GROUPMEMBERPOPWND_HPP
#define QTALK_V2_GROUPMEMBERPOPWND_HPP
#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif
#include <QFrame>
#include <QTableView>
#include <QStandardItemModel>
#include <QLabel>
#include <QPushButton>
#include <QSortFilterProxyModel>
#include <QStyledItemDelegate>
#include <QPainter>
#include <QFile>
#include <QFileInfo>
#include <QHeaderView>
#include "SearchEdit.hpp"
#include "../include/Line.h"
#include "customui_global.h"
#include "UShadowWnd.h"
#include "../QtUtil/Utils/Log.h"
#include "../UICom/qimage/qimage.h"
#include "../UICom/StyleDefine.h"

enum {
    EM_DATA_XMPPID = Qt::UserRole + 1,
    EM_DATA_USERNAME,
    EM_DATA_USERHEAD,
    EM_DATA_USERROLE,
    EM_DATA_ISONLINE,
    EM_DATA_SEARCHKEY
};

enum
{
    EM_COLUMN_NAME,
    EM_COLUMN_ROLE
};

class GroupItemPopSortModel : public QSortFilterProxyModel
{
public:
    explicit GroupItemPopSortModel(QObject *parent = Q_NULLPTR)
        : QSortFilterProxyModel(parent)
    {

    }

protected:
    bool lessThan(const QModelIndex &source_left, const QModelIndex &source_right) const override {
        if (!source_left.isValid() || !source_right.isValid())
            return false;

        int leftRole = source_left.data(EM_DATA_USERROLE).toInt();
        int rightRole = source_right.data(EM_DATA_USERROLE).toInt();

        return leftRole < rightRole;
    }
    //
    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override {
        QModelIndex index = sourceModel()->index(source_row, EM_COLUMN_NAME, source_parent);

        QString userId = index.data(EM_DATA_XMPPID).toString();
        userId = userId.section("@", 0, 0);
        bool ret = index.data(EM_DATA_USERNAME).toString().contains(filterRegExp()) ||
                    index.data(EM_DATA_SEARCHKEY).toString().contains(filterRegExp()) ||
                    userId.contains(filterRegExp());
        return ret;
    }
};

class GroupItemPopDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit GroupItemPopDelegate(QObject *parent = Q_NULLPTR)
        : QStyledItemDelegate(parent)
    {
    }

protected:
    void paint(QPainter *painter,
               const QStyleOptionViewItem &option, const QModelIndex &index) const Q_DECL_OVERRIDE
    {
        QStyledItemDelegate::paint(painter, option, index);
        painter->save();
        painter->setRenderHint(QPainter::TextAntialiasing);
        painter->fillRect(option.rect, QTalk::StyleDefine::instance().getGroupCardGroupMemberNormalColor());

        QRect rect = option.rect;
        if (index.column() == EM_COLUMN_NAME) {
            QString strText = index.data(EM_DATA_USERNAME).toString();
            painter->setPen(QTalk::StyleDefine::instance().getNavNameFontColor());
            painter->drawText(QRect(rect.x() + 30, rect.y(), rect.width() - 30, rect.height()), Qt::AlignVCenter,
                              strText);
            QString headPath = index.data(EM_DATA_USERHEAD).toString();
            bool isOnline = index.data(EM_DATA_ISONLINE).toBool();
            painter->setRenderHints(QPainter::Antialiasing, true);

            QFileInfo headInfo(headPath);
            if(!headInfo.exists() || headInfo.isDir())
            {
#ifdef _STARTALK
                headPath = ":/QTalk/image1/StarTalk_defaultHead.png";
#else
                headPath = ":/QTalk/image1/headPortrait.png";
#endif
            }

            if (!QFile(headPath).isOpen()) {
                int dpi = QTalk::qimage::instance().dpi();
                QPixmap pixmap = QTalk::qimage::instance().loadPixmap(headPath, true, true, 22 * dpi);
                QPainterPath path;
                QRect headRect(rect.x(), rect.y() + 8, 22, 22);
                path.addEllipse(headRect);
                painter->setClipPath(path);
                painter->drawPixmap(headRect, pixmap);
                painter->fillPath(path, QTalk::StyleDefine::instance().getHeadPhotoMaskColor());
            }
        } else if (index.column() == EM_COLUMN_ROLE) {
            int role = index.data(EM_DATA_USERROLE).toString().toInt();
            QString strRole = tr("");
            switch (role) {
                case 1:
                    strRole = tr("群主");
                    break;
                case 2:
                    strRole = tr("管理员");
                    break;
                case 0:
                default:
                    break;
            }
            painter->setPen(QTalk::StyleDefine::instance().getNavNameFontColor());
            painter->drawText(rect, Qt::AlignVCenter, strRole);
        }
        //

        painter->restore();
    }

    QSize sizeHint(const QStyleOptionViewItem &option,
                   const QModelIndex &index) const Q_DECL_OVERRIDE
    {
        return {option.rect.width(), 30};
    }
};

class CUSTOMUISHARED_EXPORT GroupMemberPopWnd : public UShadowDialog
{
    Q_OBJECT
public:
    explicit GroupMemberPopWnd(QWidget* parent = nullptr)
        : UShadowDialog(parent, true)
    {
        setFixedSize(330, 500);
        //
        _titleLabel = new QLabel(tr("群成员"));
        _titleLabel->setObjectName("title");
        _titleLabel->setAlignment(Qt::AlignCenter);
        _pCloseBtn = new QPushButton(this);


        QFrame* topFrm = new QFrame(this);
        topFrm->setObjectName("TopFrame");
        auto * topLay = new QHBoxLayout(topFrm);
#ifdef _MACOS
        _pCloseBtn->setFixedSize(12, 12);
        _pCloseBtn->setObjectName("gmCloseBtn");
        topLay->addWidget(_pCloseBtn);
        topLay->addWidget(_titleLabel);
#else
        _pCloseBtn->setFixedSize(30, 30);
        _pCloseBtn->setObjectName("gwCloseBtn");
        topLay->addWidget(_titleLabel);
        topLay->addWidget(_pCloseBtn);
#endif
        //
        QFrame* searchFrm = new QFrame(this);
        searchFrm->setObjectName("BodyFrm");
        auto * mainLay = new QVBoxLayout(searchFrm);
        mainLay->setContentsMargins(20, 10, 20, 10);
        mainLay->setSpacing(10);
        _pSearchEdit = new Search_Edit(this);
        //
        _pMemberView = new QTableView(this);
        _pMemberView->setFrameShape(QFrame::NoFrame);
        _pMemberView->setEditTriggers(QAbstractItemView::NoEditTriggers);
        _pMemberView->verticalHeader()->setVisible(false);
        _pMemberView->setShowGrid(false);
        _pMemberView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        _pMemberView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        _pMemberView->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
        _pMemberView->horizontalHeader()->setSelectionMode(QHeaderView::NoSelection);
        _pMemberView->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
        _pSrcModel = new QStandardItemModel;
        _pModel = new GroupItemPopSortModel;
        _pItemDelegate = new GroupItemPopDelegate;
        _pModel->setSourceModel(_pSrcModel);
        _pMemberView->setModel(_pModel);
        _pMemberView->setItemDelegate(_pItemDelegate);
        _pSrcModel->setColumnCount(2);
        //
        mainLay->addWidget(_pSearchEdit);
        mainLay->addWidget(new Line);
        mainLay->addWidget(_pMemberView);
        //
        auto * Lay = new QVBoxLayout(_pCenternWgt);
        Lay->setMargin(0);
        Lay->setSpacing(0);
        Lay->addWidget(topFrm);
        Lay->addWidget(searchFrm);
        this->setMoverAble(true, topFrm);

        connect(_pCloseBtn, &QPushButton::clicked, this, &GroupMemberPopWnd::setVisible);
        connect(_pSearchEdit, &Search_Edit::textChanged, this, &GroupMemberPopWnd::onSearch);
    }
    ~GroupMemberPopWnd() override
    = default;

public:
    void reset()
    {
        _pSrcModel->clear();
        arMembers.clear();
    }

    void addItem(const QString& xmppId, const QString& name, const QString& headSrc, const int& userRole, const bool& isOnline, const QString& searchKey)
    {
        if(arMembers.contains(xmppId))
            return;
        else
            arMembers.push_back(xmppId);

        QStandardItem* item = new QStandardItem;
        item->setData(xmppId, EM_DATA_XMPPID);
        item->setData(name, EM_DATA_USERNAME);
        item->setData(headSrc, EM_DATA_USERHEAD);
        item->setData(isOnline, EM_DATA_ISONLINE);
        item->setData(searchKey, EM_DATA_SEARCHKEY);

        QStandardItem* roleItem = new QStandardItem;
        roleItem->setData(userRole, EM_DATA_USERROLE);

        _pSrcModel->appendRow(QList<QStandardItem*>() << item << roleItem);
   }
    //
    void addEnd()
    {
        _pMemberView->setColumnWidth(0, 200);
        _pMemberView->setColumnWidth(1, 60);
        _pModel->sort(1);
        _pSrcModel->setHeaderData(0, Qt::Horizontal, tr("成员名"));
        _pSrcModel->setHeaderData(1, Qt::Horizontal, tr("群职"));
    }

protected:
    void onSearch(const QString& text)
    {
        _pModel->setFilterFixedString(text);
    }

private:
    QLabel* _titleLabel;
    QPushButton* _pCloseBtn;

    Search_Edit* _pSearchEdit;
    QTableView*  _pMemberView;
    QStandardItemModel* _pSrcModel;
    GroupItemPopSortModel* _pModel;
    GroupItemPopDelegate* _pItemDelegate;

    QList<QString> arMembers;
};


#endif //QTALK_V2_GROUPMEMBERPOPWND_HPP
