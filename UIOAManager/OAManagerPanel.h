#pragma once

#include <QWidget>
#include <QSplitter>
#include <QStackedLayout>
#include <QLabel>
#include <QMap>
#include "NavigationItem.h"
#include "MessageManager.h"
#include "OaMainWgt.h"

class OAManagerPanel : public QWidget
{
	Q_OBJECT

public:
	OAManagerPanel();
	~OAManagerPanel();

Q_SIGNALS:
	void updateStartContact();
	void updateUiSignal();

protected:
	void initUi();
	void dealNavItem();
	void onNavItemClicked(int id);
	void getLayoutData();
	void updateUi();

private:
	QSplitter     *_pSplitter;
	QStackedLayout* _pRightLay;
	QLabel        *_pEmptyLabel;
	QVBoxLayout   * _pLeftLay;

private:
	QMap<int, OANavigationItem*> _mapNavItems;
	QMap<int, OaMainWgt*>        _mapMainWgt;

private:
	MessageManager* _pMsgManager;
	std::vector<QTalk::StOAUIData> _uidata;
};
