project(UITitlebarPlug)

include_directories(.)

# 打开Qt的Moc功能和ui文件QRC文件处理功能
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5Widgets)

#
set(SOURCE_FILES
        image1.qrc
        MainPanel.cpp
        MainPanel.h
        MessageManager.cpp
        MessageManager.h
        titlebar.json
        UITitlebarPlug.cpp
        UITitlebarPlug.h
        uititlebarplug_global.h
        SeachEditPanel.cpp
        SeachEditPanel.h
        SearchEdit.cpp
        SearchEdit.h
        SearchItemDelegate.cpp
        SearchItemDelegate.h
        SearchResultPanel.cpp
        SearchResultPanel.h
        SearchThread.cpp
        SearchThread.h
        SessionBtn.cpp
        SessionBtn.h
        DropMenu.cpp
        DropMenu.h
        AboutWnd.cpp
        AboutWnd.h
        SystemSettingWnd.cpp
        SystemSettingWnd.h
        SKRecorder.cpp
        SKRecorder.h
        ChangeHeadWnd.cpp
        ChangeHeadWnd.h
        SearchView.cpp
        SearchView.h
        UCButton.cpp
        UCButton.h)


if (APPLE)
    ADD_DEFINITIONS(-D_MACOS)
    find_package(Qt5MacExtras)
    link_libraries(Qt5::MacExtras)
elseif (UNIX)
    ADD_DEFINITIONS(-D_LINUX)
else ()
    ADD_DEFINITIONS(-D_WINDOWS)
    ADD_DEFINITIONS(-DUITITLEBARPLUG_LIBRARY)
    link_directories(../Depends/curl/lib/windows/x64)
    link_directories(../Depends/zlib/lib/windows/x64)
    link_directories(../Depends/openssl/lib/windows/x64)
endif ()

set(LIBRARY_OUTPUT_PATH ../bin)
set(CMAKE_DEBUG_POSTFIX "d")

ADD_LIBRARY(${PROJECT_NAME} SHARED ${SOURCE_FILES} ${MAC_SOURCE_FILE})

#添加链接库
target_link_libraries(${PROJECT_NAME}
        Qt5::Widgets EventBus Platform CustomUi qzxing QtUtil)