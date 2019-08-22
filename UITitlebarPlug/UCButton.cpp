#include <utility>

//
// Created by QITMAC000260 on 2019-07-10.
//

#include "UCButton.h"
#include "../UICom/StyleDefine.h"
#include <QPaintEvent>
#include <QPainter>

UCButton::UCButton(QString text, QWidget *parent)
    :QFrame(parent), _content(std::move(text)), _isCheck(false)
{
    setFixedHeight(30);
    //
    _font.setWeight(400);
    _font.setPixelSize(14);
}

void UCButton::paintEvent(QPaintEvent *e) {

    QPainter painter(this);
    const QRect &rect = contentsRect();
    painter.setFont(_font);
//    painter.fillRect(rect, QColor(255, 255, 255));
    if(_isCheck)
    {
        painter.setPen(QColor(0, 202, 190));
        QFontMetricsF contentF(_font);
        qreal fw = contentF.width(_content);
        int w = 16;
        painter.setBrush(QColor(0, 202, 190));
        painter.drawRoundedRect((rect.width() - w) / 2, rect.bottom() - 6,
                w, 3, 1, 2);
    }
    else
        painter.setPen(QTalk::StyleDefine::instance().getNavNameFontColor());

    painter.drawText(rect.x() + 2, rect.y() + 4, rect.width() - 4, rect.height() - 4 - 12, Qt::AlignCenter, _content);
    QFrame::paintEvent(e);
}

void UCButton::setCheckState(bool check) {
    _isCheck = check;
	repaint();
}

bool UCButton::event(QEvent* e) {

    if(e->type() == QEvent::MouseButtonPress)
        emit clicked();

    return QFrame::event(e);
}

/**/
UCButtonGroup::UCButtonGroup(QObject* parent)
    :QObject(parent)
{

}

void UCButtonGroup::addButton(UCButton *btn, int id) {
    _mapBtns[btn] = id;
    connect(btn, &UCButton::clicked, this, &UCButtonGroup::onButtonClicked);
}

void UCButtonGroup::onButtonClicked() {
    auto *btn = qobject_cast<UCButton*>(sender());
    auto itFind = _mapBtns.find(btn);
    if(btn && itFind != _mapBtns.end())
    {
        emit clicked(itFind->second);
        //
        for(const auto& it : _mapBtns)
        {
            it.first->setCheckState(it.first == btn);
        }
    }
}

UCButton* UCButtonGroup::button(int index) {
    UCButton* btn = nullptr;
    auto itFind = std::find_if(_mapBtns.begin(), _mapBtns.end(), [index](const std::pair<UCButton*, int>& tmp){
        return index == tmp.second;
    });
    if(itFind != _mapBtns.end())
        btn = itFind->first;

    return btn;
}

void UCButtonGroup::setCheck(int index)
{
    for(const auto& it : _mapBtns)
    {
        it.first->setCheckState(it.second == index);
    }
}