//
// Created by QITMAC000260 on 2019-02-25.
//

#ifndef QTALK_V2_MACAPP_H
#define QTALK_V2_MACAPP_H


#include <QWidget>

/**
* @description: MacApp
* @author: cc
* @create: 2019-02-25 20:46
**/
class MacApp {
public:
    static void initApp();
    static void AllowMinimizeForFramelessWindow(QWidget* window);
    static void wakeUpWnd(QWidget* window);
    static void showMinWnd(QWidget* window);
    static void checkValidToVisitMicroPhone();
};


#endif //QTALK_V2_MACAPP_H
