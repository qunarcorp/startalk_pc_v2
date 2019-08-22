project(UIPictureBrowser)

include_directories(.)

# 打开Qt的Moc功能和ui文件QRC文件处理功能
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5Widgets)

#
set(SOURCE_FILES
        uipicturebrowser_global.h
        PictureBrowserPlug.cpp
        PictureBrowserPlug.h
        PictureBrowser.cpp
        PictureBrowser.h
        image1.qrc
        PictureFrm.cpp
        PictureFrm.h
        TitleFrm.cpp
        TitleFrm.h
        PicItem.cpp
        PicItem.h
        MessageManager.cpp
        MessageManager.h)

ADD_DEFINITIONS(-DUIPICTUREBROWSER_LIBRARY)

if(APPLE)
    ADD_DEFINITIONS(-D_MACOS)
elseif(UNIX)
    ADD_DEFINITIONS(-D_LINUX)
else()
    ADD_DEFINITIONS(-D_WINDOWS)
    link_directories(../Depends/curl/lib/windows/x64)
    link_directories(../Depends/zlib/lib/windows/x64)
    link_directories(../Depends/openssl/lib/windows/x64)
endif()

set(LIBRARY_OUTPUT_PATH  ../bin)
set (CMAKE_DEBUG_POSTFIX "d")

ADD_LIBRARY(${PROJECT_NAME} SHARED ${SOURCE_FILES})

#添加链接库
target_link_libraries(${PROJECT_NAME}
        Qt5::Widgets
        CustomUi
        Platform
        UICom)