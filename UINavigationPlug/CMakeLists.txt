project(UINavigationPlug)

include_directories(.)

# 打开Qt的Moc功能和ui文件QRC文件处理功能
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5Widgets)
find_package(Qt5Network)
find_package(Qt5WebEngineWidgets)
find_package(Qt5Concurrent)

#
set(SOURCE_FILES
        ContactFrm.cpp
        ContactFrm.h
        image1.qrc
        MessageManager.cpp
        MessageManager.h
        MultifunctionFrm.cpp
        MultifunctionFrm.h
        navigation.json
        NavigationMianPanel.cpp
        NavigationMianPanel.h
        SessionFrm.cpp
        SessionFrm.h
        SessionItemFrm.cpp
        SessionItemFrm.h
        TcpDisconnect.cpp
        TcpDisconnect.h
        UINavigationPlug.cpp
        UINavigationPlug.h
        uinavigationplug_global.h SessionitemDelegate.cpp SessionitemDelegate.h)

if (APPLE)
    ADD_DEFINITIONS(-D_MACOS)
elseif (UNIX)
    ADD_DEFINITIONS(-D_LINUX)
else ()
    ADD_DEFINITIONS(-D_WINDOWS)
    ADD_DEFINITIONS(-DUINAVIGATIONPLUG_LIBRARY)
    link_directories(../Depends/curl/lib/windows/x64)
    link_directories(../Depends/zlib/lib/windows/x64)
    link_directories(../Depends/openssl/lib/windows/x64)
endif ()

set(LIBRARY_OUTPUT_PATH ../bin)
set(CMAKE_DEBUG_POSTFIX "d")


ADD_LIBRARY(${PROJECT_NAME} SHARED ${SOURCE_FILES})


#添加链接库
target_link_libraries(${PROJECT_NAME}
        Qt5::Widgets
        Qt5::Network
        Qt5::WebEngineWidgets
        Qt5::Concurrent
        CustomUi EventBus Platform WebService)