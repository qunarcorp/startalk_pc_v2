#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif
#ifndef _NETEMOTICON_H_
#define _NETEMOTICON_H_

#include <QWidget>
#include "EmoticonStruct.h"
#include "../include/CommonDefine.h"

class QListWidget;
class EmoLstItemWgt;
class QListWidgetItem;
class QPushButton;
class QStackedWidget;

class NetEmoDetailWgt : public QWidget
{
	Q_OBJECT
public:
	NetEmoDetailWgt(const QString& pkgId, const QString& icon, const QString name, const QString& desc, QInt32 fileSize, bool exists);
	~NetEmoDetailWgt();

public:
	QString getPkgId();
	void updateDownloadProcess(const QString& pkgId, double dtotal, double dnow);
	void onInstalledEmoticon(const QString& pkgId);
	void onInstallError(const QString& pkgid);
	void onRemoveLocalEmotion(const QString& pkgId);

Q_SIGNALS:
	void downloadEmoticon(const QString&);

public:
	QPushButton* _pDownloadBtn;

private:
	QString trancelateFileSize(QInt32 size);
	void readyDownloadEmo();

private:
	QString      _strPkgId;
	bool         _isDownloading;
};

class NetEmoticon : public QWidget
{
	Q_OBJECT

public:
	NetEmoticon(QMap<QString, StEmoticon>& mapEmo, QWidget *parent);
	~NetEmoticon() override;

private:
	void initUi();
	void onEmoLstItemClick(QListWidgetItem *item);

public:
    void initItems();

private:
	QListWidget*                 _pNetEmoLst;
	QStackedWidget*              _pStackedNetDetailWgt;

private:
	QMap<QString, StEmoticon>&   _mapEmo;
	QMap<QListWidgetItem *, EmoLstItemWgt *>  _mapEmoItemWgt;
	QMap<QListWidgetItem *, NetEmoDetailWgt*> _mapEmoDetailWgt;
};

#endif//_NETEMOTICON_H_