#ifndef DATABASEPLUG_GLOBAL_H
#define DATABASEPLUG_GLOBAL_H

#ifdef _WINDOWS
    #define DATABASEPLUGSHARED_EXPORT __declspec(dllexport)
#endif // _WINDOWS

#ifdef _LINUX
    #define DATABASEPLUGSHARED_EXPORT
#endif

#if (defined _MACOS) || (defined _LINUX)
    #define DATABASEPLUGSHARED_EXPORT
#endif

#endif // DATABASEPLUG_GLOBAL_H
