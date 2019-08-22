#include "NavigationItem.h"
#include <QHBoxLayout>

NavigationItem::NavigationItem(const QUInt8& itemType, QWidget *parent)
        : QFrame(parent)
        , _itemType(itemType)
        , _selected(false)
        , _itemWgt(nullptr)
{
    Q_INIT_RESOURCE(ushadowdialog);

    setObjectName("NavigationItem");
    _pIconLabel = new HeadPhotoLab;
    _pTextLabel = new QLabel(this);
    //
    _pTextLabel->setObjectName("NavigationItemLabel");
    //
    switch (itemType)
    {
        case EM_ITEM_TYPE_START:
        {
            _pIconLabel->setHead(":/Resource/starContact.png", 15, false, false);
            _pTextLabel->setText(QStringLiteral("星标联系人"));
            break;
        }
        case EM_ITEM_TYPE_FRIENDLIST:
        {
            _pIconLabel->setHead(":/Resource/friendList.png", 15, false, false);
            _pTextLabel->setText(QStringLiteral("好友列表"));
            break;
        }
        case EM_ITEM_TYPE_GROUPLIST:
        {
            _pIconLabel->setHead(":/Resource/groupList.png", 15, false, false);
            _pTextLabel->setText(QStringLiteral("群组列表"));
            break;
        }
        case EM_ITEM_TYPE_STAFF:
        {
#if defined(_STARTALK)
            _pIconLabel->setHead(":/Resource/starTalk_staff.png", 15, false, false);
#else
            _pIconLabel->setHead(":/Resource/staff.png", 15, false, false);
#endif
            _pTextLabel->setText(QStringLiteral("Staff"));
            break;
        }
        case EM_ITEM_TYPE_SUBSCRIPTION:
        {
            _pIconLabel->setHead(":/Resource/subscription.png", 15, false, false);
            _pTextLabel->setText(QStringLiteral("公众号"));
            break;
        }
        case EM_ITEM_TYPE_BLACKLIST:
        {
            _pIconLabel->setHead(":/Resource/BlackList.png", 15, false, false);
            _pTextLabel->setText(QStringLiteral("黑名单"));
            break;
        }
        default:
            break;
    }

    //
    _pIconLabel->setFixedWidth(30);
    //
    auto * layout = new QHBoxLayout(this);
    layout->setSpacing(9);
    layout->setContentsMargins(20, 0, 0, 0);
    layout->addWidget(_pIconLabel);
    layout->addWidget(_pTextLabel);

    this->setFixedHeight(50);
}

NavigationItem::~NavigationItem()
= default;

/**
  * @函数名   
  * @功能描述 
  * @参数
  * @author   cc
  * @date     2018/12/15
  */
void NavigationItem::registerWgt(QWidget * wgt)
{
    _itemWgt = wgt;
}

/**
  * @函数名   
  * @功能描述 
  * @参数
  * @author   cc
  * @date     2018/12/15
  */
void NavigationItem::setSelectState(bool selected)
{
    _selected = selected;
    if(_itemWgt)
    {
        _itemWgt->setVisible(selected);
    }
}

void NavigationItem::mousePressEvent(QMouseEvent *e)
{
    emit itemClicked(_itemType);
    QFrame::mousePressEvent(e);
}

QWidget *NavigationItem::getItemWgt() {
    return _itemWgt;
}