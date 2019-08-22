#ifndef UICOM_H
#define UICOM_H

#include "uicom_global.h"
#include <QWidget>
#include <QMutexLocker>

class UICOMSHARED_EXPORT UICom
{
public:
	static  UICom* getInstance();
	~UICom();

public:
	QWidget* getAcltiveMainWnd();
	void setAcltiveMainWnd(QWidget* wnd);

private:
    UICom();


private:
	QMutex         _mutex;
	static UICom*  _pUiCommon;
	QWidget*      _pCurActiveWnd;
};

#endif // UICOM_H
