//
// Created by QITMAC000260 on 2019-07-10.
//

#ifndef QTALK_V2_UCBUTTON_H
#define QTALK_V2_UCBUTTON_H

#include <QFrame>

/**
* @description: UCButton
* @author: cc
* @create: 2019-07-10 14:22
**/
class UCButton : public QFrame{
    Q_OBJECT
public:
    explicit UCButton(QString  text, QWidget* parent = nullptr);
    ~UCButton() override = default;

public:
    void setCheckState(bool check);

signals:
    void clicked();

protected:
    void paintEvent(QPaintEvent *) override ;
    bool event(QEvent* e) override ;

private:
    QString _content;
    bool    _isCheck;
    QFont   _font;
};

class UCButtonGroup : public QObject
{
    Q_OBJECT
public:
    explicit UCButtonGroup(QObject* parent = nullptr);
    ~UCButtonGroup() override = default;

signals:
    void clicked(int);

public:
    void addButton(UCButton*, int);
    UCButton* button(int index);
    void setCheck(int);

private:
    void onButtonClicked();

private:
    std::map<UCButton*, int> _mapBtns;
};
#endif //QTALK_V2_UCBUTTON_H
