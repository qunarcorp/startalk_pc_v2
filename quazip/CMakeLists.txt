project(quazip)

## 打开Qt的Moc功能和ui文件QRC文件处理功能
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5Widgets)

#
set(SOURCE_FILES
        crypt.h
        ioapi.h
        JlCompress.cpp
        JlCompress.h
        qioapi.cpp
        quaadler32.cpp
        quaadler32.h
        quachecksum32.h
        quacrc32.cpp
        quacrc32.h
        quagzipfile.cpp
        quagzipfile.h
        quaziodevice.cpp
        quaziodevice.h
        quazip.cpp
        quazip.h
        quazip_global.h
        quazipdir.cpp
        quazipdir.h
        quazipfile.cpp
        quazipfile.h
        quazipfileinfo.cpp
        quazipfileinfo.h
        quazipnewinfo.cpp
        quazipnewinfo.h
        unzip.c
        unzip.h
        zip.c
        zip.h)

if (APPLE)
    ADD_DEFINITIONS(-D_MACOS)
elseif (UNIX)
    ADD_DEFINITIONS(-D_LINUX)
else ()
    ADD_DEFINITIONS(-D_WINDOWS)
endif ()

set(LIBRARY_OUTPUT_PATH ../bin)
set(CMAKE_DEBUG_POSTFIX "d")

ADD_LIBRARY(${PROJECT_NAME} SHARED ${SOURCE_FILES})

#添加链接库

if (WIN32)
        target_link_libraries(${PROJECT_NAME}
                Qt5::Widgets
                zlibstat)
else ()

    target_link_libraries(${PROJECT_NAME}
            Qt5::Widgets
            z)
endif ()