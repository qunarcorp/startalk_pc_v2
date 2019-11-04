//
// Created by QITMAC000260 on 2019-06-25.
//

#include "LocalSearchMainWgt.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QStackedWidget>
#include <QPushButton>
#include <QButtonGroup>
#include <QToolButton>
#include <QScrollBar>
#include <QDateTime>
#include <QFileInfo>
#include <QHeaderView>
#include <QEvent>
#include <QWheelEvent>
#include <ChatViewMainPanel.h>
#include "../../include/Line.h"
#include "../../QtUtil/Utils/Log.h"
#include "../../QtUtil/lib/cjson/cJSON.h"
#include "../../QtUtil/lib/cjson/cJSON_inc.h"
#include "../../Platform/Platform.h"

#define FILE_ICON_AUDIO ":/QTalk/image1/file_type/audio.png"
#define FILE_ICON_EXCEL ":/QTalk/image1/file_type/excel.png"
#define FILE_ICON_PDF ":/QTalk/image1/file_type/pdf.png"
#define FILE_ICON_PPT ":/QTalk/image1/file_type/ppt.png"
#define FILE_ICON_PROCESS ":/QTalk/image1/file_type/process.png"
#define FILE_ICON_TEXT ":/QTalk/image1/file_type/text.png"
#define FILE_ICON_UNKNOWN ":/QTalk/image1/file_type/unknown.png"
#define FILE_ICON_VIDEO ":/QTalk/image1/file_type/video.png"
#define FILE_ICON_WORD ":/QTalk/image1/file_type/word.png"
#define FILE_ICON_ZIP ":/QTalk/image1/file_type/zip.png"

extern ChatViewMainPanel *g_pMainPanel;

LocalSearchMainWgt::LocalSearchMainWgt(QWidget* parent)
    : UShadowDialog(parent, true)
    , _pAllListWgt(nullptr)
    , _pFileListWgt(nullptr)
    , _lazyQueue(nullptr)
{
   initUi();
   initList();

    std::function<int(STLazyQueue<std::string> *)> searchFun =

            [this](STLazyQueue<std::string> *queue) -> int {

                int runningCount = 0;
                if (queue != nullptr && !queue->empty()) {

                    std::string searKey = queue->tail();
                    while (!queue->empty()) {
                        queue->pop();
                        runningCount++;
                    }
                    emit sgSearch(0, searKey.data());
                }

                return runningCount;
            };

    _lazyQueue = new STLazyQueue<std::string>(200, searchFun);


}

LocalSearchMainWgt::~LocalSearchMainWgt() {

}

void LocalSearchMainWgt::initStyle(const QString& qss)
{
    Q_INIT_RESOURCE(image1);
    this->setStyleSheet(qss.toUtf8());
}

void LocalSearchMainWgt::initUi()
{
    setFixedWidth(360);
    setFixedHeight(600);
    // title
    QFrame* titleFrm = new QFrame(this);
    titleFrm->setObjectName("titleFrm");
    titleFrm->setFixedHeight(40);
    auto* titleLay = new QHBoxLayout(titleFrm);

    auto* closeBtn = new QToolButton();
    _pTitleLabel = new QLabel(this);
    _pTitleLabel->setObjectName("localSearchTitleLabel");
    _pTitleLabel->setAlignment(Qt::AlignCenter);
    //
#ifdef _MACOS
    closeBtn->setObjectName("gmCloseBtn");
    closeBtn->setFixedSize(12, 12);
    titleLay->addWidget(closeBtn);
    titleLay->addWidget(_pTitleLabel);
#else
    closeBtn->setFixedSize(18, 18);
    closeBtn->setObjectName("gwCloseBtn");
    titleLay->addWidget(_pTitleLabel);
    titleLay->addWidget(closeBtn);
#endif
    setMoverAble(true, titleFrm);
    //
    _searEdit = new Search_Edit(this);
    _searEdit->setFixedHeight(30);
    _pCalendarBtn = new QPushButton(this);
    _pCalendarBtn->setFixedSize(30, 30);
    _pCalendarBtn->setObjectName("CalendarBtn");
    _calendarWgt = new QCalendarWidget;
    _calendarWgt->setVisible(false);
    _pCalendarBtn->setVisible(false);
    QFrame* toolFrm = new QFrame(this);
    toolFrm->setObjectName("toolFrm");
    toolFrm->setFixedHeight(40);
    auto* toolLay = new QHBoxLayout(toolFrm);
    toolLay->setContentsMargins(14, 0, 14, 0);
    toolLay->addWidget(_searEdit, 1);
    toolLay->addWidget(_pCalendarBtn, 0);
    //
    auto* allBtn = new QPushButton(tr("全部"), this);
    auto* imgBtn = new QPushButton(tr("图片"), this);
    auto* fileBtn = new QPushButton(tr("文件"), this);
    allBtn->setObjectName("SearchWgtBtn");
    imgBtn->setObjectName("SearchWgtBtn");
    fileBtn->setObjectName("SearchWgtBtn");
    allBtn->setCheckable(true);
    imgBtn->setCheckable(true);
    fileBtn->setCheckable(true);
    _btnGroup = new QButtonGroup(this);
    _btnGroup->addButton(allBtn, EM_ALL);
    _btnGroup->addButton(imgBtn, EM_IMG);
    _btnGroup->addButton(fileBtn, EM_FILE);
    allBtn->setChecked(true);
    _btnFrm = new QFrame(this);
    _btnFrm->setObjectName("btnFrm");
    auto* btnLay = new QHBoxLayout(_btnFrm);
    btnLay->setSpacing(30);
    btnLay->addItem(new QSpacerItem(10, 10, QSizePolicy::Expanding));
    btnLay->addWidget(allBtn);
    btnLay->addWidget(imgBtn);
    btnLay->addWidget(fileBtn);
    btnLay->addItem(new QSpacerItem(10, 10, QSizePolicy::Expanding));
    imgBtn->setVisible(false);
    //
    _pSearchCountLabel = new QLabel(this);
    _pSearchCountLabel->setVisible(false);
    _pSearchCountLabel->setFixedHeight(46);
    _pSearchCountLabel->setAlignment(Qt::AlignCenter | Qt::AlignVCenter);
    //
    _pStackedWidget = new QStackedWidget(this);
    QFrame* bodyFrm = new QFrame(this);
    bodyFrm->setObjectName("LocalSearchBodyFrm");
    auto* bodyLay = new QHBoxLayout(bodyFrm);
    bodyLay->setContentsMargins(0, 0, 0, 10);
    bodyLay->addWidget(_pStackedWidget);
    //
    QFrame* mainFrm = new QFrame(this);
    mainFrm->setObjectName("LocalSearchMainFrm");
    auto* mainLay = new QVBoxLayout(mainFrm);
    mainLay->setMargin(0);
    mainLay->setSpacing(0);
    mainLay->addWidget(titleFrm, 0);
    mainLay->addWidget(toolFrm, 0);
    mainLay->addWidget(_btnFrm, 0);
    mainLay->addWidget(_pSearchCountLabel, 0);
    mainLay->addWidget(new Line);
    mainLay->addWidget(bodyFrm, 1);
    //
    auto* lay = new QVBoxLayout(_pCenternWgt);
    lay->setMargin(0);
    lay->setSpacing(0);
    lay->addWidget(mainFrm);
    //
    connect(closeBtn, &QToolButton::clicked, [this](){
        setVisible(false);
    });
    //
    connect(_btnGroup, SIGNAL(buttonClicked(int)), this, SLOT(onButtonClicked(int)));
    qRegisterMetaType<StData>("StData");
    connect(this, &LocalSearchMainWgt::sgAddAllItem, this, &LocalSearchMainWgt::onAddAllItem);
    connect(this, &LocalSearchMainWgt::sgAddFileItem, this, &LocalSearchMainWgt::onAddFileItem);
    connect(this, &LocalSearchMainWgt::sgAddFileTimeItem, this, &LocalSearchMainWgt::onAddFileTimeItem);
    connect(this, &LocalSearchMainWgt::sgAddAllTimeItem, this, &LocalSearchMainWgt::onAddAllTimeItem);
    connect(this, &LocalSearchMainWgt::sgAddSearchItem, this, &LocalSearchMainWgt::onAddSearchItem);
    //
    connect(_searEdit, &Search_Edit::textChanged, [this](const QString& text){
            _lazyQueue->push(text.toLower().toStdString());
    });

    connect(this, &LocalSearchMainWgt::sgSearch, this, &LocalSearchMainWgt::starSearch);
    connect(this, &LocalSearchMainWgt::sgRecvFRileProcess, this, &LocalSearchMainWgt::onRecvFRileProcess);

//    connect(_pCalendarBtn, &QPushButton::clicked, [this](){
//        if(_calendarWgt)
//        {
//            QPoint pos = this->geometry().topRight();
//            _calendarWgt->setVisible(true);
//            _calendarWgt->move(pos.x() - _calendarWgt->width() - 20, pos.y() + 100);
//        }
//    });
    connect(this, &LocalSearchMainWgt::sgUpdateName, this, &LocalSearchMainWgt::onSetName, Qt::QueuedConnection);
    connect(this, &LocalSearchMainWgt::sgInitStyle, this, &LocalSearchMainWgt::initStyle, Qt::QueuedConnection);
#ifdef _MACOS
    macAdjustWindows();
#endif
}

int dealWheel(QListWidget *wgt, QWheelEvent* evt)
{
    static qint64 _historyTime = 0;
    qint64 curTime = QDateTime::currentMSecsSinceEpoch();
    if(curTime - _historyTime < 500)
    {
        return 0;
    }
    else
        _historyTime = curTime;

    int scroBarval = wgt->verticalScrollBar()->value();
    int maximum = wgt->verticalScrollBar()->maximum();
    int direct = evt->angleDelta().y();

    if(scroBarval < 5 && direct > 0)
    {
        return 1;
    }
    else if(scroBarval > maximum -5 && direct < 0)
    {
        return 2;
    }
    return 0;
}

/**
 *
 * @param o
 * @param e
 * @return
 */
bool LocalSearchMainWgt::eventFilter(QObject *o, QEvent *e) {

    if(o == _pAllListWgt)
    {
        if(QEvent::Wheel == e->type())
        {
            auto* evt = (QWheelEvent*)e;
            QMutexLocker locker(&_mutex);
            int ret = dealWheel(_pAllListWgt, evt);
            if((ret == 1) && _hasBefore[EM_ALL])
                emit sgGetBeforeAllMessage(_minTime[EM_ALL]);
            else if(ret == 2)
                emit sgPositionMessage(_maxTime[EM_ALL]);
        }
    }
    else if(o == _pFileListWgt)
    {
        if(QEvent::Wheel == e->type())
        {
            auto* evt = (QWheelEvent*)e;
            QMutexLocker locker(&_mutex);
            if(dealWheel(_pFileListWgt, evt) && _hasBefore[EM_FILE])
                emit sgGetBeforeFileMessage(_minTime[EM_FILE]);
        }
    }
    return QObject::eventFilter(o, e);
}

void LocalSearchMainWgt::setHasBefore(int type, bool hasBefore)
{
    QMutexLocker locker(&_mutex);
    _hasBefore[type] = hasBefore;
}

/**
 *
 */
void LocalSearchMainWgt::initList()
{
//    // all
//    _pAllListView = new QListView(this);
//    _pAllSrcModel = new QStandardItemModel(this);
//    _pAllSortModel = new SortModel(this);
//    _pAllDelegate = new MessageALlDelegate(_pAllListView);
//    _pAllSortModel->setSourceModel(_pAllSrcModel);
//    _pAllListView->setModel(_pAllSortModel);
//    _pAllListView->setItemDelegate(_pAllDelegate);
//    _pAllListView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
//    _pAllListView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
//    _pAllListView->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
//    _pAllListView->setEditTriggers(QAbstractItemView::NoEditTriggers);
//    _pAllListView->verticalScrollBar()->setSingleStep(10);
//    _pAllListView->setFrameShape(QFrame::NoFrame);
//    _pAllListView->setAutoScroll(false);
//    _pStackedWidget->addWidget(_pAllListView);
//    _pStackedWidget->setCurrentWidget(_pAllListView);
//    // file
//    _pFileListView = new QTableView(this);
//    _pFileSrcModel = new QStandardItemModel(this);
//    _pFileSortModel = new SortModel(this);
//    _pFileDelegate = new FileMessageDelegate(_pFileListView);
//    _pFileSortModel->setSourceModel(_pFileSrcModel);
//    _pFileListView->setModel(_pFileSortModel);
//    _pFileListView->setItemDelegate(_pFileDelegate);
//    _pFileListView->horizontalHeader()->setVisible(false);
//    _pFileListView->verticalHeader()->setVisible(false);
//    _pFileListView->setShowGrid(false);
//    _pFileListView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
//    _pFileListView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
//    _pFileListView->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
//    _pFileListView->setEditTriggers(QAbstractItemView::NoEditTriggers);
//    _pFileListView->verticalScrollBar()->setSingleStep(10);
//    _pFileListView->setFrameShape(QFrame::NoFrame);
//    _pFileListView->setAutoScroll(false);
//    _pStackedWidget->addWidget(_pFileListView);
//
//    _pFileListView->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
//    _pFileListView->setColumnWidth(0, 20 + 258 + 5);
//    _pFileListView->setColumnWidth(1, 5 + 26 + 5);
//    _pFileListView->setColumnWidth(2, 5 + 26 + 10);
//
    _pAllListWgt = new QListWidget(this);
    _pAllListWgt->setObjectName("AllListWgt");
    _pAllListWgt->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    _pAllListWgt->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    _pAllListWgt->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    _pAllListWgt->setEditTriggers(QAbstractItemView::NoEditTriggers);
    _pAllListWgt->verticalScrollBar()->setSingleStep(10);
    _pAllListWgt->setFrameShape(QFrame::NoFrame);
    _pAllListWgt->setAutoScroll(false);
    _pStackedWidget->addWidget(_pAllListWgt);
    _pStackedWidget->setCurrentWidget(_pAllListWgt);

    _pFileListWgt = new QListWidget(this);
    _pFileListWgt->setObjectName("FileListWgt");
    _pFileListWgt->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    _pFileListWgt->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    _pFileListWgt->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    _pFileListWgt->setEditTriggers(QAbstractItemView::NoEditTriggers);
    _pFileListWgt->verticalScrollBar()->setSingleStep(10);
    _pFileListWgt->setFrameShape(QFrame::NoFrame);
    _pFileListWgt->setAutoScroll(false);
    _pStackedWidget->addWidget(_pFileListWgt);

    _pSearchListWgt = new QListWidget(this);
    _pSearchListWgt->setObjectName("SearchListWgt");
    _pSearchListWgt->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    _pSearchListWgt->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    _pSearchListWgt->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    _pSearchListWgt->setEditTriggers(QAbstractItemView::NoEditTriggers);
    _pSearchListWgt->verticalScrollBar()->setSingleStep(10);
    _pSearchListWgt->setFrameShape(QFrame::NoFrame);
    _pSearchListWgt->setAutoScroll(false);
    _pStackedWidget->addWidget(_pSearchListWgt);

    // event
    _pAllListWgt->installEventFilter(this);
    _pFileListWgt->installEventFilter(this);
    _pSearchListWgt->installEventFilter(this);
}

void LocalSearchMainWgt::onButtonClicked(int id)
{
    switch(id)
    {
        case EM_ALL:
            _pStackedWidget->setCurrentWidget(_pAllListWgt);
            break;
        case EM_IMG:
            break;
        case EM_FILE:
            _pStackedWidget->setCurrentWidget(_pFileListWgt);
            break;
        case EM_SEARCH:
            break;
        default:
            break;
    }
}

QString analysisMessage(const QString& content)
{
    QString tmpContent = content;
    QRegExp regExp("\\[obj type=[\\\\]?\"([^\"]*)[\\\\]?\" value=[\\\\]?\"([^\"]*)[\\\\]?\"(.*)\\]");
    regExp.setMinimal(true);

    int pos = 0;
    while ((pos = regExp.indexIn(tmpContent)) != -1) {
        QString item = regExp.cap(0); // 符合条件的整个字符串
        QString type = regExp.cap(1); // 多媒体类型
        QString val = regExp.cap(2);

        if ("url" == type) {
            tmpContent.replace(pos, item.size(), val);
        } else if ("image" == type) {
            tmpContent.replace(pos, item.size(), QObject::tr("[图片]"));
        } else if ("emoticon" == type) {
            tmpContent.replace(pos, item.size(), QObject::tr("[表情]"));
        } else {
            tmpContent.replace(pos, item.size(), QObject::tr("[未知类型]"));
        }
    }

    return tmpContent;
}

int getMessageHeight(const QString& content, int maxWidth)
{
    int ret = 10 + 15 + 10 + 10;

    if(!content.isEmpty())
    {
        QFont font;
        font.setWeight(400);
        font.setPixelSize(14);
        font.setLetterSpacing(QFont::AbsoluteSpacing, 0);

        QLabel* label = new QLabel;
        QTextOption textOption(Qt::AlignLeft | Qt::AlignTop);
        textOption.setWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
        label->setFixedWidth(maxWidth - 10);
        label->setFont(font);
        label->setWordWrap(true);
        label->setText(content);
        label->adjustSize();
        ret += label->height();
        delete  label;
    }

    return ret;
}

QString getFileIcon(const QString& suffix) {
    QString fileSuffix = suffix.toLower();

    if ("mp3" == fileSuffix)
        return FILE_ICON_AUDIO;
    else if ("txt" == fileSuffix)
        return FILE_ICON_TEXT;
    else if ("pdf" == fileSuffix)
        return FILE_ICON_PDF;
    else if ("ppt" == fileSuffix)
        return FILE_ICON_PPT;
    else if ("pptx" == fileSuffix)
        return FILE_ICON_PPT;
    else if ("doc" == fileSuffix)
        return FILE_ICON_WORD;
    else if ("docx" == fileSuffix)
        return FILE_ICON_WORD;
    else if ("xls" == fileSuffix)
        return FILE_ICON_EXCEL;
    else if ("xlsx" == fileSuffix)
        return FILE_ICON_EXCEL;
    else if ("rar" == fileSuffix)
        return FILE_ICON_ZIP;
    else if ("zip" == fileSuffix)
        return FILE_ICON_ZIP;
    else if ("json" == fileSuffix)
        return FILE_ICON_TEXT;
    else
        return FILE_ICON_UNKNOWN;
}


QString getLocalFilePath(const QTalk::Entity::ImMessageInfo &msgInfo)
{
    QString fullFileName = QString::fromStdString(g_pMainPanel->getFileMsgLocalPath(msgInfo.MsgId));
    if(fullFileName.isEmpty() || !QFile::exists(fullFileName))
    {
        QString linkFile = g_pMainPanel->getFileLink(msgInfo.FileMd5.data());
#ifdef _WINDOWS
        linkFile.append(".lnk");
#endif
        QFileInfo linkFileInfo(linkFile);
        if (linkFileInfo.exists() && !linkFileInfo.canonicalFilePath().isEmpty())
#ifdef _WINDOWS
            return linkFileInfo.symLinkTarget();
#else
            return linkFileInfo.canonicalFilePath();
#endif // _WINDOWS
        else
            return QString();
    }
    else
    {
        return fullFileName;
    }
}

/**
 * @param msg
 */
void LocalSearchMainWgt::addMessageInfo(const QTalk::Entity::ImMessageInfo &msg)
{
    {
        QMutexLocker locker(&_mutex);
        _minTime[EM_ALL] = qMin(msg.LastUpdateTime, _minTime[EM_ALL]);
        _maxTime[EM_ALL] = qMax(msg.LastUpdateTime, _maxTime[EM_ALL]);

        if(_msgIds[EM_ALL].contains(msg.MsgId))
            return;
        else
            _msgIds[EM_ALL].push_back(msg.MsgId);
    }
    
    switch (msg.Type)
    {
        case QTalk::Entity::MessageTypeText:
        case QTalk::Entity::MessageTypeRobotAnswer:
        case QTalk::Entity::MessageTypeGroupAt:
        case QTalk::Entity::MessageTypeFile:
        {
            QString strTime = creatTimeItem(EM_ALL, msg.LastUpdateTime);
            if(!strTime.isEmpty()) {
                qint64 t = QDateTime::fromString(strTime, "yyyy-MM-dd").toMSecsSinceEpoch();
                emit sgAddAllTimeItem(strTime, t);
            }

            StData data;
            data.messageId = msg.MsgId.data();
            data.time = msg.LastUpdateTime;
            data.userName = msg.UserName.data();
            data.direction = msg.Direction;
            data.type = msg.Type;

            switch (msg.Type)
            {
                case QTalk::Entity::MessageTypeText:
                case QTalk::Entity::MessageTypeRobotAnswer:
                case QTalk::Entity::MessageTypeGroupAt:
                {
                    QString content = analysisMessage(QString::fromStdString(msg.Content));
                    data.content = content;

                    emit sgAddAllItem(data);

                    break;
                }
                case QTalk::Entity::MessageTypeFile:
                {
                    cJSON *content = cJSON_Parse(msg.Content.data());
                    if (nullptr == content) {
                        return;
                    }

                    std::string fileName = QTalk::JSON::cJSON_SafeGetStringValue(content, "FileName");
                    std::string fileSize = QTalk::JSON::cJSON_SafeGetStringValue(content, "FileSize");
                    std::string fileUrl = QTalk::JSON::cJSON_SafeGetStringValue(content, "HttpUrl");
                    std::string fileMd5 = QTalk::JSON::cJSON_SafeGetStringValue(content, "FILEMD5");
                    cJSON_Delete(content);
                    //
                    QFileInfo fileInfo(fileName.data());

                    data.fileData.fileSize = fileSize.data();
                    data.fileData.fileName = fileName.data();
                    data.fileData.fileUrl = fileUrl.data();
                    data.fileData.fileMd5 = fileMd5.data();
                    data.fileData.iconPath = getFileIcon(fileInfo.suffix().toLower());
                    QString localFilePath = getLocalFilePath(msg);
                    if(localFilePath.isEmpty())
                        data.fileData.status = EM_FILE_STATUS_UN_DOWNLOAD;
                    else
                    {
                        data.fileData.status = EM_FILE_STATUS_DOWNLOADED;
                        data.fileData.filePath = localFilePath;
                    }

                    emit sgAddAllItem(data);

                    break;
                }
            }
//            _pAllSortModel->sort(0);
            break;
        }
        default:
            break;
    }
}

QString LocalSearchMainWgt::creatTimeItem(int type, qint64 time)
{
    QString strTime = QDateTime::fromMSecsSinceEpoch(time).toString("yyyy-MM-dd");

    QMutexLocker locker(&_mutex);
    if(time_items[type].contains(strTime))
        return QString();
    else {
        time_items[type].push_back(strTime);
        return strTime;
    }
}

/**
 *
 */
void LocalSearchMainWgt::resetUi()
{
    _pAllListWgt->clear();
    _pFileListWgt->clear();

    QMutexLocker locker(&_mutex);
    time_items.clear();
    _msgIds.clear();
    //
    _minTime[EM_ALL] = LLONG_MAX;
    _minTime[EM_FILE] = LLONG_MAX;
    _maxTime[EM_ALL] = 0;
    _maxTime[EM_FILE] = 0;

    _allFileItem.clear();
    _fileItem.clear();

    _hasBefore[EM_ALL] = true;
    _hasBefore[EM_FILE] = true;
}


void LocalSearchMainWgt::addFileMessageInfo(const QTalk::Entity::ImMessageInfo &msg)
{
    {
        QMutexLocker locker(&_mutex);
        _minTime[EM_FILE] = qMin(msg.LastUpdateTime, _minTime[EM_FILE]);
        _maxTime[EM_FILE] = qMax(msg.LastUpdateTime, _maxTime[EM_FILE]);
        if(_msgIds[EM_FILE].contains(msg.MsgId))
            return;
        else
            _msgIds[EM_FILE].push_back(msg.MsgId);
    }

    if(msg.Type == QTalk::Entity::MessageTypeFile)
    {
        QString strTime = creatTimeItem(EM_FILE, msg.LastUpdateTime);
        if(!strTime.isEmpty()) {
            qint64 t = QDateTime::fromString(strTime, "yyyy-MM-dd").toMSecsSinceEpoch();
            emit sgAddFileTimeItem(strTime, t);
        }

        StData data;
        data.messageId = msg.MsgId.data();
        data.time = msg.LastUpdateTime;
        data.userName = msg.UserName.data();
        data.direction = msg.Direction;
        data.type = msg.Type;
        cJSON *content = cJSON_Parse(msg.Content.data());
        if (nullptr == content) {
            return;
        }

        std::string fileName = QTalk::JSON::cJSON_SafeGetStringValue(content, "FileName");
        std::string fileSize = QTalk::JSON::cJSON_SafeGetStringValue(content, "FileSize");
        std::string fileUrl = QTalk::JSON::cJSON_SafeGetStringValue(content, "HttpUrl");
        std::string fileMd5 = QTalk::JSON::cJSON_SafeGetStringValue(content, "FILEMD5");
        cJSON_Delete(content);
        //
        QFileInfo fileInfo(fileName.data());

        data.fileData.fileSize = fileSize.data();
        data.fileData.fileName = fileName.data();
        data.fileData.fileUrl = fileUrl.data();
        data.fileData.fileMd5 = fileMd5.data();
        data.fileData.iconPath = getFileIcon(fileInfo.suffix());
        QString localFilePath = getLocalFilePath(msg);
        if(localFilePath.isEmpty())
            data.fileData.status = EM_FILE_STATUS_UN_DOWNLOAD;
        else
        {
            data.fileData.status = EM_FILE_STATUS_DOWNLOADED;
            data.fileData.filePath = localFilePath;
        }

        emit sgAddFileItem(data);
    }

}

/**
 *
 * @param msg
 */
void LocalSearchMainWgt::addSearchMessageInfo(const QTalk::Entity::ImMessageInfo &msg, const QString& key)
{
    {
        QMutexLocker locker(&_mutex);
        _minTime[EM_SEARCH] = qMin(msg.LastUpdateTime, _minTime[EM_SEARCH]);
        _maxTime[EM_SEARCH] = qMax(msg.LastUpdateTime, _maxTime[EM_SEARCH]);

        if(_msgIds[EM_SEARCH].contains(msg.MsgId))
            return;
        else
            _msgIds[EM_SEARCH].push_back(msg.MsgId);
    }

    StData data;
    data.messageId = msg.MsgId.data();
    data.time = msg.LastUpdateTime;
    data.userName = msg.UserName.data();
    data.direction = msg.Direction;
    data.type = msg.Type;

    switch (msg.Type)
    {
        case QTalk::Entity::MessageTypeText:
        case QTalk::Entity::MessageTypeRobotAnswer:
        case QTalk::Entity::MessageTypeGroupAt:
        {
            QString content = analysisMessage(QString::fromStdString(msg.Content));
            if(!content.contains(key, Qt::CaseInsensitive))
            {
                return;
            }
            data.searchData.searchKey = key;

            QFontMetricsF f(this->font());
            if(f.width(content) > 260)
            {
                data.content = QString("... <span style='background:rgba(255,216,152,1);'>%1</span>").arg(key);
                data.content += f.elidedText(content.section(key, 1, 1, QString::SectionCaseInsensitiveSeps),
                        Qt::ElideRight, 260 - f.width(key));
            }
            else
            {
                data.content = QString("%1<span style='background:rgba(255,216,152,1);'>%2</span>%3")
                        .arg(content.section(key, 0, 0, QString::SectionCaseInsensitiveSeps), key,
                             content.section(key, 1, 1, QString::SectionCaseInsensitiveSeps));
            }

            emit sgAddSearchItem(data);

            break;
        }
        case QTalk::Entity::MessageTypeFile:
        {
            cJSON *content = cJSON_Parse(msg.Content.data());
            if (nullptr == content) {
                return;
            }

            std::string fileName = QTalk::JSON::cJSON_SafeGetStringValue(content, "FileName");
            std::string fileSize = QTalk::JSON::cJSON_SafeGetStringValue(content, "FileSize");
            std::string fileUrl = QTalk::JSON::cJSON_SafeGetStringValue(content, "HttpUrl");
            std::string fileMd5 = QTalk::JSON::cJSON_SafeGetStringValue(content, "FILEMD5");
            cJSON_Delete(content);

            if(!QString::fromStdString(fileName).contains(key, Qt::CaseInsensitive))
            {
                return;
            }

            QString qFileName(fileName.data());
            QFontMetricsF f(this->font());
            if(f.width(qFileName) > 280)
            {
                data.fileData.fileName = QString("... <span style='background:rgba(255,216,152,1);'>%1</span>").arg(key);
                data.fileData.fileName += f.elidedText(qFileName.section(key, 1, 1, QString::SectionCaseInsensitiveSeps),
                                             Qt::ElideRight, 280 - f.width(key));
            }
            else
            {
                data.fileData.fileName = QString("%1<span style='background:rgba(255,216,152,1);'>%2</span>%3")
                        .arg(qFileName.section(key, 0, 0, QString::SectionCaseInsensitiveSeps), key,
                             qFileName.section(key, 1, 1, QString::SectionCaseInsensitiveSeps));
            }

            //
            QFileInfo fileInfo(fileName.data());
            data.fileData.fileSize = fileSize.data();
            data.fileData.fileMd5 = fileMd5.data();
            data.fileData.iconPath = getFileIcon(fileInfo.suffix());

            emit sgAddSearchItem(data);

            break;
        }
        default:
            break;
    }
}

int getIndex(QListWidget* wgt, qint64 time)
{
    int index = 0;
    for(index = 0; index < wgt->count(); index++)
    {
        qint64 t = wgt->item(index)->data(Qt::UserRole).toLongLong();
        if(time <= t)
            break;
    }

    return index;
}



void LocalSearchMainWgt::onAddAllItem(const StData &data)
{
    switch (data.type)
    {
        case QTalk::Entity::MessageTypeText:
        case QTalk::Entity::MessageTypeRobotAnswer:
        case QTalk::Entity::MessageTypeGroupAt:
        {
            auto* item = new QListWidgetItem;
            auto* textWgt = new TextItemWgt(data, this);
            item->setData(Qt::UserRole, data.time);

            int index = getIndex(_pAllListWgt, data.time);
            _pAllListWgt->insertItem(index, item);
            _pAllListWgt->setItemWidget(item, textWgt);
            item->setSizeHint({_pCenternWgt->width() - 50, getMessageHeight(data.content, 320)});

            _pAllListWgt->scrollToItem(item);
            break;
        }
        case QTalk::Entity::MessageTypeFile:
        {
            auto* item = new QListWidgetItem;
            auto* fileWgt = new FileItemWgt(data, true, this);
            item->setData(Qt::UserRole, data.time);

            int index = getIndex(_pAllListWgt, data.time);
            _pAllListWgt->insertItem(index, item);
            _pAllListWgt->setItemWidget(item, fileWgt);
            item->setSizeHint({_pCenternWgt->width() - 50, 95});

            _pAllListWgt->scrollToItem(item);

            _allFileItem[data.messageId] = fileWgt;
            break;
        }
        default:
            break;
    }
}

/**
 *
 * @param time
 */
void LocalSearchMainWgt::onAddAllTimeItem(const QString& strTime, qint64 time)
{
    auto* item = new QListWidgetItem;
    auto* timeItem = new TimeItemWgt(strTime, this);
    item->setData(Qt::UserRole, time);
    _pAllListWgt->insertItem(getIndex(_pAllListWgt, time), item);
    _pAllListWgt->setItemWidget(item, timeItem);
    item->setSizeHint({_pCenternWgt->width() - 50, 46});
}

void LocalSearchMainWgt::onAddFileItem(const StData &data)
{
    auto* item = new QListWidgetItem;
    auto* fileWgt = new FileItemWgt(data, false, this);
    item->setData(Qt::UserRole, data.time);

    _pFileListWgt->insertItem(getIndex(_pFileListWgt, data.time), item);
    _pFileListWgt->setItemWidget(item, fileWgt);
    item->setSizeHint({_pCenternWgt->width() - 50, 70});

    _fileItem[data.messageId] = fileWgt;
}

void LocalSearchMainWgt::onAddFileTimeItem(const QString& strTime, qint64 time)
{
    auto* item = new QListWidgetItem;
    auto* timeItem = new TimeItemWgt(strTime, this);
    item->setData(Qt::UserRole, time);
    _pFileListWgt->insertItem(getIndex(_pFileListWgt, time), item);
    _pFileListWgt->setItemWidget(item, timeItem);
    item->setSizeHint({_pCenternWgt->width() - 50, 46});
}

void setSearchLabel(QLabel* label, int count)
{
    label->setText(QObject::tr("共找到<span style='color:rgba(0,202,190,1)'> %1 </span>条相关消息").arg(count));
}

/**
 *
 * @param data
 */
void LocalSearchMainWgt::onAddSearchItem(const StData &data)
{
    setSearchLabel(_pSearchCountLabel, ++_searchCount);

    switch (data.type)
    {
        case QTalk::Entity::MessageTypeText:
        case QTalk::Entity::MessageTypeRobotAnswer:
        case QTalk::Entity::MessageTypeGroupAt:
        {
            auto* item = new QListWidgetItem;
            auto* textWgt = new SearchTextItemWgt(data, this);
            item->setData(Qt::UserRole, data.time);

            int index = getIndex(_pSearchListWgt, data.time);
            _pSearchListWgt->insertItem(index, item);
            _pSearchListWgt->setItemWidget(item, textWgt);
            item->setSizeHint({_pCenternWgt->width() - 50, getMessageHeight(data.content, 320)});

            _pSearchListWgt->scrollToItem(item);

            connect(textWgt, &SearchTextItemWgt::sgPositionMessage, this, &LocalSearchMainWgt::onPositionMessage);

            break;
        }
        case QTalk::Entity::MessageTypeFile:
        {
            auto* item = new QListWidgetItem;
            auto* fileWgt = new SearchFileItemWgt(data, this);
            item->setData(Qt::UserRole, data.time);

            int index = getIndex(_pSearchListWgt, data.time);
            _pSearchListWgt->insertItem(index, item);
            _pSearchListWgt->setItemWidget(item, fileWgt);
            item->setSizeHint({_pCenternWgt->width() - 50, 95});
            _pSearchListWgt->scrollToItem(item);

            connect(fileWgt, &SearchFileItemWgt::sgPositionMessage, this, &LocalSearchMainWgt::onPositionMessage);
            break;
        }
        default:
            break;
    }
}

void LocalSearchMainWgt::starSearch(qint64, const QString& key)
{
    bool isEmpty = key.isEmpty();
    _btnFrm->setVisible(isEmpty);
    _pSearchCountLabel->setVisible(!isEmpty);

    if(isEmpty)
        _btnGroup->button(EM_ALL)->click();
    else
    {
        _pSearchListWgt->clear();
        _pStackedWidget->setCurrentWidget(_pSearchListWgt);
        _minTime[EM_SEARCH] = LLONG_MAX;
        _msgIds[EM_SEARCH].clear();
        _searchCount = 0;
        setSearchLabel(_pSearchCountLabel, _searchCount);
    }
}

/**
 *
 * @param time
 */
void LocalSearchMainWgt::onPositionMessage(qint64 t)
{
    _pAllListWgt->clear();
    _searEdit->clear();

    QMutexLocker locker(&_mutex);
    time_items[EM_ALL].clear();
    _msgIds[EM_ALL].clear();
    //
    _minTime[EM_ALL] = LLONG_MAX;
    _maxTime[EM_ALL] = 0;
    _hasBefore[EM_ALL] = true;
    _allFileItem.clear();

    _btnFrm->setVisible(true);
    _pSearchCountLabel->setVisible(false);
    _btnGroup->button(EM_ALL)->click();

    emit sgPositionMessage(t);
}

void LocalSearchMainWgt::recvFileProcess(const double &speed, const double &dtotal, const double &dnow, const double &utotal,
                                  const double &unow, const std::string &key) {
    emit sgRecvFRileProcess(speed, dtotal, dnow, utotal, unow, key.data());
}

void LocalSearchMainWgt::onRecvFRileProcess(double speed, double dtotal, double dnow,
        double utotal, double unow, QString key) {

    QMutexLocker locker(&_mutex);
    if(_allFileItem.contains(key))
    {
        _allFileItem[key]->setProcess(speed, dtotal, dnow, utotal, unow);
    }

    if(_fileItem.contains(key))
    {
        _fileItem[key]->setProcess(speed, dtotal, dnow, utotal, unow);
    }
}

void LocalSearchMainWgt::onSetName(const QString& name) {
    QFontMetricsF uf(_pTitleLabel->font());
    QString userName = uf.elidedText(name, Qt::ElideRight, 200);
    _pTitleLabel->setText(name);
}
