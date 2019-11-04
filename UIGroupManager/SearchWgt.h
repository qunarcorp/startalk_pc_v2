//
// Created by cc on 18-11-11.
//
#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif
#ifndef QTALK_V2_SEARCHWGT_H
#define QTALK_V2_SEARCHWGT_H

#include <QFrame>
#include <QLineEdit>
#include <QToolButton>
#include <QTimer>

class SearchWgt : public QFrame
{
    Q_OBJECT
public:
    explicit SearchWgt(QWidget* parent = nullptr);
    ~SearchWgt() override;

public:
    void clearText();

Q_SIGNALS:
    void textChanged(const QString&);

private:
    void init();
    bool eventFilter(QObject *o, QEvent *e) override;

private:
    QLineEdit* _pSearchEdit;
    QToolButton* _pBtn;
};


#endif //QTALK_V2_SEARCHWGT_H
