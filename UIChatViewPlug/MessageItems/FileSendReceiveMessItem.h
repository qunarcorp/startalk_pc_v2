#ifndef FILESENDRECEIVEMESSITEM_H
#define FILESENDRECEIVEMESSITEM_H

#include "MessageItemBase.h"

#include <QLabel>
#include <QPushButton>
#include <QToolButton>
#include "FileRoundProgressBar.h"

class FileSendReceiveMessItem : public MessageItemBase
{
    Q_OBJECT
public:
    explicit FileSendReceiveMessItem(const QTalk::Entity::ImMessageInfo &msgInfo, QWidget *parent = Q_NULLPTR);
    ~FileSendReceiveMessItem() override = default;
    // QWidget interface
public:
    QSize itemWdtSize() override;

public slots:
    void setProcess(double speed, double dtotal, double dnow, double utotal, double unow);
    void onSaveAsAct();

Q_SIGNALS:
    void sgOpenDir();
    void sgOpenFile(bool);

private:
    void init();
    void initFileIconMap();
    void initMenu();
    void initLayout();
    void initSendLayout();
    void initReceiveLayout();
    void initSendContentLayout();
    void initReceiveContentLayout();
    void initSendContentTopFrmLayout();
    void initSendContentButtomFrmLayout();
    void initReceiveContentTopFrmLayout();
    void initReceiveContentButtomFrmLayout();
    void setData();
    void judgeFileIsDownLoad();
    void judgeFileIsUpLoad();
    void sendDownLoadFile(const std::string &strLocalPath, const std::string &strUri);
    void downLoadFile();
    QString getLocalFilePath();

private slots:
    void onDownLoadClicked();
    void onMenuBtnChicked();
    void onOpenFilePath();
    void onOpenFile(bool = false);

private:
    QFrame *_contentTopFrm;
    HeadPhotoLab *_contentTopFrmIconLab;
    QLabel *_contentTopFrmFileNameLab;
    QLabel *_contentTopFrmFileSizeLab;

    QFrame *_contentButtomFrm;
    QLabel *_contentButtomFrmMessLab;
    QPushButton *_contentButtomFrmDownLoadBtn;
    QToolButton *_contentButtomFrmMenuBtn;
    FileRoundProgressBar *_contentButtomFrmProgressBar;
    QPushButton *_contentButtomFrmOPenFileBtn;            // 打开文件btn

    QMenu *_downLoadMenu;
    QAction *_saveAsAct;// 另存为
    QAction *_openFileAct;

    QSize _headPixSize;
    QMargins _mainMargin;
    QMargins _leftMargin;
    QMargins _rightMargin;
    QMargins _contentMargin;
    QMargins _contentTopFrmHlayMargin;
    QMargins _contentButtomFrmHlayMargin;
    QSize _sizeMaxPix;
    QSize _contentSize;
    QSize _btnSzie;
    QSize _contentTopFrmIconLabSize;

    int _mainSpacing{};
    int _leftSpacing{};
    int _rightSpacing{};
    int _contentSpacing{};
    int _nameLabHeight{};
    int _contentButtomFrmHeight{};
    int _contentTopFrmFileNameLabHeight{};
    int _contentTopFrmFileSizeLabHeight{};
    int _contentTopFrmHlaySpacing{};
//    QString _strLocalPath;
    QMap<QString, QString> _fileIcons;
//    CustomMenu *_downLoadMenu;
    bool isDownLoad;
    bool isUpLoad;
    //
    bool _openDir;
    bool _openFile;
};

#endif // FILESENDRECEIVEMESSITEM_H
