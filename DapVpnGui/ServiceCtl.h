#ifndef SERVICECTL_H
#define SERVICECTL_H

#include "DapServiceClient.h"

class QTimer;
class ServiceCtl : public DapServiceClient
{
    Q_OBJECT
public:
    static ServiceCtl& me(){static ServiceCtl _me; return _me; }
signals:
    void sigDisconnected();
    void sigConnected();


    void sigStatistics(QString, QString);

    void sigStateAuthorizing();
    void sigStateAuthorized();
    void sigStateAuthorizeError();
    void sigStateUnauthorized();
    void sigStateUnauthorizing();

    void sigStateStreamOpened();
    void sigStateStreamOpening();
    void sigStateStreamClosing();
    void sigStateStreamError();
    void sigStateStreamClosed();

    void sigStateNetConfigTrue();
    void sigStateNetConfigRequested();
    void sigStateNetConfigError();
    void sigStateNetConfigFalse();

    void sigStateTunnelCreated();
    void sigStateTunnelCreating();
    void sigStateTunnelDestroyed();
    void sigStateTunnelDestroying();
    void sigStateTunnelError();

protected:

    void procCmd(const QString & a_cmd);

    ServiceCtl();
private:
    QString m_username, m_password;
    QString m_addrAssigned;
    QString m_addrGW;
    int m_socketUpstream;
    QString addrLine;
    QString m_addr;
    int m_port;

    QTimer * tmRestart;
};

#endif // SERVICECTL_H
