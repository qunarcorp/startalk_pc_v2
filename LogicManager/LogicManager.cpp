#include "LogicManager.h"
#include "../interface/logic/ILogicObject.h"
#include "../interface/logic/ILogicBase.h"
#include "../QtUtil/Utils/Log.h"
#include <assert.h>
#include <vector>
#ifdef _WINDOWS
#include <windows.h>
#include <tchar.h>
#else
#include <dlfcn.h>
#endif


LogicManager *LogicManager::_pLogicManager = nullptr;

LogicManager::LogicManager() :
        _dataBase(nullptr), _logicBase(nullptr) {

    _logicPlugs["LogicBase"] = nullptr;
    _logicPlugs["LogicCommunication"] = nullptr;
    _logicPlugs["DataBasePlug"] = nullptr;

    // 加载动态库
    loadLibrary();
    // 加载数据库
//    loadLibraryDb();
}

// 
void LogicManager::loadLibrary() {
#ifdef _WINDOWS
    typedef ILogicObject*(*Handle)();
#else
    typedef ILogicObject *(*Handle)();
#endif

    auto it = _logicPlugs.begin();
    for (; it != _logicPlugs.end(); it++) {
#ifdef _WINDOWS
        HINSTANCE dll;
        std::string strLibraryName = it->first;
#ifdef _DEBUG
        strLibraryName += "d";
#endif // _DEBUG
        dll = LoadLibraryA((strLibraryName.c_str()));
        if (dll == NULL)
        {
            FreeLibrary(dll);
            continue;
        }

        Handle handle = (Handle)GetProcAddress(dll, "Handle");
        if (handle == NULL)
        {
            FreeLibrary(dll);
            continue;
        }
        // 调用构造
        it->second = handle();
#else
#ifdef _MACOS
        void *dll;
        std::string strLibraryName = "./lib" + it->first;
#ifdef _DEBUG
        strLibraryName += "d";
#endif // _DEBUG
        strLibraryName += ".dylib";
        dll = dlopen(strLibraryName.c_str(), RTLD_NOW);
        if (!dll) {
            continue;
        }
        dlerror();
        auto handle = (Handle) dlsym(dll, "Handle");
        if (handle == nullptr) {
            dlclose(dll);
            continue;
        }
        // 调用构造
        it->second = handle();
#else
        void *dll;
        std::string strLibraryName = "./lib" + it->first;
#ifdef _DEBUG
        strLibraryName += "d";
#endif // _DEBUG
        strLibraryName += ".so";
        dll = dlopen (strLibraryName.c_str(), RTLD_NOW);
        if (!dll) {
            char * err = dlerror();
            continue;
        }
        Handle handle = (Handle)dlsym(dll, "Handle");
        if (handle == NULL)
        {
             dlclose(dll);
             continue;
        }
        // 调用构造
        it->second = handle();
#endif
#endif
    }
}

/**
  * @函数名
  * @功能描述 加载数据库插件
  * @参数
  * @date 2018.9.21
  */
//void LogicManager::loadLibraryDb() {
//
//    typedef IDatabasePlug *(*Handle)();
//#ifdef _WINDOWS
//    HINSTANCE dll;
//    std::string strLibraryName = "DataBasePlug";
//#ifdef _DEBUG
//    strLibraryName += "d";
//#endif // _DEBUG
//
//    dll = LoadLibraryA((strLibraryName.c_str()));
//
//    if (dll == NULL)
//    {
//        FreeLibrary(dll);
//        return;
//    }
//
//    Handle handle = (Handle)GetProcAddress(dll, "Handle");
//    if (handle == NULL)
//    {
//        FreeLibrary(dll);
//        return;
//    }
//
//    _dataBase = handle();
//
//#else
//#ifdef _MACOS
//
//    void *dll;
//    std::string strLibraryName = "./libDataBasePlug";
//#ifdef _DEBUG
//    strLibraryName += "d";
//#endif // _DEBUG
//    strLibraryName += ".dylib";
//    dll = dlopen(strLibraryName.c_str(), RTLD_NOW);
//    if (!dll) {
//        dlclose(dll);
//        return;
//    }
//    dlerror();
//    Handle handle = (Handle) dlsym(dll, "Handle");
//    if (handle == nullptr) {
//        dlclose(dll);
//        return;
//    }
//    // 调用构造
//    _dataBase = handle();
//#else
//    void *dll;
//    std::string strLibraryName =  "./libDataBasePlug";
//#ifdef _DEBUG
//    strLibraryName += "d";
//#endif // _DEBUG
//    strLibraryName += ".so";
//    dll = dlopen (strLibraryName.c_str(), RTLD_NOW);
//    if (!dll) {
//        info_log(std::string(" load error ") + dlerror());
//        std::cout << dlerror();
//        return;
//    }
//
//    Handle handle = (Handle)dlsym(dll,"Handle" );
//    if (handle == NULL)
//    {
//         dlclose(dll);
//         return;
//    }
//    // 调用构造
//    _dataBase = handle();
//#endif
//
//#endif
//}

LogicManager::~LogicManager() {

}


LogicManager *LogicManager::instance() {
    if (nullptr == _pLogicManager) {
        _pLogicManager = new LogicManager();
    }
    return _pLogicManager;
}

/**
  * @函数名
  * @功能描述
  * @参数
  * @date 2018.9.21
  */
IDatabasePlug *LogicManager::getDatabase() {
    if (nullptr == _dataBase) {
        _dataBase = dynamic_cast<IDatabasePlug*>(_logicPlugs["DataBasePlug"]);
    }
    assert(nullptr != _dataBase);
    return _dataBase;
}

ILogicBase* LogicManager::getLogicBase()
{
    if (nullptr == _logicBase) {
        _logicBase = dynamic_cast<ILogicBase*>(_logicPlugs["LogicBase"]);
    }
    assert(nullptr != _logicBase);
    return _logicBase;
}

