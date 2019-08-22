#ifndef PLATFORM_GLOBAL_H
#define PLATFORM_GLOBAL_H

#ifdef _WINDOWS
#define PLATFORMSHARED_EXPORT __declspec(dllexport)
#endif // _WINDOWS

#if (defined _MACOS) || (defined _LINUX)
#define PLATFORMSHARED_EXPORT
#endif // _MAC

#endif // PLATFORM_GLOBAL_H
