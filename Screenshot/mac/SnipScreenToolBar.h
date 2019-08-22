#ifndef SNIPSCREEMTOOLBAR_H
#define SNIPSCREEMTOOLBAR_H

#include <QWidget>
#include "ui_snipscreentoolbar.h"

class SnipScreenToolBar : public QWidget
{
    Q_OBJECT
public:
    enum TOOLTYPE
    {
        SNIP_TOOLBAR_NONE,
        SNIP_TOOLBAR_RECT,
        SNIP_TOOLBAR_RING,
        SNIP_TOOLBAR_PEN,
        SNIP_TOOLBAR_TEXT,
        SNIP_TOOLBAR_ARROW
    };
public:
    explicit SnipScreenToolBar(QWidget *parent = nullptr);
    ~SnipScreenToolBar() override;
public:
    // 根据需要工具条的位置，判断子工具条的显示位置是上面还是下面
    void showSubToolBar();
    // 初始化工具条
    void initToolBar();

    void setDirect(int d) {m_nDirect=d;};
    int getHeight();
protected slots:
    //  五个编辑条的选取，被选中的按钮数要<= 0
    void onEditToolSelected(bool checked);
signals:
    void sgColorChange(const QString& colorString);
    void sgPenSizeChange(int penSize);
    void sgToolSelect(TOOLTYPE type);
    void sgUndo();
    void sgOk();
    void sgCancle();
protected:
    bool eventFilter(QObject *, QEvent *) override;
    void enterEvent(QEvent *) override;
protected:
    // ui update
    void setPenSizeStyle(QWidget* pw,bool bselected);
    void setPenColorStyle(QWidget* pRect,QLabel* pContent,QString contentColor,bool bSelected);
    //  笔触大小选择
    void onPenSizeChanged (QWidget* pwidget);
    // 笔触颜色
    void onPenColorHovered(QWidget* pwidget,bool hovered);
    void onPenColorSelected(QWidget* pwidget);

private:
    int m_nDirect;
    Ui::SnipScreenToolBar ui;
    QList<QWidget*> m_lstDownPenSize;
    QList<QWidget*> m_lstUpPenSize;

    QList<QWidget*> m_lstDownColorRect;
    QList<QWidget*> m_lstUpColorRect;
    QList<QLabel*> m_lstDownColor;
    QList<QLabel*> m_lstUpColor;

    QList<QString> m_lstColorValue;
    QList<int> m_lstPenSizeValue;

    int m_nCurPensize;
    QString m_strCurColor;
    TOOLTYPE m_tyCurrentType;
};

#endif // SNIPSCREEMTOOLBAR_H
