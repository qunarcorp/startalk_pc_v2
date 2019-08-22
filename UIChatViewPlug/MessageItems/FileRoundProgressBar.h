#ifndef FILEROUNDPROGRESSBAR_H
#define FILEROUNDPROGRESSBAR_H

#include <QFrame>
#include <QLabel>
#include <QPainterPath>

class FileRoundProgressBar : public QFrame
{
    Q_OBJECT

public:
    explicit FileRoundProgressBar(QWidget *parent = 0);
    ~FileRoundProgressBar();

    void setUsedValue(int value);
    void setCurValue(const int &curValue);
private:
    void init();

protected:
    void paintEvent(QPaintEvent *event);

    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);



signals:
    void sgCloseDownLoadingClicked();

private:
    int _currentValue;
    QPainterPath _closePressPath;
    QRectF _colsePressRect;
    QPolygonF _closePressPlg;
    QColor _barCloseColor;
};

#endif // FILEROUNDPROGRESSBAR_H
