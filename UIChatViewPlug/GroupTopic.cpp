#include "GroupTopic.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QEvent>
#include <QScrollBar>

GroupTopic::GroupTopic(QWidget* parent)
	: QFrame(parent)
	, _pTopicEdit(nullptr)
	, _pBtnShowMore(nullptr)
{
	setObjectName("GroupTopic");
	setFrameShape(QFrame::NoFrame);
	initUi();
	setTopic("");
}


GroupTopic::~GroupTopic()
{
	if (nullptr != _pBtnShowMore)
	{
		delete _pBtnShowMore;
		_pBtnShowMore = nullptr;
	}
}

/**
  * @函数名   setTopic
  * @功能描述 设置群公告
  * @参数
  * @author   cc
  * @date     2018/10/09
  */
void GroupTopic::setTopic(const QString& topic)
{
	if (topic.isEmpty())
	{
		_pTopicEdit->setText(tr("\n\n  "
                                      "            暂无公告"));
		_pTopicEdit->setAlignment(Qt::AlignCenter);
	}
	else
	{
		_pTopicEdit->setText(topic);
		_pTopicEdit->setAlignment(Qt::AlignLeft);
	}
}

/**
  * @函数名   initUi
  * @功能描述 初始化ui
  * @参数
  * @author   cc
  * @date     2018/10/09
  */
void GroupTopic::initUi()
{
	QLabel *pLabelTitle = new QLabel(tr("群公告"));
	_pTopicEdit = new QTextEdit(this); //
    _pTopicEdit->installEventFilter(this);
    _pTopicEdit->setAcceptRichText(false);
    _pTopicEdit->verticalScrollBar()->setVisible(false);
	_pBtnShowMore = new QPushButton;

	_pTopicEdit->setReadOnly(true);
	_pTopicEdit->setFrameShape(QFrame::NoFrame);

	pLabelTitle->setObjectName("GroupTopicTitle");
	_pTopicEdit->setObjectName("TopicEdit");
	_pBtnShowMore->setObjectName("BtnShowMore");

	pLabelTitle->setContentsMargins(13, 15, 0, 0);
	_pTopicEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	auto* topLayout = new QHBoxLayout;
	topLayout->setMargin(0);
	topLayout->addWidget(pLabelTitle);
	topLayout->addItem(new QSpacerItem(10, 10, QSizePolicy::Expanding, QSizePolicy::Fixed));
	//topLayout->addWidget(_pBtnShowMore);

	auto* layout = new QVBoxLayout;
	layout->setMargin(0);
	layout->addLayout(topLayout);
	layout->addWidget(_pTopicEdit);

	setLayout(layout);
}

/**
 *
 * @param o
 * @param e
 * @return
 */
bool GroupTopic::eventFilter(QObject *o, QEvent *e)
{
    if(o == _pTopicEdit)
    {
        if(e->type() == QEvent::Enter)
        {
            _pTopicEdit->verticalScrollBar()->setVisible(true);
        }
        else if(e->type() == QEvent::Leave)
        {
            _pTopicEdit->verticalScrollBar()->setVisible(false);
        }
    }

    return QFrame::eventFilter(o, e);
}
