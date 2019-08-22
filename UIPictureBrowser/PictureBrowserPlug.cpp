//
// Created by QITMAC000260 on 2018/11/16.
//

#include "PictureBrowserPlug.h"
#include "PictureBrowser.h"

PictureBrowserPlug::PictureBrowserPlug() = default;

PictureBrowserPlug::~PictureBrowserPlug() {

    delete _mainPanel;
}

QWidget *PictureBrowserPlug::widget() {
    if (nullptr != _mainPanel)
        return _mainPanel;
    return nullptr;
}

void PictureBrowserPlug::init() {
    _mainPanel = new PictureBrowser;
}
