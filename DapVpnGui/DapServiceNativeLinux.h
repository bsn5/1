#ifndef DAPSERVICENATIVELINUX_H
#define DAPSERVICENATIVELINUX_H

#include "DapServiceNativeAbstract.h"

class DapServiceNativeLinux : public DapServiceNativeAbstract
{
public:
    DapServiceNativeLinux();
    void checkInstallation() override;
    void restartService() override;
};

#endif // DAPSERVICENATIVELINUX_H
