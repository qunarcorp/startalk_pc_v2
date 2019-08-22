#ifndef EmoticonStruct_H_
#define EmoticonStruct_H_

#include <QString>
#include <QMap>
#include <QVector>

struct StEmoticonItem
{
	QString emoid;
	QString shortcut;
	QString tooltip;
	QString fileorg;
	QString filefixd;
};

class UnorderMapKey : public QString
{
public:
	UnorderMapKey(){}
	UnorderMapKey(const char* str): QString(str){}
	UnorderMapKey(const QString& str): QString(str){}
	UnorderMapKey(const std::string& str): QString(QString::fromStdString(str)){}
	bool operator<(const UnorderMapKey& )const { return true; }
};

struct StEmoticon
{
    bool    isShowALl;
	QString pkgid;
	QString name;
	QString iconPath;
	QMap<UnorderMapKey, StEmoticonItem> mapEmoticon; // <shortcut, StEmoticonItem>

    StEmoticon():isShowALl(false) {}
};

#endif // EmoticonStruct_H_
