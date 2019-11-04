#ifndef _EMOICONWGT_H_
#define _EMOICONWGT_H_

#include <QFrame>
#include <QStackedWidget>
#include <QToolButton>
#include <QMap>

class QHBoxLayout;
class EmoIcon : public QFrame
{
	Q_OBJECT
public:
	EmoIcon(const QString& iconPath);
	~EmoIcon();

Q_SIGNALS:
	void clicked();

public:
	QString getIconPath();
	void    setCheckState(bool check);

private:
	virtual void paintEvent(QPaintEvent *e);
	virtual void mousePressEvent(QMouseEvent *e);

private:
	QString _pgkId;
	QString _icon;

private:
	bool _isChecked;
};

class EmoIconRow : public QFrame
{
	Q_OBJECT

public:
	EmoIconRow(QWidget* parent);
	~EmoIconRow();

public:
	void addEmoIcon(EmoIcon* icon, const QString& id);
	int  getCount();

public:
	bool hasEmoIcon(const QString& pkgId) { return _mapEmoId.contains(pkgId); };
	EmoIcon* getEmoIcon(const QString& pkgId) { return _mapEmoId[pkgId]; }
	void removeIcon(const QString& pkgId);
	void resetState();

private:
	QMap<QString, EmoIcon*> _mapEmoId;
	EmoIcon*               _pFirstEmo;
	int          _iconCount;
	QHBoxLayout* _pLayout;
};


class EmoIconWgt : public QStackedWidget
{
	Q_OBJECT

public:
	EmoIconWgt(QWidget *parent = nullptr);
	~EmoIconWgt();

public:
	EmoIcon* addEmoIcon(const QString& iconPath, const QString& id, const QString& name);
	void deleteEmoIcon(const QString& pkgId);

private:
	QVector<EmoIconRow*> _arEmoIconRow;
};

#endif//_EMOICONWGT_H_