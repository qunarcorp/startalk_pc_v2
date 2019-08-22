#ifndef _EMOLSTITEMWGT_H_
#define _EMOLSTITEMWGT_H_

#include <QFrame>

class EmoLstItemWgt : public QFrame
{
public:
	EmoLstItemWgt(QString  pkgId, QString  iconPath, QString  emoName);
	~EmoLstItemWgt();

public:
	QString getPkgId();
	QString getIconPath();
	void    setCheckState(bool check);

private:
	virtual void paintEvent(QPaintEvent *e);

private:
	QString _pgkId;
	QString _icon;
	QString _name;

private:
	bool _isChecked;
};

#endif//