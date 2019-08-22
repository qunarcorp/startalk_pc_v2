project(LogicCommunication)

include_directories(.)
include_directories(../include)
include_directories(../QtUtils)
include_directories(../QtUtil/Utils)

if (APPLE)
    ADD_DEFINITIONS(-D_MACOS)
elseif (UNIX)
    ADD_DEFINITIONS(-D_LINUX)
else ()
    ADD_DEFINITIONS(-D_WINDOWS)
    ADD_DEFINITIONS(-D_CRT_SECURE_NO_WARNINGS)
    link_libraries(Ws2_32.lib)
    link_libraries(Wldap32.lib)
    link_libraries(Crypt32.lib)
endif ()

#
set(SOURCE_FILES
        Communication.cpp
        Communication.h
        communication_global.h
        dllMain.cpp
        FileHelper.cpp
        FileHelper.h
        GroupManager.cpp
        GroupManager.h
        MessageManager.cpp
        MessageManager.h
        OfflineMessageManager.cpp
        OfflineMessageManager.h
        OnLineManager.cpp
        OnLineManager.h
        UserManager.cpp
        UserManager.h
        SearchManager.cpp
        SearchManager.h
        UserConfig.cpp
        UserConfig.h
        HotLinesConfig.h
        HotLinesConfig.cpp)

set(LIBRARY_OUTPUT_PATH ../bin)
set(CMAKE_DEBUG_POSTFIX "d")

ADD_LIBRARY(${PROJECT_NAME} SHARED ${SOURCE_FILES})

#添加链接库

if (WIN32)
    target_link_libraries(${PROJECT_NAME}
            EventBus
            QtUtil
            LogicManager
            Platform
            libssl
            libcrypto
            libprotobuf)
else ()
    target_link_libraries(${PROJECT_NAME}
            EventBus
            QtUtil
            LogicManager
            Platform
            ssl
            crypto
            protobuf)
endif ()
