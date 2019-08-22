//
// Created by cc on 19-1-4.
//

#ifndef QTALK_V2_MODBUTTON_H
#define QTALK_V2_MODBUTTON_H

#include <QPushButton>

class ModButton : public QPushButton
{
public:
    //
    ModButton(QWidget* parent = nullptr)
        : QPushButton(parent)
    {
        setObjectName("ModButton");
        setFixedSize(20, 20);
    }
    //
    ~ModButton() {}
};

#endif //QTALK_V2_MODBUTTON_H
