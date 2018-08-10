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
    /* Auth signals */
    void sigStateAuthorized();
    void sigStateAuthorizeError();

    void sigStateUnauthorized();

    void sigStateStreamOpened();
    void sigStateStreamClosed();

    void sigStateNetConfigTrue();
    void sigStateNetConfigFalse();

    void sigStateTunnelCreated();
    void sigStateTunnelDestroyed();
public slots:
};

#endif // DAPSTATESHANDLER_H
