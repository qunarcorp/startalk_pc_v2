project(UIAddressBook)

# 打开Qt的Moc功能和ui文件QRC文件处理功能
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

include_directories(.)
find_package(Qt5Widgets)

#
set(SOURCE_FILES
        ListItemView.cpp
        AddressBookPanel.h
        uiaddressbook_global.h
        AddressBookPlug.h
        AddressBookPanel.cpp
        image1.qrc
        StaffStructure.cpp
        CreatGroupPoupWnd.h
        AddressBookPlug.cpp
        UserCard.cpp
        UserCard.h
        CreatGroupPoupWnd.cpp
        StaffDelegate.h
        StaffStructure.h
        StaffDelegate.cpp
        MessageManager.cpp
        MessageManager.h
        ListItemView.h
        )


if (APPLE)
    ADD_DEFINITIONS(-D_MACOS)
elseif (UNIX)
    ADD_DEFINITIONS(-D_LINUX)
else ()
    message("windows")
    ADD_DEFINITIONS(-D_WINDOWS)
    ADD_DEFINITIONS(-DUIADDRESSBOOK_LIB)
    link_directories(../Depends/curl/lib/windows/x64)
    link_directories(../Depends/zlib/lib/windows/x64)
    link_directories(../Depends/openssl/lib/windows/x64)
endif ()

set(LIBRARY_OUTPUT_PATH ../bin)
set(CMAKE_DEBUG_POSTFIX "d")


ADD_LIBRARY(${PROJECT_NAME} SHARED ${SOURCE_FILES})

#添加链接库
target_link_libraries(${PROJECT_NAME}
        Qt5::Widgets
        CustomUi
        QtUtil
        Platform
        EventBus)