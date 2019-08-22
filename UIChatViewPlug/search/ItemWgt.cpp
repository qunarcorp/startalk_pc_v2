#include <utility>


//
// Created by QITMAC000260 on 2019-06-26.
//

#include "ItemWgt.h"
#include "../../entity/im_message.h"
#include <QPainter>
#include <utility>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QDateTime>
#include <QTextBrowser>
#include <QToolButton>
#include <QLabel>
#include <ChatViewMainPanel.h>
#include <QDesktopServices>
#include <QProcess>
#include <QFileInfo>
#include <QFileDialog>
#include <MessageItems/FileRoundProgressBar.h>
#include "../MessageItems/TextBrowser.h"
#include "../../CustomUi/QtMessageBox.h"
#include "../../Platform/AppSetting.h"
#include "../../UICom/StyleDefine.h"
#include <math.h>

#ifdef _WINDOWS
#include<windows.h>
#define MessageBox MessageBox
#else
#include <unistd.h>
#endif

extern ChatViewMainPanel *g_pMainPanel;
#define compare_doule_Equal(x, y) (abs(x - y) < 0.00001)

NameTitleLabel::NameTitleLabel(int dir, QString name, QString time, QWidget *parent)
    : QFrame(parent), _dir(dir), _name(std::move(name)), _time(std::move(time))
{
    setFixedHeight(15);
}

void NameTitleLabel::paintEvent(QPaintEvent* e)
{
    QPainter painter(this);

    QString content = QString("%1  %2").arg(_name, _time);
    QRect rect = this->contentsRect();

    if(QTalk::Entity::MessageDirectionSent == _dir)
        painter.setPen(QColor(0,202,190));
    else
        painter.setPen(QColor(155,155,155));

    painter.drawText(rect, Qt::AlignLeft | Qt::AlignVCenter, content);

    return QFrame::paintEvent(e);
}

/**
 *
 * @param data
 * @param parent
 */
TextItemWgt::TextItemWgt(StData data, QWidget *parent)
    : QFrame(parent), _data(std::move(data))
{
    QString strTime = QDateTime::fromMSecsSinceEpoch(_data.time).toString("hh:mm:ss");
    auto* title = new NameTitleLabel(_data.direction, _data.userName, strTime, this);
    _browser = new TextBrowser(this);
    _browser->setFrameShape(QFrame::NoFrame);
    _browser->setFrameShape(QFrame::NoFrame);
    _browser->setOpenLinks(false);
    _browser->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    _browser->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    _browser->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    _browser->setStyleSheet(QString("background-color:rgba(0,0,0,0)"));
    _browser->setContextMenuPolicy(Qt::NoContextMenu);
    _browser->insertHtml(_data.content);
    auto* lay = new QVBoxLayout(this);
    lay->addWidget(title);
    lay->addWidget(_browser);
}

QSize TextItemWgt::getSize() {

    _browser->setFixedSize(_browser->sizeHint());
    this->adjustSize();
    return this->sizeHint();
}

SearchTextItemWgt::SearchTextItemWgt(StData data, QWidget* parent)
        : QFrame(parent), _data(std::move(data))
{
    setFixedHeight(60);
    QString strTime = QDateTime::fromMSecsSinceEpoch(_data.time).toString("yyyy-MM-dd hh:mm:ss");
    auto* title = new NameTitleLabel(_data.direction, _data.userName, strTime, this);

    QLabel* contentLabel = new QLabel(this);
    contentLabel->setObjectName("SearchTextContent");
    contentLabel->setText(_data.content);

    auto* btn = new QToolButton(this);
    btn->setObjectName("openMessageBtn");

    auto* mainLay = new QVBoxLayout;
    mainLay->addWidget(title);
    mainLay->addWidget(contentLabel);

    auto* lay = new QHBoxLayout(this);
    lay->addLayout(mainLay);
    lay->addWidget(btn);
    lay->setAlignment(btn, Qt::AlignRight);
    lay->setStretch(0, 1);
    lay->setStretch(1, 0);

    connect(btn, &QToolButton::clicked, [this](){
        emit sgPositionMessage(_data.time);
    });
}


FileItemWgt::FileItemWgt(StData data, bool showTitle, QWidget *parent)
        : QFrame(parent), _data(std::move(data))
{
    if(showTitle)
        setFixedHeight(95);
    else
        setFixedHeight(70);
    //
    auto* leftFrm = new QFrame(this);
    leftFrm->setFixedWidth(258);
    leftFrm->setObjectName("FileItemWgtLeftFrm");
    _openPathBtn = new QToolButton(this);
    auto* downloadBtn = new QToolButton(this);
    auto* moreBtn = new QToolButton(this);
    _processBar = new FileRoundProgressBar;

    _openPathBtn->setObjectName("FileItemWgt_openPathBtn");
    downloadBtn->setObjectName("FileItemWgt_downloadBtn");
    moreBtn->setObjectName("FileItemWgt_moreBtn");
    _openPathBtn->setFixedSize(26, 26);
    downloadBtn->setFixedSize(26, 26);
    moreBtn->setFixedSize(26, 26);
    _processBar->setFixedSize(26, 26);

    QLabel* iconLabel = new QLabel(this);
    iconLabel->setFixedSize(36, 36);
    iconLabel->setPixmap(QPixmap(_data.fileData.iconPath));

    QLabel* fileNameLabel = new QLabel(_data.fileData.fileName, this);
    QLabel* fileSizeLabel = new QLabel(_data.fileData.fileSize, this);
    QLabel* timeLabel = new QLabel(this);
    QLabel* nameLabel = new QLabel(_data.userName, this);
    timeLabel->setText(QDateTime::fromMSecsSinceEpoch(_data.time).toString("hh:mm"));
    timeLabel->setObjectName("FileItemWgtTimeLabel");
    nameLabel->setObjectName("FileItemWgtNameLabel");
    fileNameLabel->setObjectName("FileItemWgtFileNameLabel");
    fileSizeLabel->setObjectName("FileItemWgtFileSizeLabel");

    auto* infoLay = new QHBoxLayout;
    infoLay->addWidget(timeLabel);
    infoLay->addWidget(nameLabel);
    infoLay->addWidget(fileSizeLabel);
    infoLay->setAlignment(fileSizeLabel, Qt::AlignRight);

    auto* bbLay = new QVBoxLayout;
    bbLay->setMargin(0);
    bbLay->setSpacing(5);
    bbLay->addWidget(fileNameLabel);
    bbLay->addLayout(infoLay);

    auto* leftLay = new QHBoxLayout(leftFrm);
    leftLay->setMargin(10);
    leftLay->addWidget(iconLabel);
    leftLay->addLayout(bbLay);
    leftLay->setAlignment(iconLabel, Qt::AlignVCenter);

    auto* bodyLay = new QHBoxLayout;
    bodyLay->setContentsMargins(10, 0, 10, 0);
    bodyLay->setSpacing(10);
    bodyLay->addWidget(leftFrm);
    bodyLay->addWidget(_openPathBtn);
    bodyLay->addWidget(downloadBtn);
    bodyLay->addWidget(_processBar);
    bodyLay->addWidget(moreBtn);

    _processBar->setVisible(false);
    if(_data.fileData.status == EM_FILE_STATUS_UN_DOWNLOAD)
    {
        _openPathBtn->setVisible(false);
    }
    else if(_data.fileData.status == EM_FILE_STATUS_DOWNLOADED)
    {
        downloadBtn->setVisible(false);
    }

    auto* lay = new QVBoxLayout(this);
    lay->setContentsMargins(10, 5, 0, 5);
    lay->setSpacing(5);
    if(showTitle)
    {
        QString strName = QDateTime::fromMSecsSinceEpoch(_data.time).toString("hh:mm:ss");
        auto* title = new NameTitleLabel(_data.direction, _data.userName, strName, this);
        lay->addWidget(title);
    }
    lay->addLayout(bodyLay);

    //
    auto* _menu = new QMenu(this);
    _menu->setAttribute(Qt::WA_TranslucentBackground, true);

    auto* forwardAct = new QAction("转发", _menu);
    _menu->addAction(forwardAct);

    connect(moreBtn, &QToolButton::clicked, [this, _menu](){
        _menu->exec(QCursor::pos());
    });
    connect(forwardAct, &QAction::triggered, [this](){
        g_pMainPanel->forwardMessage(_data.messageId.toStdString());
    });

    connect(_openPathBtn, &QToolButton::clicked, [this, downloadBtn]() {

        QString fileName = _data.fileData.filePath;
        QFileInfo info(_data.fileData.filePath);
        if (fileName.isEmpty() || !info.exists()) {
            QtMessageBox::warning(g_pMainPanel, "提醒", "未找到本地文件!");
            _openPathBtn->setVisible(false);
            downloadBtn->setVisible(true);
        } else {
            std::thread([info](){
#if defined(_WINDOWS)
                QStringList params;
        QString path = info.absoluteFilePath();
        if (!QFileInfo(path).isDir()) {
            params << QLatin1String("/select,");
            params << QDir::toNativeSeparators(path);
            QProcess::startDetached(QLatin1String("explorer.exe"), params);
        }
#elif defined(_MACOS)
                QStringList scriptArgs;
                QDesktopServices::openUrl(QUrl::fromLocalFile(info.absolutePath()));
                scriptArgs << QLatin1String("-e")
                           << QString::fromLatin1(R"(tell application "Finder" to reveal POSIX file "%1")")
                                   .arg(info.absoluteFilePath());
                QProcess::execute(QLatin1String("/usr/bin/osascript"), scriptArgs);
#else
                QDesktopServices::openUrl(QUrl::fromLocalFile(info.absolutePath()));
#endif
            }).detach();
        }
    });

    connect(downloadBtn, &QToolButton::clicked, [this, downloadBtn](){
        downloadBtn->setVisible(false);
        _processBar->setVisible(true);

        QString fileName = _data.fileData.fileName;
        fileName.replace(QRegExp("[\"|\\<|\\>|\\“|\\”|\\、|\\╲|\\/|\\\\|\\:|\\*|\\?]"), "");
        std::ostringstream src;
        src << AppSetting::instance().getFileSaveDirectory()
            << "/"
            << fileName.toStdString();
        QString qLocalFilePath = QFileDialog::getSaveFileName(g_pMainPanel, "选择文件下载路径", src.str().data());

        if(!qLocalFilePath.isEmpty())
        {
            //下载文件
            std::string localPath = qLocalFilePath.toStdString();
            g_pMainPanel->getMessageManager()->sendDownLoadFile(localPath,
                                                                _data.fileData.fileUrl.toStdString(), _data.messageId.toStdString());
            // 回写配置文件
            g_pMainPanel->setFileMsgLocalPath(_data.messageId.toStdString(), localPath);


        }
    });
}

QString getLocalFilePath(const std::string& messageId, const std::string& md5)
{
    QString fullFileName = QString::fromStdString(g_pMainPanel->getFileMsgLocalPath(messageId));
    if(fullFileName.isEmpty() || !QFile::exists(fullFileName))
    {
        QString linkFile = g_pMainPanel->getFileLink(md5.data());
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

void FileItemWgt::setProcess(double speed, double dtotal, double dnow, double utotal, double unow)
{
    double process = 0;
    if (dtotal > 0) {
        process = (dnow * 100.0) / dtotal;
    } else if (utotal > 0) {
        process = (unow * 100.0) / utotal;
    } else {
        return;
    }

    if (_processBar) {
        _processBar->setCurValue(process);
    }

    if (compare_doule_Equal(100, process)) //
    {
        // 创建软链
        std::thread([this](){
            int i = 0;
            QString localFilePath(getLocalFilePath(_data.messageId.toStdString(),
                                              _data.fileData.fileMd5.toStdString()));
            while (!QFile::exists(localFilePath) && i < 100) {
#ifdef _WINDOWS
                Sleep(100);
#else
                usleep(100000);
#endif
                ++i;
            }
            if(!localFilePath.isEmpty())
            {
                g_pMainPanel->makeFileLink(localFilePath, _data.fileData.fileMd5.toStdString().data());
                _data.fileData.filePath = localFilePath;
            }

        }).join();

        _processBar->setVisible(false);
        _openPathBtn->setVisible(true);
    }
}


TimeItemWgt::TimeItemWgt(QString content, QWidget *parent)
        : QFrame(parent), _conent(std::move(content))
{
    setFixedHeight(46);
}

void TimeItemWgt::paintEvent(QPaintEvent *e) {
    QPainter painter(this);

    QRect rect = this->contentsRect();
    painter.setPen(QTalk::StyleDefine::instance().getLocalSearchTimeFontColor());
    painter.drawText(rect, Qt::AlignCenter | Qt::AlignVCenter, _conent);

    return QFrame::paintEvent(e);
}


SearchFileItemWgt::SearchFileItemWgt(StData data, QWidget* parent)
        : QFrame(parent), _data(std::move(data))
{
    setFixedHeight(95);
    QString strName = QDateTime::fromMSecsSinceEpoch(_data.time).toString("yyyy-MM-dd hh:mm:ss");
    auto* title = new NameTitleLabel(_data.direction, _data.userName, strName, this);
    //
    auto* leftFrm = new QFrame(this);
    leftFrm->setFixedWidth(258);
    leftFrm->setObjectName("FileItemWgtLeftFrm");

    QLabel* iconLabel = new QLabel(this);
    iconLabel->setFixedSize(36, 36);
    iconLabel->setPixmap(QPixmap(_data.fileData.iconPath));

    QLabel* fileNameLabel = new QLabel(_data.fileData.fileName, this);
    QLabel* fileSizeLabel = new QLabel(_data.fileData.fileSize, this);
    QLabel* timeLabel = new QLabel(this);
    QLabel* nameLabel = new QLabel(_data.userName, this);
    timeLabel->setText(QDateTime::fromMSecsSinceEpoch(_data.time).toString("hh:mm"));
    timeLabel->setObjectName("FileItemWgtTimeLabel");
    nameLabel->setObjectName("FileItemWgtNameLabel");
    fileNameLabel->setObjectName("FileItemWgtFileNameLabel");
    fileSizeLabel->setObjectName("FileItemWgtFileSizeLabel");


    auto* btn = new QToolButton(this);
    btn->setObjectName("openMessageBtn");

    auto* infoLay = new QHBoxLayout;
    infoLay->addWidget(timeLabel);
    infoLay->addWidget(nameLabel);
    infoLay->addWidget(fileSizeLabel);
    infoLay->setAlignment(fileSizeLabel, Qt::AlignRight);

    auto* bbLay = new QVBoxLayout;
    bbLay->setMargin(0);
    bbLay->setSpacing(5);
    bbLay->addWidget(fileNameLabel);
    bbLay->addLayout(infoLay);

    auto* leftLay = new QHBoxLayout(leftFrm);
    leftLay->setMargin(10);
    leftLay->addWidget(iconLabel);
    leftLay->addLayout(bbLay);
    leftLay->setAlignment(iconLabel, Qt::AlignVCenter);

    auto* bodyLay = new QHBoxLayout;
    bodyLay->setContentsMargins(10, 0, 10, 0);
    bodyLay->setSpacing(10);
    bodyLay->addWidget(leftFrm);
    bodyLay->addWidget(btn);
    bodyLay->setAlignment(leftFrm, Qt::AlignLeft);
    bodyLay->setAlignment(btn, Qt::AlignRight);

    auto* lay = new QVBoxLayout(this);
    lay->setContentsMargins(10, 5, 0, 5);
    lay->setSpacing(5);
    lay->addWidget(title);
    lay->addLayout(bodyLay);

    connect(btn, &QToolButton::clicked, [this](){
        emit sgPositionMessage(_data.time);
    });
}