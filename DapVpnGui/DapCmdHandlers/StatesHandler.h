#ifndef DAPSTATESHANDLER_H
#define DAPSTATESHANDLER_H

#include <QObject>
#include <QJsonObject>
#include <QDebug>
#include "DapIndicatorState.h"
#include "DapCmdHandlerAbstract.h"

using IndicatorState = DapIndicatorState::IndicatorState;

class DapCmdStatesHandler : public QObject, public DapCmdHandlerAbstract
{
    Q_OBJECT
public:
    void handler(const QJsonObject * params) override;
    explicit DapCmdStatesHandler(QObject *parent = nullptr);
private:
    static const QString stateNameParam;
    void authorizeHandler(IndicatorState state);
    void tunnelHandler(IndicatorState state);
    void streamHandler(IndicatorState state);
    void netconfigHandler(IndicatorState state);
signals:
    /* Auth signals group*/
    void sigAuthorizing();
    void sigAuthorized();
    void sigUnauthorizing();
    void sigAuthorizeError();
    void sigAuthorizeErrorAuth();
    void sigAuthorizeErrorNetwork();
    void sigUnauthorized();

    /* Tunnel signals group */
    void sigTunnelCreated();
    void sigTunnelDestroyed();
    void sigTunnelCreating();
    void sigTunnelError();
    void sigTunnelErrorAuth();
    void sigTunnelErrorNetwork();
    void sigTunnelDestroying();

    /* Stream signalns group */
    void sigStreamOpened();
    void sigStreamOpening();
    void sigStreamClosed();
    void sigStreamClosing();
    void sigStreamError();
    void sigStreamErrorAuth();
    void sigStreamErrorNetwork();

    /* NetConfig signals group */
    void sigNetConfigTrue();
    void sigNetConfigFalse();
    void sigNetConfigError();
    void sigNetConfigErrorAuth();
    void sigNetConfigErrorNetwork();
    void sigNetConfigRequesting();

public slots:
};

#endif // DAPSTATESHANDLER_H
