#include "DataBasePlug.h"
#include "databaseplug_global.h"

extern "C" DATABASEPLUGSHARED_EXPORT IDatabasePlug* Handle()
{
    return new DataBasePlug();
}
