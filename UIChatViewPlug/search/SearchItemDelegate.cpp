//
// Created by QITMAC000260 on 2019-06-25.
//

#include "SearchItemDelegate.h"
#include "../../UICom/qimage/qimage.h"
#include <QPainter>
#include <QListView>

SortModel::SortModel(QObject* parent)
        : QSortFilterProxyModel(parent)
{

}

bool SortModel::lessThan(const QModelIndex &source_left, const QModelIndex &source_right) const {
    if (!source_left.isValid() || !source_right.isValid())
    return false;

    qint64 leftTime = source_left.data(EM_DATA_TYPE_LAST_UPDATE_TIME).toLongLong();
    qint64 rightTime = source_right.data(EM_DATA_TYPE_LAST_UPDATE_TIME).toLongLong();

    return leftTime < rightTime;
}

/**/
MessageALlDelegate::MessageALlDelegate(QWidget* parent)
    :QStyledItemDelegate(parent), parentWgt(parent)
{
    _nameFont.setWeight(400);
    _nameFont.setPixelSize(14);

    _contentFont.setWeight(400);
    _contentFont.setPixelSize(14);
}

MessageALlDelegate::~MessageALlDelegate() = default;


void MessageALlDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const {

    QStyledItemDelegate::paint(painter, option, index);
    painter->save();
    painter->setRenderHint(QPainter::TextAntialiasing);

    int itemType = index.data(EM_DATA_TYPE_ITEM_TYPE).toInt();


    if ((option.state & QStyle::State_Selected) > 0 && EM_ITEM_TYPE_TIME != itemType)
    {
        painter->fillRect(option.rect, QColor(242,242,242));
    }
    else
    {
        painter->fillRect(option.rect, QColor(255,255,255));
    }

    QRect rect = option.rect;

    QString content = index.data(EM_DATA_TYPE_CONTENT).toString();
    if(EM_ITEM_TYPE_TIME == itemType)
    {
        painter->drawText(rect, Qt::AlignCenter | Qt::AlignVCenter, content);
    }
    else
    {
        // name
        QString nameTime = index.data(EM_DATA_TYPE_NAME).toString();
        nameTime += "  ";
        nameTime += index.data(EM_DATA_TYPE_STR_TIME).toString();

        int dir = index.data(EM_DATA_TYPE_DIRECTION).toInt();
        painter->setFont(_nameFont);
        painter->setPen(dir ? QColor(0,202,190) : QColor(155,155,155));
        painter->drawText(QRect(rect.x() + 20, rect.y() + 5, rect.width() - 40, 18), nameTime);

        QRect conRect = QRect(rect.x() + 20, rect.y() + 30,
                              ALL_MAX_WIDTH, rect.height() - 35);

        switch (itemType)
        {
            case EM_ITEM_TYPE_TEXT:
            {
                painter->setFont(_contentFont);
                painter->setPen(QColor(51, 51, 51));
                painter->drawText(conRect, content);
                break;
            }
            case EM_ITEM_TYPE_FILE:
            {
                QString fileName = index.data(EM_DATA_TYPE_CONTENT).toString();
                QString fileSize = index.data(EM_DATA_TYPE_FILE_SIZE).toString();
                QString fileIcon = index.data(EM_DATA_TYPE_FILE_ICON).toString();
                //
                QRect fileContentRect = {conRect.x(), conRect.y(), conRect.width(), 60};
                painter->setPen(QColor(237,238,237));
                painter->drawRoundedRect(fileContentRect, 2, 2);
                //
                int dpi = QTalk::qimage::instance().dpi();
                QPixmap pixmap = QTalk::qimage::instance().loadPixmap(fileIcon, true, true, 40 * dpi);
                int w = pixmap.width() / dpi;
                int h = pixmap.height() / dpi;
                painter->drawPixmap((40 - w) / 2 + conRect.x() + 10,
                                    (40 - h) / 2 + conRect.y() + 10, w, h, pixmap);
                //
                painter->setPen(QColor(153,153,153));
                painter->drawText(fileContentRect.x() + 10 + 40 + 10, fileContentRect.bottom() - 10, fileSize);
                //
                painter->setPen(QColor(51, 51, 51));
                QRect textRect(conRect.x() + 10 + 40 + 10,
                               conRect.y() + 10, conRect.width() - (10 + 40 + 10 + 10), conRect.height());
                painter->drawText(textRect, content);
                break;
            }
            default:
                break;
        }
    }
    painter->restore();
}

QSize MessageALlDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const {
    auto size = QStyledItemDelegate::sizeHint(option, index);

    int height = index.data(EM_DATA_TYPE_CONTENT_HEIGHT).toInt();
    return {size.width(), height };
}

/**/
FileMessageDelegate::FileMessageDelegate(QWidget* parent)
        :QStyledItemDelegate(parent)
{

}

FileMessageDelegate::~FileMessageDelegate() = default;


void FileMessageDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const {

    QStyledItemDelegate::paint(painter, option, index);
    painter->save();
    painter->setRenderHint(QPainter::TextAntialiasing);

    painter->fillRect(option.rect, QColor(255,255,255));

    QRect rect = option.rect;
    int column = index.column();

    int itemType = index.data(EM_DATA_TYPE_ITEM_TYPE).toInt();
    QString content = index.data(EM_DATA_TYPE_CONTENT).toString();

    if(EM_ITEM_TYPE_TIME == itemType)
    {
        painter->drawText(rect, Qt::AlignCenter | Qt::AlignVCenter, content);
    }
    else
    {
        bool isHover = (option.state & QStyle::State_MouseOver);

        switch(column)
        {
            case EM_FILE_ITEM_COLUMN_0:
            {
                QString strName = index.data(EM_DATA_TYPE_NAME).toString();
                QString strTime = index.data(EM_DATA_TYPE_STR_TIME).toString();
                int dir = index.data(EM_DATA_TYPE_DIRECTION).toInt();
                QRect conRect = QRect(rect.x() + 20, rect.y() + 10,
                                      258, 60);

                QString fileName = index.data(EM_DATA_TYPE_CONTENT).toString();
                QString fileSize = index.data(EM_DATA_TYPE_FILE_SIZE).toString();
                QString fileIcon = index.data(EM_DATA_TYPE_FILE_ICON).toString();
                //
                painter->setPen(QColor(237,238,237));
                painter->drawRoundedRect(conRect, 2, 2);
                if(isHover)
                    painter->fillRect(conRect, QColor(242,242,242));
                //
                int dpi = QTalk::qimage::instance().dpi();
                QPixmap pixmap = QTalk::qimage::instance().loadPixmap(fileIcon, true, true, 40 * dpi);
                int w = pixmap.width() / dpi;
                int h = pixmap.height() / dpi;
                painter->drawPixmap((40 - w) / 2 + conRect.x() + 10,
                                    (40 - h) / 2 + conRect.y() + 10, w, h, pixmap);
                //
                painter->setPen(QColor(153,153,153));
                painter->drawText(conRect.x() + 10 + 40 + 10, conRect.bottom() - 10, fileSize);
                //
                painter->setPen(QColor(51, 51, 51));
                QRect textRect(conRect.x() + 10 + 40 + 10,
                               conRect.y() + 10, conRect.width() - (10 + 40 + 10 + 10), conRect.height());
                painter->drawText(textRect, content);

                break;
            }
            case EM_FILE_ITEM_COLUMN_1:
            {
                int status = index.data(EM_DADA_TYPE_FILE_STATUS).toInt();
                QRect conRect = QRect(rect.x() + 5, rect.y() + 10,
                                      26, 26);
//                if(EM_FILE_STATUS_UN_DOWNLOAD == status)
//                {
//                    painter->drawPixmap(conRect, isHover ? QPixmap(":/chatview/image1/messageItem/contentButtomFrmOPenFileBtn_hover.png") :
//                                                 QPixmap(":/chatview/image1/messageItem/contentButtomFrmOPenFileBtn.png"));
//                }
//                else if(EM_FILE_STATUS_DOWNLOADING == status)
//                {
//                    int process = index.data(EM_DADA_TYPE_FILE_PROCESS).toInt();
//                }
//                else if(EM_FILE_STATUS_DOWNLOADED == status)
//                {
//                    painter->drawPixmap(conRect, isHover ? QPixmap(":/chatview/image1/messageItem/contentButtomFrmDownLoadBtn_hover.png") :
//                                                 QPixmap(":/chatview/image1/messageItem/contentButtomFrmDownLoadBtn.png"));
//                }
//                else
//                {
//
//                }

                break;
            }
            case EM_FILE_ITEM_COLUMN_2:
            {
                QRect conRect = QRect(rect.x() + 5, rect.y() + 10,
                                      26, 26);
                painter->drawPixmap(conRect, isHover ? QPixmap(":/chatview/image1/messageItem/contentButtomFrmMenuBtn_hover.png") :
                                             QPixmap(":/chatview/image1/messageItem/contentButtomFrmMenuBtn.png"));
                break;
            }
            default:
                break;
        }
    }



//    else
//    {
//        // name
//
//
//
//
//        switch (itemType)
//        {
//            case EM_ITEM_TYPE_TEXT:
//            {
//                painter->setFont(_contentFont);
//                painter->setPen(QColor(51, 51, 51));
//                painter->drawText(conRect, content);
//                break;
//            }
//            case EM_ITEM_TYPE_FILE:
//            {
//
//                break;
//            }
//            default:
//                break;
//        }
//    }
    painter->restore();
}

QSize FileMessageDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const {

    int column = index.column();

    switch(column)
    {
        case EM_FILE_ITEM_COLUMN_0:
            return {20 + 258 + 5, 70};
        case EM_FILE_ITEM_COLUMN_1:
            return {5 + 26 + 5, 70};
        case EM_FILE_ITEM_COLUMN_2:
            return {5 + 26 + 10, 70};
        default:
            return {0, 70};
    }
}