//
// Created by cc on 18-11-7.
//

#include "LinkButton.h"
#include <QApplication>
#include <QClipboard>
#include "LiteMessageBox.h"

LinkButton::LinkButton(const QString& text, QWidget* parent)
    :QLabel(parent), sendSignal(true)
{
    if(!text.isEmpty())
        setLinkTextSlot(text);

    connect(this, SIGNAL(setLinkTextSignal(const QString&)), this, SLOT(setLinkTextSlot(const QString&)));
}

LinkButton::~LinkButton()
{

}

//
void LinkButton::mousePressEvent(QMouseEvent *e)
{
    if(sendSignal)
        emit clicked();
    else
    {
        QString text = this->text();
        QApplication::clipboard()->setText(text);
        LiteMessageBox::success(tr("复制成功"), 1000);
    }

    QLabel::mousePressEvent(e);
}

//
void LinkButton::setLinkText(const QString &text)
{
    sendSignal = true;
    emit setLinkTextSignal(text);
}

void LinkButton::setLinkTextSlot(const QString &t)
{
    setText(QString());
    setText(QString("<a href='#' style='color:rgb(0,195,188);text-decoration:none;'>%1</a>").arg(t));
}

void LinkButton::setNormalText(const QString &text)
{
    sendSignal = false;
    setText(text);
}
