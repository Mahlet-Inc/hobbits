#include "hobbitsrunnerinfo.h"


QString HobbitsRunnerInfo::getRunnerVersion()
{
#ifdef HOBBITS_RUNNER_VERSION
    QString version = HOBBITS_RUNNER_VERSION;
#else
    QString version = "Non-standard Build Version";
#endif
    return version;
}
