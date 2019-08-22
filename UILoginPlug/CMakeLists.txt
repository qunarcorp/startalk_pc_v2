project(UILoginPlug)

include_directories(.)

# 打开Qt的Moc功能和ui文件QRC文件处理功能
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

if (APPLE)
    ADD_DEFINITIONS(-D_MACOS)
elseif (UNIX)
    ADD_DEFINITIONS(-D_LINUX)
else ()
    ADD_DEFINITIONS(-D_WINDOWS)
    ADD_DEFINITIONS(-DUILOGINPLUG_LIBRARY)
    #    set(QT_HOME C:\\Qt\\5.12.3\\msvc2017_64)
    #    set(CMAKE_PREFIX_PATH ${QT_HOME})
    #    link_directories(../Depends/curl/lib/windows/x64)
    #    link_directories(../Depends/zlib/lib/windows/x64)
    #    link_directories(../Depends/openssl/lib/windows/x64)
endif ()

find_package(Qt5Core)
find_package(Qt5Widgets)
find_package(Qt5WebEngineWidgets)
#
set(SOURCE_FILES
        ../qzxing/QZXing.h
        image1.qrc
        login.json
        LoginPanel.cpp
        LoginPanel.h
        MessageManager.cpp
        MessageManager.h
        UILoginPlug.cpp
        UILoginPlug.h
        uiloginplug_global.h
        NavManager.cpp
        NavManager.h
        NavView.cpp
        NavView.h
        AddNavWnd.cpp
        AddNavWnd.h
        QWebLogin.cpp
        QWebLogin.h)


set(LIBRARY_OUTPUT_PATH ../bin)
set(CMAKE_DEBUG_POSTFIX "d")
IF (CMAKE_BUILD_TYPE MATCHES "Debug")
    ADD_DEFINITIONS(-D_DEBUG)
endif ()

ADD_LIBRARY(${PROJECT_NAME} SHARED ${SOURCE_FILES})
#添加链接库
target_link_libraries(${PROJECT_NAME}
        Qt5::Core
        Qt5::Widgets
        Qt5::WebEngineWidgets
        EventBus
        CustomUi
        UICom
        QtUtil
        Platform
        qzxing
        WebService)