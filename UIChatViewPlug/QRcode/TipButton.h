//
// Created by QITMAC000260 on 2019-04-17.
//

#ifndef QTALK_V2_TIPBUTTON_H
#define QTALK_V2_TIPBUTTON_H

#include <QFrame>
#include <QPainter>

/**
* @description: TipButton
* @author: cc
* @create: 2019-04-17 15:18
**/
class TipButton : public QFrame{
    Q_OBJECT
public:
    explicit TipButton(QString text, int id, QWidget* parent = nullptr);
    ~TipButton() override;

public:
    void setCheckState(bool isCheck);

Q_SIGNALS:
    void clicked(int);

protected:
    void paintEvent(QPaintEvent *) override;
    void mousePressEvent(QMouseEvent* e) override;

private:
    bool _isSelected;
    QString _text;
    int _id;
    //
    QColor _normalColor;
    QColor _selectedColor;
    QFont _font;
};


#endif //QTALK_V2_TIPBUTTON_H
