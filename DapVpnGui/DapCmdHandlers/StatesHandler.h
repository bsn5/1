#ifndef DAPSTATESHANDLER_H
#define DAPSTATESHANDLER_H

#include <QObject>
#include <QJsonObject>
#include <QDebug>
#include "DapIndicatorState.h"

using IndicatorState = DapIndicatorState::IndicatorState;

class DapCmdStatesHandler : public QObject
{
    Q_OBJECT
public:
    static DapCmdStatesHandler& me(){static DapCmdStatesHandler _me; return _me; }
    static void handler(const QJsonObject * params);
private:
    static void authorizeHandler(IndicatorState state);
    static void tunnelHandler(IndicatorState state);
    static void streamHandler(IndicatorState state);
    static void netconfigHandler(IndicatorState state);
    explicit DapCmdStatesHandler(QObject *parent = nullptr);
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
