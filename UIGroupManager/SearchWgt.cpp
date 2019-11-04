//
// Created by cc on 18-11-11.
//

#include "SearchWgt.h"
#include <QHBoxLayout>
#include <QEvent>
#include <QKeyEvent>

SearchWgt::SearchWgt(QWidget* parent)
    : QFrame(parent)
{
    init();
}

SearchWgt::~SearchWgt()
{

}

void SearchWgt::init()
{
    setObjectName("searchWgt");
    setFixedHeight(30);
    _pBtn = new QToolButton(this);
    _pSearchEdit = new QLineEdit(this);
    _pBtn->setFixedWidth(20);

    _pSearchEdit->setPlaceholderText(tr("搜索"));

    _pBtn->setObjectName("SearchIcon");
    _pSearchEdit->setObjectName("SearchEdit");

    auto * layout = new QHBoxLayout(this);
    layout->setContentsMargins(5, 0, 5, 0);
    layout->setSpacing(5);
    layout->addWidget(_pBtn);
    layout->addWidget(_pSearchEdit);

    _pSearchEdit->installEventFilter(this);

    connect(_pSearchEdit, &QLineEdit::textChanged, [this](const QString& text){
        emit textChanged(text);
    });
}

bool SearchWgt::eventFilter(QObject *o, QEvent *e)
{
    if(o == _pSearchEdit && e->type() == QEvent::KeyPress)
    {
        QString strText = _pSearchEdit->text();

        if(strText.isEmpty())
        {
            //emit resetTree();
        }
        else
        {
//            QKeyEvent* evt = (QKeyEvent*)e;
//            if(evt && (evt->key() == Qt::Key_Enter || evt->key() == Qt::Key_Return))
//            {
//                emit textChanged(strText);
//            }
        }
    }
    return QFrame::eventFilter(o ,e);
}

void SearchWgt::clearText() {
    if(nullptr != _pSearchEdit)
    {
        _pSearchEdit->clear();
    }
}
