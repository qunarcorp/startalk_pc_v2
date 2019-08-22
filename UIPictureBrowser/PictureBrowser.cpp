//
// Created by QITMAC000260 on 2018/11/16.
//

#include "PictureBrowser.h"
#include "TitleFrm.h"
#include "../Platform/Platform.h"
#include "../CustomUi/QtMessageBox.h"
#include <QHBoxLayout>
#include <QWindow>
#include <QKeyEvent>
#include <QApplication>
#include <QFileInfo>

PictureBrowser::PictureBrowser(QWidget *parent)
        : UShadowDialog(nullptr, true)
        , _curIndex(0)
        , _hasBefore(false)
        , _hasNext(false) {

    _msgManager = new PictureMsgManager;
    setWindowFlags(windowFlags() | Qt::Widget);
    initUi();

    connect(this, &PictureBrowser::sgGotSourceImg, this, &PictureBrowser::gotSourceImg);
}

PictureBrowser::~PictureBrowser() = default;

void PictureBrowser::initUi() {
    this->setMinimumSize(480, 480);
    // title
    _pPTitleFrm = new TitleFrm(this);

    _pPicFrm = new PictureFrm(this);
    _pPicFrm->setContentsMargins(6, 6, 6, 6);
    _pPicFrm->setObjectName("bottomFrame");
    _pPicFrm->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    auto *layout = new QVBoxLayout();
    layout->setMargin(0);
    layout->setSpacing(0);
    layout->addWidget(_pPTitleFrm);
    layout->addWidget(_pPicFrm);
    _pCenternWgt->setLayout(layout);

    setMoverAble(true, _pPTitleFrm);
    //
#ifdef _MACOS
    macAdjustWindows();
#endif
}

void PictureBrowser::onShowChatPicture(const QString &messageId, const QString &messageContent, int index) {

    //
    {
        _pPTitleFrm->setBeforeBtnEnable(false);
        _pPTitleFrm->setNextBtnEnable(false);

        _hasBefore = true;
        _hasNext = true;
        _images.clear();
        _curIndex = index;
        analyseMessage(messageId.toStdString() ,messageContent, true);
        if(_images.empty())
        {
            QtMessageBox::warning(this, "警告", "无法加载该图片!");
            return;
        }
        loadImage(true);
    }
    getBeforeImgs(messageId.toStdString());
    getNextImgs(messageId.toStdString());
    QApplication::setActiveWindow(this);
    this->raise();
}

void PictureBrowser::showPicture(const QString &picPath) {
    if(_pPicFrm->loadNewPicture(picPath, true))
    {
        if(this->isVisible())
        {
            this->setVisible(false);
        }
        this->showNormal();
        activateWindow();
    }
}

void PictureBrowser::keyPressEvent(QKeyEvent *e)
{
    if(e->key() == Qt::Key_Up || e->key() == Qt::Key_Right)
    {
        turnNext();
    }
    else if(e->key() == Qt::Key_Down || e->key() == Qt::Key_Left)
    {
        turnBefore();
    }
    UShadowDialog::keyPressEvent(e);
}

/**
 * analyse message content
 * Don't lock it.
 */
void PictureBrowser::analyseMessage(const std::string& msgId, const QString& msg, bool next)
{
    QRegExp regExp("\\[obj type=[\\\\]?\"([^\"]*)[\\\\]?\" value=[\\\\]?\"([^\"]*)[\\\\]?\"(.*)\\]");
    regExp.setMinimal(true);

    std::vector<std::pair<std::string, std::string> > tmpImages;

    int pos = 0;
    while ((pos = regExp.indexIn(msg, pos)) != -1) {
        QString item = regExp.cap(0); // 符合条件的整个字符串
        QString type = regExp.cap(1); // 多媒体类型
        QString val = regExp.cap(2); // 路径

        if ("image" == type)
        {

            if(!val.isEmpty())
            {
                QString rawUrl = QUrl(val).toEncoded();
                if(next)
                    _images.emplace_back(msgId, rawUrl.toStdString());
                else
                    tmpImages.insert(tmpImages.begin(),
                            std::make_pair(msgId, rawUrl.toStdString()));
            }
        }
        else if("emoticon" == type)
        {
            // todo
        }
        //
        pos += item.length();
    }

    if(!next && !tmpImages.empty())
    {
        for(const auto& tmpImg : tmpImages)
        {
            _curIndex++;
            _images.insert(_images.begin(), tmpImg);
        }
    }

}

void PictureBrowser::gotSourceImg(const QString &link, const QString &srcImg, bool isFirst)
{
    if(this->isVisible() && _curLink == link.toStdString())
    {
        _pPicFrm->setEnabled(true);
        _pPicFrm->loadNewPicture(srcImg, isFirst);
    }

}

void PictureBrowser::turnBefore()
{
    QMutexLocker locker(&_mutex);
    if(_curIndex > 0)
    {
        _curIndex--;
        loadImage(false);
    }
    else
    {
        if(_hasBefore)
        {
            getBeforeImgs(_images[_curIndex].first);
        }
        else
        {
            QtMessageBox::warning(this, "警告", "无法加载更早的图片了!");
        }
    }
}

void PictureBrowser::turnNext()
{
    QMutexLocker locker(&_mutex);
    if(_curIndex < _images.size() - 1)
    {
        _curIndex++;
        loadImage(false);
    }
    else
    {
        if(_hasNext)
        {
            getNextImgs(_images[_curIndex].first);
        }
        else
        {
            QtMessageBox::warning(this, "警告", "已看完最后一张图片!");
        }
    }
}

/**
 * load image
 * Don't lock it.
 */
void PictureBrowser::loadImage(bool isFirst)
{
    auto item = _images[_curIndex];
    _curLink = item.second;
    QString imgPath = QString::fromStdString(QTalk::GetSrcImagePathByUrl(_curLink));
    QFileInfo info(imgPath);
    if(!info.exists() || info.isDir())
    {
        imgPath = QString::fromStdString(QTalk::GetImagePathByUrl(_curLink));
        _pPicFrm->setEnabled(false);
        std::thread([item, isFirst, this](){
            if(_msgManager)
            {
                std::string srcImg = _msgManager->getSourceImage(_curLink);
                if(!srcImg.empty())
                    emit sgGotSourceImg(QString::fromStdString(_curLink), QString::fromStdString(srcImg), isFirst);
            }
        }).detach();
    }
    info = QFileInfo(imgPath);
    if(info.exists() && info.isFile())
    {
        if(_pPicFrm->loadNewPicture(imgPath, isFirst) && !isVisible())
        {
//            this->setWindowModality(Qt::WindowModal);
            this->showNormal();
        }
    }
}

/**
 *
 * @param msgId
 */
void PictureBrowser::getBeforeImgs(const std::string &msgId)
{
    static bool isGetting = false;
    if(isGetting)
        return;
    if(!_hasBefore)
        return;

    isGetting = true;

    std::thread([msgId, this](){
        if(_msgManager)
        {
            std::vector<std::pair<std::string, std::string>> msgs;
            _msgManager->getBeforeImgMessages(msgId, msgs);
            QMutexLocker locker(&_mutex);
            if(msgs.empty())
            {
                _hasBefore = false;
            }
            else
            {
                if(msgs.size() < 20)
                    _hasBefore = false;

                for(const auto& msg : msgs)
                {
                    analyseMessage(msg.first, QString::fromStdString(msg.second), false);
                }
            }
            _pPTitleFrm->setBeforeBtnEnable(true);

            isGetting = false;
        }
    }).detach();
}

/**
 *
 * @param msgId
 */
void PictureBrowser::getNextImgs(const std::string &msgId)
{
    static bool isGetting = false;
    if(isGetting)
        return;
    if(!_hasNext)
        return;

    isGetting = true;

    std::thread([msgId, this](){
        if(_msgManager)
        {
            std::vector<std::pair<std::string, std::string>> msgs;
            _msgManager->getNextImgMessages(msgId, msgs);
            if(msgs.empty())
            {
                _hasNext = false;
            }
            else
            {
                if(msgs.size() < 20)
                    _hasNext = false;

                QMutexLocker locker(&_mutex);
                for(const auto& msg : msgs)
                {
                    analyseMessage(msg.first, QString::fromStdString(msg.second), true);
                }

            }
            _pPTitleFrm->setNextBtnEnable(true);

            isGetting = false;
        }
    }).detach();
}