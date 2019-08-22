#include "EmoPreviewWgt.h"
#include <QTableWidget>
#include <QHeaderView>
#include <QLabel>
#include <QFileInfo>
#include "../Platform/Platform.h"
#include "EmoCellWidget.h"

EmoPreviewWgt::EmoPreviewWgt(int col, QWidget *parent)
	: QStackedWidget(parent), _colCount(col)
{
    _pEmoView = new EmoticonView(this);
}

EmoPreviewWgt::~EmoPreviewWgt()
{

}

/**
  * @函数名   
  * @功能描述 
  * @参数
  * @author   cc
  * @date     2018/10/18
  */
QTableWidget*  EmoPreviewWgt::addEmoticon(const QMap<UnorderMapKey, StEmoticonItem>& arEmos, const QString& id)
{
	auto* wgt = new QTableWidget(this);
	wgt->setAutoScroll(false);
	wgt->setFrameShape(QFrame::NoFrame);
	wgt->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	wgt->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	wgt->horizontalHeader()->setVisible(false);
	wgt->verticalHeader()->setVisible(false);
	wgt->setShowGrid(false);
	wgt->setWindowFlags(Qt::FramelessWindowHint);
	wgt->setColumnCount(_colCount);
	wgt->setFocusPolicy(Qt::NoFocus);
	std::string dirPath = Platform::instance().getLocalEmoticonPath(id.toStdString());

	int row = 0, col = 0;
	auto it = arEmos.begin();
	for (; it != arEmos.end(); it++)
	{
		if (wgt->rowCount() != row + 1)
		{
			wgt->insertRow(row);
			wgt->setRowHeight(row, 60);
		}

		QString fPath = QString("%1/%2").arg(dirPath.data()).arg(it->fileorg);
		if (!QFileInfo(fPath).exists()) continue;
		EmoCellWidget* itemWgt = new EmoCellWidget(fPath, id, it->shortcut);
        connect(itemWgt, &EmoCellWidget::sgPreviewImage, this, &EmoPreviewWgt::loadEmoView);
		itemWgt->setToolTip(it->tooltip);
		wgt->setCellWidget(row, col, itemWgt);
		if (++col == _colCount)
		{
			col = 0;
			row++;
		}
	}

	addWidget(wgt);
	_mapPkgIdWgt[id] = wgt;

	return wgt;
}

/**
  * @函数名   deleteEmoticon
  * @功能描述 
  * @参数
  * @author   cc
  * @date     2018/10/21
  */
void EmoPreviewWgt::deleteEmoticon(QTableWidget* wgt)
{
	removeWidget(wgt);
	delete wgt;
	wgt = nullptr;
	if (count() > 0)
	{
		setCurrentIndex(0);
	}
}

void EmoPreviewWgt::deleteEmoticonById(const QString& pkgId)
{
	if (_mapPkgIdWgt.contains(pkgId))
	{
		deleteEmoticon(_mapPkgIdWgt[pkgId]);
	}
}

/**
 *
 */
void EmoPreviewWgt::updateCollection(QTableWidget *wgt, const std::map<UnorderMapKey, std::string>& collections)
{
    wgt->clear();
    wgt->setRowCount(0);
    //
	int row = 0, col = 0;

	for(const auto& item : collections)
    {
        if (wgt->rowCount() != row + 1)
        {
            wgt->insertRow(row);
            wgt->setRowHeight(row, 60);
        }

        QString fPath = item.first;
        if (!QFileInfo(fPath).exists()) continue;
        EmoCellWidget* itemWgt = new EmoCellWidget(fPath, item.first, QString::fromStdString(item.second));
        connect(itemWgt, &EmoCellWidget::sgPreviewImage, this, &EmoPreviewWgt::loadEmoView);
        wgt->setCellWidget(row, col, itemWgt);
        if (++col == _colCount)
        {
            col = 0;
            row++;
        }
    }
}

/**
 *
 * @param imagePath
 */
void EmoPreviewWgt::loadEmoView(const QString &imagePath)
{
    auto* wgt = qobject_cast<EmoCellWidget*>(sender());
    if(nullptr != wgt)
    {
        QPoint pos = mapToGlobal(wgt->geometry().topRight());
        pos.setX(pos.x() - (_pEmoView->width() - wgt->width()) / 2 - wgt->width());
        pos.setY(pos.y() - _pEmoView->height() - 5);
        _pEmoView->move(pos);
        _pEmoView->setImagePath(imagePath);
        _pEmoView->setVisible(true);
        //
    }
}

/**
 *
 */
void EmoPreviewWgt::hidePreviewWnd()
{
    if(nullptr != _pEmoView)
    {
        _pEmoView->setVisible(false);
    }
}

bool EmoPreviewWgt::event(QEvent* e)
{
    if(e->type() == QEvent::Leave || e->type() == QEvent::WindowDeactivate)
    {
        if(_pEmoView->isVisible())
            _pEmoView->setVisible(false);
    }

    return QStackedWidget::event(e);
}
