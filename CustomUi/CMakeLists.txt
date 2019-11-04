project(CustomUi)

# 打开Qt的Moc功能和ui文件QRC文件处理功能
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5Widgets CONFIG REQUIRED)

#
set(SOURCE_FILES
        customui_global.h
        HeadPhotoLab.cpp
        HeadPhotoLab.h
        ShadowHeadPhoto.cpp
        ShadowHeadPhoto.h
        UShadowWnd.cpp
        UShadowWnd.h
        ushadowdialog.qrc
        UShadowEffect.cpp
        UShadowEffect.h
        LinkButton.cpp
        LinkButton.h
        TextEdit.cpp
        TextEdit.h
        SearchEdit.hpp
        GroupMemberPopWnd.hpp
        UShadowBorder.cpp
        UShadowBorder.h
        QtMessageBox.cpp
        QtMessageBox.h
        LiteMessageBox.cpp
        LiteMessageBox.h
        NavigationItem.cpp
        NavigationItem.h MedalWgt.cpp MedalWgt.h)

if (APPLE)
    ADD_DEFINITIONS(-D_MACOS)
elseif (UNIX)
    ADD_DEFINITIONS(-D_LINUX)
else ()
    ADD_DEFINITIONS(-D_WINDOWS)
    ADD_DEFINITIONS(-DCUSTOMUI_LIBRARY)
    link_directories(../Depends/curl/lib/windows/x64)
    link_directories(../Depends/zlib/lib/windows/x64)
    link_directories(../Depends/openssl/lib/windows/x64)
endif ()

set(LIBRARY_OUTPUT_PATH ../bin)
set(CMAKE_DEBUG_POSTFIX "d")


ADD_LIBRARY(${PROJECT_NAME} SHARED ${SOURCE_FILES})

#添加链接库
target_link_libraries(${PROJECT_NAME} Qt5::Widgets UICom QtUtil Platform EventBus)