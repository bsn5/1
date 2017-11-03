#ifndef DAPSERVICENATIVEWINDOWS_H
#define DAPSERVICENATIVEWINDOWS_H

#include"DapServiceNativeAbstract.h"

class DapServiceNativeWindows : public DapServiceNativeAbstract
{
public:
    DapServiceNativeWindows();
    void restartService();
    void checkInstallation();
};

#endif // DAPSERVICENATIVEWINDOWS_H
