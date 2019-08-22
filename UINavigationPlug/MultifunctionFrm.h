#ifndef MULTIFUNCTIONFRM_H
#define MULTIFUNCTIONFRM_H

#include <QFrame>

class MultifunctionFrm : public QFrame
{
    Q_OBJECT

public:
    explicit MultifunctionFrm(QWidget *parent = 0);
    ~MultifunctionFrm();

public:
    void openQiangDan();

private:
};

#endif // MULTIFUNCTIONFRM_H
