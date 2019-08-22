#ifndef SEARCHEDIT_H
#define SEARCHEDIT_H

#include <QLineEdit>



class SearchEdit : public QLineEdit
{
    Q_OBJECT
public:
    explicit SearchEdit(QWidget *parent = 0);
	~SearchEdit() override;

    // QWidget interface
protected:
//    void inputMethodEvent(QInputMethodEvent *event);
    void mousePressEvent(QMouseEvent *event) override;
    void keyPressEvent(QKeyEvent* e) override;

signals:
    void sgIsOpenSearch(const bool &isOpenSearch);
    void sgSelectUp();
    void sgSelectDown();
    void sgSelectItem();
    void sgKeyEsc();

public:
    bool _isEditing;
};

#endif // SEARCHEDIT_H
