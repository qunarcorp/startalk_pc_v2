//
// Created by QITMAC000260 on 2019-06-26.
//

#ifndef QTALK_V2_ITEMWGT_H
#define QTALK_V2_ITEMWGT_H

#include <QFrame>
#include <QMenu>

struct StTextData {

};

struct StFileData {
    int     status;
    QString fileName;
    QString fileSize;
    QString fileUrl;
    QString filePath;
    QString fileMd5;
    QString iconPath;
};

struct StSearchData {
    QString searchKey;
};

struct StData {
    int type;
    QString messageId;
    qint64  time;
    QString userName;
    int     direction;
    QString content;

    StFileData fileData;
    StSearchData searchData;
};

enum {

    EM_FILE_STATUS_INVALID,
    EM_FILE_STATUS_UN_DOWNLOAD,
    EM_FILE_STATUS_DOWNLOADING,
    EM_FILE_STATUS_DOWNLOADED,
};

class NameTitleLabel : public QFrame {

public:
    explicit NameTitleLabel(int dir, QString  name, QString  time, QWidget* parent = nullptr);
    ~NameTitleLabel() override = default;

protected:
    void paintEvent(QPaintEvent* e) override ;

private:
    int _dir;
    QString _name;
    QString _time;
};

class TextBrowser;
class TextItemWgt : public QFrame {

public:
    explicit TextItemWgt(StData data, QWidget* parent);
    ~TextItemWgt() override = default;

public:
    QSize getSize();

private:
    StData _data;
    TextBrowser* _browser;
};

class SearchTextItemWgt : public QFrame {
    Q_OBJECT
public:
    explicit SearchTextItemWgt(StData data, QWidget* parent);
    ~SearchTextItemWgt() override = default;

Q_SIGNALS:
    void sgPositionMessage(qint64 );

private:
    StData _data;
};

class SearchFileItemWgt : public QFrame {
    Q_OBJECT
public:
    explicit SearchFileItemWgt(StData data, QWidget* parent);
    ~SearchFileItemWgt() override = default;

Q_SIGNALS:
    void sgPositionMessage(qint64 );

private:
    StData _data;
};

class FileRoundProgressBar;
class QToolButton;
class FileItemWgt : public QFrame {
    Q_OBJECT
public:
    explicit FileItemWgt(StData data, bool showTitle, QWidget* parent);
    ~FileItemWgt() override = default;

public:
    void setProcess(double speed, double dtotal, double dnow, double utotal, double unow);

private:
    StData _data;
    FileRoundProgressBar *_processBar;
    QToolButton* _openPathBtn;
};

class TimeItemWgt : public QFrame {

public:
    explicit TimeItemWgt(QString content, QWidget* parent);
    ~TimeItemWgt() override = default;

protected:
    void paintEvent(QPaintEvent* e) override ;

private:
    QString _conent;
};

#endif //QTALK_V2_ITEMWGT_H
