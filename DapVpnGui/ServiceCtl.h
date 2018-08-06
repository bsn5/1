#ifndef SERVICECTL_H
#define SERVICECTL_H

#include "DapServiceClient.h"
#include "DapJsonCmd.h"

#include <QMap>

class ServiceCtl;

typedef void (*CommandHandler)(const QJsonObject*);

class QTimer;
class ServiceCtl : public DapServiceClient
{
    Q_OBJECT
public:
    static ServiceCtl& me(){static ServiceCtl _me; return _me; }
    using commandHalders = QMap<DapJsonCommands, CommandHandler>;
    static commandHalders m_commandHandlers;

    void stateHandler();
    //void foo(const QJsonObject* s);
signals:
    void sigStatistics(QString, QString);

    void sigStateAuthorized();
    void sigStateAuthorizeError();
    void sigStateUnauthorized();

    void sigStateStreamOpened();
    void sigStateStreamClosed();

    void sigStateNetConfigTrue();
    void sigStateNetConfigFalse();

    void sigStateTunnelCreated();
    void sigStateTunnelDestroyed();

protected:
    void procCmdHandler(const QByteArray &a_cmd) override;

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
