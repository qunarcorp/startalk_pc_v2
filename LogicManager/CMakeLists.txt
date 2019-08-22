project(LogicManager)

include_directories(../QtUtil)

#
set(SOURCE_FILES
        LogicManager.cpp
        LogicManager.h
        logicmanager_global.h)

if (APPLE)
    ADD_DEFINITIONS(-D_MACOS)
elseif (UNIX)
    ADD_DEFINITIONS(-D_LINUX)
else ()
    ADD_DEFINITIONS(-D_WINDOWS)
endif ()

set(LIBRARY_OUTPUT_PATH ../bin)
set(CMAKE_DEBUG_POSTFIX "d")

IF (CMAKE_BUILD_TYPE MATCHES "Debug")
    ADD_DEFINITIONS(-D_DEBUG)
endif ()

ADD_LIBRARY(${PROJECT_NAME} SHARED ${SOURCE_FILES})