project(UICom)

## 打开Qt的Moc功能和ui文件QRC文件处理功能
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5Widgets)
find_package(Qt5Xml)

#
set(SOURCE_FILES
        Config/configobject.h
        Config/configobject.cpp
        qconfig/qconfig.cpp
        qconfig/qconfig.h
        uicom_global.h
        uicom.h
        uicom.cpp
        UIEntity.h
        qimage/qimage.cpp
        qimage/qimage.h
        StyleDefine.cpp StyleDefine.h)

if (APPLE)
    ADD_DEFINITIONS(-D_MACOS)
elseif (UNIX)
    ADD_DEFINITIONS(-D_LINUX)
else ()
    ADD_DEFINITIONS(-D_WINDOWS)
    ADD_DEFINITIONS(-DUICOM_LIBRARY)
endif ()

set(LIBRARY_OUTPUT_PATH ../bin)
set(CMAKE_DEBUG_POSTFIX "d")

ADD_LIBRARY(UICom SHARED ${SOURCE_FILES})

#添加链接库
target_link_libraries(${PROJECT_NAME}
        Qt5::Widgets
        Qt5::Xml)