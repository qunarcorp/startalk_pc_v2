#ifndef _GROUPTOPIC_H_
#define _GROUPTOPIC_H_

#include <QFrame>
#include <QLabel>
#include <QTextEdit>
#include <QPushButton>

class GroupTopic : public QFrame
{
public:
	explicit GroupTopic(QWidget* parent = nullptr);
	~GroupTopic() override;

public:
	void setTopic(const QString& topic);

protected:
	void initUi();
    bool eventFilter(QObject *, QEvent *) override;

private:
	QTextEdit*   _pTopicEdit;
	QPushButton* _pBtnShowMore;
};

#endif//_GROUPTOPIC_H_