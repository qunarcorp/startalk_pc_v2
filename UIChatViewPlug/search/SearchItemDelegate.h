//
// Created by QITMAC000260 on 2019-06-25.
//

#ifndef QTALK_V2_SEARCHITEMDELEGATE_H
#define QTALK_V2_SEARCHITEMDELEGATE_H

#include <QSortFilterProxyModel>
#include <QStyledItemDelegate>

enum
{
    EM_DATA_TYPE_ITEM_TYPE = Qt::UserRole + 1,
    EM_DATA_TYPE_MESSAGE_ID,
    EM_DATA_TYPE_NAME,
    EM_DATA_TYPE_DIRECTION,
    EM_DATA_TYPE_CONTENT,
    EM_DATA_TYPE_CONTENT_HEIGHT,
    EM_DATA_TYPE_LAST_UPDATE_TIME,
    EM_DATA_TYPE_STR_TIME,

    EM_DATA_TYPE_FILE_SIZE,
    EM_DATA_TYPE_FILE_ICON,
    EM_DATA_TYPE_FILE_URL,
    EM_DATA_TYPE_FILE_PATH,
    EM_DADA_TYPE_FILE_STATUS,
    EM_DADA_TYPE_FILE_PROCESS,
    EM_DATA_TYPE_FILE_MD5,
};



enum
{
    EM_FILE_ITEM_COLUMN_0,
    EM_FILE_ITEM_COLUMN_1,
    EM_FILE_ITEM_COLUMN_2,
};

enum {
    EM_ITEM_TYPE_INVALID,
    EM_ITEM_TYPE_TIME,
    EM_ITEM_TYPE_TEXT,
    EM_ITEM_TYPE_IMAGE,
    EM_ITEM_TYPE_EMOTION,
    EM_ITEM_TYPE_FILE,
};

#define ALL_MAX_WIDTH 320

class SortModel : public QSortFilterProxyModel
{
public:
    explicit SortModel(QObject* parent = nullptr);

protected:
    bool lessThan(const QModelIndex &source_left, const QModelIndex &source_right) const override;
};


class MessageALlDelegate : public QStyledItemDelegate
{
public:
    explicit MessageALlDelegate(QWidget * parent = nullptr);
    ~MessageALlDelegate() override ;

protected:
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;

private:
    QFont _nameFont;
    QFont _contentFont;

private:
    QWidget* parentWgt;

};

class FileMessageDelegate : public QStyledItemDelegate
{
public:
    explicit FileMessageDelegate(QWidget * parent = nullptr);
    ~FileMessageDelegate() override ;

protected:
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;
};

#endif //QTALK_V2_SEARCHITEMDELEGATE_H
