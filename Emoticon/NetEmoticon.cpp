#include "NetEmoticon.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QPixmap>
#include <QPushButton>
#include <QListWidget>
#include <QStackedWidget>
#include <QDebug>
#include "EmoticonMainWgt.h"
#include "EmoLstItemWgt.h"
#include <cmath>

#define STDSTR2QSTR(str) QString::fromStdString(str)
#define DOUBLE_IS_EQUAL(x, y) (std::abs(x - y)) < 10e-6

NetEmoDetailWgt::NetEmoDetailWgt(const QString& pkgId, const QString& icon, const QString name, const QString& desc, QInt32 fileSize, bool exists)
	:QWidget(nullptr), _strPkgId(pkgId), _isDownloading(false)
{
	QPixmap iconPix(icon);
	QLabel *iconLabel = new QLabel();
	iconLabel->setFixedSize(128, 128);
	iconLabel->setAlignment(Qt::AlignCenter | Qt::AlignHCenter);
	if (iconPix.width() > 128 || iconPix.height() > 128)
	{
		iconLabel->setScaledContents(true);
	}
	iconLabel->setPixmap(iconPix);

	QHBoxLayout* iconLayout = new QHBoxLayout;
	iconLayout->addSpacerItem(new QSpacerItem(10, 10, QSizePolicy::Expanding));
	iconLayout->addWidget(iconLabel);
	iconLayout->addSpacerItem(new QSpacerItem(10, 10, QSizePolicy::Expanding));

	QLabel *nameLabel = new QLabel(name);
	nameLabel->setAlignment(Qt::AlignCenter | Qt::AlignHCenter);
	nameLabel->setObjectName("NetEmoNameLabel");

	QLabel *descLabel = new QLabel(desc);
	descLabel->setAlignment(Qt::AlignCenter | Qt::AlignHCenter);
	descLabel->setObjectName("NetEmoDescLabel");

	QLabel *fSizeLabel = new QLabel(trancelateFileSize(fileSize));
	fSizeLabel->setAlignment(Qt::AlignCenter | Qt::AlignHCenter);
	fSizeLabel->setObjectName("NetEmoFileSizeLabel");

	_pDownloadBtn = new QPushButton();
	_pDownloadBtn->setObjectName("DownloadEmoticonButton");
	QHBoxLayout* downloadBtnLayout = new QHBoxLayout;
	downloadBtnLayout->addSpacerItem(new QSpacerItem(10, 10, QSizePolicy::Expanding));
	downloadBtnLayout->addWidget(_pDownloadBtn);
	downloadBtnLayout->addSpacerItem(new QSpacerItem(10, 10, QSizePolicy::Expanding));
	if (exists)
	{
		_pDownloadBtn->setText(tr("已下载"));
		_pDownloadBtn->setEnabled(false);
	}
	else
	{
		_pDownloadBtn->setText(tr("下载"));
	}

	QVBoxLayout* mLayout = new QVBoxLayout;
	mLayout->setAlignment(Qt::AlignCenter);
	mLayout->addSpacerItem(new QSpacerItem(10, 40, QSizePolicy::Fixed, QSizePolicy::Fixed));
	mLayout->addLayout(iconLayout);
	mLayout->addSpacerItem(new QSpacerItem(10, 20, QSizePolicy::Fixed, QSizePolicy::Fixed));
	mLayout->addWidget(nameLabel);
	mLayout->addWidget(descLabel);
	mLayout->addWidget(fSizeLabel);
	mLayout->addSpacerItem(new QSpacerItem(10, 20, QSizePolicy::Fixed, QSizePolicy::Fixed));
	mLayout->addLayout(downloadBtnLayout);
	mLayout->addSpacerItem(new QSpacerItem(10, 60, QSizePolicy::Fixed, QSizePolicy::Expanding));

	setLayout(mLayout);

	connect(_pDownloadBtn, &QPushButton::clicked, this, &NetEmoDetailWgt::readyDownloadEmo);
}

NetEmoDetailWgt::~NetEmoDetailWgt()
{
	
}

/**
  * @函数名   
  * @功能描述 
  * @参数
  * @author   cc
  * @date     2018/10/22
  */
QString NetEmoDetailWgt::getPkgId()
{
	return _strPkgId;
}

/**
  * @函数名   updateDownloadProcess
  * @功能描述 
  * @参数
  * @author   cc
  * @date     2018/10/22
  */
void NetEmoDetailWgt::updateDownloadProcess(const QString& pkgId, double dtotal, double dnow)
{
	if (!_isDownloading || _strPkgId != pkgId || dtotal == 0) return;
	
	if (DOUBLE_IS_EQUAL(dtotal, dnow))
	{
		_isDownloading = false;
		_pDownloadBtn->setEnabled(false);
		_pDownloadBtn->setText(tr("安装中..."));
		EmoticonMainWgt::getInstance()->installEmoticon(pkgId);
	}
	else
	{
		double process = dnow / dtotal * 100;
		_pDownloadBtn->setText(tr("下载中 ( % %1 ) ").arg(QString::number(process, 'f', 0)));
	}
}

/**
  * @函数名   onInstalledEmoticon
  * @功能描述 
  * @参数
  * @author   cc
  * @date     2018/10/23
  */
void NetEmoDetailWgt::onInstalledEmoticon(const QString& pkgId)
{
	if (_isDownloading || _strPkgId != pkgId) return;

	_pDownloadBtn->setText(tr("已下载"));
}

/**
  * @函数名   
  * @功能描述 
  * @参数
  * @author   cc
  * @date     2018/10/28
  */
void NetEmoDetailWgt::onInstallError(const QString& pkgId)
{
	if (_isDownloading || _strPkgId != pkgId) return;

	_pDownloadBtn->setText(tr("安装错误"));
}

/**
  * @函数名   
  * @功能描述 
  * @参数
  * @author   cc
  * @date     2018/10/28
  */
void NetEmoDetailWgt::onRemoveLocalEmotion(const QString& pkgId)
{
	if (_isDownloading || _strPkgId != pkgId) return;
	_pDownloadBtn->setEnabled(true);
	_pDownloadBtn->setText(tr("下载"));
}

/**
  * @函数名   trancelateFileSize
  * @功能描述 
  * @参数
  * @author   cc
  * @date     2018/10/22
  */
QString NetEmoDetailWgt::trancelateFileSize(QInt32 size)
{
	QString ret;
	if (size > 1024 * 1024)
		ret = QString("%1MB").arg(QString::number(size / (1024 * 1024.00), 'f', 2));
	else if (size > 1024)
		ret = QString("%1KB").arg(QString::number(size / 1024.00, 'f', 2));
	return ret;
}

/**
  * @函数名   readyDownloadEmo
  * @功能描述 
  * @参数
  * @author   cc
  * @date     2018/10/22
  */
void NetEmoDetailWgt::readyDownloadEmo()
{
	if (_isDownloading)
	{
		return;
	}
	_isDownloading = true;
	_pDownloadBtn->setText(tr("等待下载"));
	emit downloadEmoticon(_strPkgId);
}



/****************************************************************************/
NetEmoticon::NetEmoticon(QMap<QString, StEmoticon>& mapEmo, QWidget *parent)
	: QWidget(parent), _mapEmo(mapEmo)
{
	initUi();


}

NetEmoticon::~NetEmoticon()
{
}

/**
  * @函数名   initUi
  * @功能描述 
  * @参数
  * @author   cc
  * @date     2018/10/21
  */
void NetEmoticon::initUi()
{
	//
	_pNetEmoLst = new QListWidget;
	_pNetEmoLst->setFrameShape(QFrame::NoFrame);
	_pNetEmoLst->setFocusPolicy(Qt::NoFocus);
	_pNetEmoLst->setObjectName("NetEmoLst");
	_pNetEmoLst->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	_pNetEmoLst->setFixedWidth(165);
	_pStackedNetDetailWgt = new QStackedWidget;
	auto* layout = new QHBoxLayout(this);
	layout->setMargin(0);
	layout->addWidget(_pNetEmoLst);
	layout->addWidget(_pStackedNetDetailWgt);
	layout->setStretch(1, 45);
	setLayout(layout);

	connect(_pNetEmoLst, &QListWidget::itemClicked, this, &NetEmoticon::onEmoLstItemClick);
	connect(EmoticonMainWgt::getInstance(), &EmoticonMainWgt::sgGotNetEmo, this, &NetEmoticon::initItems);
}

void NetEmoticon::initItems()
{
    _pNetEmoLst->clear();
    _mapEmoItemWgt.clear();
    for(const auto & it : _mapEmoDetailWgt)
    {
        _pStackedNetDetailWgt->removeWidget(it);
    }
    _mapEmoDetailWgt.clear();


    ArStNetEmoticon arNetEmo = EmoticonMainWgt::getInstance()->getNetEmoticonInfo();
    for (const auto& emo : arNetEmo)
    {
        if (emo->iconPath.empty())
            continue;

        auto* lstItem = new QListWidgetItem(_pNetEmoLst);
        EmoLstItemWgt* lstWgt = new EmoLstItemWgt(STDSTR2QSTR(emo->pkgid), STDSTR2QSTR(emo->iconPath), STDSTR2QSTR(emo->emoName));
        lstItem->setSizeHint(QSize(155, 50));
        _pNetEmoLst->setItemWidget(lstItem, lstWgt);
        if (_mapEmoItemWgt.empty())
        {
            lstItem->setSelected(true);
            lstWgt->setCheckState(true);
        }
        _mapEmoItemWgt[lstItem] = lstWgt;

        NetEmoDetailWgt* detailWgt =
                new NetEmoDetailWgt(STDSTR2QSTR(emo->pkgid), STDSTR2QSTR(emo->iconPath),
                                    STDSTR2QSTR(emo->emoName), STDSTR2QSTR(emo->desc), emo->filesize, _mapEmo.contains(STDSTR2QSTR(emo->pkgid)));
        _mapEmoDetailWgt[lstItem] = detailWgt;
        _pStackedNetDetailWgt->addWidget(detailWgt);
        connect(detailWgt, &NetEmoDetailWgt::downloadEmoticon, EmoticonMainWgt::getInstance(), &EmoticonMainWgt::downloadNetEmoticon);
        connect(EmoticonMainWgt::getInstance(), &EmoticonMainWgt::updateProcessSignal, detailWgt, &NetEmoDetailWgt::updateDownloadProcess);
        connect(EmoticonMainWgt::getInstance(), &EmoticonMainWgt::installedEmotion, detailWgt, &NetEmoDetailWgt::onInstalledEmoticon);
        connect(EmoticonMainWgt::getInstance(), &EmoticonMainWgt::installEmoticonError, detailWgt, &NetEmoDetailWgt::onInstallError);
        connect(EmoticonMainWgt::getInstance(), &EmoticonMainWgt::removeEmoticon, detailWgt, &NetEmoDetailWgt::onRemoveLocalEmotion);
    }
}

/**
  * @函数名   
  * @功能描述 
  * @参数
  * @author   cc
  * @date     2018/10/22
  */
void NetEmoticon::onEmoLstItemClick(QListWidgetItem *item)
{
	if (_mapEmoItemWgt.contains(item) && _mapEmoDetailWgt.contains(item))
	{
		EmoLstItemWgt *lstWgt = _mapEmoItemWgt[item];
        foreach (EmoLstItemWgt* w, _mapEmoItemWgt)
		{
			w->setCheckState(w == lstWgt);
		}
		_pStackedNetDetailWgt->setCurrentWidget(_mapEmoDetailWgt[item]);
	}
}
