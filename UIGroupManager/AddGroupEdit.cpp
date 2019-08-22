//
// Created by QITMAC000260 on 2019-03-22.
//

#include "AddGroupEdit.h"
#include <QKeyEvent>
#include <QApplication>
#include <QClipboard>
#include <QMimeData>

AddGroupEdit::AddGroupEdit(QWidget* parent)
    : QTextEdit(parent)
{
    setAcceptRichText(false);
    setPlaceholderText(tr("输入需要邀请的群成员ID (以 \";\" 分隔)"));
}

AddGroupEdit::~AddGroupEdit() = default;

void AddGroupEdit::keyPressEvent(QKeyEvent *e)
{
    if(e == QKeySequence::Paste ||
            (e->modifiers() == Qt::ControlModifier && e->key() == Qt::Key_V))
    {
        const QMimeData *mimeData = QApplication::clipboard()->mimeData();
        if (mimeData == nullptr) {
            return;
        }
        if (mimeData->hasText()) {
            QString plainText = mimeData->text();
            this->insertPlainText(plainText);
        }
        return;
    }
    QTextEdit::keyPressEvent(e);
}
