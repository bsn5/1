#ifndef DAPTUNWORKERUNIX_H
#define DAPTUNWORKERUNIX_H

#include "DapTunWorkerAbstract.h"

class DapTunWorkerUnix : public DapTunWorkerAbstract
{
public:
    static const struct timespec BREAK_LATENCY;

    DapTunWorkerUnix(DapTunAbstract *a_tun);
    int breaker(int arg){ return m_breakerSet[arg];}
public slots:

    void loop() override;

private:
    int m_breakerSet[2];
};

#endif // DAPTUNWORKERUNIX_H
