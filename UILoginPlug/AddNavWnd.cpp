//
// Created by cc on 18-12-27.
//

#include "AddNavWnd.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QUrlQuery>
#include <QDesktopServices>
#include "../CustomUi/QtMessageBox.h"
#include "NavView.h"

AddNavWnd::AddNavWnd(NavView* view )
    : UShadowDialog(view, true), _pNavView(view)
{

    setFixedSize(400, 300);
    setObjectName("AddNavWnd");
    //
    QFrame* titleFrm = new QFrame(this);
    titleFrm->setObjectName("NavManager_TopFrm");
    auto* titleLay = new QHBoxLayout(titleFrm);
    QLabel* label = new QLabel(tr("新增导航地址"));
    label->setObjectName("NavManager_TitleLabel");
    titleLay->addWidget(label);
    //
    _pNameEdit = new QLineEdit(this);
    _pAddrEdit = new QTextEdit(this);
    _pAddrEdit->setAcceptRichText(false);
    //
    _pAddrEdit->setMaximumHeight(70);
    _pNameEdit->setObjectName("AddNavWnd_Name");
    _pAddrEdit->setObjectName("AddNavWnd_Addr");
    //
    QFrame* mainFrm = new QFrame(this);
    mainFrm->setObjectName("NavManager_MainFrm");
    auto* mainLay = new QGridLayout(mainFrm);
    mainLay->setMargin(20);
    mainLay->setHorizontalSpacing(15);
    mainLay->setVerticalSpacing(20);
    QLabel *nameLab = new QLabel(tr("名称"));
    QLabel *addrLab = new QLabel(tr("导航地址"));
    mainLay->addWidget(nameLab, 0, 0, Qt::AlignLeft);
    mainLay->addWidget(addrLab, 1, 0, Qt::AlignLeft | Qt::AlignTop);
    mainLay->addWidget(_pNameEdit, 0, 1, 1, 2);
    mainLay->addWidget(_pAddrEdit, 1, 1, 1, 2);
    //
    mainLay->addItem(new QSpacerItem(10, 10, QSizePolicy::Minimum, QSizePolicy::Expanding), 3, 0 ,1, 3);
    //
    _pCancelBtn = new QPushButton(tr("取消"));
    _pAddBtn = new QPushButton(tr("添加"));
    _pCancelBtn->setObjectName("AddNavWnd_cancel");
    _pCancelBtn->setFixedSize(70, 30);
    _pAddBtn->setObjectName("AddNavWnd_Add");
    _pAddBtn->setFixedSize(70, 30);
    auto* bottomLay = new QHBoxLayout;
    bottomLay->setSpacing(15);
    bottomLay->addItem(new QSpacerItem(10, 10, QSizePolicy::Expanding));
    bottomLay->addWidget(_pCancelBtn);
    bottomLay->addWidget(_pAddBtn);

    mainLay->addLayout(bottomLay, 4, 0, 1, 3);
    //
    auto* layout = new QVBoxLayout(_pCenternWgt);
    layout->setMargin(0);
    layout->setSpacing(0);
    layout->addWidget(titleFrm);
    layout->addWidget(mainFrm);
    //
    setMoverAble(true, titleFrm);

    connect(_pCancelBtn, &QPushButton::clicked, [this](){
        _evtRet = EM_NO;
        this->setVisible(false);
    });
    connect(_pAddBtn, &QPushButton::clicked, [this]() {

        QString strUrl = _pAddrEdit->toPlainText();
        QString strName = _pNameEdit->text();
        bool isDebug = false;

        QUrl qUrl(strUrl);
        QUrlQuery query(qUrl.query());
        if(query.hasQueryItem("debug"))
            isDebug = query.queryItemValue("debug") == "true";

        if(strName.trimmed().isEmpty())
        {
            QtMessageBox::warning(this, tr("警告"), tr("导航名称不能为空"));
            return;
        }
        if(strUrl.trimmed().isEmpty())
        {
            QtMessageBox::warning(this, tr("警告"), tr("导航地址不能为空"));
            return;
        }
        if(_pNavView && _pNavView->checkName(strName.trimmed()))
        {
            QtMessageBox::warning(this, tr("警告"), tr("该导航名称已存在"));
            return;
        }

        emit addNavSinal(strName, strUrl, isDebug);
        _evtRet = EM_YES;
        this->setVisible(false);
    });
}

AddNavWnd::~AddNavWnd()
{

}

void AddNavWnd::resetWnd()
{
    _pNameEdit->clear();
    _pAddrEdit->clear();
}
