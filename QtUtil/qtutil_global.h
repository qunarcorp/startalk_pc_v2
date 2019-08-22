#ifndef _QTALKUTILGLOBAL_H_
#define _QTALKUTILGLOBAL_H_

#ifdef _WINDOWS
#define QTALK_UTIL_EXPORT __declspec(dllexport)
#endif // _WINDOWS

#if (defined _MACOS) || (defined _LINUX)
#define QTALK_UTIL_EXPORT
#endif // _MAC

#endif // _QTALKUTILGLOBAL_H_
