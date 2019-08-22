
#ifndef NAVIGATIONITEM_H
#define NAVIGATIONITEM_H

#include <QFrame>
#include <QLabel>
#include "../include/CommonDefine.h"
#include "customui_global.h"
#include "HeadPhotoLab.h"

enum itemType
{
	EM_ITEM_TYPE_EMPTY,
	EM_ITEM_TYPE_START,
	EM_ITEM_TYPE_FRIENDLIST,
	EM_ITEM_TYPE_GROUPLIST,
	EM_ITEM_TYPE_STAFF,
	EM_ITEM_TYPE_SUBSCRIPTION,
	EM_ITEM_TYPE_BLACKLIST
};
class AddressBookPanel;
class CUSTOMUISHARED_EXPORT NavigationItem : public QFrame
{
	Q_OBJECT

public:
	NavigationItem(const QUInt8& itemType, QWidget *parent);
	~NavigationItem() override;

public:
	void registerWgt(QWidget* wgt);
	void setSelectState(bool selected);
	QWidget *getItemWgt();

public:
	inline QUInt8 getItemType()
	{
		return _itemType;
	}

signals:
	void itemClicked(QUInt8);

protected:
    void mousePressEvent(QMouseEvent *e) override;

private:
    HeadPhotoLab* _pIconLabel;
	QLabel* _pTextLabel;

private:
	QWidget* _itemWgt;
	QUInt8   _itemType;

private:
	AddressBookPanel* _mainPanel;

private:
	bool _selected;
};

#endif // !NAVIGATIONITEM_H