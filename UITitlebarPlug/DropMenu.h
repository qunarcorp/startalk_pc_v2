#include <utility>

//
// Created by cc on 19-1-20.
//
#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif
#ifndef QTALK_V2_DROPMENU_H
#define QTALK_V2_DROPMENU_H

#include "../CustomUi/UShadowWnd.h"
#include <QLabel>
#include <iostream>
#include <QPainter>
#include <QLabel>
#include "../CustomUi/HeadPhotoLab.h"
#include "AboutWnd.h"
#include "SystemSettingWnd.h"
#include <QHBoxLayout>
#include <QMenu>
#include "../UICom/StyleDefine.h"

using  namespace QTalk;
typedef unsigned int Item_Index;
class ComboBox : public QFrame
{
    Q_OBJECT
public:
    explicit ComboBox(QWidget* parent = nullptr)
        : QFrame(parent), _index(-1)
    {
        setMinimumHeight(20);
    }

public:
    void addItem(const QString& text)
    {
        Item_Index index = items.size();
        if(index == 0)
            _index = index;
        items.push_back(QPair<Item_Index, QString>(index, text));
    }

    void setText(const QString& val)
    {
        auto itFind = std::find_if(items.begin(), items.end(), [this, val](const auto& item){
            return item.second == val;
        });
        if(itFind != items.end())
        {
            _index = itFind->first;
            update();
        }
    }

signals:
    void clicked();

protected:
    void paintEvent(QPaintEvent* evt) override
    {
        if(_index != -1)
        {
            auto itFind = std::find_if(items.begin(), items.end(), [this](const auto& item){
                return item.first == _index;
            });
            if(itFind != items.end())
            {
                QString text = itFind->second;
                QPainter painter(this);
                painter.setPen(QPen(QColor(153,153,153)));
                QFont font;
                font.setPixelSize(12);
                font.setWeight(400);
                painter.setFont(font);
                QFontMetricsF f(font);
                int width = (int)f.width(text);
                painter.drawText(20, 0, width, 20, Qt::AlignLeft | Qt::AlignVCenter, text);
                //
                painter.drawPixmap(20 + width + 5, 0, 20, 20, QPixmap(":/QTalk/image1/arrow_sort_down_small.png"));
            }
        }
        QFrame::paintEvent(evt);
    }

    void mousePressEvent(QMouseEvent* e) override
    {
        emit clicked();
        QFrame::mousePressEvent(e);
    }

private:
    Item_Index _index;
    QList<QPair<Item_Index, QString>> items;
};

class ActionLabel : public QLabel
{
Q_OBJECT
public:
    explicit ActionLabel(QString text, QWidget* parent = nullptr)
            : QLabel(parent), _isHover(false), _hasTip(false), _hasHover(true)
            , _checkAble(false), _checked(false), _text(std::move(text)){
        setContentsMargins(0, 4, 0, 4);
    }
    ~ActionLabel() override = default;

Q_SIGNALS:
    void clicked();

public:
	inline void setTip()
	{
		_hasTip = true;
	}

	void setHasHover(bool hasHover) {
	    _hasHover = hasHover;
	}

	void setItemIndex(int itemIndex) {
	    _itemIndex = itemIndex;
	}

	int getItemIndex() {
        return _itemIndex;
	}

	void setCheckAble(bool checkAble)
    {
        _checkAble = checkAble;
    }

	void setCheckState(bool checked)
    {
	    _checked = checked;
	    update();
    }

protected:
    bool event(QEvent* e) override
    {
        if(e->type() == QEvent::Enter)
        {
            _isHover = true;
            setCursor(Qt::PointingHandCursor);
            update();
        }
        else if(e->type() == QEvent::Leave)
        {
            _isHover = false;
            setCursor(Qt::ArrowCursor);
            update();
        }
        else if(e->type() == QEvent::MouseButtonPress)
        {
            emit clicked();
        }
        return QLabel::event(e);
    };


    void paintEvent(QPaintEvent* e) override {

        QPainter painter;
        painter.begin(this);
		QRect rect = this->rect();

		bool hoverBackground = (_hasHover & _isHover) | (_checkAble & _checked);
        if(hoverBackground)
        {
            painter.fillRect(rect, QBrush(StyleDefine::instance().getDropSelectColor()));
            painter.setPen(QPen(StyleDefine::instance().getDropSelectFontColor()));
            painter.drawText(QRect(rect.x() + this->contentsMargins().left(),
                    rect.y(), rect.width() - this->contentsMargins().left(), rect.height()), Qt::AlignVCenter, _text);
        }
        else
        {
            painter.fillRect(rect, QBrush(StyleDefine::instance().getDropNormalColor()));
            painter.setPen(QPen(StyleDefine::instance().getDropNormalFontColor()));
            painter.drawText(QRect(rect.x() + this->contentsMargins().left(),
                                   rect.y(), rect.width() - this->contentsMargins().left(), rect.height()), Qt::AlignVCenter, _text);
        }

        if(_checkAble && _checked)
            painter.fillRect(rect.x() + 1, rect.y(), 5, rect.height(), QBrush(QColor(0, 202, 190)));

		//
		if (_hasTip)
		{
			painter.setPen(Qt::NoPen);
			painter.setBrush(QBrush(Qt::red));
			painter.drawEllipse(rect.x() + 10, rect.y() + (rect.height() - 6) / 2, 6, 6);
		}
        painter.end();
        QLabel::paintEvent(e);
    }

private:
    bool _hasHover;
    bool _isHover;
	bool _hasTip;
	bool _checkAble;
	bool _checked;
	int  _itemIndex{};
	QString _text;
};

class DropMenu : public UShadowDialog
{
    Q_OBJECT
public:
    explicit DropMenu(QWidget* parent = nullptr);
    ~DropMenu() override;

public:
    void setName(const QString& name);
    void setHead(const QString& headPath);
    void onSwitchUserStatusRet(const QString& status);

Q_SIGNALS:
    void showSelfUserCard();
    void sysQuit();
    void sgCheckUpdate();
    void sgShowAboutWnd();
    void sgShowSystemSetting();
    void sgSwitchUserStatus(const QString& );

private:
    void initUi();
    void switchUserStatus();

private:
    QLabel* _pNameLabel;
    HeadPhotoLab* _headLabel;
    ActionLabel* _pUserCardLabel;
    ActionLabel* _pSettingLabel;
    ActionLabel* _pSysQuitLabel;
    ActionLabel* _pLogoutLabel;
    ActionLabel* _pAboutLabel;

private:
    QMenu* _menu;
    ComboBox* comboBox;
};


#endif //QTALK_V2_DROPMENU_H
