//
// Created by cc on 19-1-9.
//

#ifndef QTALK_V2_ATMESSAGETIP_H
#define QTALK_V2_ATMESSAGETIP_H

#include <QFrame>
#include <QListWidget>
#include <QLabel>
#include <QMutexLocker>

struct StShowAtInfo {
    bool            isAtAll;
    QString          name;
    QListWidgetItem* atItem;

    StShowAtInfo(): isAtAll(false) {}
    StShowAtInfo(bool all, const QString& n, QListWidgetItem* item)
        : isAtAll(all), name(n), atItem(item)
    {

    }
};

class ChatMainWgt;
class AtMessageTip : public QFrame
{
	Q_OBJECT
public:
    AtMessageTip(ChatMainWgt* chatMainWgt);
    ~AtMessageTip();

public:
    void addAt(const StShowAtInfo& info);
    void showAtInfo();

protected:
    virtual void mousePressEvent(QMouseEvent* e);

private:
    ChatMainWgt*      _pChatMainWgt;
    QLabel*           _pNameLabel;
    QLabel*           _pAtLabel;

private:
    QList<StShowAtInfo> _arAtItems;

private:
    QMutex             _mutex;
};


#endif //QTALK_V2_ATMESSAGETIP_H
