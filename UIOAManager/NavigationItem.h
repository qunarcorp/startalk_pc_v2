#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif

#ifndef NAVIGATIONITEM_H
#define NAVIGATIONITEM_H

#include <QFrame>
#include <QLabel>
#include "../include/CommonDefine.h"

enum itemType
{
	EM_ITEM_TYPE_EMPTY,
	EM_ITEM_TYPE_OFFICEOA,
	EM_ITEM_TYPE_UTILITIES,
	EM_ITEM_TYPE_IT_ASSISTANT,
	EM_ITEM_TYPE_TASK,
};
class OAManagerPanel;
class OANavigationItem : public QFrame
{
	Q_OBJECT

public:
    OANavigationItem(const int& id, QString name, QString icon, OAManagerPanel *parent = nullptr);
	~OANavigationItem() override;

public:
	void registerWgt(QWidget* wgt);
	void setSelectState(bool selected);

public:
	inline int getId()
	{
		return _id;
	}

Q_SIGNALS:
	void itemClicked(int);

protected:
	void mousePressEvent(QMouseEvent *e) override;
	void paintEvent(QPaintEvent *e) override;

private:
	QWidget* _itemWgt;
	int      _id;

private:
	OAManagerPanel* _mainPanel;

private:
	bool _selected;

private:
    QString _name;
    QString _icon;

};

#endif // !NAVIGATIONITEM_H