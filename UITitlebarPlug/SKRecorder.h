//
// Created by admin on 2019-02-25.
//
#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif
#ifndef QTALK_V2_SKRECORDER_H
#define QTALK_V2_SKRECORDER_H

#include <QFrame>
#include <QLabel>
#include <QPushButton>
#include <functional>

class SKRecorder : public QFrame {
public:
    typedef enum {
        SKRecorderState_Normal,
        SKRecorderState_Input,
        SKRecorderState_Delete,
    }SKRecorderState;

public:
    explicit SKRecorder(QWidget* parent = Q_NULLPTR, int fixedWidth = 120, int fixedHeight = 32);
    ~SKRecorder() override;
    void setKeyChangeEvent(std::function<void(const QKeySequence& keys)> callback);
    void setShorCut(const QKeySequence& keys);

protected:
    void focusInEvent(QFocusEvent *e) override;
    void focusOutEvent(QFocusEvent *e) override;
    void keyPressEvent(QKeyEvent *e) override;
    void keyReleaseEvent(QKeyEvent *e) override;

private:
    void initUI();
    void onBtnClick();
    void updateState();
    void updateKeyLabel();
    std::function<void(const QKeySequence& keys)> _keyChangeEvent;

private:
    QFrame *_contentFrame;
    QLabel *_keyLabel;
    QPushButton *_optionBtn;
    SKRecorderState _state;

    QKeySequence _keySequence;
    bool _changed;
};


#endif //QTALK_V2_SKRECORDER_H
