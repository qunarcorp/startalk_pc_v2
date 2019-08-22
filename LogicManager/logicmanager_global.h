#ifndef _LOGICMANAGER_GLOBAL_H_
#define _LOGICMANAGER_GLOBAL_H_

#ifdef _WINDOWS
#define LOGICMANAGER_EXPORT __declspec(dllexport)
#endif // _WINDOWS

#if (defined _MACOS) || (defined _LINUX)
#define LOGICMANAGER_EXPORT
#endif // _MAC

#endif // _LOGICMANAGER_GLOBAL_H_
