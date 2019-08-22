#include "EmoticonMainWgt.h"
#include <QFile>
#include <QDir>
#include <QDomDocument>
#include <QStackedWidget>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QDebug>
#include <QRect>
#include <QTableWidget>
#include <QMouseEvent>
#include <QDesktopWidget>
#include <QTemporaryFile>
#include <QApplication>
#include "EmoPreviewWgt.h"
#include "EmoIconWgt.h"
#include "../Platform/Platform.h"
#include "MessageManager.h"
#include "EmoCellWidget.h"
#include "EmoticonManager.h"
#include "../quazip/JlCompress.h"
#include "LocalEmoticon.h"
#ifdef _WINDOWS
#include <windows.h>
#else
#include <unistd.h>
#endif // _WINDOWS


#define DEM_FIXED_WIDTH 480
#define DEM_FIXED_HEIGHT 280
#define DEM_ICON_WID 30
#define DEM_EMOTICON_MODEL "[obj type=\"emoticon\" value=\"[%1]\" width=%2 height=%2 ]"
#define DEM_COLLECTION_MODEL "[obj type=\"image\" value=\"%1\" width=%2 height=%3 ]"

EmoticonMainWgt *EmoticonMainWgt::_pEmoticonMainWgt = nullptr;

EmoticonMainWgt::EmoticonMainWgt()
        : UShadowDialog(nullptr, true), _pManager(nullptr) {
    _pMessageManager = new EmoMsgManager;
    _pMessageListener = new EmoMsgListener;
    initUi();
    init();
    initDefaultEmo();
    addMyCollection();
    getLocalEmoticon();

    connect(this, &EmoticonMainWgt::readLocalEmoticons, this, &EmoticonMainWgt::initLocalEmoticon);
    connect(_pBtnLeftPage, &QPushButton::clicked, this, &EmoticonMainWgt::turnLeft);
    connect(_pBtnRightPage, &QPushButton::clicked, this, &EmoticonMainWgt::turnRight);
    connect(_pBtnSetting, &QPushButton::clicked, this, &EmoticonMainWgt::onSettingBtnClick);
    connect(this, &EmoticonMainWgt::addLocalEmoSignal, this, &EmoticonMainWgt::addLocalEmoticon);
    connect(this, &EmoticonMainWgt::removeEmoticon, _pStackIconWgt, &EmoIconWgt::deleteEmoIcon);
    connect(this, &EmoticonMainWgt::removeEmoticon, _pStackEmoWgt, &EmoPreviewWgt::deleteEmoticonById);
    connect(this, &EmoticonMainWgt::setTurnPageBtnEnableSignal, this, &EmoticonMainWgt::setTurnPageBtnEnable);
    connect(this, &EmoticonMainWgt::updateConllectionSignal, this, &EmoticonMainWgt::updateCollection);
}

EmoticonMainWgt *EmoticonMainWgt::getInstance() {
    if (nullptr == _pEmoticonMainWgt) {
        _pEmoticonMainWgt = new EmoticonMainWgt;
    }
    return _pEmoticonMainWgt;
}

EmoticonMainWgt::~EmoticonMainWgt() {
}

/**
  * @函数名   
  * @功能描述 
  * @参数
  * @author   cc
  * @date     2018/10/19
  */
void EmoticonMainWgt::setConversionId(const QString &conversionId) {
    _strConversionId = conversionId;
}

/**
  * @函数名   
  * @功能描述 
  * @参数
  * @author   cc
  * @date     2018/10/19
  */
QString EmoticonMainWgt::downloadEmoticon(const QString &pkgId, const QString &shortCut) {
    QString realFilePath;
    QString fileName(shortCut);
    fileName.replace("/", "");
    fileName = QString("%1_%2").arg(pkgId, fileName);

    // 下载表情
    if (!QFile::exists(realFilePath) && _pMessageManager) {
        const std::string &strPath = _pMessageManager->getEmoRealFilePath(pkgId.toStdString(),
                                                                          shortCut.toStdString(),
                                                                          fileName.toStdString());
        realFilePath = QString::fromStdString(strPath);
    }
    return realFilePath;
}

/**
 *
 * @param pkgId
 * @param shortCut
 */
QString EmoticonMainWgt::getEmoticonLocalFilePath(const QString &pkgId, const QString &shortCut)
{
    QString realFilePath;

    if (_mapEmoticonInfos.contains(pkgId)) {

        const QMap<UnorderMapKey, StEmoticonItem>& mapEmo = _mapEmoticonInfos[pkgId].mapEmoticon;
        auto itFind = std::find_if(mapEmo.begin(), mapEmo.end(), [shortCut](const StEmoticonItem& item){
            return item.shortcut == shortCut;
        });
        if(itFind != mapEmo.end())
        {
            const std::string &strDirPath = Platform::instance().getLocalEmoticonPath(pkgId.toStdString());
            realFilePath = QString("%1/%2").arg(strDirPath.c_str()).arg(itFind->fileorg);
        }
    }

    if (realFilePath.isEmpty() || !QFile::exists(realFilePath)) {
        QString fileName(shortCut);
        fileName.replace("/", "");
        fileName = QString("%1_%2").arg(pkgId, fileName);
        // temp 路径下表情
        const QString &strDirPath = QString(Platform::instance().getTempEmoticonPath(pkgId.toStdString()).c_str());
        if (QDir(strDirPath).exists()) {
            realFilePath = QString("%1/%2").arg(strDirPath, fileName);
            QFileInfoList infoLst = QDir(strDirPath).entryInfoList(QDir::Files | QDir::NoDotAndDotDot);
            auto it = std::find_if(infoLst.begin(), infoLst.end(), [fileName](const QFileInfo &info) {
                return info.baseName() == fileName;
            });
            if (it != infoLst.end()) {
                realFilePath += "." + it->suffix();
            }
        }
    }

    return realFilePath;
}

/**
  * @函数名   removeLocalEmoticon
  * @功能描述 
  * @参数
  * @author   cc
  * @date     2018/10/21
  */
void EmoticonMainWgt::removeLocalEmoticon(const QString &pkgid, const QString &iconPath) {
    std::thread t([this, pkgid, iconPath]() {
        //
#ifdef _MACOS
        pthread_setname_np("EmoticonMainWgt::removeLocalEmoticon");
#endif
        std::string localEmoDir = Platform::instance().getLocalEmoticonPath(pkgid.toStdString());
        QDir dir(QString(localEmoDir.c_str()));
        if (dir.exists()) {
            dir.removeRecursively();
            emit removeEmoticon(pkgid);
            _mapBtnTableWgt.remove(pkgid);
            // 设置左右 切换
            emit setTurnPageBtnEnableSignal();
        }
    });
    t.detach();
}

/**
  * @函数名   
  * @功能描述 
  * @参数
  * @author   cc
  * @date     2018/10/21
  */
ArStNetEmoticon EmoticonMainWgt::getNetEmoticonInfo() {
    QMutexLocker locker(&_mutex);
    return _arNetEmoInfo;
}

/**
  * @函数名   
  * @功能描述 
  * @参数
  * @author   cc
  * @date     2018/10/22
  */
void EmoticonMainWgt::downloadNetEmoticon(const QString &pkgId) {
    std::thread t([this, pkgId]() {
#ifdef _MACOS
        pthread_setname_np("EmoticonMainWgt::downloadNetEmoticon");
#endif
        if (_pMessageManager) {
            auto it = std::find_if(_arNetEmoInfo.begin(), _arNetEmoInfo.end(),
                                   [pkgId](std::shared_ptr<StNetEmoticon> stNetEmo) {
                                       return stNetEmo->pkgid == pkgId.toStdString();
                                   });
            if (it != _arNetEmoInfo.end()) {
                std::string localPath = Platform::instance().getLocalEmoticonPacketPath(pkgId.toStdString());
                _pMessageManager->downloadNetEmoticon((*it)->emoFile, localPath, pkgId.toStdString());
            }
        }
    });

    t.detach();
}

/**
  * @函数名   updateDownloadNetEmotiocnProcess
  * @功能描述 
  * @参数
  * @author   cc
  * @date     2018/10/22
  */
void EmoticonMainWgt::updateDownloadNetEmotiocnProcess(const std::string &key, double dtotal, double dnow) {
    emit updateProcessSignal(key.data(), dtotal, dnow);
}

/**
  * @函数名   
  * @功能描述 
  * @参数
  * @author   cc
  * @date     2018/10/22
  */
void EmoticonMainWgt::installEmoticon(const QString &pkgId) {
    std::thread t([this, pkgId]() {
#ifdef _MACOS
        pthread_setname_np("EmoticonMainWgt::installEmoticon");
#endif
        QString pkgPath = QString::fromStdString(
                Platform::instance().getLocalEmoticonPacketPath(pkgId.toStdString()));
        while (!QFile::exists(pkgPath)) {}

        QString emoDir = QString("%1/emoticon").arg(Platform::instance().getAppdataRoamingPath().c_str());
        QStringList lstFile = JlCompress::extractDir(pkgPath, emoDir);

        int tmpIndex = 0;
        while (lstFile.empty())
        {
            if(tmpIndex++ == 100)
            {
                break;
            }
#ifdef _WINDOWS
			Sleep(1);
#else
			sleep(1);
#endif // _WINDOWS
            lstFile = JlCompress::extractDir(pkgPath, emoDir);
        }

        //
        if (!lstFile.empty()) {
            // 表情目录可能与头像id不一致 改个名
            QFileInfo dir(lstFile[0]);
            if (dir.isDir() && dir.baseName() != pkgId) {
                QFile(lstFile[0]).rename(emoDir + "/" + pkgId);
            }

            emit addLocalEmoSignal(pkgId);
        } else {
            emit installEmoticonError(pkgId);
        }
        QFile::remove(pkgPath);
        //
        emit setTurnPageBtnEnableSignal();
    });

    t.detach();
}

/**
  * @函数名   
  * @功能描述 
  * @参数
  * @author   cc
  * @date     2018/10/30
  */
void EmoticonMainWgt::init() {
//
}

/**
  * @函数名   
  * @功能描述 
  * @参数
  * @author   cc
  * @date     2018/10/18
  */
void EmoticonMainWgt::addEmoticon(const StEmoticon &stEmo) {
    if (_pStackIconWgt && _pStackEmoWgt) {
        EmoIcon *btn = _pStackIconWgt->addEmoIcon(stEmo.iconPath, stEmo.pkgid, stEmo.name);
        QTableWidget *tableWgt = _pStackEmoWgt->addEmoticon(stEmo.mapEmoticon, stEmo.pkgid);

        if (_mapBtnTableWgt.empty()) {
            btn->setCheckState(true);
            _pStackEmoWgt->setCurrentWidget(tableWgt);
        }

        _mapBtnTableWgt[stEmo.pkgid] = QPair<EmoIcon *, QTableWidget *>(btn, tableWgt);
        bindBtnTableWgt(btn, tableWgt);
    }
}

/**
  * @函数名   
  * @功能描述 
  * @参数
  * @author   cc
  * @date     2018/10/18
  */
bool EmoticonMainWgt::readEmoticonXml(const QString &xmlPath, StEmoticon &stEmoticon) {
    bool ret = false;

    QFile xml(xmlPath);
    QDomDocument doc;
    ret = xml.exists() && xml.open(QIODevice::ReadOnly);
    if (ret) {
        ret = doc.setContent(&xml);
        if (ret) {
            QDomElement root = doc.documentElement();
            if ("FACESETTING" == root.tagName()) {
                QDomNode dft_n = root.firstChild();
                QDomElement dft_e = dft_n.toElement();
                if ("DEFAULTFACE" == dft_e.tagName()) {
                    stEmoticon.name = dft_e.attribute("emotionName");
                    stEmoticon.isShowALl = dft_e.attribute("showall") == "1";

                    QDomNode face_n = dft_e.firstChild();
                    while (!face_n.isNull()) {
                        StEmoticonItem item;
                        QDomElement face_e = face_n.toElement();
                        item.emoid = face_e.attribute("id");
                        item.shortcut = face_e.attribute("shortcut");
                        item.tooltip = face_e.attribute("tip");

                        QDomNode tmpNode = face_e.firstChild();
                        item.fileorg = tmpNode.toElement().text();
                        tmpNode = tmpNode.nextSibling();
                        item.filefixd = tmpNode.toElement().text();

                        stEmoticon.mapEmoticon[item.shortcut] = item;
                        face_n = face_n.nextSibling();
                    }
                }
            }
        }
        xml.close();
    }

    return ret;
}

/**
  * @函数名   onEmoticonItemClick
  * @功能描述 
  * @参数
  * @author   cc
  * @date     2018/10/19
  */
void EmoticonMainWgt::onEmoticonItemClick(int row, int col) {
    auto *wgt = (QTableWidget *) sender();
    if (wgt) {
        auto *cellWgt = (EmoCellWidget *) wgt->cellWidget(row, col);
        if (cellWgt) {
            QString pkgId = cellWgt->getPkgId();
            QString shortCut = cellWgt->getShortCut();
            if (pkgId.isEmpty() || shortCut.isEmpty()) {
                return;
            }
            bool isShowAll = false;
            QString emoticonContent = QString(DEM_EMOTICON_MODEL).arg(shortCut, pkgId);
            if(_mapEmoticonInfos.contains(pkgId))
            {
                isShowAll = _mapEmoticonInfos[pkgId].isShowALl;
                if(isShowAll)
                {
                    emoticonContent = getEmoticonLocalFilePath(pkgId, shortCut);
                }
            }

            emit sendEmoticon(_strConversionId, emoticonContent, isShowAll, "");
        }
    }
    this->setVisible(false);
}

/**
 *
 * @param row
 * @param col
 */
void EmoticonMainWgt::onCollectionItemClick(int row, int col) {
    auto *wgt = (QTableWidget *) sender();
    if (wgt) {
        auto *cellWgt = (EmoCellWidget *) wgt->cellWidget(row, col);
        if (cellWgt) {
            QString localPath = cellWgt->getPkgId();
            QString emoNetPath = cellWgt->getShortCut();
            if (localPath.isEmpty() || emoNetPath.isEmpty()) {
                return;
            }
//            QPixmap pixmap(localPath);
//            QString emoticonContent = QString(DEM_COLLECTION_MODEL).arg(emoNetPath)
//                    .arg(pixmap.width()).arg(pixmap.height());
            emit sendCollectionImage(_strConversionId, localPath, emoNetPath);
        }
    }
    this->setVisible(false);
}

/**
  * @函数名   setTurnPageBtnEnable
  * @功能描述 
  * @参数
  * @author   cc
  * @date     2018/10/19
  */
void EmoticonMainWgt::setTurnPageBtnEnable() {
    int index = _pStackIconWgt->currentIndex();
    _pBtnLeftPage->setEnabled(index != 0 && index != -1);
    _pBtnRightPage->setEnabled(index != (_pStackIconWgt->count() - 1));
}

/**
  * @函数名   
  * @功能描述 
  * @参数
  * @author   cc
  * @date     2018/10/19
  */
void EmoticonMainWgt::turnLeft() {
    int index = _pStackIconWgt->currentIndex();
    if (index != 0) {
        _pStackIconWgt->setCurrentIndex(index - 1);
    }
    setTurnPageBtnEnable();
}

/**
  * @函数名   
  * @功能描述 
  * @参数
  * @author   cc
  * @date     2018/10/19
  */
void EmoticonMainWgt::turnRight() {
    int index = _pStackIconWgt->currentIndex();
    if (index != _pStackIconWgt->count() - 1) {
        _pStackIconWgt->setCurrentIndex(index + 1);
    }
    setTurnPageBtnEnable();
}

/**
  * @函数名   onSettingBtnClick
  * @功能描述 
  * @参数
  * @author   cc
  * @date     2018/10/19
  */
void EmoticonMainWgt::onSettingBtnClick() {

    //
    if (_pManager == nullptr) {
        _pManager = new EmoticonManager(_mapEmoticonInfos, this);
        _pManager->getLocalManager()->initCollection(_collectionEmo);
        _pManager->getLocalManager()->updateCollection(_mapCollections);
        _pManager->getLocalManager()->initEmoticon();

        connect(_pManager->getLocalManager(), &LocalEmoticon::removeCollection,
                this, &EmoticonMainWgt::onRemoveCollection);
    }
    //
    getNetEmoticon();
    // 显示屏幕中间
    QDesktopWidget *deskTop = QApplication::desktop();
    int curMonitor = deskTop->screenNumber(this);
    QRect deskRect = deskTop->screenGeometry(curMonitor);
    _pManager->move((deskRect.width() - 630) / 2 + deskRect.x(), (deskRect.height() - 560) / 2 + deskRect.y());
    _pManager->showModel();
}

/**
  * @函数名   
  * @功能描述 
  * @参数
  * @author   cc
  * @date     2018/10/21
  */
void EmoticonMainWgt::getNetEmoticon() {
    std::thread t([this]() {
#ifdef _MACOS
        pthread_setname_np("EmoticonMainWgt::getNetEmoticon");
#endif
        QMutexLocker locker(&_mutex);
        _arNetEmoInfo = _pMessageManager->getNetEmoticon();
        emit sgGotNetEmo();
    });
    t.detach();
}

/**
  * @函数名   
  * @功能描述 
  * @参数
  * @author   cc
  * @date     2018/10/22
  */
void EmoticonMainWgt::addLocalEmoticon(const QString &pkgId) {
    QDir xmlDir(QString(Platform::instance().getLocalEmoticonPath(pkgId.toStdString()).c_str()));
    QFileInfoList xmlLst = xmlDir.entryInfoList(QDir::Files | QDir::NoDotAndDotDot);
    QDir iconDir(Platform::instance().getEmoticonIconPath().c_str());
    QFileInfoList fileLst = iconDir.entryInfoList(QDir::Files | QDir::NoDotAndDotDot);

    auto xmlFind = std::find_if(xmlLst.begin(), xmlLst.end(), [pkgId](const QFileInfo &info) {
        return info.suffix().toUpper() == "XML";
    });
    auto itFind = std::find_if(fileLst.begin(), fileLst.end(), [pkgId](const QFileInfo &info) {
        return info.baseName() == pkgId;
    });
    if (xmlFind != xmlLst.end() && itFind != fileLst.end()) {
        StEmoticon stEmoticon;
        stEmoticon.pkgid = pkgId;
        stEmoticon.iconPath = itFind->absoluteFilePath();
        //
        if (readEmoticonXml(xmlFind->absoluteFilePath(), stEmoticon)) addEmoticon(stEmoticon);

        if (_pManager) {
            _pManager->getLocalManager()->addEmoticon(stEmoticon);
        }
        _mapEmoticonInfos[pkgId] = stEmoticon;

        emit installedEmotion(pkgId);
    }
}

/**
  * @函数名   onRemoveEmoticon
  * @功能描述 
  * @参数
  * @author   cc
  * @date     2018/10/28
  */
void EmoticonMainWgt::onRemoveEmoticon(const QString &pkgId) {
    Q_UNUSED(pkgId);
}

/**
  * @函数名   
  * @功能描述 
  * @参数
  * @author   cc
  * @date     2018/10/17
  */
void EmoticonMainWgt::initUi() {
    setWindowFlags(windowFlags() | Qt::Widget);

    _pStackEmoWgt = new EmoPreviewWgt(6);
    _pStackIconWgt = new EmoIconWgt();
    _pBtnLeftPage = new QPushButton();
    _pBtnRightPage = new QPushButton();
    _pBtnSetting = new QPushButton();
    //
    _pBtnLeftPage->setObjectName("EmoticonLeftPage");
    _pBtnRightPage->setObjectName("EmoticonRightPage");
    _pBtnSetting->setObjectName("EmoticonSetting");
    _pBtnLeftPage->setFixedSize(DEM_ICON_WID, DEM_ICON_WID);
    _pBtnRightPage->setFixedSize(DEM_ICON_WID, DEM_ICON_WID);
    _pBtnSetting->setFixedSize(DEM_ICON_WID, DEM_ICON_WID);
    //
    QFrame *topFrame = new QFrame(this);
    topFrame->setObjectName("EmoticonTop");
    auto *topLayout = new QHBoxLayout;
    topLayout->setContentsMargins(6, 6, 6, 0);
    topLayout->addWidget(_pStackEmoWgt);
    topFrame->setLayout(topLayout);
    //layout
    QFrame *bottomFrame = new QFrame(this);
    bottomFrame->setObjectName("EmoticonBottom");
    auto *bottomLayout = new QHBoxLayout;
    bottomLayout->setContentsMargins(15, 0, 15, 0);
    bottomLayout->addWidget(_pStackIconWgt);
    bottomLayout->addItem(new QSpacerItem(10, 10, QSizePolicy::Expanding, QSizePolicy::Fixed));
    bottomLayout->addWidget(_pBtnLeftPage);
    bottomLayout->addWidget(_pBtnRightPage);
    bottomLayout->addItem(new QSpacerItem(10, 10));
    bottomLayout->addWidget(_pBtnSetting);
    bottomFrame->setLayout(bottomLayout);
    bottomFrame->setFixedHeight(46);

    _pBtnLeftPage->setFocusPolicy(Qt::NoFocus);
    _pBtnRightPage->setFocusPolicy(Qt::NoFocus);
    _pBtnSetting->setFocusPolicy(Qt::NoFocus);

    auto *layout = new QVBoxLayout(_pCenternWgt);
    layout->setMargin(0);
    layout->setSpacing(0);
    layout->addWidget(topFrame);
    layout->addWidget(bottomFrame);
    layout->setStretch(0, 8);
    setLayout(layout);

    setFixedSize(DEM_FIXED_WIDTH, DEM_FIXED_HEIGHT);
#ifdef _MACOS
    macAdjustWindows();
#endif
}

/**
  * @函数名   
  * @功能描述 
  * @参数
  * @author   cc
  * @date     2018/10/18
  */
void EmoticonMainWgt::initLocalEmoticon() {
    for (const StEmoticon &stEmo : _mapEmoticonInfos) {
        addEmoticon(stEmo);
    }

    setTurnPageBtnEnable();
}

void EmoticonMainWgt::getLocalEmoticon() {
    std::thread t([this]() {
#ifdef _MACOS
        pthread_setname_np("EmoticonMainWgt::getLocalEmoticon");
#endif
        QDir iconDir(Platform::instance().getEmoticonIconPath().c_str());
        QFileInfoList fileLst = iconDir.entryInfoList(QDir::Files | QDir::NoDotAndDotDot);
            for(const QFileInfo& info : fileLst) {
                QString pkgid = info.baseName();
                QDir xmlDir(QString(Platform::instance().getLocalEmoticonPath(pkgid.toStdString()).c_str()));
                QFileInfoList xmlLst = xmlDir.entryInfoList(QDir::Files | QDir::NoDotAndDotDot);
                auto xmlFind = std::find_if(xmlLst.begin(), xmlLst.end(), [pkgid](const QFileInfo &info) {
                    return info.suffix().toUpper() == "XML";
                });

                if (xmlFind != xmlLst.end()) {
                    StEmoticon stEmoticon;
                    stEmoticon.pkgid = pkgid;
                    stEmoticon.iconPath = info.absoluteFilePath();
                    // read xml
                    if (!readEmoticonXml(xmlFind->absoluteFilePath(), stEmoticon)) continue;

                    _mapEmoticonInfos[pkgid] = stEmoticon;
                }
            }

        emit readLocalEmoticons();
    });

    t.detach();
}

/**
  * @函数名   
  * @功能描述 
  * @参数
  * @author   cc
  * @date     2018/10/18
  */
void EmoticonMainWgt::addMyCollection() {

    _collectionEmo.iconPath = ":/chatview/image1/Emoticon/MyEmoticon.png";
    _collectionEmo.pkgid = DEM_COLLECTION;
    _collectionEmo.name = QStringLiteral("我的收藏");
    addEmoticon(_collectionEmo);

    auto wgt = _mapBtnTableWgt.value(DEM_COLLECTION).second;
    disconnect(wgt, &QTableWidget::cellClicked, this, &EmoticonMainWgt::onEmoticonItemClick);
    disconnect(wgt, &QTableWidget::cellDoubleClicked, this, &EmoticonMainWgt::onEmoticonItemClick);
    connect(wgt, &QTableWidget::cellClicked, this, &EmoticonMainWgt::onCollectionItemClick);
    connect(wgt, &QTableWidget::cellDoubleClicked, this, &EmoticonMainWgt::onCollectionItemClick);
}

/**
  * @函数名   
  * @功能描述 
  * @参数
  * @author   cc
  * @date     2018/10/18
  */
void EmoticonMainWgt::bindBtnTableWgt(EmoIcon *btn, QTableWidget *wgt) {
    connect(btn, &EmoIcon::clicked, [this, btn, wgt]() {
        for (auto & it : _mapBtnTableWgt) {
            it.first->setCheckState(it.first == btn);
        }
        _pStackEmoWgt->setCurrentWidget(wgt);
    });

    connect(wgt, &QTableWidget::cellClicked, this, &EmoticonMainWgt::onEmoticonItemClick);
    connect(wgt, &QTableWidget::cellDoubleClicked, this, &EmoticonMainWgt::onEmoticonItemClick);
}

/**
  * @函数名
  * @功能描述
  * @参数
  * @author   cc
  * @date     2018/10/17
  */
void EmoticonMainWgt::focusOutEvent(QFocusEvent *e) {
    this->hide();
    QWidget::focusOutEvent(e);
}

/**
  * @函数名
  * @功能描述
  * @参数
  * @author   cc
  * @date     2018/10/17
  */
void EmoticonMainWgt::mousePressEvent(QMouseEvent *e) {
    QPoint pos = e->pos();
    if (!realContentsRect().contains(pos)) {
        this->hide();
    }
    UShadowDialog::mousePressEvent(e);
}

/**
 * 更新收藏
 * @param arConfigs
 */
void EmoticonMainWgt::updateCollectionConfig(const std::vector<QTalk::Entity::ImConfig> &arConfigs) {

    std::thread t([this, arConfigs]() {

#ifdef _MACOS
        pthread_setname_np("EmoticonMainWgt::updateCollectionConfig");
#endif

        QMutexLocker locker(&_mutex);

        QSet<QString> tmps;
        for(const auto& str : _mapCollections)
            tmps.insert(str.first);

        bool mod = false;
        std::vector<std::string> downloads;
        _collections.clear();
        _mapCollections.clear();
        for (const auto &config  : arConfigs) {
            if (config.ConfigKey == "kCollectionCacheKey") {
                std::string netPath = config.ConfigValue;
                QString localPath = QString::fromStdString(QTalk::getCollectionPath(netPath));

                if(tmps.contains(localPath))
                {
                    tmps.remove(localPath);
//                    continue;
                }
                else
                    mod = true;

                QFileInfo fileINfo(localPath);
                if (!fileINfo.exists()) {
                    downloads.push_back(netPath);
                }
                _collections[config.ConfigSubKey] = config.ConfigValue;
                _mapCollections.insert(std::pair<UnorderMapKey, std::string>(localPath, netPath));
            }
        }

        //
        mod |= !tmps.empty();
        //
        if (mod && _pMessageManager) {
            if(!downloads.empty())
                _pMessageManager->downloadCollections(downloads);
            emit updateConllectionSignal();
        }
    });
    t.detach();
}

void EmoticonMainWgt::updateCollectionConfig(const std::map<std::string, std::string> &deleteData,
                                             const std::vector<QTalk::Entity::ImConfig>& arImConfig) {

    std::thread t([this, deleteData, arImConfig]() {

#ifdef _MACOS
        pthread_setname_np("EmoticonMainWgt::updateCollectionConfig");
#endif

        QMutexLocker locker(&_mutex);

        if(!deleteData.empty())
        {
            bool mod = false;
            for (const auto &config  : deleteData) {
                if (config.second == "kCollectionCacheKey" && _collections.find(config.first) != _collections.end()) {
                    std::string configVal = _collections[config.first];
                    _collections.erase(config.first);

                    auto itFind =  std::find_if(_mapCollections.begin(), _mapCollections.end(),
                            [configVal](const auto pairConfig) {
                        return pairConfig.second == configVal;
                    });
                    if(itFind != _mapCollections.end())
                    {
                        mod = true;
                        _mapCollections.erase(itFind);
                    }
                }
            }
            if(mod)
                emit updateConllectionSignal();
        }
        if(!arImConfig.empty())
        {
            bool mod = false;
            std::vector<std::string> downloads;
            for (const auto &config  : arImConfig) {
                if (config.ConfigKey == "kCollectionCacheKey" &&
                        _collections.find(config.ConfigSubKey) == _collections.end()) {

                    mod = true;
                    std::string netPath = config.ConfigValue;
                    QString localPath = QString::fromStdString(QTalk::getCollectionPath(netPath));
                    QFileInfo fileINfo(localPath);
                    if (!fileINfo.exists()) {
                        downloads.push_back(netPath);
                    }
                    _collections[config.ConfigSubKey] = config.ConfigValue;
                    _mapCollections.insert(std::pair<UnorderMapKey, std::string>(localPath, netPath));
                }
            }
            if (mod && _pMessageManager) {
                if(!downloads.empty())
                    _pMessageManager->downloadCollections(downloads);
                emit updateConllectionSignal();
            }
        }

    });
    t.detach();
}

/**
 * 更新收藏
 */
void EmoticonMainWgt::updateCollection() {
    //
    auto wgt = _mapBtnTableWgt.value(DEM_COLLECTION).second;
    if(wgt)
    {
        _pStackEmoWgt->updateCollection(wgt, _mapCollections);
    }
    //
    if(_pManager)
        _pManager->getLocalManager()->updateCollection(_mapCollections);
}

void EmoticonMainWgt::initDefaultEmo()
{
    Q_INIT_RESOURCE(emoticon);
    QString userDirPath = QString::fromStdString(AppSetting::instance().getUserDirectory());
    QString emoDirPath = QString("%1/emoticon").arg(userDirPath);
    QString emojiPath = QString("%1/EmojiOne").arg(emoDirPath);
    QString emojiZip = QString("%1/emoticon.zip").arg(emoDirPath);

    if(!QFile::exists(emoDirPath))
    {
        QDir userDir(userDirPath);
        userDir.mkpath(emoDirPath);
    }

    if(!QFile::exists(emojiPath))
    {
        QTemporaryFile* tmpFile = QTemporaryFile::createNativeFile(":/emo/emoticon.zip");
        tmpFile->copy(emojiZip);

        if(QFile::exists(emojiZip))
        {
            QStringList lstFile = JlCompress::extractDir(emojiZip, emoDirPath);
            QFile::remove(emoDirPath + "/emoticon.zip");
        }
    }
}

void EmoticonMainWgt::onRemoveCollection(const QString& emoPath)
{
    if(_pMessageManager)
    {
        std::string key = emoPath.toStdString();
        QMutexLocker locker(&_mutex);
        for(const auto& item : _collections)
        {
            if(item.second == key)
            {
                _pMessageManager->updateUserSetting(true, "kCollectionCacheKey", item.first, key);
            }
        }
    }
}
