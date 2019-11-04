//
// Created by QITMAC000260 on 2019-02-28.
//

#include "SendCodeWnd.h"
#include "../ChatViewMainPanel.h"
#include "../../include/Line.h"
#include <QPushButton>
#include <QHBoxLayout>
#include "../../CustomUi/QtMessageBox.h"

extern ChatViewMainPanel *g_pMainPanel;
SendCodeWnd::SendCodeWnd(QWidget* parent)
    : UShadowDialog(parent, true)
{
    initUi();
}

SendCodeWnd::~SendCodeWnd()
{

}

/**
 * 初始化
 */
void SendCodeWnd::initUi()
{
    _pCodeEdit = new CodeEdit(this);
    _pCodeShell = new CodeShell(tr("发送代码片段"), _pCodeEdit, this);

    // bottom
    QPushButton* cancelBtn = new QPushButton(tr("取消"), this);
    QPushButton* sendBtn = new QPushButton(tr("发送"), this);

    cancelBtn->setObjectName("cancelBtn");
    sendBtn->setObjectName("sendBtn");

    auto* bottomFrm = new QFrame(this);
    auto * bottomLay = new QHBoxLayout(bottomFrm);
    bottomFrm->setObjectName("bodyFrm");
    bottomLay->addItem(new QSpacerItem(10, 10, QSizePolicy::Expanding));
    bottomLay->addWidget(cancelBtn);
    bottomLay->addWidget(sendBtn);

    _pCodeShell->getMainLay()->addWidget(new Line());
    _pCodeShell->getMainLay()->addWidget(bottomFrm);
    //
    setMoverAble(true, _pCodeShell->getTitleFrm());
    //
    auto* lay = new QHBoxLayout(_pCenternWgt);
    lay->setMargin(0);
    lay->addWidget(_pCodeShell);

    connect(cancelBtn, &QPushButton::clicked, [this](){setVisible(false);});
    connect(_pCodeShell, &CodeShell::closeWnd, [this](){
        _pCodeEdit->clear();
        setVisible(false);
    });

    connect(sendBtn, &QPushButton::clicked, this, &SendCodeWnd::sendCode);

#ifdef _MACOS
    setWindowFlags(this->windowFlags() | Qt::Tool);
#endif
}

/**
 *
 */
void SendCodeWnd::addCode(const QTalk::Entity::UID& uid, const QString& code)
{
    _uid = uid;
    if(_pCodeEdit)
    {
        _pCodeEdit->insertPlainText(code);
    }
}

/**
 *
 */
void SendCodeWnd::sendCode()
{
    if(_pCodeEdit && g_pMainPanel)
    {
        QString code = _pCodeEdit->toPlainText();

        if(code.toLocal8Bit().size() > 1024 * 64)
        {
            QtMessageBox::warning(g_pMainPanel, tr("警告"), tr("输入的代码过长，无法发送!"));
            return;
        }

        std::string codeStyle = _pCodeShell->getCodeStyle().toStdString();
        std::string codeLanguage = _pCodeShell->getCodeLanguage().toStdString();

        g_pMainPanel->sendCodeMessage(_uid, code.toStdString(), codeStyle, codeLanguage);

        _pCodeEdit->clear();
        this->setVisible(false);
    }
}
