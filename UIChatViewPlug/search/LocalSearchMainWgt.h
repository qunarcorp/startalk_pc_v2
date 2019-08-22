//
// Created by QITMAC000260 on 2019-06-25.
//

#ifndef QTALK_V2_LOCALSEARCHMAINWGT_H
#define QTALK_V2_LOCALSEARCHMAINWGT_H

#include "../../CustomUi/UShadowWnd.h"
#include "../CustomUi/SearchEdit.hpp"
#include <QStackedWidget>
#include <QListView>
#include <QTableView>
#include <QStandardItemModel>
#include <QListWidget>
#include <QCalendarWidget>
#include "SearchItemDelegate.h"
#include "../../entity/im_message.h"
#include "ItemWgt.h"
#include <QMutexLocker>
#include "../../include/STLazyQueue.h"

/**
* @description: SearchMainWgt
* @author: cc
* @create: 2019-06-25 11:02
**/
class LocalSearchMainWgt : public UShadowDialog {
    Q_OBJECT
public:
    explicit LocalSearchMainWgt(QWidget* parent = nullptr);
    ~LocalSearchMainWgt() override ;

public:
    void addMessageInfo(const QTalk::Entity::ImMessageInfo &msg);
    void addFileMessageInfo(const QTalk::Entity::ImMessageInfo &msg);
    void addSearchMessageInfo(const QTalk::Entity::ImMessageInfo &msg, const QString& key);
    void resetUi();
    void setHasBefore(int, bool);
    void recvFileProcess(const double &speed, const double &dtotal, const double &dnow, const double &utotal, const double &unow, const std::string &key);
    void initStyle(const QString& qss);

protected:
    bool eventFilter(QObject* o, QEvent* e) override ;

private:
    void initUi();
    void initList();
    void onAddAllItem(const StData &data);
    void onAddFileItem(const StData &data);
    void onAddFileTimeItem(const QString &data, qint64 );
    void onAddAllTimeItem(const QString &data, qint64);
    void onAddSearchItem(const StData &data);
    void onRecvFRileProcess(double speed, double dtotal, double dnow, double utotal, double unow, QString key);

Q_SIGNALS:
    void sgAddAllItem(const StData &data);
    void sgAddFileItem(const StData &data);
    void sgAddFileTimeItem(const QString &time, qint64 );
    void sgAddAllTimeItem(const QString &time, qint64);
    void sgGetBeforeAllMessage(qint64 time);
    void sgGetBeforeFileMessage(qint64 time);
    void sgSearch(qint64, const QString &);
    void sgAddSearchItem(const StData &data);
    void sgPositionMessage(qint64 );
    void sgRecvFRileProcess(double speed, double dtotal, double dnow, double utotal, double unow, QString key);
    void sgUpdateName(const QString& name);
    void sgInitStyle(const QString& qss);

private slots:
    void onButtonClicked(int id);
    void starSearch(qint64 ,const QString &);
    void onPositionMessage(qint64 );
    void onSetName(const QString& name);

private:
    QString creatTimeItem(int Type, qint64 time);

public:
    enum
    {
        EM_INVALID_,
        EM_ALL,
        EM_IMG,
        EM_FILE,
        EM_SEARCH
    };

private:
    QLabel* _pTitleLabel = nullptr;
    Search_Edit* _searEdit = nullptr;
    QPushButton* _pCalendarBtn = nullptr;
    QCalendarWidget* _calendarWgt = nullptr;
    QStackedWidget* _pStackedWidget = nullptr;

//    QListView* _pAllListView = nullptr; // 全部
//    QStandardItemModel* _pAllSrcModel = nullptr;
//    SortModel*          _pAllSortModel = nullptr;
//    MessageALlDelegate* _pAllDelegate = nullptr;
//
//    //
//    QTableView*          _pFileListView = nullptr; // 文件
//    QStandardItemModel* _pFileSrcModel = nullptr;
//    SortModel*          _pFileSortModel = nullptr;
//    FileMessageDelegate* _pFileDelegate = nullptr;

    QListWidget* _pAllListWgt;
    QListWidget* _pFileListWgt;
    QListWidget* _pSearchListWgt;

    QButtonGroup* _btnGroup;
    QFrame*       _btnFrm;
    QLabel       *_pSearchCountLabel;

    STLazyQueue<std::string> *_lazyQueue;

private:
    QMutex             _mutex;
    QMap<int, QVector<QString>> time_items;
    QMap<int, qint64 > _minTime;
    QMap<int, qint64 > _maxTime;
    QMap<int, QVector<std::string> > _msgIds;
    QMap<int, bool > _hasBefore;

    int  _searchCount = 0;

    QMap<QString, FileItemWgt*> _allFileItem;
    QMap<QString, FileItemWgt*> _fileItem;
};


#endif //QTALK_V2_LOCALSEARCHMAINWGT_H
