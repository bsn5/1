#ifndef DAPTUNMAC_H
#define DAPTUNMAC_H

#include "DapTunAbstract.h"

class DapTunMac : public DapTunAbstract
{
public:
    DapTunMac();
    ~DapTunMac();
protected:
    void tunDeviceCreate();
    void tunDeviceDestroy();
    void onWorkerStarted();

    void workerPrepare(){;}
    void workerStop(){;}
    void signalWriteQueueProc(){;}
};

#endif // DAPTUNMAC_H
