#ifndef SHADOWHEADPHOTO_H
#define SHADOWHEADPHOTO_H

#include <QFrame>
#include <QLabel>
#include "customui_global.h"

namespace Ui {
class ShadowHeadPhoto;
}

class CUSTOMUISHARED_EXPORT ShadowHeadPhoto : public QFrame
{
    Q_OBJECT

public:
    explicit ShadowHeadPhoto(QWidget *parent = 0);
    ~ShadowHeadPhoto();

    void setHead(const QString &headPath);

private:
    void init();

private:
    QLabel *_photoLab;
};

#endif // SHADOWHEADPHOTO_H
