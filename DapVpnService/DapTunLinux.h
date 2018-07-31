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

    // Getting currently using connection interface name from nmcli command-line tool
    QString currentConnectionInterface();
private:
    QString nmcliVersion;
    static int nmcliVersionMajor;
    static int nmcliVersionMinor;
    static int nmcliVersionBuild;

    // Connection witch used before creating DiveVPN Interface
    QString m_lastUsedConnectionName;
    void setLastUsedConnection();
};

#endif // DAPTUNLINUX_H
