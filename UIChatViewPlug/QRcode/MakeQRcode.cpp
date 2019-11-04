//
// Created by QITMAC000260 on 2019-04-16.
//

#include "MakeQRcode.h"
#include <QPushButton>
#include <QHBoxLayout>
#include <QTextEdit>
#include <QEvent>
#include <QMouseEvent>
#include <QApplication>
#include <QClipboard>
#include <QFileDialog>
#include <QFileInfo>
#include <QMimeData>
#include "TipButton.h"
#include "QRcode.h"
#include "../ChatViewMainPanel.h"
#include "../../include/Line.h"
#include "../../qzxing/QZXing"
#include "../../Platform/Platform.h"
#include "../../CustomUi/QtMessageBox.h"

extern ChatViewMainPanel *g_pMainPanel;
MakeQRcode::MakeQRcode(QRcode* parent)
    : QFrame(parent), _pQRcode(parent)
{
    //
    _pInputEdit = new QTextEdit(this);
    _pInputEdit->setAcceptRichText(false);
    _pInputEdit->setObjectName("QRCodeEdit");
    _pInputEdit->setPlaceholderText(tr("请输入链接"));
    _pInputEdit->setFixedHeight(68);
    auto* submitBtn = new QPushButton(tr("生成二维码"), this);
    submitBtn->setObjectName("makeQRCodeBtn");
    submitBtn->setFixedSize(94, 32);
    _pQRCodeLabel = new QLabel(this);
    _pQRCodeLabel->setFixedSize(140, 140);
    _pQRCodeLabel->installEventFilter(this);

    QFrame* qrCodeFrm = new QFrame(this);
    qrCodeFrm->setObjectName("QRCodeFrm");
    auto* qrCodeLay = new QHBoxLayout(qrCodeFrm);
    qrCodeLay->addWidget(_pQRCodeLabel);
    qrCodeFrm->setFixedSize(160, 160);
    qrCodeLay->setAlignment(_pQRCodeLabel, Qt::AlignCenter);

    auto* bodyFrm = new QFrame(this);
    auto* bodyLay = new QVBoxLayout(bodyFrm);
    bodyLay->setContentsMargins(30, 20, 30, 20);
    bodyLay->setSpacing(12);
    bodyLay->addWidget(_pInputEdit);
    bodyLay->addWidget(submitBtn);
    bodyLay->addWidget(qrCodeFrm);
    bodyLay->setAlignment(submitBtn, Qt::AlignRight);
    bodyLay->setAlignment(qrCodeFrm, Qt::AlignCenter);
    //
    auto* mainLay = new QVBoxLayout(this);
    mainLay->setMargin(0);
    mainLay->addWidget(new Line(Qt::Horizontal), 1);
    mainLay->addWidget(bodyFrm, 380);
    //
    _pMenu = new QMenu(this);
    _pMenu->setAttribute(Qt::WA_TranslucentBackground, true);
    auto* copyAct = new QAction(tr("拷贝"), _pMenu);
    auto* saveAct = new QAction(tr("保存"), _pMenu);
    _pMenu->addAction(copyAct);
    _pMenu->addAction(saveAct);
    //
    connect(submitBtn, &QPushButton::clicked, [this](){
        QString encodeStr = _pInputEdit->toPlainText();
        if(encodeStr.isEmpty())
            return;
        QImage img = QZXing::encodeData(encodeStr,
                                        QZXing::EncoderFormat_QR_CODE, {140, 140});

        if(!img.isNull())
        {
            _pixmap = QPixmap::fromImage(img);
            _pQRCodeLabel->setPixmap(_pixmap);
        }

    });

    connect(copyAct, &QAction::triggered, this, &MakeQRcode::onCopyAct);
    connect(saveAct, &QAction::triggered, this, &MakeQRcode::onSaveAct);
}

MakeQRcode::~MakeQRcode() {

}

void MakeQRcode::resetWnd()
{
    _pInputEdit->clear();
    _pixmap = QPixmap(":/chatview/image1/QRCode.png");
    _pQRCodeLabel->setPixmap(_pixmap);
}

bool MakeQRcode::event(QEvent *e) {
    if(e->type() == QEvent::Show)
    {
        resetWnd();
    }
    return QFrame::event(e);
}

bool MakeQRcode::eventFilter(QObject *o, QEvent *e) {
    if(o == _pQRCodeLabel && e->type() == QEvent::MouseButtonPress)
    {
        auto* evt = (QMouseEvent*)e;
        if(evt->button() == Qt::RightButton)
        {
            _pMenu->exec(QCursor::pos());
        }
    }
    return QObject::eventFilter(o, e);
}

void MakeQRcode::onCopyAct(bool)
{
	if (!_pixmap.isNull())
	{
		QString localPath = QString("%1/image/temp/").arg(Platform::instance().getAppdataRoamingUserPath().c_str());
		QString fileName = localPath + QDateTime::currentDateTime().toString("yyyyMMddhhmmsszzz.png");
		bool bret = _pixmap.save(fileName, "PNG");
		if (bret) {
			// 将截图放到 剪切板
			auto *mimeData = new QMimeData;
			mimeData->setUrls(QList<QUrl>() << QUrl::fromLocalFile(fileName));
			QApplication::clipboard()->setMimeData(mimeData);
		}
	}
}

void MakeQRcode::onSaveAct(bool)
{
    if(!_pixmap.isNull())
    {
        std::string histor = Platform::instance().getHistoryDir();
        QString fileName = QFileDialog::getSaveFileName(g_pMainPanel, tr("请选择需要保存的目录"), histor.data());
        if(!fileName.isEmpty())
        {
            QFileInfo info(fileName);
            QString suffix = info.suffix();
            if(suffix.isEmpty())
                fileName += ".png";
            else
                fileName += ".png";

            bool isOk = _pixmap.save(fileName, "png");
            if(isOk)
                QtMessageBox::information(g_pMainPanel, tr("提示"), QString(tr("文件已成功保存至: %1")).arg(fileName));
            else
                QtMessageBox::warning(g_pMainPanel, tr("错误"), tr("文件保存失败"));
        }
    }
}
