//
// Created by cc on 19-1-6.
//

#ifndef QTALK_V2_SEARCHEDIT_HPP
#define QTALK_V2_SEARCHEDIT_HPP
#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif
#include <QLineEdit>
#include <QToolButton>
#include <QHBoxLayout>
#include "customui_global.h"

class CUSTOMUISHARED_EXPORT Search_Edit : public QFrame
{
    Q_OBJECT
public:
    Search_Edit(QWidget* parent = nullptr)
        :QFrame(parent)
    {
        //
        setObjectName("search");
        setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        setFixedHeight(40);
        //
        _pSearchBtn = new QToolButton(this);
        _pEdit = new QLineEdit(this);

        //
        QHBoxLayout* mainLay = new QHBoxLayout(this);
        mainLay->setMargin(0);
        mainLay->setSpacing(0);
        mainLay->addWidget(_pSearchBtn);
        mainLay->addWidget(_pEdit);
        //
        _pSearchBtn->setObjectName("searchBtn");
        _pSearchBtn->setFixedSize(20, 20);
        _pEdit->setObjectName("searchEdt");
        _pEdit->setPlaceholderText("搜索");

        connect(_pEdit, &QLineEdit::textChanged, this, &Search_Edit::textChanged);
        connect(_pEdit, &QLineEdit::textEdited, this, &Search_Edit::textEdited);
        connect(_pEdit, &QLineEdit::returnPressed, this, &Search_Edit::returnPressed);
    }

    //
    void clear()
    {
        if(_pEdit)
            _pEdit->clear();
    }

signals:
    void textChanged(const QString &);
    void textEdited(const QString &);
    void returnPressed();

private:
    QToolButton* _pSearchBtn;
    QLineEdit*    _pEdit;
};

#endif //QTALK_V2_SEARCHEDIT_HPP
