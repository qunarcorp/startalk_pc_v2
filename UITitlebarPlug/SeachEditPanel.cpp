#include "SeachEditPanel.h"
#include "../QtUtil/Utils/Log.h"
#include "../UICom/StyleDefine.h"
#include <QStyle>
#include <QHBoxLayout>

SeachEditPanel::SeachEditPanel(QWidget *parent) : QFrame(parent),
    _isEditing(Q_NULLPTR),
    _searchEdt(nullptr),
    _searchBtn(nullptr)
{
    initPanel();
    this->setFocusProxy(_searchEdt);
}

SeachEditPanel::~SeachEditPanel()
= default;

/**
  * @函数名
  * @功能描述
  * @参数
  * @date 2018.11.08
  */
void SeachEditPanel::closeSearch()
{
    if (_searchEdt)
    {
        _searchEdt->_isEditing = false;
        _searchEdt->clear();
    }
}

/**
  * @函数名
  * @功能描述
  * @参数
  * @date 2018.11.06
  */
void SeachEditPanel::mouseReleaseEvent(QMouseEvent *event)
{
//    if (!_isEditing)
//    {
//        _isEditing = true;
//        emit sgIsOpenSearch(_isEditing);
//    }
    QFrame::mouseReleaseEvent(event);
}

/**
  * @函数名
  * @功能描述
  * @参数
  * @date 2018.11.06
  */
void SeachEditPanel::initPanel()
{
    _searchBtn = new QToolButton(this);
    _searchBtn->setObjectName(QStringLiteral("searchBtn"));
    _searchBtn->setFixedSize(_property._searchBtnSize);
    _searchEdt = new SearchEdit(this);
    _searchEdt->setPlaceholderText(tr("搜索"));
    _clearBtn = new QToolButton(this);
    _clearBtn->setObjectName("ClearBtn");
    auto * searchLay = new QHBoxLayout(this);
    searchLay->addWidget(_searchBtn);
    searchLay->addWidget(_searchEdt);
    searchLay->addWidget(_clearBtn);
    _clearBtn->setVisible(false);
    searchLay->setContentsMargins(8, 0, 8, 0);
    searchLay->setSpacing(0);
    _searchEdt->installEventFilter(this);
    connect(_searchEdt, &SearchEdit::sgIsOpenSearch, this, &SeachEditPanel::sgIsOpenSearch);
    connect(_searchEdt, &SearchEdit::textChanged, this, &SeachEditPanel::sgStartSearch);
    connect(_searchEdt, &SearchEdit::sgSelectUp, this, &SeachEditPanel::sgSelectUp);
    connect(_searchEdt, &SearchEdit::sgSelectDown, this, &SeachEditPanel::sgSelectDown);
    connect(_searchEdt, &SearchEdit::sgSelectItem, this, &SeachEditPanel::sgSelectItem);
    connect(_searchEdt, &SearchEdit::sgKeyEsc, this, &SeachEditPanel::sgKeyEsc);

    connect(_searchEdt, &QLineEdit::textChanged, [this](){
        _clearBtn->setVisible(!_searchEdt->text().isEmpty());
        _searchEdt->style()->polish(this);
    });
    connect(_clearBtn, &QToolButton::clicked, [this](){
        _searchEdt->clear();
    });
}

bool SeachEditPanel::eventFilter(QObject *o, QEvent *e) {

    if(o == _searchEdt)
    {
        if(e->type() == QEvent::FocusIn)
        {
            _clearBtn->setVisible(true);
            QColor selectColor = QTalk::StyleDefine::instance().getTitleSearchSelectColor();
            QString qss = QString("QFrame#searchFrm{background:rgba(%1, %2, %3, %4);"
                                  "border:1px solid rgba(0,202,190,1);}")
                                          .arg(selectColor.red()).arg(selectColor.green())
                                          .arg(selectColor.blue()).arg(selectColor.alphaF());
            this->setStyleSheet(qss);
            style()->polish(this);
        }
        else if(e->type() == QEvent::FocusOut)
        {
            _clearBtn->setVisible(false);
            QColor normalColor = QTalk::StyleDefine::instance().getTitleSearchNormalColor();
            QString qss = QString("QFrame#searchFrm{background:rgba%1, %2, %3, %4);};") .arg(normalColor.red()).arg(normalColor.green())
                    .arg(normalColor.blue()).arg(normalColor.alphaF());
            this->setStyleSheet(qss);
            style()->polish(this);
        }
    }
    return QObject::eventFilter(o, e);;
}

bool SeachEditPanel::event(QEvent *e) {
    if(e->type() == QEvent::FocusIn)
    {

    }
    return QFrame::event(e);
}