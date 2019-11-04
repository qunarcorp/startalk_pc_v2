#ifndef STATICIMAGEMESSITEM_H
#define STATICIMAGEMESSITEM_H

#include "MessageItemBase.h"

#include <QLabel>
#include <QTimer>



class ImageMessItem : public MessageItemBase
{
    Q_OBJECT
public:
    explicit ImageMessItem(const QTalk::Entity::ImMessageInfo &msgInfo,
            const QString& link,
            const QString& path,
            const QSize& size,
            QWidget *parent = nullptr);
    ~ImageMessItem() override;
    // MessageItemBase interface
public:
    QSize itemWdtSize() override;
    void onImageDownloaded();

    // QWidget interface
protected:
    void resizeEvent(QResizeEvent *event) override;
    void mousePressEvent(QMouseEvent* e) override;
    bool event(QEvent* e) override ;

private:
    void init();
    void initLayout();
    void initSendLayout();
    void initReceiveLayout();
    void setImage();  

private slots:
    void onMoveTimer();

Q_SIGNALS:
    void sgItemChanged();

private:
    QLabel *_imageLab;
    QMovie *_movie;
    bool    _isGIF;

    QSize _headPixSize;
    QMargins _mainMargin;
    QMargins _leftMargin;
    QMargins _rightMargin;
    QMargins _contentMargin;
    QSize _sizeMaxPix;

    int _mainSpacing;
    int _leftSpacing;
    int _rightSpacing;
    int _contentSpacing;
    int _nameLabHeight;

    QTimer _moveTimer;

public:
    QString _imagePath;
    QString _imageLink;
    QSize   _size;
};

#endif // STATICIMAGEMESSITEM_H
