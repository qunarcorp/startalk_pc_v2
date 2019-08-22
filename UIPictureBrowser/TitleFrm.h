//
// Created by QITMAC000260 on 2018/11/16.
//
#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif
#ifndef QTALK_V2_TITLEFRM_H
#define QTALK_V2_TITLEFRM_H

#include <QFrame>

class PictureBrowser;
class QToolButton;
class TitleFrm : public QFrame
{
public:
    explicit TitleFrm(PictureBrowser* pPicBrowser);
    ~TitleFrm() override;

protected:
    bool event(QEvent* e) override ;

private:
    void initUi();

public:
    void setBeforeBtnEnable(bool);
    void setNextBtnEnable(bool);

private:
    PictureBrowser* _pPicBrowser;

    QToolButton *_pTurnBeforeBtn;
    QToolButton *_pTurnNextBtn;
    QToolButton *_pEnlargeBtn;
    QToolButton *_pNarrowBtn;
    QToolButton *_pOne2OneBtn;
    QToolButton *_pRotateBtn;
    QToolButton *_pSaveAsBtn;
    QToolButton *_pMinBtn;
    QToolButton *_pMaxBtn;
    QToolButton *_pRestBtn;
    QToolButton *_pCloseBtn;
};


#endif //QTALK_V2_TITLEFRM_H
