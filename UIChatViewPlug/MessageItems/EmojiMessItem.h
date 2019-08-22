#ifndef EMOJIMESSITEM_H
#define EMOJIMESSITEM_H

#include "MessageItemBase.h"
#include <QLabel>
#include <QTimer>

class EmojiMessItem : public MessageItemBase
{
    Q_OBJECT
public:
    EmojiMessItem(const QTalk::Entity::ImMessageInfo &msgInfo,
                  QString path,
                   const QSizeF& size,
                   QWidget *parent = nullptr);

    // MessageItemBase interface
public:
    QSize itemWdtSize() override;
    void onImageDownloaded(const QString& path);

    // QWidget interface
protected:
    void resizeEvent(QResizeEvent *event) override;
    bool event(QEvent* e) override ;

private:
    void init();
    void initLayout();
    void initSendLayout();
    void initReceiveLayout();
    void setImage();

private slots:
    void onMoveTimer();

signals:
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
    QSizeF   _size;
};

#endif // EMOJIMESSITEM_H
