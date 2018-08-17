#ifndef SERVICECTL_H
#define SERVICECTL_H

#include "DapServiceClient.h"
#include "DapJsonCmd.h"
#include "StatesHandler.h"
#include "StatsHandler.h"

#include <QMap>

class ServiceCtl;

class QTimer;
class ServiceCtl : public DapServiceClient
{
    Q_OBJECT
public:
    static ServiceCtl& me(){static ServiceCtl _me; return _me; }
    const DapCmdStatsHandler * getStatsHandleObj() { return m_statsHandler; }
    const DapCmdStatesHandler * getStatesHandleObj() { return m_statesHandler; }
protected:
    void procCmdController(const QByteArray &a_cmd) override;
    DapJsonCmdHandlersMap m_commandHandlers;
    ServiceCtl();
private:
    DapCmdStatesHandler * m_statesHandler;
    DapCmdStatsHandler * m_statsHandler = Q_NULLPTR;

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
