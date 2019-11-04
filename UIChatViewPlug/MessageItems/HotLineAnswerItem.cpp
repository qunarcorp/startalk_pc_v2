//
// Created by QITMAC000260 on 2019/08/29.
//

#include "HotLineAnswerItem.h"

#include "ChatViewMainPanel.h"
#include <QSpacerItem>
#include <QJsonDocument>
#include <QDesktopServices>
#include <QMouseEvent>
#include <QJsonDocument>
#include <QFileInfo>
#include <QJsonObject>
#include <QScrollBar>
#include <QJsonArray>
#include <ChatUtil.h>
#include <QTextFormat>
#include "blocks/block_define.h"
#include "../../UICom/qimage/qimage.h"
#include "TextBrowser.h"
#include "ChatMainWgt.h"
#include "../../UICom/StyleDefine.h"
#include "../../Platform/Platform.h"
#include "../../WebService/WebService.h"

#define DEM_LINK_HTML "<a href=\"%1\" style=\"text-decoration:none; color:rgba(%2);\">%1</a>"
#define DEM_AT_HTML "<span style=\"color:#FF4E3F;\">%1</span>"
#define DEM_IMAGE_STR " [图片] "

extern ChatViewMainPanel *g_pMainPanel;

class DottedLine : public QFrame {
public:
    explicit DottedLine(QWidget* parent) :QFrame(parent){
        setFixedHeight(2);
    }

protected:
    void paintEvent(QPaintEvent* e) override {
        QPainter painter(this);
        QRect rect = contentsRect();
        painter.setPen(QPen(QColor(136, 136, 136), 1, Qt::DashLine));
        painter.drawLine(rect.bottomLeft(), rect.bottomRight());
        QFrame::paintEvent(e);
    }
};

HotLineTipDelegate::HotLineTipDelegate(QWidget* parent)
    :QStyledItemDelegate(parent)
{
    _font.setWeight(400);
    _font.setPixelSize(14);
}

void HotLineTipDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const {
    painter->save();
    painter->setRenderHint(QPainter::TextAntialiasing);

    QRect rect = option.rect;
    painter->fillRect(rect, QTalk::StyleDefine::instance().getHotLineTipItemColor());
    QString text = index.data(EM_ITEM_DATA_TYPE_TEXT).toString();

    painter->setPen(Qt::NoPen);
    painter->setBrush(QColor(0, 202, 190));
    painter->drawRoundedRect(rect.x(), rect.y() + (rect.height() - 4) / 2, 4, 4, 1, 1);

    painter->setPen(QTalk::StyleDefine::instance().getHotLineTipItemFontColor());
    painter->setFont(_font);
    painter->drawText(rect.x() + 12, rect.y(), rect.width() - 12, rect.height(), Qt::AlignVCenter, text);
    painter->restore();
}

bool HotLineTipDelegate::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option,
                                     const QModelIndex &index) {
    if(event->type() == QEvent::MouseButtonPress)
    {
        QString type = index.data(EM_ITEM_DATA_TYPE_ACTION_TYPE).toString();
        if( "text" == type )
        {
            QString text = index.data(EM_ITEM_DATA_TYPE_TEXT).toString();
            QString uid = index.data(EM_ITEM_DATA_TYPE_ACTION_UID).toString();

            if(g_pMainPanel)
                g_pMainPanel->sendTextMessage(QTalk::Entity::UID(uid), QTalk::Enum::Consult, text.toStdString());
        }
        else
        {
            QString url = index.data(EM_ITEM_DATA_TYPE_ACTION_URL).toString();
            if(g_pMainPanel)
                g_pMainPanel->sendGetRequest(url.toStdString());
        }
    }
    return QStyledItemDelegate::editorEvent(event, model, option, index);
}

QSize HotLineTipDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const {
    const QSize& size = QStyledItemDelegate::sizeHint(option, index);
    auto h = index.data(EM_ITEM_DATA_TYPE_ACTION_HEIGHT).toInt();
    return {size.width(), h};
}

/** **/
HotLineAnswerItem::HotLineAnswerItem(const QTalk::Entity::ImMessageInfo &msgInfo, ChatMainWgt *parent) :
        MessageItemBase(msgInfo, parent),
        _textBrowser(nullptr),
        _pTipListView(nullptr),
        _pTipListModel(nullptr),
        _pMainWgt(parent) {
    init();
    connect(_textBrowser, &TextBrowser::anchorClicked, this, &HotLineAnswerItem::onAnchorClicked);
    connect(_textBrowser, &TextBrowser::sgImageClicked, this, &HotLineAnswerItem::onImageClicked);
}

HotLineAnswerItem::~HotLineAnswerItem() {
    if(nullptr != _textBrowser)
    {
        delete _textBrowser;
        _textBrowser = nullptr;
    }
}

void HotLineAnswerItem::init() {
    initLayout();
}

void HotLineAnswerItem::initLayout() {

    _contentSize = QSize(370, 160);
    _mainMargin = QMargins(15, 0, 20, 0);
    _mainSpacing = 10;
    if (QTalk::Entity::MessageDirectionSent == _msgDirection) {
        _headPixSize = QSize(0, 0);
        _nameLabHeight = 0;
        _leftMargin = QMargins(0, 0, 0, 0);
        _rightMargin = QMargins(0, 0, 0, 0);
        _leftSpacing = 0;
        _rightSpacing = 0;
        initSendLayout();
    } else if (QTalk::Entity::MessageDirectionReceive == _msgDirection) {
        _headPixSize = QSize(28, 28);
        _nameLabHeight = 16;
        _leftMargin = QMargins(0, 10, 0, 0);
        _rightMargin = QMargins(0, 0, 0, 0);
        _leftSpacing = 0;
        _rightSpacing = 4;
        initReceiveLayout();
    }
    _nameLabHeight = 0;
    setContentsMargins(0, 5, 0, 5);
}

QSize HotLineAnswerItem::itemWdtSize() {
    int height = qMax(_mainMargin.top() + _nameLabHeight + _mainSpacing + _contentFrm->height() + _mainMargin.bottom(),
                      _headPixSize.height()); // 头像和文本取大的
    int width = _contentFrm->width();
    if(nullptr != _readStateLabel)
    {
        height += 12;
    }
    return {width, height + 8};
}

/**
  * @函数名
  * @功能描述
  * @参数
  * @author cc
  * @date 2018.10.19
  */
void HotLineAnswerItem::initSendLayout() {
    auto *mainLay = new QHBoxLayout(this);
    mainLay->setContentsMargins(_mainMargin);
    mainLay->setSpacing(_mainSpacing);
    mainLay->addWidget(_btnShareCheck);
    auto *horizontalSpacer = new QSpacerItem(40, 1, QSizePolicy::Expanding, QSizePolicy::Fixed);
    mainLay->addItem(horizontalSpacer);
    auto *rightLay = new QVBoxLayout;
    rightLay->setContentsMargins(_rightMargin);
    mainLay->addLayout(rightLay);
    if (!_contentFrm) {
        _contentFrm = new QFrame(this);
    }
    _contentFrm->setObjectName("messSendContentFrm");
    _contentFrm->setFixedWidth(_contentSize.width());
    //
    auto* tmpLay = new QHBoxLayout;
    tmpLay->setMargin(0);
    tmpLay->setSpacing(5);
    if(nullptr == _sending)
    {
        _sending = new HeadPhotoLab(":/chatview/image1/messageItem/loading.gif", 10, false, false, true, this);
        tmpLay->addItem(new QSpacerItem(10, 10, QSizePolicy::Expanding));
        tmpLay->addWidget(_sending);
        bool startMovie = (0 == _msgInfo.ReadedTag && 0 == _msgInfo.State);
        _sending->setVisible(startMovie);
        if(startMovie)
            _sending->startMovie();
    }
    if(nullptr != _resending)
    {
        tmpLay->addWidget(_resending);
        _resending->setVisible(false);
    }
    tmpLay->addWidget(_contentFrm);
    tmpLay->setAlignment(_contentFrm, Qt::AlignRight);
    rightLay->addLayout(tmpLay);
    if (nullptr != _readStateLabel) {
        auto *rsLay = new QHBoxLayout;
        rsLay->addItem(new QSpacerItem(10, 10, QSizePolicy::Expanding));
        rsLay->setMargin(0);
        rsLay->addWidget(_readStateLabel);
        rightLay->addLayout(rsLay);
    }
    mainLay->setStretch(0, 1);
    mainLay->setStretch(1, 0);

    initContentLayout();
}

/**
  * @函数名
  * @功能描述
  * @参数
  * @author cc
  * @date 2018.10.19
  */
void HotLineAnswerItem::initReceiveLayout() {
    auto *mainLay = new QHBoxLayout(this);
    mainLay->setContentsMargins(_mainMargin);
    mainLay->setSpacing(_mainSpacing);
    mainLay->addWidget(_btnShareCheck);
    auto *leftLay = new QVBoxLayout;
    leftLay->setContentsMargins(_leftMargin);
    leftLay->setSpacing(_leftSpacing);
    mainLay->addLayout(leftLay);
    if (!_headLab) {
        _headLab = new HeadPhotoLab;
    }
    _headLab->setFixedSize(_headPixSize);
    _headLab->setHead(QString::fromStdString(_msgInfo.HeadSrc), HEAD_RADIUS);
    _headLab->installEventFilter(this);
    leftLay->addWidget(_headLab);
    auto *vSpacer = new QSpacerItem(1, 1, QSizePolicy::Fixed, QSizePolicy::Expanding);
    leftLay->addItem(vSpacer);

    leftLay->setStretch(0, 0);
    leftLay->setStretch(1, 1);

    auto *rightLay = new QVBoxLayout;
    rightLay->setContentsMargins(_rightMargin);
    rightLay->setSpacing(_rightSpacing);
    mainLay->addLayout(rightLay);

    if (!_contentFrm) {
        _contentFrm = new QFrame(this);
    }
//    _contentFrm->setObjectName("messReceiveContentFrm");
    _contentFrm->setFixedWidth(_contentSize.width());
    rightLay->addWidget(_contentFrm);
    rightLay->setStretch(0, 0);
    rightLay->setStretch(1, 1);

    auto *horizontalSpacer = new QSpacerItem(40, 1, QSizePolicy::Expanding, QSizePolicy::Fixed);
    mainLay->addItem(horizontalSpacer);
    mainLay->setStretch(0, 0);
    mainLay->setStretch(1, 1);

    initContentLayout();
}

qreal HotLineAnswerItem::getRealString(const QString &src, qreal &lastLineWidth) {

    const qreal width = 500;

    QFontMetricsF fm(_textBrowser->font());
    qreal nCount = src.count("\n");
    qreal nMaxWidth = 0;
    if (nCount == 0) {
        nMaxWidth = lastLineWidth = fm.width(src) + lastLineWidth;
        if (nMaxWidth > width) {
            nMaxWidth = width;
        }
    } else {
        QStringList valLst = src.split("\n");
        for (int i = 0; i < (nCount + 1); i++) {

            const QString &value = valLst.at(i);

            qreal fmWidth = fm.width(value);
            if (i == 0)
                lastLineWidth = fmWidth += lastLineWidth;
            else
                lastLineWidth = fmWidth;

            nMaxWidth = fmWidth > nMaxWidth ? fmWidth : nMaxWidth;
            if (fm.width(value) > width) {
                nMaxWidth = width;
                break;
            }
        }
    }
    return nMaxWidth;
}

/**
 *
 */
void HotLineAnswerItem::initContentLayout() {

    qreal height = 40;
    const qreal width = 500;
    //
    auto* contentLay = new QVBoxLayout(_contentFrm);
    contentLay->setContentsMargins(0, 0, 0, 10);
    contentLay->setSpacing(0);
    //
    QJsonDocument jsonDocument = QJsonDocument::fromJson(_msgInfo.ExtendedInfo.data());
    if (!jsonDocument.isNull()) {
        QJsonObject jsonObject = jsonDocument.object();
        //
        QString question = jsonObject.value("question").toString();
        QString content = jsonObject.value("content").toString();
        QString listTips = jsonObject.value("listTips").toString();
        QJsonObject listArea = jsonObject.value("listArea").toObject();
        QJsonArray items = listArea.value("items").toArray();
        //
        QString bottom_tips = jsonObject.value("bottom_tips").toString();
        QJsonArray bottom = jsonObject.value("bottom").toArray();

        bool hasTip = !listTips.isEmpty() && !items.isEmpty();
        bool hasBottom = !bottom_tips.isEmpty() && !bottom.isEmpty();
        //
        bool addLine = false;
        //
        if(!content.isEmpty())
        {
            auto* frm = new QFrame(this);
            if(!hasBottom && !hasTip)
                frm->setObjectName("HotLineContentFrmNoBottom");
            else
                frm->setObjectName("HotLineContentFrm");
            auto* lay = new QHBoxLayout(frm);
            lay->setMargin(10);
            contentLay->addWidget(frm);
            addLine = true;
            //
            QVector<StTextMessage> msgs;
            _pMainWgt->analysisMessage(msgs, content.toStdString(), _msgInfo.MsgId);

            if(nullptr == _textBrowser)
            {
                _textBrowser = new TextBrowser;
                lay->addWidget(_textBrowser);
            }

            _textBrowser->setFixedWidth(width);
            _textBrowser->document()->setTextWidth(width);
            QTextCharFormat f;
            f.setFontLetterSpacingType(QFont::AbsoluteSpacing);
            f.setFontWordSpacing(0);
            f.setFontLetterSpacing(0);

            for (const auto &msg : msgs) {
                switch (msg.type) {
                    case StTextMessage::EM_TEXT:
                        _textBrowser->insertPlainText(msg.content);
                        break;
                    case StTextMessage::EM_IMAGE:
                    case StTextMessage::EM_EMOTICON: {
//                        QString imagePath = msg.content;
                        QString imagePath = QTalk::GetImagePathByUrl(msg.imageLink.toStdString()).data();
                        qreal imageWidth = msg.imageWidth;
                        qreal imageHeight = msg.imageHeight;

                        bool isDownload = false;
                        if(QFile::exists(imagePath) && QFileInfo(imagePath).isFile())
                        {
                            if (QPixmap(imagePath).isNull()) {
                                QString realPath = QTalk::qimage::instance().getRealImagePath(imagePath);
                                if (QPixmap(realPath).isNull()) {
                                    imageWidth = imageHeight = 80;
                                } else {
                                    isDownload = true;
                                    imagePath = realPath;
                                }
                            }
                            else
                                isDownload = true;
                        }

                        QTextImageFormat imageFormat;
                        imageFormat.setWidth(imageWidth);
                        imageFormat.setHeight(imageHeight);
                        imageFormat.setName(isDownload ? imagePath : msg.imageLink);
                        imageFormat.setProperty(imagePropertyLink, msg.imageLink);

                        _textBrowser->textCursor().insertImage(imageFormat);
                        _textBrowser->setCurrentCharFormat(f);
                        //
                        break;
                    }
                    case StTextMessage::EM_LINK: {
                        QTextCharFormat linkFormat = _textBrowser->textCursor().charFormat();
                        linkFormat.setForeground(QBrush(QTalk::StyleDefine::instance().getLinkUrl()));
                        linkFormat.setAnchor(true);
                        linkFormat.setAnchorHref(msg.content);
                        linkFormat.setAnchorName(msg.content);
                        _textBrowser->textCursor().insertText(msg.content, linkFormat);
                        break;
                    }
                    case StTextMessage::EM_ATMSG: {
                        QString tmpContent = QString(DEM_AT_HTML).arg(msg.content);
                        _textBrowser->insertHtml(tmpContent);
                        _textBrowser->setCurrentCharFormat(f);
                        break;
                    }
                    default:
                        break;
                }
            }

            _textBrowser->setFixedSize(_textBrowser->sizeHint());

            height += _textBrowser->sizeHint().height();
        }

        //
        if(hasTip)
        {
            //
            if(addLine)
                contentLay->addWidget(new DottedLine(this));



            auto* frm = new QFrame(this);
            frm->setObjectName("HotLineTipFrm");
            auto* lay = new QVBoxLayout(frm);
            lay->setMargin(10);
            contentLay->addWidget(frm);

            auto *tipLabel = new QLabel(listTips, this);
            tipLabel->setObjectName("hotLineTipLabel");
            lay->addWidget(tipLabel);
            tipLabel->setFixedHeight(25);
            height += 25;
            //
            if(!addLine)
                lay->addWidget(new DottedLine(this));
            addLine = true;
            //
            if(nullptr == _pTipListView)
                _pTipListView = new QListView(this);
            if(nullptr == _pTipListModel)
                _pTipListModel = new QStandardItemModel(this);

            _pTipListView->setModel(_pTipListModel);
            lay->addWidget(_pTipListView);
            auto* hotLineTipDelegate = new HotLineTipDelegate(this);
            _pTipListView->verticalScrollBar()->setVisible(false);
            _pTipListView->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
            _pTipListView->verticalScrollBar()->setSingleStep(10);
            _pTipListView->setItemDelegate(hotLineTipDelegate);
            _pTipListView->setFrameShape(QFrame::NoFrame);
            _pTipListView->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
            _pTipListView->setEditTriggers(QAbstractItemView::NoEditTriggers);

            qreal listHeight = 0;
            QFontMetricsF fontMetricsF(hotLineTipDelegate->_font);
            //
            auto it = items.begin();
            while (it != items.end())
            {
                QJsonObject obj = it->toObject();
                QJsonObject event = obj.value("event").toObject();
                QString type = event.value("type").toString();
                QString url = event.value("url").toString();
                QString text = obj.value("text").toString();

                int h = fontMetricsF.boundingRect(QString("·    %1").arg(text)).height() + 12;

                auto* item = new QStandardItem;
                item->setData(text, EM_ITEM_DATA_TYPE_TEXT);
                item->setData(type, EM_ITEM_DATA_TYPE_ACTION_TYPE);
                item->setData(url, EM_ITEM_DATA_TYPE_ACTION_URL);
                item->setData(h, EM_ITEM_DATA_TYPE_ACTION_HEIGHT);
                item->setData(_msgInfo.From.data(), EM_ITEM_DATA_TYPE_ACTION_UID);

                _pTipListModel->appendRow(item);
                listHeight += h;
                // 换行
                it++;
            }

            _pTipListView->setFixedHeight(listHeight);
            height += listHeight;
        }


        if(hasBottom)
        {
            //
            if(addLine)
                contentLay->addWidget(new DottedLine(this));

            auto* frm = new QFrame(this);
            frm->setObjectName("HotLineTipFrm");
            auto* lay = new QVBoxLayout(frm);
            lay->setMargin(15);
            lay->setSpacing(10);
            contentLay->addWidget(frm);

            auto *tipLabel = new QLabel(bottom_tips, this);
            tipLabel->setObjectName("hotLineTipLabel");
            lay->addWidget(tipLabel);
            tipLabel->setFixedHeight(25);
            height += 25;

            auto* bottomLay = new QHBoxLayout;
            bottomLay->setMargin(0);
            bottomLay->setSpacing(15);
            lay->addLayout(bottomLay);

            //
            bool has_check = jsonObject.contains("bottom_check");
            int check_id = jsonObject.value("bottom_check").toInt();
            //
            std::vector<ReqButton*> btns;
            auto it = bottom.begin();
            while (it != bottom.end())
            {
                QJsonObject obj = it->toObject();

                int id = obj.value("id").toInt();
                QString url = obj.value("url").toString();
                QString text = obj.value("text").toString();

                auto *reqButton = new ReqButton(id, text, url, this);
                reqButton->setFixedHeight(30);
                bottomLay->addWidget(reqButton);
                //
                btns.push_back(reqButton);

                if(has_check)
                {
                    reqButton->setReqEnable(false);
                    if(id == check_id)
                        emit reqButton->sgSendReq(id, url);
                }

                height += 30;

                it++;
            }

            if(!has_check)
            {
                for(const auto* btn : btns)
                {
                    connect(btn, &ReqButton::sgSendReq, [this, btns, jsonObject](int id, const QString& url){
                        for(auto* tmpBtn : btns)
                            tmpBtn->setReqEnable(false);

                        if(g_pMainPanel)
                            g_pMainPanel->sendGetRequest(url.toStdString());

                        //
                        QJsonDocument newDoc;
                        QJsonObject newObj(jsonObject);
                        newObj.insert("bottom_check", id);
                        newDoc.setObject(newObj);
                        QByteArray byteArray =newDoc.toJson(QJsonDocument::Compact);

                        if(g_pMainPanel)
                            g_pMainPanel->updateMessageExtendInfo(_msgInfo.MsgId, byteArray.data());
                    });
                }
            }

            bottomLay->addItem(new QSpacerItem(10, 10, QSizePolicy::Expanding));
        }
    }
    else
    {

    }

    _contentFrm->setFixedSize(width + 20, height);
}

void HotLineAnswerItem::onImageDownloaded(const QString &link) {

    QString imagePath = QTalk::GetImagePathByUrl(link.toStdString()).data();

    QPixmap pixmap = QTalk::qimage::instance().loadPixmap(imagePath, false);
    _textBrowser->document()->addResource(QTextDocument::ImageResource,
                                          link, pixmap);
    _textBrowser->setLineWrapColumnOrWidth(_textBrowser->lineWrapColumnOrWidth());
}

void HotLineAnswerItem::onAnchorClicked(const QUrl &url) {
    QString strUrl = url.toString();
    bool userDftBrowser = AppSetting::instance().getOpenLinkWithAppBrowser();
    if (!strUrl.startsWith("http")) {
        strUrl = (QString("http://%1").arg(strUrl));
    }
    if (userDftBrowser)
        WebService::loadUrl(QUrl(strUrl));
    else
        QDesktopServices::openUrl(QUrl(strUrl));
}

void HotLineAnswerItem::onImageClicked(const QString& path, const QString& link) {
    QString imagePath = QTalk::GetImagePathByUrl(link.toStdString()).data();
    emit g_pMainPanel->sgShowPicture(imagePath, link);
}
