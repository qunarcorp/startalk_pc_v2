//
// Created by QITMAC000260 on 2019-04-16.
//
#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif
#ifndef QTALK_V2_QRCODE_H
#define QTALK_V2_QRCODE_H

#include <QStackedLayout>
#include "../../CustomUi/UShadowWnd.h"
#include "TipButton.h"

/**
* @description: QRcode
* @author: cc
* @create: 2019-04-16 20:20
**/
enum {
    em_wnd_invalid,
    em_wnd_scan,
    em_wnd_make
};

class ScanQRcode;
class MakeQRcode;
class QRcode : public UShadowDialog {
    Q_OBJECT
public:
    explicit QRcode(QWidget* parent = nullptr);
    ~QRcode() override;

public:
    void onChangeWnd(int id);

public:
    ScanQRcode* _pScanQRcode;
    MakeQRcode* _pMakeQRcode;
    TipButton* scanBtn;
    TipButton* makeBtn;
};


#endif //QTALK_V2_QRCODE_H
