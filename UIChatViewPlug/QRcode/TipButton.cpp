#include <utility>

//
// Created by QITMAC000260 on 2019-04-17.
//

#include "TipButton.h"
#include "QRcode.h"

TipButton::TipButton(QString text, int id, QWidget* parent)
    : QFrame(parent)
    , _isSelected(false)
    , _text(std::move(text))
    , _id(id)
{
    setMinimumHeight(30);

    setFrameShape(QFrame::NoFrame);
    _normalColor = QColor(51, 51, 51);
    _selectedColor = QColor(0, 202, 190);
    _font.setPixelSize(14);
    _font.setWeight(400);

    //
    QFontMetricsF qfm(_font);
    int w = qfm.width(_text);
    setMinimumWidth(qfm.width(_text));
}

TipButton::~TipButton() {

}

void TipButton::setCheckState(bool isCheck)
{
    _isSelected = isCheck;
    if(_isSelected)
        _font.setWeight(600);
    else
        _font.setWeight(400);

    QFontMetricsF qfm(_font);
    setMinimumWidth(qfm.width(_text) + 10);
    update();
}

void TipButton::paintEvent(QPaintEvent *e)
{
    QPainter painter(this);
    QColor color;
    if(_isSelected)
        color = _selectedColor;
    else
        color = _normalColor;

    painter.setPen(color);
    painter.setFont(_font);
    QRect rect = this->contentsRect();
    painter.drawText(rect, _text);

    if(_isSelected)
    {
        painter.fillRect(QRect(rect.x() + (rect.width() - 16) / 2, rect.bottom() - 3, 16, 3), color);
    }

    QFrame::paintEvent(e);
}

void TipButton::mousePressEvent(QMouseEvent *e)
{
    emit clicked(_id);
    QWidget::mousePressEvent(e);
}
