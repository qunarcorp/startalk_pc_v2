//
// Created by cc on 18-12-19.
//

#include "TextEdit.h"
#include <QTextBlock>

#define defaultLineHeight 2

TextEdit::TextEdit(QWidget* parent)
    :QTextEdit(parent)
    ,_lineHeight(0)
    ,_maxRowCount(999)
    ,_autoAdjustHeight(false)
{
    setAcceptRichText(false);
    setLineHeight(defaultLineHeight);
    setAlignment(Qt::AlignTop);
}

TextEdit::~TextEdit() {

}

/**
 *
 * @param rowCount
 */
void TextEdit::setRowCount(int rowCount)
{
    _maxRowCount = rowCount;
}

/**
 *
 * @param flags
 */
void TextEdit::setFlag(int flags)
{
    _flags = flags;
    setReadOnly(flags & EM_NO_EDIT);
    if(flags & EM_NO_BORDER)
        setFrameShape(QFrame::NoFrame);
    if(flags & EM_NO_H_SCROLLBAR)
        setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    if(flags & EM_NO_V_SCROLLBAR)
        setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    _autoAdjustHeight = (flags & EM_AUTO_JUST_HEIGHT);
}

/**
 *
 * @param text
 */
void TextEdit::setText(const QString& text)
{
    this->setReadOnly(false);
    QTextEdit::setText(text);
    if(_autoAdjustHeight)
    {
        adjustHeight();
    }
    this->setReadOnly(_flags & EM_NO_EDIT);
}

void TextEdit::adjustHeight()
{
    int lineCount = getLineCount();
    if(lineCount > _maxRowCount)
        lineCount = _maxRowCount;

    QFont fot = this->font();
    fot.letterSpacing();
    QFontMetricsF f(fot);
    setFixedHeight(f.height() * lineCount + _lineHeight * lineCount);
}

/**
 *
 * @param h
 */
void TextEdit::setLineHeight(int h)
{
    _lineHeight = h;
    QTextCursor textCursor = this->textCursor();
    QTextBlockFormat textBlockFormat;
    textBlockFormat.setLineHeight(h, QTextBlockFormat::FixedHeight);
    textCursor.setBlockFormat(textBlockFormat);
    this->setTextCursor(textCursor);
}

int TextEdit::getLineCount()
{
    this->adjustSize();
    int width = this->width();
    QString text = this->toPlainText();
    QFontMetricsF f(this->font());
    double w = f.width(text);
    int row = w / width;
    int tmp = (int)w % width;
    if(tmp > 0)
        row ++;

    return row;
}
