//
// Created by QITMAC000260 on 2019/10/22.
//
#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif
#ifndef QTALK_V2_USERHEADWGT_H
#define QTALK_V2_USERHEADWGT_H

#include <QFrame>

/**
* @description: UserHeadWgt
* @author: cc
* @create: 2019-10-22 20:47
**/
class UserHeadWgt : public QFrame {
    Q_OBJECT
public:
    explicit UserHeadWgt(QString name, const QString &headSrc,
                         bool onlyShowText = false, QWidget* parent = nullptr);

protected:
    void paintEvent(QPaintEvent* e) override ;
    void mousePressEvent(QMouseEvent* e) override ;

Q_SIGNALS:
    void clicked();

private:
    QString _name;
    QString _headSrc;

    bool    _onlyShowText = false;
};


#endif //QTALK_V2_USERHEADWGT_H
