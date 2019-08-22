project(EventBus)
#
set(SOURCE_FILES
        Event.hpp
        eventbus_global.h
        EventBus.h
        EventBus.cpp
        EventHandler.hpp
        HandlerRegistration.hpp
        Object.hpp)

set(LIBRARY_OUTPUT_PATH  ../bin)
set (CMAKE_DEBUG_POSTFIX "d")
#

if(APPLE)
    ADD_DEFINITIONS(-D_MACOS)
elseif(UNIX)
    ADD_DEFINITIONS(-D_LINUX)
else()
    ADD_DEFINITIONS(-D_WINDOWS)
endif()


ADD_LIBRARY(${PROJECT_NAME} SHARED ${SOURCE_FILES})