#ifndef USHADOWDIALOG_H
#define USHADOWDIALOG_H

#include <QDialog>
#include <QEventLoop>
#include <QMap>
#include "customui_global.h"
#include "UShadowBorder.h"

#define DIALOG_SHAODW_SIZE 10

class UShadowDialogPrivate;
class CUSTOMUISHARED_EXPORT UShadowDialog : public QDialog
{
    Q_OBJECT
    Q_PROPERTY(bool resizable READ resizable WRITE setResizable)

public:
    explicit UShadowDialog(QWidget *parent = nullptr, bool radius = false, bool hasBorder = true);
    ~UShadowDialog() override;

public:
    void setResizable(bool);
    bool resizable() const;
    inline void setResizing(bool isResize) { _isResizing = isResize;}
    inline bool getResizing() { return _isResizing;}
    QRect realContentsRect();
    //
    int showModel();
    int showCenter(bool model, QWidget* parent);

public:
    void setMoverAble(bool moveAble, QWidget* moveContentWgt = nullptr);

public:
    inline int getEvtRet() { return _evtRet; }

protected:
    void changeEvent(QEvent * event) override;

    void mousePressEvent(QMouseEvent *e) override;

    void mouseReleaseEvent(QMouseEvent *e) override;

    void mouseMoveEvent(QMouseEvent *e) override;

    void closeEvent(QCloseEvent *e) override;

    void keyPressEvent(QKeyEvent* e) override;

    void paintEvent(QPaintEvent* e) override ;

#ifdef _MACOS
protected:
    void macAdjustWindows();
#endif

signals:
    void closeSignal();

public slots:

private:
    Q_DISABLE_COPY(UShadowDialog)

protected:
	int         _evtRet;
    bool        _hasBorder;

protected:
    bool       _isResizing;
    bool       _moveAble;
    bool       _movePressed;
    QWidget*   _pMoveContentWgt;
    QPoint     _moveStartPos;


protected:
    QWidget*    _pCenternWgt;
    QMap<int, UShadowBorder*> _borders;
};

#endif // USHADOWDIALOG_H
