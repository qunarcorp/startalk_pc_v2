//
// Created by cc on 18-12-21.
//

#include "StaffStructure.h"
#include "CreatGroupPoupWnd.h"
#include <QHBoxLayout>
#include <QVBoxLayout>

StaffStructure::StaffStructure(QWidget *parent)
    : QFrame(parent), _structureCount(0)
{
    _pParentLabel = new QLabel(this);
    _pCurrentLabel = new QLabel(this);
    _pCountLabel = new QLabel(this);
    _pCreatGroupBtn = new QPushButton(QStringLiteral("创建群聊"), this);
	_pCreatGroupBtn->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	_pCreatGroupBtn->setFixedWidth(290);

    _pParentLabel->setObjectName("Staff_ParentLabel");
    _pCurrentLabel->setObjectName("Staff_CurrentLabel");
    _pCountLabel->setObjectName("Staff_CountLabel");
    _pCreatGroupBtn->setObjectName("Staff_CreatGroupBtn");
    //
    _pParentLabel->setAlignment(Qt::AlignCenter);
    _pCurrentLabel->setAlignment(Qt::AlignCenter);
    _pCountLabel->setAlignment(Qt::AlignCenter);

    //
    auto * bodyLay = new QVBoxLayout(this);
    bodyLay->setSpacing(30);
    bodyLay->addItem(new QSpacerItem(10, 10, QSizePolicy::Minimum, QSizePolicy::Expanding));
    bodyLay->addWidget(_pParentLabel);
    bodyLay->addWidget(_pCurrentLabel);
    bodyLay->addWidget(_pCountLabel);
    bodyLay->addWidget(_pCreatGroupBtn);
	bodyLay->setAlignment(_pCreatGroupBtn, Qt::AlignCenter);
    bodyLay->addItem(new QSpacerItem(10, 10, QSizePolicy::Minimum, QSizePolicy::Expanding));

    _pCreatGroupPoupWnd = new CreatGroupPoupWnd(this);

    connect(_pCreatGroupBtn, &QPushButton::clicked, [this]()
    {
        QString structure = _structureStr;
        int count = _structureCount;
        _pCreatGroupPoupWnd->setCunt(count);

        QRect geometry = this->geometry();
        QPoint pos = mapToGlobal(geometry.topLeft());
        _pCreatGroupPoupWnd->move((width() - _pCreatGroupPoupWnd->width()) / 2 + pos.x(),
                (height() - _pCreatGroupPoupWnd->height()) / 2 + pos.y() - 50);
        _pCreatGroupPoupWnd->showModel();
    });
    connect(_pCreatGroupPoupWnd, &CreatGroupPoupWnd::sgCreatGroupSignal, [this](){
        emit creatGroupSignal(_structureStr, _pCreatGroupPoupWnd->getGroupName());
    });
}

StaffStructure::~StaffStructure()
{

}

/**
 * 设置显示数据
 * @param parentStr
 * @param name
 * @param num
 */
void StaffStructure::setData(const QString &parentStr, const QString &name, int num)
{
    _structureCount = num;
	if (parentStr.isEmpty())
	{
		_structureStr = "/" + name;
		_pParentLabel->setText("");
	}
	else
	{
		_structureStr = QStringLiteral("%1/%2").arg(parentStr, name);
		_pParentLabel->setText(parentStr.mid(1));
	}

    _pCurrentLabel->setText(name);
    _pCountLabel->setText(QStringLiteral("- %1人 -").arg(num));
    _pCreatGroupBtn->setEnabled(num < 1000);
}


