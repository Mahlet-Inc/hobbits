#include "hobbitsguiinfo.h"

QString HobbitsGuiInfo::getGuiVersion()
{
#ifdef HOBBITS_GUI_VERSION
    QString guiVersion = HOBBITS_GUI_VERSION;
#else
    QString guiVersion = "Elusive Mystery Version";
#endif
    return guiVersion;
}
