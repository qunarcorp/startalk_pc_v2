#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif
#ifndef _EmoticonMainWgt_H_
#define _EmoticonMainWgt_H_

#include "../CustomUi/UShadowWnd.h"
#include <QMap>
#include <QPair>
#include <QMutexLocker>
#include "emoticon_global.h"
#include "EmoticonStruct.h"
#include "../include/ThreadPool.h"
#include "../include/CommonStrcut.h"
#include "../entity/im_config.h"

class QStackedWidget;

class QPushButton;

class EmoIconWgt;

class EmoPreviewWgt;

class QToolButton;

class QTableWidget;

class EmoMsgManager;

class EmoticonManager;

class EmoMsgListener;

class EmoIcon;

class EMOTICON_EXPORT EmoticonMainWgt : public UShadowDialog
{
    Q_OBJECT
private:
    EmoticonMainWgt();

public:
    static EmoticonMainWgt *getInstance();

    ~EmoticonMainWgt() override;

public:
    void setConversionId(const QString &conversionId);

    QString downloadEmoticon(const QString &pkgId, const QString &shortCut);

    QString getEmoticonLocalFilePath(const QString &pkgId, const QString &shortCut);

    void removeLocalEmoticon(const QString &pkgid, const QString &iconPath);

    ArStNetEmoticon getNetEmoticonInfo();

    void downloadNetEmoticon(const QString &pkgId);

    void updateDownloadNetEmotiocnProcess(const std::string &key, double dtotal, double dnow);

    void installEmoticon(const QString &pkgId);

    void init();
    // 更新收藏
    void updateCollectionConfig(const std::vector<QTalk::Entity::ImConfig>& arConfigs);
    void updateCollectionConfig(const std::map<std::string, std::string> &deleteData,
                                const std::vector<QTalk::Entity::ImConfig>& arImConfig);

Q_SIGNALS:
    void readLocalEmoticons();

    void sendEmoticon(const QString &, const QString &, bool, const std::string&);
    //
    void sendCollectionImage(const QString &, const QString &, const QString);

    void updateProcessSignal(const QString &, double, double);

    void addLocalEmoSignal(const QString &);

    void installedEmotion(const QString &);

    void installEmoticonError(const QString &);

    void removeEmoticon(const QString &);

    void setTurnPageBtnEnableSignal();
    //
    void updateConllectionSignal();
    //
    void sgGotNetEmo();

protected:
    void focusOutEvent(QFocusEvent *e) override;

    void mousePressEvent(QMouseEvent *e) override;

    void initUi();

    void initLocalEmoticon();

    void getLocalEmoticon();

    void addMyCollection();
    //
    void updateCollection();

    void initDefaultEmo();

private:
    void bindBtnTableWgt(EmoIcon *btn, QTableWidget *wgt);

    void addEmoticon(const StEmoticon &stEmo);

    bool readEmoticonXml(const QString &xmlPath, StEmoticon &stEmoticon);

    void onEmoticonItemClick(int row, int col);
    //
    void onCollectionItemClick(int row, int col);

    void setTurnPageBtnEnable();

    void turnLeft();

    void turnRight();

    void onSettingBtnClick();

    void addLocalEmoticon(const QString &pkgId);

    void onRemoveEmoticon(const QString &pkgId);

    void getNetEmoticon();

    void onRemoveCollection(const QString&);

private:
    QMap<QString, QPair<EmoIcon *, QTableWidget *> > _mapBtnTableWgt;
    QMap<QString, StEmoticon> _mapEmoticonInfos;
    QString _strConversionId;
    ArStNetEmoticon _arNetEmoInfo;
    //
    std::map<UnorderMapKey, std::string>  _mapCollections;
    std::map<std::string, std::string>    _collections;
    //
    StEmoticon _collectionEmo;

private:
    EmoPreviewWgt *_pStackEmoWgt{};
    EmoIconWgt *_pStackIconWgt{};
    QPushButton *_pBtnLeftPage{};
    QPushButton *_pBtnRightPage{};
    QPushButton *_pBtnSetting{};

    EmoMsgManager *_pMessageManager;
    EmoMsgListener *_pMessageListener;
    EmoticonManager *_pManager;

    static EmoticonMainWgt *_pEmoticonMainWgt;
    QMutex _mutex;
};

#endif//_EmoticonMainWgt_H_
