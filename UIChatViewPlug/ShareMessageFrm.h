//
// Created by QITMAC000260 on 2019-04-11.
//

#ifndef QTALK_V2_SHAREMESSAGEFRM_H
#define QTALK_V2_SHAREMESSAGEFRM_H
#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif
#include <QFrame>
#include <QLabel>
#include <QToolButton>
#include <QVBoxLayout>

/**
* @description: ShareMessageFrm
* @author: cc
* @create: 2019-04-11 14:51
**/
class FunctionButton : public QFrame {
    Q_OBJECT
public:
    explicit FunctionButton(const QString& text, const QString& objectName, QWidget* parent = nullptr)
        : QFrame(parent)
    {
        auto * btn = new QToolButton(this);
        auto* label = new QLabel(text, this);

        btn->setObjectName(objectName);
        label->setObjectName("FunctionButtonLabel");
        auto* lay = new QVBoxLayout(this);
        lay->setSpacing(12);
        lay->setMargin(0);
        lay->addWidget(btn, 1);
        lay->addWidget(label, 0);
        lay->setAlignment(btn, Qt::AlignCenter);
        btn->setFixedSize(40, 40);
        setFixedSize(52, 70);
        connect(btn, &QToolButton::clicked, this, &FunctionButton::clicked);
    }
    ~FunctionButton() override = default;

signals:
    void clicked();

};

class ShareMessageFrm : public QFrame {
    Q_OBJECT
public:
    explicit ShareMessageFrm(QWidget* parent = nullptr);
    ~ShareMessageFrm() override;

public:
    void setSelectCount(unsigned int count);

signals:
    void sgSetShareMessageState(bool);
    void sgShareMessage();

private:
    QLabel* _pSelectCount;
    FunctionButton* _pShareBtn;
    QToolButton* _pCancelBtn;
};


#endif //QTALK_V2_SHAREMESSAGEFRM_H
