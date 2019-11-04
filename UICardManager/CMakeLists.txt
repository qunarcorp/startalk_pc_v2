project(UICardManager)

# 打开Qt的Moc功能和ui文件QRC文件处理功能
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

include_directories(.)
find_package(Qt5Widgets)
find_package(Qt5Concurrent)

#
set(SOURCE_FILES
        cardmanager_global.h
        CardManager.h
        CardManager.cpp
        UserCard.h
        UserCard.cpp
        GroupCard.h
        GroupCard.cpp
        image1.qrc
        CardManagerPlug.h
        CardManagerPlug.cpp
        UserCardMsgManager.cpp
        UserCardMsgManager.h
        medal/UserMedalWnd.cpp
        medal/UserMedalWnd.h
        medal/UserMedalItem.cpp
        medal/UserMedalItem.h
        medal/MedalDetailWnd.cpp
        medal/MedalDetailWnd.h
        medal/UserHeadWgt.cpp
        medal/UserHeadWgt.h
        medal/UserListWnd.cpp
        medal/UserListWnd.h)

if(APPLE)
    ADD_DEFINITIONS(-D_MACOS)
elseif(UNIX)
    ADD_DEFINITIONS(-D_LINUX)
else()
    ADD_DEFINITIONS(-D_WINDOWS)
    ADD_DEFINITIONS(-DCARDMANAGER_LIB)
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
        Qt5::Concurrent
        CustomUi
        EventBus
        Platform
        QtUtil
        UICom)