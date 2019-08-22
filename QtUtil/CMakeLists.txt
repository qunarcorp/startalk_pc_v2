project(QtUtil)

include_directories(.)
include_directories(./lib)

#include_directories(../3rdparty/openssl/include)

#link_directories(../3rdparty/openssl/lib)

#include_directories(/usr/local/opt/openssl@1.1/include)

#link_directories(/usr/local/opt/openssl@1.1/lib)
#include_directories(/usr/local/Cellar/protobuf@2.6/2.6.1/include)
#
#if (APPLE)
#    ADD_DEFINITIONS(-D_MACOS)
#    set(MAC_SOURCE_FILE macfunc.mm macfunc.h)
#    FIND_LIBRARY(COREFOUNDATION_Library CoreFoundation)
#elseif (UNIX)
#    ADD_DEFINITIONS(-D_LINUX)
#else ()
#    ADD_DEFINITIONS(-D_WINDOWS)
#endif ()

if (APPLE)
    ADD_DEFINITIONS(-D_MACOS)
elseif (UNIX)
    ADD_DEFINITIONS(-D_LINUX)
    link_libraries(uuid)
else ()
    ADD_DEFINITIONS(-D_WINDOWS)
    link_libraries(uuid)

endif ()

set(SOURCE_FILES
        Config/ClientConfig.h
        Config/ClientConfig.cpp
        Entity/JID.cpp
        Entity/JID.h
        JsonConfig/JsonConfig.h
        JsonConfig/JsonConfig.cpp
        lib/Base64/base64.h
        lib/Base64/base64.cpp
        lib/cjson/cJSON.h
        lib/cjson/cJSON.c
        lib/cjson/cJSON_Utils.c
        lib/cjson/cJSON_Utils.h
        lib/http/QtHttpRequest.h
        lib/http/QtHttpRequest.cpp
        lib/Md5/Md5.h
        lib/Md5/Md5.cpp
        lib/ini/ini.c
        lib/ini/ini.h
        lib/ini/ConfigLoader.cpp
        lib/ini/ConfigLoader.h
        Utils/httplib.h
        Utils/MemoryStream.cpp
        Utils/MemoryStream.h
        Utils/utils.cpp
        Utils/utils.h
        QtalkUtil.cpp
        QtalkUtil.h
        qtutil_global.h
        lib/ini/ConfigLoader.cpp
        lib/ini/ConfigLoader.h
        Utils/Log.h
        Utils/Log.cpp
        lib/spdlog/async.h
        lib/spdlog/async_logger.h
        lib/spdlog/common.h
        lib/spdlog/formatter.h
        lib/spdlog/logger.h
        lib/spdlog/spdlog.h
        lib/spdlog/tweakme.h
        lib/spdlog/version.h
        lib/spdlog/details/async_logger_impl.h
        lib/spdlog/details/circular_q.h
        lib/spdlog/details/console_globals.h
        lib/spdlog/details/file_helper.h
        lib/spdlog/details/fmt_helper.h
        lib/spdlog/details/log_msg.h
        lib/spdlog/details/logger_impl.h
        lib/spdlog/details/mpmc_blocking_q.h
        lib/spdlog/details/null_mutex.h
        lib/spdlog/details/os.h
        lib/spdlog/details/pattern_formatter.h
        lib/spdlog/details/periodic_worker.h
        lib/spdlog/details/registry.h
        lib/spdlog/details/thread_pool.h
        lib/spdlog/fmt/bin_to_hex.h
        lib/spdlog/fmt/fmt.h
        lib/spdlog/fmt/ostr.h
        lib/spdlog/fmt/bundled/chrono.h
        lib/spdlog/fmt/bundled/color.h
        lib/spdlog/fmt/bundled/core.h
        lib/spdlog/fmt/bundled/format-inl.h
        lib/spdlog/fmt/bundled/format.h
        lib/spdlog/fmt/bundled/locale.h
        lib/spdlog/fmt/bundled/ostream.h
        lib/spdlog/fmt/bundled/posix.h
        lib/spdlog/fmt/bundled/printf.h
        lib/spdlog/fmt/bundled/ranges.h
        lib/spdlog/fmt/bundled/time.h
        lib/spdlog/sinks/android_sink.h
        lib/spdlog/sinks/ansicolor_sink.h
        lib/spdlog/sinks/base_sink.h
        lib/spdlog/sinks/basic_file_sink.h
        lib/spdlog/sinks/daily_file_sink.h
        lib/spdlog/sinks/dist_sink.h
        lib/spdlog/sinks/msvc_sink.h
        lib/spdlog/sinks/null_sink.h
        lib/spdlog/sinks/ostream_sink.h
        lib/spdlog/sinks/rotating_file_sink.h
        lib/spdlog/sinks/sink.h
        lib/spdlog/sinks/stdout_color_sinks.h
        lib/spdlog/sinks/stdout_sinks.h
        lib/spdlog/sinks/wincolor_sink.h
        lib/cjson/cJSON_inc.h
        lib/cjson/cJSON_externsion.cpp
        lib/http/HttpRequest.h)

if (APPLE)
    set(MAC_SOURCE_FILE
            #            lib/http/asihttprequest/ASIAuthenticationDialog.h
            #            lib/http/asihttprequest/ASIAuthenticationDialog.m

            lib/MacHelper.h
            lib/MacHelper.mm)

    #    set_target_properties(${MAC_SOURCE_FILE} PROPERTIES
    #            COMPILE_FLAGS "-fobjc-arc"
    #            )

    set_property(SOURCE ASIDataCompressor.m APPEND_STRING PROPERTY COMPILE_FLAGS "-fno-objc-arc")

    FIND_LIBRARY(CoreFoundation_Library CoreFoundation)
    if (NOT CoreFoundation_Library)
        message(FATAL_ERROR "CoreFoundation not found")
    endif ()

    FIND_LIBRARY(Foundation_Library Foundation)
    if (NOT Foundation_Library)
        message(FATAL_ERROR "Foundation not found")
    endif ()

    FIND_LIBRARY(CFNetwork_Library CFNetwork)
    if (NOT CFNetwork_Library)
        message(FATAL_ERROR "CFNetwork not found")
    endif ()

    FIND_LIBRARY(CoreServices_Library CoreServices)
    if (NOT CoreServices_Library)
        message(FATAL_ERROR "CoreServices not found")
    endif ()

    FIND_LIBRARY(SystemConfiguration_Library SystemConfiguration)
    if (NOT SystemConfiguration_Library)
        message(FATAL_ERROR "SystemConfiguration not found")
    endif ()
endif ()

#set(CMAKE_SKIP_BUILD_RPATH FALSE)
#set(CMAKE_SKIP_RPATH FALSE)
#set(CMAKE_SKIP_INSTALL_RPATH FALSE)

set(LIBRARY_OUTPUT_PATH ../bin)
set(CMAKE_DEBUG_POSTFIX "d")

ADD_LIBRARY(${PROJECT_NAME} SHARED ${SOURCE_FILES} ${MAC_SOURCE_FILE})
if (APPLE)
    set_property(TARGET ${PROJECT_NAME} APPEND_STRING PROPERTY
            COMPILE_FLAGS "-fobjc-arc")
endif ()
#添加链接库

if (WIN32)
        target_link_libraries(${PROJECT_NAME}
                libcurl libcrypto libssl zlibstat)
else ()

    target_link_libraries(${PROJECT_NAME}
            ${COREFOUNDATION_Library}
            ${CFNetwork_Library}
            ${CoreServices_Library}
            ${SystemConfiguration_Library}
            ${Foundation_Library}
            curl
            z
            ssl
            crypto)
endif ()