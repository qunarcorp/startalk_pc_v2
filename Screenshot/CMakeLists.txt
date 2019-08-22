project(Screenshot)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5Widgets)

#
set(SOURCE_FILES
        image1.qrc
        style.qrc)

#qt5_wrap_ui(ui snipscreentoolbar.ui)

if(APPLE)
    ADD_DEFINITIONS(-D_MACOS)
elseif(UNIX)
    ADD_DEFINITIONS(-D_LINUX)
else()
    ADD_DEFINITIONS(-D_WINDOWS)
    ADD_DEFINITIONS(-DSCREENSHOT_LIB)
    include_directories(../Depends/curl/include)
    include_directories(../Depends/zlib/include)
    include_directories(../Depends/openssl/include)
    link_directories(../Depends/curl/lib/windows/x64)
    link_directories(../Depends/zlib/lib/windows/x64)
    link_directories(../Depends/openssl/lib/windows/x64)
endif()

if(APPLE)
    set(PLAT_FILES
            mac/ActionCommand.cpp
            mac/ActionCommand.h
            mac/DrawTextDocumentHelper.cpp
            mac/DrawTextDocumentHelper.h
            mac/floatTextEdit.cpp
            mac/floatTextEdit.h
            mac/FullScreenPrivate.cpp
            mac/FullScreenPrivate.h
            mac/FullScreenWidget.cpp
            mac/FullScreenWidget.h
            mac/screenshot_global.h
            mac/SnipScreenTool.cpp
            mac/SnipScreenTool.h
            mac/SnipScreenToolBar.cpp
            mac/SnipScreenToolBar.h
            image1.qrc
            style.qrc)
else()
    set(PLAT_FILES
            ActionCommand.cpp
            ActionCommand.h
            DrawTextDocumentHelper.cpp
            DrawTextDocumentHelper.h
            floatTextEdit.cpp
            floatTextEdit.h
            FullScreenPrivate.cpp
            FullScreenPrivate.h
            FullScreenWidget.cpp
            FullScreenWidget.h
            screenshot_global.h
            SnipScreenTool.cpp
            SnipScreenTool.h
            SnipScreenToolBar.cpp
            SnipScreenToolBar.h
            image1.qrc
            style.qrc)
endif()

set(LIBRARY_OUTPUT_PATH  ../bin)
set (CMAKE_DEBUG_POSTFIX "d")

ADD_LIBRARY(${PROJECT_NAME} SHARED ${SOURCE_FILES} ${PLAT_FILES})

#添加链接库
target_link_libraries(${PROJECT_NAME}
        Qt5::Widgets QtUtil)