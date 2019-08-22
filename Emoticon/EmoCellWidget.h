#ifndef _EMOCELLWIDGET_H_
#define _EMOCELLWIDGET_H_

#include <QLabel>
#include <QMovie>

class EmoCellWidget : public QLabel
{
	Q_OBJECT

public:
    explicit EmoCellWidget(QString  emoPath, QString  pkgId = QString(), QString  shortCut = QString(), QWidget *parent = nullptr);
	~EmoCellWidget() override;

public:
	QString getPkgId() { return _strPkgId; }
	QString getShortCut() { return _strEmoShortCut; }

signals:
    void sgPreviewImage(const QString&);

protected:
	bool event(QEvent *event) override;
	void paintEvent(QPaintEvent* e) override;

private:
	QString _strPkgId;
	QString _strEmoShortCut;
	QString _imagePath;

    int _width = 0;

private:
//	QMovie* _mov;
};

#endif//_EMOCELLWIDGET_H_