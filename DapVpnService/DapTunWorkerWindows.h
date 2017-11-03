#ifndef DAPTUNWORKERWINDOWS_H
#define DAPTUNWORKERWINDOWS_H

#include "DapTunWorkerAbstract.h"
class DapTunWorkerWindows : public DapTunWorkerAbstract
{
public:
    DapTunWorkerWindows(DapTunAbstract *a_tun);
public slots:
    void loop() override;
};

#endif // DAPTUNWORKERWINDOWS_H
