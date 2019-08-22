#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif
#ifndef SEACHEDITPANEL_H
#define SEACHEDITPANEL_H

#include "SearchEdit.h"

#include <QFrame>
#include <QToolButton>
#include <qevent.h>


class Property
{
public:
    Property()
    {
        _searchFrmSize = QSize(198, 30);
        _searchBtnSize = QSize(20, 20);
    }
    QSize _searchFrmSize;
    QSize _searchBtnSize;
};


class SeachEditPanel : public QFrame
{
    Q_OBJECT
public:
    explicit SeachEditPanel(QWidget *parent = nullptr);
	~SeachEditPanel() override;
	//
    void closeSearch();

    void setEditFocus() {_searchEdt->setFocus();}

public slots:

    // QWidget interface
protected:
    void mouseReleaseEvent(QMouseEvent *event) override;
    bool eventFilter(QObject* o, QEvent* e) override;
    bool event(QEvent* e) override ;

signals:
    void sgStartSearch(const QString &value);
    void sgIsOpenSearch(const bool &isOpenSearch);
    void sgSelectUp();
    void sgSelectDown();
    void sgSelectItem();
    void sgKeyEsc();

private:
    void initPanel();

private:
    QToolButton * _searchBtn;
    QToolButton*  _clearBtn;
    Property _property;
    bool _isEditing;

public:
    SearchEdit * _searchEdt;
};

#endif // SEACHEDITPANEL_H
