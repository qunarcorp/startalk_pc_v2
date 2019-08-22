#ifndef FLOATTEXTEDIT_H
#define FLOATTEXTEDIT_H

#include <QTextEdit>

class floatTextEdit : public QTextEdit
{
    Q_OBJECT

public:
    explicit floatTextEdit(QWidget *parent);
    ~floatTextEdit() override;

public:
    void InitWidget();
    bool ShowOrHideEditWidget(const QRect& rect ,const QPoint& p);
    void calculateWH(int& width, int& height);
    QPoint LeftTopPoint() {return posPoint;}
    void focusOutEvent(QFocusEvent *e) override;

public:
    void setColor(const QColor& color);

protected Q_SLOTS:
        void onTextChangeSlot();
signals:
        void sgTextInputComplete(const QString&);

private:
    static QString sStyle;
    static int wOffset;
    static int hOffset;
    QPoint posPoint;
    QRect  posRectLimited;

    QColor _color;
};

#endif // FLOATTEXTEDIT_H
