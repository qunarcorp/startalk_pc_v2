#include <utility>

//
// Created by admin on 2019-02-25.
//

#include "SKRecorder.h"
#include <QGridLayout>
#include <QKeyEvent>
#include <QEvent>

SKRecorder::SKRecorder(QWidget* parent, int fixedWidth, int fixedHeight):QFrame(parent)
{
    _changed = false;
    _state = SKRecorderState_Normal;
    this->setObjectName("SKBgFrame");
    this->setFixedSize(fixedWidth,fixedHeight);
    this->setFocusPolicy(Qt::StrongFocus);
    this->initUI();
    this->updateState();
}

SKRecorder::~SKRecorder() {
    this->setFocusPolicy(Qt::NoFocus);
}

void SKRecorder::setShorCut(const QKeySequence& keys) {
    _keySequence = keys;
    this->updateState();
}

void SKRecorder::initUI() {

    auto *mainLay = new QGridLayout;
    mainLay->setSpacing(0);
    mainLay->setContentsMargins(2,0,0,0);
    mainLay->setAlignment(this,Qt::AlignCenter);
    this->setLayout(mainLay);

    _contentFrame = new QFrame(this);
    _contentFrame->setFixedSize(this->width()-4,this->height() - 6);
    _contentFrame->setObjectName("SKContentFrame");
    mainLay->addWidget(_contentFrame);

    auto *hLayout = new QHBoxLayout;
    hLayout->setMargin(0);
    hLayout->setSpacing(4);
    hLayout->setAlignment(this,Qt::AlignLeft);
    _contentFrame->setLayout(hLayout);

    _keyLabel = new QLabel;
    _keyLabel->setObjectName("SKKeyLabel");
    _keyLabel->setFixedSize(_contentFrame->width() - 24,_contentFrame->height());
    _keyLabel->setAlignment(Qt::AlignCenter);
    hLayout->addWidget(_keyLabel);

    QPixmap btnIcon( ":/title/image1/btn_sk_cancel.png");
    _optionBtn = new QPushButton;
    _optionBtn->setFocusPolicy(Qt::NoFocus);
    _optionBtn->setIcon(btnIcon);
    _optionBtn->setFixedSize(20,20);
    _optionBtn->setFlat(true);
    hLayout->addWidget(_optionBtn);

    connect(_optionBtn, &QPushButton::clicked, this, &SKRecorder::onBtnClick);

}

void SKRecorder::setKeyChangeEvent(std::function<void(const QKeySequence& keys)> callback) {
    this->_keyChangeEvent = std::move(callback);
}

void SKRecorder::focusInEvent(QFocusEvent *e) {
    this->_state = SKRecorderState_Input;
    this->updateState();
}

void SKRecorder::focusOutEvent(QFocusEvent *e) {
    this->_state = SKRecorderState_Normal;
    this->updateState();
}

void SKRecorder::keyPressEvent(QKeyEvent *e) {
    if (this->_state == SKRecorderState_Input) {
        if (e->type() == QEvent::KeyPress)
        {
            auto key = e->key();
            auto modifiers = e->modifiers();

            bool altKeyPress = (modifiers & Qt::AltModifier) == Qt::AltModifier;
            // mac controlPress command
            bool controlPress = (modifiers & Qt::ControlModifier) == Qt::ControlModifier;
            // mac metaPress control
            bool metaPress = (modifiers & Qt::MetaModifier) == Qt::MetaModifier;
            bool shift = (modifiers & Qt::ShiftModifier) == Qt::ShiftModifier;

            if((altKeyPress||controlPress||metaPress||shift) && key > 0 && key < 256){
                this->_state = SKRecorderState_Normal;
                _changed = true;
                _keySequence = QKeySequence(e->modifiers() | e->key());
                updateKeyLabel();
            }
        }
    }

    QFrame::keyPressEvent(e);
}

void SKRecorder::keyReleaseEvent(QKeyEvent *e) {
    if (e->type() == QEvent::KeyRelease) {

        //if (_changed) {
            _changed = false;
            this->_keyChangeEvent(_keySequence);
            this->updateState();
        //} else {
        //    _keyLabel->setText("设置快捷键");
        //}
    }
}



void SKRecorder::onBtnClick() {
    if (this->_state == SKRecorderState_Normal || this->_state == SKRecorderState_Delete) {
        _keySequence = QKeySequence(0 | -1);
        this->_state = SKRecorderState_Input;
    } else {
        this->_state = SKRecorderState_Delete;
    }
    this->updateState();
}

void SKRecorder::updateKeyLabel() {
    this->_keyLabel->setText(_keySequence.toString(QKeySequence::NativeText));
}

void SKRecorder::updateState() {
    switch (this->_state) {
        case SKRecorderState_Delete:
        case SKRecorderState_Normal:
        {
            if (this->_state == SKRecorderState_Normal) {
                this->setStyleSheet("QFrame#SKBgFrame {border-radius:4px;border:1px solid rgba(221,221,221,1);}");
            } else {
                this->setStyleSheet("QFrame#SKBgFrame {background:#6B97EE;border-radius:4px;}");
            }
            //
            _keyLabel->setStyleSheet("QFrame {font-size:12px;color:#212121;background:#FFFFFF;border-radius:"+QString::number(_contentFrame->height()/2.0)+"px;}");
            this->updateKeyLabel();
            QPixmap btnIcon( ":/title/image1/btn_sk_delete.png");
            _optionBtn->setIcon(btnIcon);
            _optionBtn->setHidden(false);
            _contentFrame->setStyleSheet("QFrame#SKContentFrame {background:#FFFFFF;border-radius:"+QString::number(_contentFrame->height()/2.0)+"px;}");
        }
            break;
        case SKRecorderState_Input:
        {
            this->setStyleSheet("QFrame#SKBgFrame {background:#6B97EE;border-radius:4px;}");
            _keyLabel->setText("设置快捷键");
            _keyLabel->setStyleSheet("QLabel#SKKeyLabel {font-size:12px;color:#bfbfbf;background:#FFFFFF;border-radius:"+QString::number(_contentFrame->height()/2.0)+"px;}");
            QPixmap btnIcon( ":/title/image1/btn_sk_cancel.png");
            _optionBtn->setHidden(false);
            _optionBtn->setIcon(btnIcon);
            _contentFrame->setStyleSheet("QFrame#SKContentFrame {background:#6B97EE;border-radius:"+QString::number(_contentFrame->height()/2.0)+"px;}");
        }
            break;
        default:
            break;
    }
}
