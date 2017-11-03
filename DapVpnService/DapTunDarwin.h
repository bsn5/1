#ifndef DAPTUNDARWIN_H
#define DAPTUNDARWIN_H

#include "DapTunUnixAbstract.h"

class DapTunDarwin : public DapTunUnixAbstract
{
public:
    DapTunDarwin();
protected:
    void tunDeviceCreate() override;
    void onWorkerStarted() override;
    void onWorkerStopped() override;
};

#endif // DAPTUNDARWIN_H
