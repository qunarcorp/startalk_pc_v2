#include "ShadowHeadPhoto.h"
#include <QBitmap>
#include "HeadPhotoLab.h"
#include "../UICom/qimage/qimage.h"

ShadowHeadPhoto::ShadowHeadPhoto(QWidget *parent) :
    QFrame(parent)
{
    this->setFixedSize(180, 180);
    init();
}

ShadowHeadPhoto::~ShadowHeadPhoto()
{

}

/**
  * @函数名
  * @功能描述
  * @参数
  * @date 2018.10.10
  */
void ShadowHeadPhoto::setHead(const QString &headPath)
{
    QPixmap head = QTalk::qimage::instance().loadCirclePixmap(headPath, 52);
    _photoLab->setPixmap(head);
}

/**
  * @函数名
  * @功能描述
  * @参数
  * @date 2018.10.10
  */
void ShadowHeadPhoto::init()
{
    _photoLab = new QLabel(this);
    _photoLab->setGeometry(38, 38, 104, 104);
//    QPixmap pixmapBack = ":/head_mask";
//    _photoLab->setMask(pixmapBack.mask());
//    this->setStyleSheet("border-image:url(:/head_shadow)");
    QPixmap head_shadow;
    head_shadow.load(":/head_shadow");
    setAutoFillBackground(true);   // 这个属性一定要设置
    QPalette pal(palette());
    pal.setBrush(QPalette::Window, QBrush(head_shadow.scaled(size(), Qt::IgnoreAspectRatio,  Qt::SmoothTransformation)));
    setPalette(pal);
}
