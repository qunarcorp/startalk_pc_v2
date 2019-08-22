project(Emoticon)

include_directories(.)
include_directories(/usr/include/x86_64-linux-gnu/qt5/QtXml)

# 打开Qt的Moc功能和ui文件QRC文件处理功能
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5Widgets)
find_package(Qt5Xml)

#
set(SOURCE_FILES
        EmoCellWidget.cpp
        EmoCellWidget.h
        EmoIconWgt.cpp
        EmoIconWgt.h
        EmoLstItemWgt.cpp
        EmoLstItemWgt.h
        EmoPreviewWgt.cpp
        EmoPreviewWgt.h
        emoticon_global.h
        EmoticonMainWgt.cpp
        EmoticonMainWgt.h
        EmoticonManager.cpp
        EmoticonManager.h
        EmoticonStruct.h
        LocalEmoticon.cpp
        LocalEmoticon.h
        MessageManager.cpp
        MessageManager.h
        NetEmoticon.cpp
        NetEmoticon.h
        emoticon.qrc EmoticonView.cpp EmoticonView.h)

if(APPLE)
    ADD_DEFINITIONS(-D_MACOS)
elseif(UNIX)
    ADD_DEFINITIONS(-D_LINUX)
else()
    ADD_DEFINITIONS(-D_WINDOWS)
    ADD_DEFINITIONS(-DEMOTICON_LIB)
    include_directories(../Depends/zlib/include)
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
        Qt5::Xml
        CustomUi
        Platform
        EventBus
        quazip)