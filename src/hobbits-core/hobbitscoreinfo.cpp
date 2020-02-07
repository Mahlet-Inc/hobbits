#include "hobbitscoreinfo.h"

namespace HobbitsCoreInfo
{
QString getLibVersion()
{
#ifdef HOBBITS_CORE_LIB_VERSION
    return HOBBITS_CORE_LIB_VERSION;
#else
    return "Spooky Unknown Version";
#endif
}

}
