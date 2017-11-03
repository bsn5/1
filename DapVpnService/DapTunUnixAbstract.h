#ifndef DAPTUNUNIXABSTRACT_H
#define DAPTUNUNIXABSTRACT_H

#include <QObject>
#include "DapTunAbstract.h"

class DapTunWorkerUnix;

class DapTunUnixAbstract : public DapTunAbstract
{
    Q_OBJECT
public:
    DapTunUnixAbstract();
protected:

    QString m_ethDevice; // eth adapter device name
    QString m_TunDeviceH;// tunDevice name human freendly name

    void tunDeviceCreate(){;}


    void workerStop();
    void workerPrepare();
    void tunDeviceDestroy();
    void signalWriteQueueProc();

    int breaker0,breaker1;
    DapTunWorkerUnix * tunWorkerUnix;

};

#endif // DAPTUNUNIX_H
