//
// Created by cc on 18-11-7.
//

#ifndef QTALK_V2_LINKBUTTON_H
#define QTALK_V2_LINKBUTTON_H

#include "customui_global.h"
#include <QLabel>

class CUSTOMUISHARED_EXPORT LinkButton : public QLabel
{
    Q_OBJECT

public:
    explicit LinkButton(const QString& text = QString(), QWidget* parent = nullptr);
    ~LinkButton() override;

public:
    void setLinkText(const QString& text);
    void setNormalText(const QString& text);

Q_SIGNALS:
    void clicked();
    void setLinkTextSignal(const QString&);

protected:
    void mousePressEvent(QMouseEvent* e) override;

private slots:
    void setLinkTextSlot(const QString& t);

private:
    bool sendSignal;
};


#endif //QTALK_V2_LINKBUTTON_H
