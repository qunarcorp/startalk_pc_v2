project(WebService)

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
    ADD_DEFINITIONS(-DWEBSERVICE_LIBRARY)
    link_directories(${QT_HOME}/lib)
endif ()

set(CMAKE_FRAMEWORK_PATH ${QT_HOME})


find_package(Qt5Widgets REQUIRED PATHS ${QT_HOME}/lib/)
find_package(Qt5WebEngineWidgets REQUIRED PATHS ${QT_HOME}/lib/)
find_package(Qt5Network REQUIRED PATHS ${QT_HOME}/lib/)
#
set(SOURCE_FILES
        WebService.cpp
        WebService.h
        webservice_global.h
        WebView.cpp
        WebView.h
        WebService.qrc
        WebEnginePage.cpp
        WebEnginePage.h
        AudioVideo.cpp
        AudioVideo.h WebJsObj.cpp WebJsObj.h)


set(LIBRARY_OUTPUT_PATH ../bin)
set(CMAKE_DEBUG_POSTFIX "d")

ADD_LIBRARY(${PROJECT_NAME} SHARED ${SOURCE_FILES})

#添加链接库
target_link_libraries(${PROJECT_NAME}
        Qt5::Widgets
        Qt5::WebEngineWidgets
        Qt5::Network
        CustomUi)