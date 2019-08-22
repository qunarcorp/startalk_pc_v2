#ifndef SESSIONITEMFRM_H
#define SESSIONITEMFRM_H

#include <QFrame>


class HeadPhotoLab;
class QLabel;
class SessionItemFrm : public QFrame
{
    Q_OBJECT
public:
    explicit SessionItemFrm(QWidget *parent = 0);
    ~SessionItemFrm();    

    void setOnline(const bool & _isOnline);
    void setName(const QString &name);
public:

//    HeadPhotoLab * _headPhotoLab;
    QLabel * _headPhotoLab;
    QLabel * _sessionNameLab;
    QLabel * _timeLab;
    QLabel * _messageLab;


private:
    void init();
    void initLayout();
    QString geteElidedText(QFont font, QString str, int MaxWidth);


private:
    bool _isOnline;
    QString _name;
};

#endif // SESSIONITEMFRM_H
