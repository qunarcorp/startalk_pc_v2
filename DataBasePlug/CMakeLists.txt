project(DataBasePlug)

include_directories(.)

#
set(SOURCE_FILES
        ConfigDao.cpp
        ConfigDao.h
        DataBasePlug.cpp
        DataBasePlug.h
        databaseplug_global.h
        dllmain.cpp
        GroupDao.cpp
        GroupDao.h
        GroupMemberDao.cpp
        GroupMemberDao.h
        MessageDao.cpp
        MessageDao.h
        SessionListDao.cpp
        SessionListDao.h
        UserDao.cpp
        UserDao.h
        UserMemberDao.cpp
        UserMemberDao.h
        sqlite/database.cpp
        sqlite/database.h
        sqlite/shell.c
        sqlite/sqlite3.c
        sqlite/sqlite3.h
        sqlite/sqlite3ext.h UserSupplementDao.cpp UserSupplementDao.h DbConfig.cpp DbConfig.h FriendListDao.cpp FriendListDao.h CacheDataDao.h CacheDataDao.cpp QuickReplyDao.h QuickReplyDao.cpp TriggerConfig.h TriggerConfig.cpp)

if(APPLE)
    ADD_DEFINITIONS(-D_MACOS)
elseif(UNIX)
    ADD_DEFINITIONS(-D_LINUX)
else()
    ADD_DEFINITIONS(-D_WINDOWS)
    link_directories(../Depends/curl/lib/windows/x64)
    link_directories(../Depends/zlib/lib/windows/x64)
    link_directories(../Depends/openssl/lib/windows/x64)
endif()

set(LIBRARY_OUTPUT_PATH  ../bin)
set (CMAKE_DEBUG_POSTFIX "d")

ADD_LIBRARY(${PROJECT_NAME} SHARED ${SOURCE_FILES})

#添加链接库
target_link_libraries(${PROJECT_NAME}
        QtUtil
        Platform)