//
// Created by QITMAC000260 on 2019/10/17.
//

#include "MedalWgt.h"
#include "../Platform/Platform.h"
#include "../Platform/dbPlatForm.h"
#include "../UICom/qimage/qimage.h"
#include <QFileInfo>
#include <QFile>
#include <QDebug>
#include <QPainter>

#define DEM_MARGIN 8
#define DEM_MAX_COL 6

MedalWgt::MedalWgt(int icon_width, QWidget* parent)
    :QFrame(parent), _icon_width(icon_width)
{
    this->setMinimumWidth(_icon_width * DEM_MAX_COL + DEM_MARGIN * (DEM_MAX_COL - 1));
    this->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
}

MedalWgt::~MedalWgt() {

}

void MedalWgt::addMedal(int medal_id) {
    if(_medals.find(medal_id) != _medals.end())
    {
        auto info = dbPlatForm::instance().getMedal(medal_id);
        if(info.medalId != -1)
        {
            _medals[medal_id] = info;
            setHeight();
            update();
        }
    }
}

void MedalWgt::setHeight() {
    auto size = _medals.size();
    int rowCount = size / DEM_MAX_COL;
    if(size % DEM_MAX_COL)
        rowCount += 1;

    auto h = _icon_width * rowCount + DEM_MARGIN * (rowCount - 1);
    this->setFixedHeight(qMax(h, _icon_width));
}

void MedalWgt::addMedals(const std::set<QTalk::StUserMedal> &user_medal, bool showValid) {

    _medals.clear();
    for(const auto& item : user_medal)
    {
        if(showValid && ((item.medalStatus & 0x02) != 0x02))
            continue;

        auto medal_id = item.medalId;
        if(_medals.find(medal_id) == _medals.end())
        {
            auto info = dbPlatForm::instance().getMedal(medal_id);
            if(info.medalId != -1)
            {
                _medals[medal_id] = info;
            }
        }
    }
    setHeight();
    update();
}

void MedalWgt::removeMedal(int medal_id) {
    if(_medals.find(medal_id) != _medals.end())
    {
        _medals.erase(medal_id);
        setHeight();
        update();
    }
}

void MedalWgt::paintEvent(QPaintEvent *e) {

    int row = 0, col = 0;

    for(const auto& medal : _medals)
    {
        auto info = medal.second;

        auto imgLink = info.smallIcon;
        QString imgPath = QTalk::getMedalPath(imgLink).data();
        QFileInfo fileInfo(imgPath);
        if(fileInfo.isDir() || !fileInfo.exists())
            imgPath = ":/QTalk/image1/medal_default.png";

        int x = col * _icon_width + col * DEM_MARGIN;
        int y = row * _icon_width + row * DEM_MARGIN;

        QPainter painter(this);
        painter.setRenderHints(QPainter::Antialiasing, true);
        painter.setRenderHints(QPainter::SmoothPixmapTransform, true);
        auto dpi = QTalk::qimage::instance().dpi();
        QPixmap image = QTalk::qimage::instance().loadPixmap(imgPath, true, true, dpi * _icon_width);
        painter.drawPixmap(x, y, _icon_width, _icon_width, image);

        if(++col >= DEM_MAX_COL)
        {
            row++;
            col = 0;
        }
    }

    QFrame::paintEvent(e);
}
