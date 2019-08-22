#ifndef _EVENTBUS_GLOBAL_H_
#define _EVENTBUS_GLOBAL_H_

#ifdef _WINDOWS
#define EVENTBUS_EXPORT __declspec(dllexport)
#endif // _WINDOWS

#if (defined _MACOS) || (defined _LINUX)
#define EVENTBUS_EXPORT
#endif // _MAC

#endif // _EVENTBUS_GLOBAL_H_
