//
// Created by QITMAC000260 on 2019-04-16.
//

#ifndef QTALK_V2_SCANQRCODE_H
#define QTALK_V2_SCANQRCODE_H

#include <QFrame>
#include "../../qzxing/QZXing"

/**
* @description: ScanQRcode
* @author: cc
* @create: 2019-04-16 20:23
**/
class QRcode;
class ScanMainFrm;
class QLabel;
class ScanQRcode : public QFrame {
    Q_OBJECT
public:
    explicit ScanQRcode(QRcode* parent);
    ~ScanQRcode() override;

public:
    void scanPixmap(const QPixmap& pix, bool flag = false);

protected:
    bool event(QEvent* e) override;

private:
    void onScan();
    void onScanSuccess(const QString&);

Q_SIGNALS:
    void sgScanSuccess(const QString&);

private:
    ScanMainFrm* _pScanFrm;
    QTimer*      _pTimer;
    QZXing       _qzxing;

private:
    QRcode*      _pQRcode;
};


#endif //QTALK_V2_SCANQRCODE_H
