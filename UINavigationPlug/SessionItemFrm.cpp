#include "SessionItemFrm.h"
#include "../CustomUi/HeadPhotoLab.h"
#include <QHBoxLayout>
#include <QLabel>

SessionItemFrm::SessionItemFrm(QWidget *parent) :
    QFrame(parent),
    _headPhotoLab(nullptr),
    _sessionNameLab(nullptr),
    _timeLab(nullptr),
    _messageLab(nullptr),
    _isOnline(true)
{
    init();
}

SessionItemFrm::~SessionItemFrm()
{

}

/**
  * @函数名
  * @功能描述 设置是否在线
  * @参数
  * @date 2018.9.30
  */
void SessionItemFrm::setOnline(const bool &isOnline)
{
    _isOnline = isOnline;
    //    _headPhotoLab->showGreyPixmap(isOnline);
}

/**
  * @函数名
  * @功能描述
  * @参数
  * @date 2018.10.17
  */
void SessionItemFrm::setName(const QString &name)
{
    _name = name;
    QString showName = geteElidedText(_sessionNameLab->font(), _name, _sessionNameLab->width());
    if (_sessionNameLab)
    {
        _sessionNameLab->setText(showName);
    }
}

/**
  * @功能描述
  * @参数
  * @date 2018.9.18
  */
void SessionItemFrm::init()
{
    initLayout();
}

/**
  * @功能描述
  * @参数
  * @date 2018.9.18
  */
void SessionItemFrm::initLayout()
{
    QHBoxLayout * mainlayout = new QHBoxLayout(this);
    mainlayout->setContentsMargins(20, 12, 20, 11);
    mainlayout->setSpacing(10);

    _headPhotoLab = new QLabel;
    _headPhotoLab->setObjectName("headPhotoLab");
    _headPhotoLab->setFixedSize(42, 42);
    mainlayout->addWidget(_headPhotoLab);
    QVBoxLayout * vbox = new QVBoxLayout;
    vbox->setMargin(0);
    vbox->setSpacing(5);
    mainlayout->addLayout(vbox);

    QHBoxLayout * hbox = new QHBoxLayout;
    hbox->setMargin(0);
    hbox->setSpacing(5);
    vbox->addLayout(hbox);
    _sessionNameLab = new QLabel;
    _sessionNameLab->setObjectName("sessionNameLab");
    _sessionNameLab->setFixedHeight(25);
    _sessionNameLab->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    hbox->addWidget(_sessionNameLab);
    _timeLab = new QLabel;
    _timeLab->setObjectName("timeLab");
    _timeLab->setFixedSize(67, 25);
    _timeLab->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    hbox->addWidget(_timeLab);

    _messageLab = new QLabel;
    _messageLab->setObjectName("messageLab");
    _messageLab->setFixedHeight(15);
    _messageLab->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    vbox->addWidget(_messageLab);
}

/**
  * @函数名
  * @功能描述
  * @参数
  * @date 2018.10.17
  */
QString SessionItemFrm::geteElidedText(QFont font, QString str, int MaxWidth)
{
    QFontMetrics fontWidth(font);
    int width = fontWidth.width(str);  //计算字符串宽度
    if(width>=MaxWidth)  //当字符串宽度大于最大宽度时进行转换
    {
        str = fontWidth.elidedText(str,Qt::ElideRight, MaxWidth);  //右部显示省略号
    }
    return str;   //返回处理后的字符串
}
