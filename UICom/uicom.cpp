#include "uicom.h"
#include <QDialog>
#include <iostream>

UICom* UICom::_pUiCommon = nullptr;
//
UICom* UICom::getInstance()
{
	if (_pUiCommon == nullptr)
	{
		_pUiCommon = new UICom;
	}
	return _pUiCommon;
}

UICom::UICom()
	: _pCurActiveWnd(nullptr)
{

}

UICom::~UICom()
{

}

QWidget* UICom::getAcltiveMainWnd()
{
	QMutexLocker locker(&_mutex);
	return _pCurActiveWnd;
}

void UICom::setAcltiveMainWnd(QWidget* wnd)
{
	QMutexLocker locker(&_mutex);
	if (wnd)
	{
		_pCurActiveWnd = wnd;
	}
}
