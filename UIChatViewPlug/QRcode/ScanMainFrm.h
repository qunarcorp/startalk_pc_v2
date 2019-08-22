//
// Created by QITMAC000260 on 2019-04-16.
//

#ifndef QTALK_V2_SCANMAINFRM_H
#define QTALK_V2_SCANMAINFRM_H

#include <QFrame>
#include <QTimer>
#include <QLabel>
#include <QPainter>
#include <QPropertyAnimation>

/**
* @description: ScanMainFrm
* @author: cc
* @create: 2019-04-16 21:11
**/

class PixmapLabel : public QFrame
{
public:
    explicit PixmapLabel(QWidget* parent)
        : QFrame(parent)
    {
    }

protected:
    void paintEvent(QPaintEvent* e) override
    {
        QPainter p(this);
        p.drawPixmap(contentsRect(), QPixmap(":/chatview/image1/scan.png"));
        QFrame::paintEvent(e);
    }
};

class ScanMainFrm : public QFrame {
    Q_OBJECT
public:
    explicit ScanMainFrm(QWidget* parnet = nullptr);
    ~ScanMainFrm() override;

public:
    void start();
    void stop();

protected:
    void paintEvent(QPaintEvent* e) override;

private:
    QTimer* _pTimer;

    PixmapLabel* label;
    QPropertyAnimation* animation;
};


#endif //QTALK_V2_SCANMAINFRM_H
