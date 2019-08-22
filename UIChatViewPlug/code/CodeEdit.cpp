//
// Created by QITMAC000260 on 2019-02-28.
//

#include "CodeEdit.h"
#include <QKeyEvent>
#include <QApplication>
#include <QMimeData>
#include <QClipboard>

CodeEdit::CodeEdit(QWidget* parent)
    : QTextEdit(parent)
{
    setPlaceholderText("请输入代码片段");
}

CodeEdit::~CodeEdit() {

}

/**
 *
 * @param e
 */
void CodeEdit::keyPressEvent(QKeyEvent *e)
{
    if(e == QKeySequence::Paste ||
       (e->modifiers() == Qt::ControlModifier && e->key() == Qt::Key_V))
    {
        e->accept();
        //
        const QMimeData *mimeData = QApplication::clipboard()->mimeData();
        if (mimeData == nullptr) {
            return;
        }

        // 暂时处理 以后提函数处理
        /*if (mimeData->hasUrls()) {
            bool isRet = false;

            QList<QUrl> urls = mimeData->urls();
            for (const QUrl &url : urls) {
				QString strUrl = url.toString();
                this->insertPlainText(url.toString());
                this->append("\n");

                if(!isRet)
                    isRet = true;
            }

            if (isRet) {
                QApplication::clipboard()->setMimeData(new QMimeData);
                return;
            }
        }*/

        if(mimeData->hasText())
        {
            this->insertPlainText(mimeData->text());
        }
    }
    else
        QTextEdit::keyPressEvent(e);
}
