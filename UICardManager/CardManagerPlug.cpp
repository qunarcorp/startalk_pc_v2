//
// Created by cc on 18-11-6.
//

#include "CardManagerPlug.h"
#include "CardManager.h"

CardManagerPlug::CardManagerPlug()
    :_pCardManager(nullptr)
{
}

QWidget *CardManagerPlug::widget()
{
    if(_pCardManager)
        return _pCardManager;

    return nullptr;
}


void CardManagerPlug::init()
{
    _pCardManager = new CardManager;
}
