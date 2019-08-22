//
// Created by cc on 19-1-4.
//

#ifndef QTALK_V2_LINE_H
#define QTALK_V2_LINE_H

#include <QFrame>

class Line : public QFrame
{
public:
//    Line(Qt::Orientation direction = Qt::Horizontal, QWidget* parent = nullptr)
//        : QFrame(parent)
//    {
//        if(direction == Qt::Horizontal)
//        {
//            setFixedHeight(1);
//            setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
//        }
//        else if(direction == Qt::Vertical)
//        {
//            setFixedWidth(1);
//            setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
//        }
//        this->setStyleSheet("background:rgba(238,238,238,1);");
//    }

    explicit Line(Qt::Orientation direction = Qt::Horizontal, QWidget* parent = nullptr)
            : QFrame(parent)
    {
        setObjectName("Line");
        if(direction == Qt::Horizontal)
        {
            setFixedHeight(1);
            setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        }
        else if(direction == Qt::Vertical)
        {
            setFixedWidth(1);
            setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
        }
//        if (colorStr == nullptr) {
//            this->setStyleSheet("background:rgba(238,238,238,1);");
//        } else {
//            this->setStyleSheet("background:"+colorStr+";");
//        }
    }

    ~Line() override = default;


};

#endif //QTALK_V2_LINE_H
