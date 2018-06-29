#ifndef DAPTUNANDROID_H
#define DAPTUNANDROID_H

#include "DapTunUnixAbstract.h"

class DapTunAndroid : public DapTunUnixAbstract
{
public:
    DapTunAndroid();
    void tunDeviceCreate();
    void tunDeviceDestroy();
    void onWorkerStarted();
};

#endif // DAPTUNANDROID_H
