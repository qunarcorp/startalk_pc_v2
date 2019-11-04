//
// Created by QITMAC000260 on 2018/11/16.
//

#ifndef QTALK_V2_PICTUREBROWSER_H
#define QTALK_V2_PICTUREBROWSER_H

#include "../CustomUi/UShadowWnd.h"
#include "PictureFrm.h"
#include "TitleFrm.h"
#include "MessageManager.h"
#include <set>
#include <QMutexLocker>

class QToolButton;

class PictureBrowser : public UShadowDialog {
Q_OBJECT
public:
    explicit PictureBrowser(QWidget *parent = nullptr);
    ~PictureBrowser() override;

public slots:

    void onShowChatPicture(const QString &messageId, const QString &messageContent, int index);
    void showPicture(const QString &picPath, const QString& linkPath);

Q_SIGNALS:
    void enlargeSignal();
    void narrowSignal();
    void one2oneSiganl();
    void rotateSiganl();
    void saveAsSignal();
    void sgGotSourceImg(const QString&, const QString&, bool);

public:
    void turnBefore();
    void turnNext();

protected:
    void keyPressEvent(QKeyEvent* e) override;
    void changeEvent(QEvent *e) override ;

private:
    void initUi();
    void analyseMessage(const std::string& msgId, const QString&, bool next);
    void gotSourceImg(const QString& link, const QString& srcImg, bool isFirst);
    void loadImage(bool isFirst);
    void getBeforeImgs(const std::string& msgId);
    void getNextImgs(const std::string& msgId);

private:
    PictureFrm *_pPicFrm{};
    TitleFrm *_pPTitleFrm{};

    PictureMsgManager* _msgManager;

private:
    std::vector<std::pair<std::string, std::string> > _images; // <local, link>
    std::string _curLink;
    int _curIndex;

    bool _hasBefore;
    bool _hasNext;

    QMutex _mutex;
};


#endif //QTALK_V2_PICTUREBROWSER_H
