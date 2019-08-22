#ifndef _EMOPREVIEWWGT_H_
#define _EMOPREVIEWWGT_H_

#include <QStackedWidget>
#include <QTableWidget>
#include "EmoticonStruct.h"
#include "EmoticonView.h"

enum
{
	EM_DATAROLE_PKGID = Qt::UserRole + 1,
	EM_DATAROLE_SHORTCUT
};

class EmoPreviewWgt : public QStackedWidget
{
	Q_OBJECT

public:
	explicit EmoPreviewWgt(int col, QWidget *parent = nullptr);
	~EmoPreviewWgt() override;

public:
	QTableWidget* addEmoticon(const QMap<UnorderMapKey, StEmoticonItem>& arEmos, const QString& id);
	void deleteEmoticon(QTableWidget* wgt);
	void deleteEmoticonById(const QString& pkgId);
	void updateCollection(QTableWidget* wgt, const std::map<UnorderMapKey, std::string>& collections);

public:
    void loadEmoView(const QString& imagePath);
    void hidePreviewWnd();

protected:
    bool event(QEvent* e) override;

protected:
	int _colCount;
	QMap<QString, QTableWidget*> _mapPkgIdWgt;

private:
    EmoticonView* _pEmoView;
};

#endif //_EMOPREVIEWWGT_H_