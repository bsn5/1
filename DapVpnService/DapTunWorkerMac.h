#ifndef DAPTUNWORKERMAC_H
#define DAPTUNWORKERMAC_H

#include "DapTunWorkerAbstract.h"


class DapTunWorkerMac : public DapTunWorkerAbstract
{
public:
    DapTunWorkerMac(DapTunAbstract *a_tun);

    int breaker( int arg ) { return m_breakerSet[arg]; }

public slots:
    void loop() override;

private:
    int m_breakerSet[2];
};

#endif // DAPTUNWORKERMAC_H
