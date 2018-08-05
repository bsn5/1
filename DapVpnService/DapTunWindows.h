#ifndef DAPTUNWINDOWS_H
#define DAPTUNWINDOWS_H

#include "DapTunAbstract.h"
class DapTunWindows : public DapTunAbstract
{
public:
    DapTunWindows();
protected:
    void tunDeviceCreate() override;
    void tunDeviceDestroy() override;
    void onWorkerStarted() override;
};

#endif // DAPTUNWINDOWS_H
