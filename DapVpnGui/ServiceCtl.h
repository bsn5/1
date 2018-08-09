#ifndef SERVICECTL_H
#define SERVICECTL_H

#include "DapServiceClient.h"
#include "DapJsonCmd.h"

#include <QMap>

class ServiceCtl;

class QTimer;
class ServiceCtl : public DapServiceClient
{
    Q_OBJECT
public:
    static ServiceCtl& me(){static ServiceCtl _me; return _me; }
protected:
    void procCmdHandler(const QByteArray &a_cmd) override;

    ServiceCtl();
private:
    static DapJsonCmdHandlersMap m_commandHandlers;

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
