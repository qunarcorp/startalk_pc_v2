//
// Created by QITMAC000260 on 2019-01-27.
//

#include "QtMessageBox.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QKeyEvent>
#include <QApplication>
#include <QClipboard>
#include <QDesktopServices>

QtMessageBox::QtMessageBox(QWidget* parent, int type, const QString& message, const QString& subMessage, int buttons)
    : UShadowDialog(parent, true), _retButton(EM_BUTTON_INVALID)
{
    setFixedWidth(380);

    QLabel* pixLabel = new QLabel(this);
    mainMessageLabel = new QLabel(this);
    subMessageLabel = new QLabel(this);
    //
    pixLabel->setFixedSize(22, 22);
    mainMessageLabel->setAlignment(Qt::AlignTop);
    mainMessageLabel->setObjectName("MessageBoxMessage");
    subMessageLabel->setObjectName("MessageBoxSubMessage");
    subMessageLabel->setWordWrap(true);
    subMessageLabel->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);
    subMessageLabel->setAlignment(Qt::AlignTop);
    //
    auto * leftLay = new QVBoxLayout;
    leftLay->setMargin(0);
    leftLay->addWidget(pixLabel);
    leftLay->addItem(new QSpacerItem(1, 1, QSizePolicy::Fixed, QSizePolicy::Expanding));
    //
    auto *rightLay = new QVBoxLayout;
    rightLay->setMargin(0);
    rightLay->addWidget(mainMessageLabel);
    rightLay->addWidget(subMessageLabel);
    //
    auto *buttonLay = new QHBoxLayout;
    buttonLay->addItem(new QSpacerItem(1, 1, QSizePolicy::Expanding));
    if(buttons & EM_BUTTON_NO)
    {
        auto * noButton = new QPushButton(tr("取消"), this);
        noButton->setObjectName("NoButton");
        buttonLay->addWidget(noButton);
        noButton->setFixedSize(72, 32);
        connect(noButton, &QPushButton::clicked, [this](){
            _retButton = EM_BUTTON_NO;
            this->setVisible(false);
            _evtLoop->exit();
        });
    }
    if(buttons & EM_BUTTON_CANCEL_UPDATE)
    {
        auto * noButton = new QPushButton(tr("暂不更新"), this);
        noButton->setObjectName("NoButton");
        buttonLay->addWidget(noButton);
        noButton->setFixedSize(72, 32);
        connect(noButton, &QPushButton::clicked, [this](){
            _retButton = EM_BUTTON_CANCEL_UPDATE;
            this->setVisible(false);
            _evtLoop->exit();
        });
    }
    if(buttons & EM_BUTTON_YES)
    {
        auto *yesButton = new QPushButton(tr("确定"), this);
        yesButton->setObjectName("YesButton");
        buttonLay->addWidget(yesButton);
        yesButton->setFixedSize(72, 32);

        connect(yesButton, &QPushButton::clicked, [this](){
            _retButton = EM_BUTTON_YES;
            this->setVisible(false);
            _evtLoop->exit();
        });
    }
    if(buttons & EM_BUTTON_DELETE)
    {
        auto *deleteButton = new QPushButton(tr("删除"), this);
        deleteButton->setObjectName("DeleteButton");
        buttonLay->addWidget(deleteButton);
        deleteButton->setFixedSize(72, 32);
        connect(deleteButton, &QPushButton::clicked, [this](){
            _retButton = EM_BUTTON_DELETE;
            this->setVisible(false);
            _evtLoop->exit();
        });
    }
    rightLay->addLayout(buttonLay);

    QFrame* mainFrm = new QFrame(this);
    mainFrm->setObjectName("MessageBox");
    auto *mainLay = new QHBoxLayout(mainFrm);
    mainLay->setContentsMargins(32, 32, 32, 20);
    mainLay->setSpacing(16);
    mainLay->addLayout(leftLay);
    mainLay->addLayout(rightLay);

    auto *lay = new QHBoxLayout(_pCenternWgt);
    lay->addWidget(mainFrm);
    lay->setMargin(0);
    setMoverAble(true);
    _evtLoop = new QEventLoop(this);

    switch (type)
    {
        case EM_LEVEL_SUCCESS:
            pixLabel->setPixmap(QPixmap(":/success"));
            break;
        case EM_LEVEL_INFO:
            pixLabel->setPixmap(QPixmap(":/information"));
            break;
        case EM_LEVEL_QUESTION:
            pixLabel->setPixmap(QPixmap(":/question"));
            break;
        case EM_LEVEL_WARNING:
            pixLabel->setPixmap(QPixmap(":/warning"));
            break;
        case EM_LEVEL_ERROR:
            pixLabel->setPixmap(QPixmap(":/error"));
            break;
        default:
            break;
    }
    mainMessageLabel->setText(message);
    subMessageLabel->setText(subMessage);
#ifdef _MACOS
    macAdjustWindows();
#endif

    connect(subMessageLabel, &QLabel::linkActivated, [this](const QString& link){
        QDesktopServices::openUrl(QUrl(link));
        _retButton = EM_BUTTON_YES;
        this->setVisible(false);
        _evtLoop->exit();
    });
}

QtMessageBox::~QtMessageBox() = default;

int QtMessageBox::information(QWidget *parent, const QString &message, const QString &subMessage, int buttons) {

    auto* box = new QtMessageBox(parent, EM_LEVEL_INFO, message, subMessage, buttons);
    box->showModel();
    box->_evtLoop->exec();
    return box->_retButton;
}

int QtMessageBox::success(QWidget *parent, const QString &message, const QString &subMessage, int buttons) {
    auto* box = new QtMessageBox(parent, EM_LEVEL_SUCCESS, message, subMessage, buttons);
    box->showModel();
    return box->_retButton;
}

int QtMessageBox::question(QWidget *parent, const QString &message, const QString &subMessage, int buttons) {
    auto* box = new QtMessageBox(parent, EM_LEVEL_QUESTION, message, subMessage, buttons);
    box->showModel();
    box->_evtLoop->exec();
    return box->_retButton;
}

int QtMessageBox::warning(QWidget *parent, const QString &message, const QString &subMessage, int buttons) {
    auto* box = new QtMessageBox(parent, EM_LEVEL_WARNING, message, subMessage, buttons);
    box->showModel();
    box->_evtLoop->exec();
    return box->_retButton;
}

int QtMessageBox::error(QWidget *parent, const QString &message, const QString &subMessage, int buttons) {
    auto* box = new QtMessageBox(parent, EM_LEVEL_ERROR, message, subMessage, buttons);
    box->showModel();
    box->_evtLoop->exec();
    return box->_retButton;
}

void QtMessageBox::keyPressEvent(QKeyEvent *e)
{
    if(e == QKeySequence::Copy)
    {
        QString text = QString("%1 => %2").arg(mainMessageLabel->text()).arg(subMessageLabel->text());
        QApplication::clipboard()->setText(text);
    }
    else if(e->key() == Qt::Key_Enter || e->key() == Qt::Key_Return)
    {
        _retButton = EM_BUTTON_YES;
        this->setVisible(false);
        _evtLoop->exit();
    }
    else if(e->key() == Qt::Key_Escape || e->key() == Qt::Key_Space)
    {
        _retButton = EM_BUTTON_NO;
        this->setVisible(false);
        _evtLoop->exit();
    }
#ifdef _MACOS
    if(e->modifiers() == Qt::ControlModifier && e->key() == Qt::Key_W)
    {
        _retButton = EM_BUTTON_NO;
        this->setVisible(false);
        _evtLoop->exit();
    }
#endif
    UShadowDialog::keyPressEvent(e);
}
