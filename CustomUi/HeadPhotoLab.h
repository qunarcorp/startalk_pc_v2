#ifndef HEADPHOTOBTN_H
#define HEADPHOTOBTN_H

#include <QWidget>
#include <QToolButton>
#include <QLabel>
#include <QMap>
#include <QPixmap>
#include <QMutexLocker>
#include <QMovie>
#include "customui_global.h"

class CUSTOMUISHARED_EXPORT HeadPhotoLab : public QLabel
{
public:
    explicit HeadPhotoLab(
            const QString& strHeadPath = QString(),
            int radius = 0,
            bool isGrey = false,
            bool startMovie = false,
            bool showRect = false,
            QWidget *parent = nullptr);
    ~HeadPhotoLab() override;

public:
    void setHead(const QString& headPath,
            int radius,
            bool isGrey = false,
            bool startMovie = false,
            bool showRect = false);

    void startMovie();
    void stopMovie();

protected:
    void paintEvent(QPaintEvent* e) override;
    void initMovie();
    bool event(QEvent* e) override ;

private:
    QString _imagePath;
    int     _radius;
    bool    _isGrey;
    bool    _startMovie;
    bool    _showRect;

private:
    QMovie* _mov;

    QVector<QPixmap> _pixmap;
};

#endif // HEADPHOTOBTN_H
