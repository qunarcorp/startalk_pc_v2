#include "ChatViewMainPanel.h"
#include "InputWgt.h"
#include <QKeyEvent>
#include <QDateTime>
#include <QClipboard>
#include <QApplication>
#include <QFileInfo>
#include <QMovie>
#include <QTextDocument>
#include <QTextBlock>
#include <QMimeData>
#include <QBuffer>
#include <QMenu>
#include <QAction>
#include <QDir>
#include <QTextDocumentFragment>
#include <QTextCharFormat>
#include <QJsonArray>
#include <QJsonDocument>

#include "ChatMainWgt.h"
#include "blocks/AtBlock.h"
#include "blocks/block_define.h"
#include "../QtUtil/Utils/utils.h"
#include "../Platform/Platform.h"
#include "../UICom/uicom.h"
#include "../UICom/qimage/qimage.h"
#include "blocks/QuoteBlock.h"
#include "../CustomUi/LiteMessageBox.h"
#include "ToolWgt.h"
#include "../QtUtil/lib/cjson/cJSON.h"
#include "../QtUtil/lib/cjson/cJSON_inc.h"
#include "../QtUtil/Entity/JID.h"
#include "../CustomUi/QtMessageBox.h"

#define ImagePath       0x5012
#define EmotionShortText  0x5013
#define EmotionUniqueKey  0x5014
#define ImageType         0x5015
#define EmotionPkgid      0x0516
#define ImageUrl          0x5017

#define ImageType_Image "ImageType_Image"
#define ImageType_Emot "ImageType_Emot"

extern ChatViewMainPanel *g_pMainPanel;
InputWgt::InputWgt(ChatMainWgt *pMainWgt, ChatViewItem *parent)
        : QTextEdit(parent), _pMenu(nullptr),
        _pMainWgt(pMainWgt), _pChatView(parent),
        _atView(nullptr), _isSending(false) {
    setAcceptRichText(false);
    initUi();
    this->setFrameShape(QFrame::NoFrame);
    document()->setDocumentMargin(0);
    //setLineWrapMode(QTextEdit::WidgetWidth);
    setAcceptDrops(true);
    // 注册@块
    auto *atBlock = new AtBlock;
    auto *quoteBlock = new QuoteBlock;
    atBlock->setParent(this);
    quoteBlock->setParent(this);
    document()->documentLayout()->registerHandler(atObjectType, atBlock);
    document()->documentLayout()->registerHandler(quoteObjectType, quoteBlock);

//    connect(this, &QTextEdit::textChanged, [this]{ style()->polish(this); });
    connect(this, &InputWgt::sgShowMessage, this, &InputWgt::showMessage, Qt::QueuedConnection);
}


InputWgt::~InputWgt() {


}

// 发送文件处理
void sendFile(ChatViewItem *pChatView, const QFileInfo &fileInfo)
{
    QInt64 size = fileInfo.size();
    if(0 == size)
    {
        QtMessageBox::warning(g_pMainPanel, QObject::tr("警告"), QObject::tr("无效的空文件!"));
        return;
    }

    QString strSize;
    if (size > 1024 * 1024 * 1024) {
        QtMessageBox::warning(g_pMainPanel, QObject::tr("警告"), QObject::tr("文件太大 暂时不支持大文件发送!"));
        return;
    } else if (size > 1024 * 1024)
        strSize = QString("%1MB").arg(QString::number(size / (1024 * 1024.00), 'f', 2));
    else
        strSize = QString("%1KB").arg(QString::number(size / 1024.00, 'f', 2));

    if (g_pMainPanel && pChatView) {
        g_pMainPanel->sendFileMessage(pChatView->_uid, pChatView->_chatType, fileInfo.absoluteFilePath(), fileInfo.fileName(), strSize);
    }
}


/**
  * @函数名   dealFile
  * @功能描述 处理文件
  * @参数
  * @author   cc
  * @date     2018/09/21
  */
void InputWgt::dealFile(const QString &filePath, bool isFile, const QString& imageLink) {
    QFileInfo fileInfo(filePath);
    if (fileInfo.exists())
    {
        if(isFile)
            sendFile(_pChatView, fileInfo);
        else
        {
            QString format = QTalk::qimage::instance().getRealImageSuffix(filePath);
            QRegExp imgReg("(JPEG|jpeg|JPG|jpg|GIF|gif|BMP|bmp|PNG|png|ico|ICO|WEBP|webp)$");
            if (imgReg.exactMatch(format)) {
                // load image
                QImage img;
                img.load(filePath, format.toStdString().c_str());
                if (img.isNull()) {
                    insertPlainText(tr("[图片已损坏]"));
                    return;
                }
                //
                qreal fixw = img.width();
                qreal fixh = img.height(); //  最宽150 最高100 算纵横比

                auto scaleRate = qMax(fixh / 100, fixw / 150);
                if (scaleRate > 1.0) {
                    // 计算缩放
                    fixw = img.width() / scaleRate;
                    fixh = img.height() / scaleRate;
                }

                QTextImageFormat imageFormat;
                imageFormat.setWidth(fixw);
                imageFormat.setHeight(fixh);
                if(fileInfo.suffix().toLower() != format.toLower())
                {
                    QString suffix = QTalk::qimage::instance().getRealImageSuffix(filePath);
                    if (suffix.isEmpty())
                        imageFormat.setName(filePath);
                    else
                    {
                        QString newPath = QString("%1/%2.%3").arg(fileInfo.absolutePath()).arg(fileInfo.baseName()).arg(suffix);
                        if (!QFile::exists(newPath))
                            QFile::copy(filePath, newPath);
                        imageFormat.setName(newPath);
                    }
                }
                else
                    imageFormat.setName(filePath);
                imageFormat.setProperty(EmotionUniqueKey, QTalk::utils::getMessageId().data());
                imageFormat.setProperty(ImagePath, filePath);
                imageFormat.setProperty(ImageUrl, imageLink);
                imageFormat.setProperty(ImageType, ImageType_Image);

                textCursor().insertImage(imageFormat);
            }
            else {
                sendFile(_pChatView, fileInfo);
            }
        }
    }
    else
    {
        QtMessageBox::warning(g_pMainPanel, tr("警告"), tr("无法找到该文件!"));
    }
}

/**
  * @函数名   keyPressEvent
  * @功能描述 
  * @参数
  * @author   cc
  * @date     2018/09/18
  */
void InputWgt::keyPressEvent(QKeyEvent *e) {

    if(_atView && _atView->isVisible() && e->key() == Qt::Key_Escape)
    {
        _atView->setVisible(false);
        e->accept();
        return;
    }

//#ifndef _WINDOWS
//    if(_atView && (e->text() == "@" || e->text() == tr("＠")))
//        _showAtView = true;
//#endif

    if (_pChatView->_chatType == QTalk::Enum::GroupChat) {
        if (_atView && _atView->isVisible() && !_atView->eventFilter(_pChatView, e)) {
            return;
        }
    }

    if (e->key() == Qt::Key_Return || e->key() == Qt::Key_Enter)
    {
        std::string sendMessageKey = AppSetting::instance().getSendMessageKey();
        QKeySequence sendMessageKeySeq(sendMessageKey.data());

        int mod(sendMessageKeySeq[0] & Qt::KeyboardModifierMask);

        if((e->modifiers() & 0x0E000000) != (mod & 0x0E000000))
        {
            // 光标移动
            this->textCursor().insertText("\n");
            QTextCharFormat f;
            setCurrentCharFormat(f);
        }
        else
            sendMessage();

        return;
    } else if (e == QKeySequence::Paste ||
               (e->modifiers() == Qt::ControlModifier && e->key() == Qt::Key_V)) {

        onPaste();
    }
    else if(e->key() == Qt::Key_Space)
    {
        QTextCharFormat f;
        setCurrentCharFormat(f);
        textCursor().insertText(" ");
    }
    else if (e == QKeySequence::Copy ||
               (e->modifiers() == Qt::ControlModifier && e->key() == Qt::Key_C)) {
        onCopy();
    }
    else if(e == QKeySequence::Cut || (e->modifiers() == Qt::ControlModifier && e->key() == Qt::Key_X))
    {
        //
        onCopy();

        auto csor = textCursor();
        if(!csor.hasSelection())
            selectAll();
        csor.removeSelectedText();
    }
    else {
        if (e->key() == Qt::Key_Up || e->key() == Qt::Key_Down) {
            if (this->toPlainText().isEmpty()) {
                emit g_pMainPanel->sgShortCutSwitchSession(e->key());
                return;
            }
            else
            {
                int pos = textCursor().position(), newPos = 0;
                moveCursor((e->key() == Qt::Key_Up) ? QTextCursor::Up : QTextCursor::Down, QTextCursor::MoveAnchor);
                newPos = textCursor().position();
                if(pos != newPos) return;
                moveCursor((e->key() == Qt::Key_Up) ? QTextCursor::StartOfBlock : QTextCursor::EndOfBlock, QTextCursor::MoveAnchor);
            }
        }
        QTextEdit::keyPressEvent(e);
    }
}

void InputWgt::sendMessage()
{
    QString content = this->toPlainText();
    QString tmpText = content;
    tmpText = tmpText.trimmed().replace("\n", "");
    if(!tmpText.isEmpty()) {
        if(!_isSending)
        {
            this->setReadOnly(true);
            _isSending = true;
            QPointer<InputWgt> pThis(this);
            std::thread([pThis](){
#ifdef _MACOS
                pthread_setname_np("translate message thread");
#endif
                if(pThis)
                {
                    QString srcText = pThis->translateText();
                    emit pThis->sgTranslated(srcText);
                }

            }).detach();
        }
    }
}

/**
  * @函数名   
  * @功能描述 
  * @参数
  * @author   cc
  * @date     2018/10/16
  */
void InputWgt::dropEvent(QDropEvent *e) {

    const QMimeData* mimeData = e->mimeData();
    if(mimeData->hasUrls())
    {
        QList<QUrl> urls = e->mimeData()->urls();
        for (const QUrl &url : urls) {
            if (url.isLocalFile()) {
                // 拖拽进来的文件 默认以文件形式发送
                QString strLocalPath = url.toLocalFile();
                dealFile(strLocalPath, true);
            }
        }
        return;
    }
    else if(mimeData->hasText())
    {
        return;
    }

    return QTextEdit::dropEvent(e);
}

//
void InputWgt::initUi() {

    setObjectName("InputWgt");
//    this->setPlaceholderText(DEM_PLACEHODER);
    this->setAlignment(Qt::AlignVCenter);

    if(_pChatView->_chatType == QTalk::Enum::GroupChat)
        _atView = new AtMessageView(this);

    setContextMenuPolicy(Qt::CustomContextMenu);
    _pMenu = new QMenu(this);
    _pMenu->setAttribute(Qt::WA_TranslucentBackground, true);
    QAction *selectAllAct = new QAction(tr("全选"));
    QAction *copyAct = new QAction(tr("复制"));
    QAction *pasteAct = new QAction(tr("粘贴"));
    QAction *pasteTextAct = new QAction(tr("粘贴为文本"));
    QAction *pasteImageAct = new QAction(tr("粘贴为图片"));

    _pMenu->addAction(selectAllAct);
    _pMenu->addAction(copyAct);
    _pMenu->addAction(pasteAct);
    _pMenu->addAction(pasteTextAct);
    _pMenu->addAction(pasteImageAct);

    connect(this, &InputWgt::customContextMenuRequested, [this, pasteTextAct, pasteImageAct, pasteAct](const QPoint &pos) {

        const QMimeData *mimeData = QApplication::clipboard()->mimeData();
        pasteImageAct->setVisible(false);
        pasteTextAct->setVisible(false);
        QByteArray userData = mimeData->data("userData");
        bool isOk = userData.isEmpty() && mimeData->hasImage() && !mimeData->imageData().isNull() && mimeData->hasText();
        pasteImageAct->setVisible(isOk);
        pasteTextAct->setVisible(isOk);
        pasteAct->setVisible(!isOk);

        _pMenu->exec(mapToGlobal(pos));
    });

    connect(selectAllAct, &QAction::triggered, [this]() {
        this->selectAll();
    });
    connect(copyAct, &QAction::triggered, [this]() {
        this->copy();
    });
    connect(pasteAct, &QAction::triggered, [this]() {
        onPaste();
    });

    connect(pasteTextAct, &QAction::triggered, [this]() {
        const QMimeData *mimeData = QApplication::clipboard()->mimeData();
        if (mimeData == nullptr)
            return;

        this->insertPlainText(mimeData->text());
    });

    connect(pasteImageAct, &QAction::triggered, [this]() {
        const QMimeData *mimeData = QApplication::clipboard()->mimeData();
        if (mimeData == nullptr)
            return;

        QImage image = QApplication::clipboard()->image();
        if (!image.isNull()) {
            QString tmpFilePath = QString("%1/%2.png").arg(Platform::instance().getTempFilePath().c_str())
                    .arg(QDateTime::currentMSecsSinceEpoch());

            QDir dir(Platform::instance().getTempFilePath().c_str());
            if (!dir.exists()) {
                dir.mkpath(dir.absolutePath());
            }

            image.save(tmpFilePath);
            dealFile(tmpFilePath, false);
        }
    });

    connect(this, &InputWgt::cursorPositionChanged, this, &InputWgt::onTextChanged);
    connect(this, &InputWgt::sgTranslated, this, &InputWgt::sendText);
}

/**
  * @函数名   sendText
  * @功能描述 发送消息接口
  * @参数
  * @author   cc
  * @date     2018/09/18
  */
void InputWgt::sendText(const QString& srcText)
{
    this->setReadOnly(false);
    clearDocument();
    _isSending = false;
    if(srcText.isEmpty())
        return;

    // pre send -> insert to db and show ui
    QPointer<InputWgt> pThis(this);
    if(!pThis || nullptr == pThis->_pChatView)
        return;
    QTalk::Entity::UID uid = pThis->_pChatView->_uid;
    int chatType = pThis->_pChatView->_chatType;
    std::string messageId = QTalk::utils::getMessageId();
    g_pMainPanel->preSendMessage(uid, chatType,
            _atMessage ? QTalk::Entity::MessageTypeGroupAt : QTalk::Entity::MessageTypeText,
            srcText, messageId);

    std::thread([pThis, uid, chatType, srcText, messageId](){
        std::map<std::string, std::string> mapAt;
        bool success = false;
        QString messageContent = g_pMainPanel->getRealText(srcText, messageId.data(), success, mapAt);
        if(!success)
        {
            if(pThis)
                    emit pThis->_pMainWgt->sgSendFailed(messageId.data());
            return;
        }
        //
        QString tmpText = messageContent;
        tmpText = tmpText.trimmed().replace("\n", "");
        if(g_pMainPanel && !tmpText.isEmpty()) {
            g_pMainPanel->sendTextMessage(uid, chatType, messageContent.toStdString(), mapAt, messageId);
        }
        else
            warn_log("empty message {0}", messageId);
    }).detach();
}

/**
 *
 */
void InputWgt::clearDocument()
{
    // 清除样式
    this->clear();
    QTextCharFormat f;
    setCurrentCharFormat(f);
}

/**
  * @函数名   translateText
  * @功能描述 转化文本
  * @参数
  * @author   cc
  * @date     2018/09/18
  */

void addJsonItem(QJsonArray &array, const int& key, const QString& value, const QString& extendedInfo = QString())
{
    QJsonObject obj;
    obj.insert("key", key);
    obj.insert("value", value);
    if(!extendedInfo.isEmpty())
        obj.insert("info", extendedInfo);
    //
    array.append(obj);
}

QString InputWgt::translateText() {

    QTextDocument *dt = this->document();
    QTextBlock currentBlock = dt->begin();
    QTextBlock::iterator it;

    QJsonArray array;
    while (currentBlock.isValid()) {
        for (it = currentBlock.begin(); !(it.atEnd()); it++) {
            QTextFragment currentFragment = it.fragment();
            QTextFormat format = currentFragment.charFormat();
            if (currentFragment.charFormat().isImageFormat()) {
                QTextImageFormat newImageFormat = currentFragment.charFormat().toImageFormat();
                if (newImageFormat.isValid()) {
                    QString imageType = newImageFormat.stringProperty(ImageType);
                    QString imagePath = newImageFormat.stringProperty(ImagePath);
                    QString imageLink = newImageFormat.stringProperty(ImageUrl);

                    if (imagePath.isEmpty()) {
                        imagePath = newImageFormat.name();
                        imageType = ImageType_Image;
                    }
                    //
                    if (imageType == ImageType_Image) {
                        QFileInfo imageInfo(imagePath);
                        if (imageInfo.exists() && imageInfo.isFile()) {
                            addJsonItem(array, Type_Image, imagePath, imageLink);
                        }
                        else
                        {
                            error_log("invalid image {0}", imagePath.toStdString());
                            continue;
                        }
                    } else if (imageType == ImageType_Emot) {

                    } else {
                        //
                    }
                }
            } else if (format.isCharFormat()) {
                if (currentFragment.isValid()) {

                    if (currentFragment.charFormat().objectType() == atObjectType) {
                        QString strText = format.property(atPropertyText).toString();
                        QString xmppId = format.property(atPropertyXmppId).toString();
                        QString name = format.property(atPropertyName).toString();

                        _atMessage = true;
                        addJsonItem(array, Type_At, name, xmppId);
                        continue;
                    } else if (currentFragment.charFormat().objectType() == quoteObjectType) {
                        QString strText = format.property(quotePropertySource).toString();
                        QString name = format.property(quotePropertyName).toString();

                        QString content = QString("「 %1: %2 」\n ------------------------- ").arg(name).arg(strText);
                        addJsonItem(array, Type_Text, content);
                        continue;
                    }

                    QString strTmp = currentFragment.text();
                    QString strText = strTmp;
                    // 替换普通空格为特殊空格
                    strText = strText.replace(0xa0, 0x20);
                    strText = strText.replace(0xc2a0, 0x20);

                    QRegExp regExp(
                            "(((ftp|https?)://|www.)[a-zA-Z0-9\\.\\-]+\\.([a-zA-Z0-9]{1,4})"
                            "([^ ^\"^\\<^\\>^\\'^\\;^\\；^\\，^\\。^\\《^\\》^\\￥"
                            "^\\[^\\]^\\r^\\n]*))");

                    auto pos = 0, oldPos = 0;
                    while ((pos = regExp.indexIn(strTmp, pos)) != -1) {
                        QString url = regExp.cap(0);
                        addJsonItem(array, Type_Text, strTmp.mid(oldPos, pos - oldPos));
                        addJsonItem(array, Type_Url, url);
                        oldPos = pos + regExp.matchedLength();
                        pos += regExp.matchedLength();
                    }
                    addJsonItem(array, Type_Text, strTmp.mid(oldPos));
                }
            }

        }
        addJsonItem(array, Type_Text, "\n");
        currentBlock = currentBlock.next();
    }

    // 去除最后一个换行
    array.removeAt(array.size() - 1);
    //先替换全角的@
//    strRet.replace("＠", "@");
    //替换非法字符
//    QRegExp regExp("[\\x00-\\x08\\x0b-\\x0c\\x0e-\\x1f]");
//    strRet.replace(regExp, "");

    // 去掉最后一个换行
//    while (strRet.right(1) == "\n") {
//        strRet = strRet.mid(0, strRet.size() - 1);
//    }
    if(array.isEmpty())
    {
        return QString();
    }
    else
    {
        QJsonDocument document;
        document.setArray(array);
        return document.toJson(QJsonDocument::Compact);
    }
}

/**
  * @函数名   getNetImgPath
  * @功能描述 本地图片地址转换网络图片地址
  * @参数
     QString 网络图片路径
  * @author   cc
  * @date     2018/09/26
  */
QString InputWgt::getNetImgPath(const QString &localPath) {
    QString retPath;

    QFile f(localPath);
    if (f.exists() && _pChatView) {
        std::string netFilePath = g_pMainPanel->getMessageManager()->getNetFilePath(
                std::string((const char *) localPath.toLocal8Bit()));
        retPath = QString(netFilePath.c_str());
        //
        QApplication::processEvents(QEventLoop::AllEvents, 100);
    }
    return retPath;
}

/**
  * @函数名
  * @功能描述
  * @参数
  * @author   cc
  * @date     2018/10/16
  */
void InputWgt::onSnapFinish(const QString &id) {
    if (id != _pChatView->conversionId()) return;

    QString localPath = QString("%1/image/temp/").arg(Platform::instance().getAppdataRoamingUserPath().c_str());
    QString fileName = localPath + QDateTime::currentDateTime().toString("yyyyMMddhhmmsszzz.png");
    bool bret = QApplication::clipboard()->pixmap().save(fileName, "PNG");
    if (bret) {
        // 将截图放到 剪切板
        auto *mimeData = new QMimeData;
//        mimeData->setUrls(QList<QUrl>() << QUrl::fromLocalFile(fileName));
        mimeData->setImageData(QImage(fileName));
        QApplication::clipboard()->setMimeData(mimeData);
        //
        dealFile(fileName, false);
        //发送图片
        setFocus();
        QWidget *wgt = UICom::getInstance()->getAcltiveMainWnd();
        if (wgt->isHidden()) {
            wgt->show();
        }
    }
}

/**
  * @函数名
  * @功能描述
  * @参数
  * @author   cc
  * @date     2018/10/16
  */
void InputWgt::onSnapCancel(const QString &id) {
    if (id != _pChatView->conversionId()) return;

    QWidget *wgt = UICom::getInstance()->getAcltiveMainWnd();
    if (wgt->isHidden()) {
        wgt->show();
    }

    setFocus();
}

void InputWgt::updateGroupMember(const std::map<std::string, QTalk::StUserCard> &member) {

    if(nullptr == _atView)
        return;

    QMutexLocker locker(&_mutex);

    static std::set<std::string> members;
    std::set<std::string> deletedMembers(members);
    members.clear();
    auto it = member.begin();
    int row = 0;
    for (; it != member.end(); it++) {

        if (deletedMembers.find(it->first) !=  deletedMembers.end())
        {
            deletedMembers.erase(it->first);
        }
        members.insert(it->first);

        std::string name = it->second.nickName;
        if(name.empty())
            name = it->second.userName;
        if(name.empty())
            name = QTalk::Entity::JID(it->first.data()).username();

        _atView->addItem(QString::fromStdString(it->second.headerSrc), QString::fromStdString(it->first),
                         QString::fromStdString(name), QString::fromStdString(it->second.searchKey));


        row++;
    }

    // 删除
    for (const std::string& id : deletedMembers)
    {
        _atView->deleteItem(QString::fromStdString(id));
    }
}

void InputWgt::onTextChanged() {
    if (_pChatView->_chatType != QTalk::Enum::GroupChat) {
        return;
    }
    if (nullptr != _atView) {
        if (_atView->isVisible()) {
            _atView->updateFilter();
        } else { }

//#ifndef _WINDOWS
//            if(_showAtView)
//            {
//                _showAtView = false;
//                showAtView();
//            }
//#else
        QTextCursor csor = textCursor();
        int pos = csor.position();
        if (pos >= 0) {
            csor.setPosition(qMax(pos - 5, 0), QTextCursor::KeepAnchor);
            QString strSelectText = csor.selectedText();
            csor.setPosition(pos, QTextCursor::KeepAnchor);
            if (strSelectText.contains("@") || strSelectText.contains("＠")) {
                auto match = strSelectText;
                match.replace("＠", "@");
                auto index = match.lastIndexOf("@");
                match = match.mid(index + 1);
                if((!match.isEmpty() && _atView->match(match)) || match.isEmpty())
                    showAtView();
            }
        }
//#endif
    }
}

void InputWgt::hideEvent(QHideEvent *e) {

    if (_atView && !_atView->isHidden()) {
        _atView->hide();
    }
    QWidget::hideEvent(e);
}

void InputWgt::focusOutEvent(QFocusEvent *e) {


    QTextEdit::focusOutEvent(e);
}

//
void InputWgt::insertAt(const int &pos, const QString &name, const QString &XmppId) {
    // 删除多输入的文本包括@ 选中删除
    QTextCursor csor = textCursor();
    csor.insertText(" ");
    csor.setPosition(pos, QTextCursor::KeepAnchor);
    csor.removeSelectedText();
    //csor.movePosition(QTextCursor::NextWord, QTextCursor::KeepAnchor);
    //csor.removeSelectedText();

    // 添加@ atFormat
    QString atText = QString("@%1 ").arg(name);
    QTextCharFormat atFromat;
    atFromat.setObjectType(atObjectType);
    atFromat.setProperty(atPropertyXmppId, XmppId);
    atFromat.setProperty(atPropertyName, name);
    atFromat.setProperty(atPropertyText, atText);
    atFromat.setProperty(atPropertyFont, this->font());

    csor.insertText(QString(QChar::ObjectReplacementCharacter), atFromat);
}

void InputWgt::dragEnterEvent(QDragEnterEvent *e) {
    e->acceptProposedAction();
}

void InputWgt::dragMoveEvent(QDragMoveEvent *e) {
    e->setDropAction(Qt::MoveAction);
    e->accept();
}

void InputWgt::insertQuote(const QString &userName, const QString &src) {

    //
    QString ret = "";
    QStringList contents = src.split("\n");

    QString tmpContent = contents.first();
    QRegExp regExp("\\[obj type=[\\\\]?\"([^\"]*)[\\\\]?\" value=[\\\\]?\"([^\"]*)[\\\\]?\"(.*)\\]");
    regExp.setMinimal(true);

    int pos = 0;
    while ((pos = regExp.indexIn(tmpContent)) != -1) {
        QString item = regExp.cap(0); // 符合条件的整个字符串
        QString type = regExp.cap(1); // 多媒体类型

        if ("url" == type) {
            tmpContent.replace(pos, item.size(), tr("[链接]"));
        } else if ("image" == type) {
            tmpContent.replace(pos, item.size(), tr("[图片]"));
        } else if ("emoticon" == type) {
            tmpContent.replace(pos, item.size(), tr("[表情]"));
        }
    }

    ret += tmpContent;
    //
    const int maxFontCount = 50;
    bool addDot = (contents.size() > 1) || (ret.size() > maxFontCount);
    //
    if (ret.size() > maxFontCount) {
        ret = ret.mid(0, maxFontCount);
    }
    if (addDot) {
        ret += " ...";
    }
    //
    QTextCharFormat quoteFromat;
    quoteFromat.setObjectType(quoteObjectType);
    quoteFromat.setProperty(quotePropertySource, src);
    quoteFromat.setProperty(quotePropertyName, userName);
    quoteFromat.setProperty(quotePropertyText, ret);
    quoteFromat.setProperty(quotePropertyFont, this->font());

    textCursor().insertText(QString(QChar::ObjectReplacementCharacter), quoteFromat);
    //
    textCursor().insertText(" \n");
    textCursor().movePosition(QTextCursor::End);
    this->setFocus();
}

void InputWgt::showAtView() {
    if(nullptr == _atView)
        return;

    _atView->adjustSize();
    QPoint pos = mapToGlobal(this->rect().topLeft());
    _atView->move(pos.x() - 100, pos.y() - _atView->height());
    _atView->setVisible(true);
    this->setFocus();
}

void InputWgt::onAppDeactivated() {
    if (_atView && _atView->isVisible()) {
        _atView->setVisible(false);
    }
}

void InputWgt:: onPaste() {
    // 获取系统剪切板
    const QMimeData *mimeData = QApplication::clipboard()->mimeData();
    if (mimeData == nullptr) {
        return;
    }

    QByteArray userData = mimeData->data("userData");
    if(!userData.isEmpty())
    {
        cJSON* data = cJSON_Parse(userData);
        if(data == nullptr)
            error_log("userData parse error");

        int itemSize = cJSON_GetArraySize(data);

        for(int i = 0; i < itemSize; i++)
        {
            cJSON* item = cJSON_GetArrayItem(data, i);
            // type 1  文字 2 图片 ...
            int type = QTalk::JSON::cJSON_SafeGetIntValue(item, "type");
            switch (type)
            {
                case 1:
                {
                    std::string content(QTalk::JSON::cJSON_SafeGetStringValue(item, "text"));
                    textCursor().insertText(QString::fromStdString(content));
                    break;
                }
                case 2:
                {
                    std::string imagePath(QTalk::JSON::cJSON_SafeGetStringValue(item, "image"));
                    std::string imageLink(QTalk::JSON::cJSON_SafeGetStringValue(item, "imageLink"));
                    dealFile(QString::fromStdString(imagePath), false, QString::fromStdString(imageLink));
                    break;
                }
                case 3:
                {
                    std::string source(QTalk::JSON::cJSON_SafeGetStringValue(item, "source"));
                    std::string name(QTalk::JSON::cJSON_SafeGetStringValue(item, "name"));

                    insertQuote(name.data(), source.data());
                    break;
                }
                default:
                    break;
            }
        }

        cJSON_Delete(data);
        return;
    }

    QStringList formats = mimeData->formats();
    for(const auto& f: formats)
        info_log("----- {0} ----", f.toStdString());

    // 暂时处理 以后提函数处理
    if (mimeData->hasUrls()) {
        bool isRet = false;

        QList<QUrl> urls = mimeData->urls();
        for (const QUrl &url : urls) {
            if (url.isLocalFile()) {
                //在这里判断是否是图片的文件，和普通文件
                QString strLocalPath = url.toLocalFile();
                dealFile(strLocalPath, false);
                isRet = true;
            }
        }

        if (isRet) {
            return;
        }
    }

    if (mimeData->hasImage()) {
        QImage image = QApplication::clipboard()->image();
        if (!image.isNull()) {
            QString tmpFilePath = QString("%1/%2.png").arg(Platform::instance().getTempFilePath().c_str())
                    .arg(QDateTime::currentMSecsSinceEpoch());

            QDir dir(Platform::instance().getTempFilePath().c_str());
            if (!dir.exists()) {
                dir.mkpath(dir.absolutePath());
            }

            image.save(tmpFilePath);
            dealFile(tmpFilePath, false);
        }
    } else {
        QStringList mimeFormatList;
        mimeFormatList << "application/x-qt-windows-mime;value=\"QChat_RichEdit_Format\""
                       << "application/x-qt-windows-mime;value=\"WeChat_RichEdit_Format\""
                       << "application/x-qt-windows-mime;value=\"QQ_Unicode_RichEdit_Format\""
                       << "QChat_RichEdit_Format"
                       << "WeChat_RichEdit_Format"
                       << "QQ_Unicode_RichEdit_Format";

        QStringList formats = mimeData->formats();
        bool bParsed = false;
        for (const QString &format : formats) {
            info_log(format.toStdString());

            if (mimeFormatList.contains(format)) {
                // todo
            }
            // 判断文本
            if (mimeData->hasText()) {
                QString plainText = mimeData->text();
                if(plainText.toLocal8Bit().size() > 1024 * 64)
                {
                    int ret = QtMessageBox::information(g_pMainPanel, tr("警告"), tr("输入的文本过长，将转换为文件发送!"));
                    if(ret == QtMessageBox::EM_BUTTON_YES)
                    {
                        QString localPath = QString("%1/temp/").arg(Platform::instance().getAppdataRoamingUserPath().c_str());
                        QString fileName = localPath + QDateTime::currentDateTime().toString("yyyyMMddhhmmsszzz") + ".txt";
                        QFile file(fileName);
                        if(file.open(QIODevice::WriteOnly))
                        {
                            file.write(plainText.toLocal8Bit());
                            file.close();
                        }

                        dealFile(fileName, true);
                    }
                    this->setReadOnly(false);
                    clearDocument();
                    _isSending = false;
                    _atMessage = false;
                }
                else
                    this->insertPlainText(plainText);
                break;
            }
        }
    }
}

void InputWgt::removeGroupMember(const std::string &memberId) {
    if (_atView) {
        _atView->deleteItem(QString::fromStdString(memberId));
    }
}

/**
 *
 */
void InputWgt::onCopy()
{
    //
    QString mimeDataText;
    cJSON* objs = cJSON_CreateArray();
    auto *mimeData = new QMimeData;

    auto cursor = this->textCursor();
    bool hasSelect = cursor.hasSelection();
    if (!hasSelect) {
        selectAll();
    }

    auto startPos = cursor.selectionStart();
    auto endPos = cursor.selectionEnd();

    QTextDocument *dt = this->document();
    QTextBlock currentBlock = dt->begin();
    QTextBlock::iterator it;
    while (currentBlock.isValid())
    {
        bool isSelected = false;
        for (it = currentBlock.begin(); !(it.atEnd()); it++)
        {
            QTextFragment currentFragment = it.fragment();
            QTextFormat format = currentFragment.charFormat();
            if(!currentFragment.isValid())
                continue;

            std::string str = currentFragment.text().toStdString();
            auto curPos = currentFragment.position();
            auto curMaxPos = curPos + currentFragment.length();
            if(curPos > endPos || curMaxPos < startPos)
                continue;

            isSelected = true;

            if (currentFragment.charFormat().isImageFormat())
            {
                QTextImageFormat newImageFormat = currentFragment.charFormat().toImageFormat();
                if (newImageFormat.isValid())
                {
                    QString imagePath = newImageFormat.stringProperty(ImagePath);
                    QString imageLink = newImageFormat.stringProperty(ImageUrl);

                    cJSON* obj = cJSON_CreateObject();
                    cJSON_AddNumberToObject(obj, "type", 2); // 1  文字 2 图片 ...
                    cJSON_AddStringToObject(obj, "imageLink", imageLink.toStdString().data());
                    cJSON_AddStringToObject(obj, "image", imagePath.toStdString().data());
                    cJSON_AddItemToArray(objs, obj);
                }
            }
            else if (format.isCharFormat())
            {

                cJSON* obj = cJSON_CreateObject();

                if (currentFragment.charFormat().objectType() == atObjectType)
                {
                    QString strText = format.property(atPropertyText).toString();
                    mimeDataText += strText;

                    cJSON_AddNumberToObject(obj, "type", 1); // 1  文字 2 图片 ...
                    cJSON_AddStringToObject(obj, "text", strText.toStdString().data());
                    cJSON_AddItemToArray(objs, obj);

                    continue;
                }
                else if (currentFragment.charFormat().objectType() == quoteObjectType)
                {
                    QString strText = format.property(quotePropertySource).toString();
                    QString name = format.property(quotePropertyName).toString();

                    QString content = QString("「 %1: %2 」\n ------------------------- ").arg(name).arg(strText);
                    mimeDataText += content;

                    cJSON_AddNumberToObject(obj, "type", 3); // 1  文字 2 图片 3 引用消息...
                    cJSON_AddStringToObject(obj, "source", strText.toStdString().data());
                    cJSON_AddStringToObject(obj, "name", name.toStdString().data());
                    cJSON_AddItemToArray(objs, obj);

                    continue;
                }

                int sp = qMax(startPos, curPos) - curPos;
                int ep = qMin(endPos, curMaxPos) - curPos;
                QString strTmp = currentFragment.text().mid(sp, ep - sp);
                mimeDataText += strTmp;

                cJSON_AddNumberToObject(obj, "type", 1);
                cJSON_AddStringToObject(obj, "text", strTmp.toStdString().data());
                cJSON_AddItemToArray(objs, obj);
            }

        }

        if(isSelected )
        {
            cJSON* obj = cJSON_CreateObject();
            cJSON_AddNumberToObject(obj, "type", 1);
            cJSON_AddStringToObject(obj, "text", "\n");
            cJSON_AddItemToArray(objs, obj);
            mimeDataText += '\n';
        }

        currentBlock = currentBlock.next();
    }

    std::string userData = QTalk::JSON::cJSON_to_string(objs);
    mimeData->setText(mimeDataText);
    mimeData->setData("userData", userData.data());
    QApplication::clipboard()->setMimeData(mimeData);

    if(!hasSelect)
        moveCursor(QTextCursor::End);
}

/**
 *
 * @param member
 */
void InputWgt::updateGroupMemberInfo(const std::vector<QTalk::StUserCard> &member)
{
    if(_atView)
        _atView->updateGroupMemberInfo(member);
}

void InputWgt::insertAt(const std::string &xmppId)
{
    if(xmppId.empty())
        return;

    std::string name = QTalk::getUserNameNoMask(xmppId);

    QString atText = QString("@%1 ").arg(name.data());
    QTextCharFormat atFromat;
    atFromat.setObjectType(atObjectType);
    atFromat.setProperty(atPropertyXmppId, xmppId.data());
    atFromat.setProperty(atPropertyName, name.data());
    atFromat.setProperty(atPropertyText, atText);
    atFromat.setProperty(atPropertyFont, this->font());

    textCursor().insertText(QString(QChar::ObjectReplacementCharacter), atFromat);
    this->setFocus();
}

void InputWgt::showMessage(const QString& message)
{
    QtMessageBox::warning(g_pMainPanel, tr("警告"), message);
    this->setReadOnly(false);
    this->setFocus();
}

void InputWgt::setContentByJson(const QString &json) {

    auto document = QJsonDocument::fromJson(json.toUtf8());
    if(document.isNull()){

    }
    else
    {
        QJsonArray array = document.array();
        for(auto && i : array)
        {
            QJsonObject obj = i.toObject();
            int key = obj.value("key").toInt();
            QString value = obj.value("value").toString();
            switch (key)
            {
                case Type_Text:
                case Type_Url:
                {
                    this->insertPlainText(value);
                    break;
                }
                case Type_Image:
                {
                    QString imageLink = obj.value("info").toString();
                    dealFile(value, false, imageLink);
                    break;
                }
                case Type_At:
                {
                    QString xmppId = obj.value("info").toString();
                    insertAt(xmppId.toStdString());
                    break;
                }
                case Type_Invalid:
                default:
                    break;
            }
        }
    }
}

void InputWgt::inputMethodEvent(QInputMethodEvent *e) {

//    if(!hasFocus())
//        e->setCommitString("");
    QTextEdit::inputMethodEvent(e);
}
