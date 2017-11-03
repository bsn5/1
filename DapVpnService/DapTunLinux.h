#ifndef DAPTUNLINUX_H
#define DAPTUNLINUX_H

#include "DapTunUnixAbstract.h"

class DapTunLinux : public DapTunUnixAbstract
{
public:
    DapTunLinux();
protected:
    void tunDeviceCreate();
    void tunDeviceDestroy();
    void onWorkerStarted();
private:
    QString nmcliVersion;
    static int nmcliVersionMajor;
    static int nmcliVersionMinor;
    static int nmcliVersionBuild;
};

#endif // DAPTUNLINUX_H
