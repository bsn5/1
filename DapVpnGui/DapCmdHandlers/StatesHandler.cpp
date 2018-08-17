#include "StatesHandler.h"
#include "DapJsonCmd.h"
#include <QDebug>

DapCmdStatesHandler::DapCmdStatesHandler(QObject *parent)
    : QObject(parent)
{

}

void DapCmdStatesHandler::handler(const QJsonObject * params) {
    static QMap<QString, void(DapCmdStatesHandler::*)(IndicatorState)> stateCallbacks = {
    {"authorization", &DapCmdStatesHandler::authorizeHandler},
    {"tunnel", &DapCmdStatesHandler::tunnelHandler},
    {"stream", &DapCmdStatesHandler::streamHandler},
    {"netconfig", &DapCmdStatesHandler::netconfigHandler}
};

    qDebug() << "Call stateHandler" << *params;
    if (!params->contains(g_stateName) ||
            !params->contains("value")) {
        qWarning() << "Not found mandatory parameter!";
        return;
    }
    const QString stateName = params->value(g_stateName).toString();
    const IndicatorState state = DapIndicatorState::fromString(params->value("value").toString());


    if(!stateCallbacks.contains(stateName)) {
        qWarning() << "Not found handler for " << stateName;
        return;
    }
    (this->*stateCallbacks[stateName])(state);
}

void DapCmdStatesHandler::authorizeHandler(IndicatorState state) {
    switch (state) {
    case IndicatorState::False:
        emit sigUnauthorized();
        return;
    case IndicatorState::SwitchingToTrue:
        emit sigAuthorizing();
        return;
    case IndicatorState::True:
        emit sigAuthorized();
        return;
    case IndicatorState::SwitchingToFalse:
        emit sigUnauthorizing();
        return;
    case IndicatorState::Error:
        emit sigAuthorizeError();
        return;
    case IndicatorState::ErrorAuth:
        emit sigAuthorizeErrorAuth();
        return;
    case IndicatorState::ErrorNetwork:
        emit sigAuthorizeErrorNetwork();
        return;
    }
    throw std::runtime_error("Can't handle Authorize state");
}

void DapCmdStatesHandler::tunnelHandler(IndicatorState state) {
    switch (state) {
    case IndicatorState::False:
        emit sigTunnelDestroyed();
        return;
    case IndicatorState::SwitchingToTrue:
        emit sigTunnelCreating();
        return;
    case IndicatorState::True:
        emit sigTunnelCreated();
        return;
    case IndicatorState::SwitchingToFalse:
        emit sigTunnelDestroying();
        return;
    case IndicatorState::Error:
        emit sigTunnelError();
        return;
    case IndicatorState::ErrorAuth:
        emit sigTunnelErrorAuth();
        return;
    case IndicatorState::ErrorNetwork:
        emit sigTunnelErrorNetwork();
        return;
    }
    throw std::runtime_error("Can't handle Tunnel state");
}

void DapCmdStatesHandler::streamHandler(IndicatorState state) {
    switch (state) {
    case IndicatorState::False:
        emit sigStreamClosed();
        return;
    case IndicatorState::SwitchingToTrue:
        emit sigStreamOpening();
        return;
    case IndicatorState::True:
        emit sigStreamOpened();
        return;
    case IndicatorState::SwitchingToFalse:
        emit sigStreamClosing();
        return;
    case IndicatorState::Error:
        emit sigStreamError();
        return;
    case IndicatorState::ErrorAuth:
        emit sigStreamErrorAuth();
        return;
    case IndicatorState::ErrorNetwork:
        emit sigStreamErrorNetwork();
        return;
    }
    throw std::runtime_error("Can't handle stream state");
}

void DapCmdStatesHandler::netconfigHandler(IndicatorState state) {
    switch (state) {
    case IndicatorState::False:
        emit sigNetConfigFalse();
        return;
    case IndicatorState::SwitchingToTrue:
        emit sigNetConfigRequesting();
        return;
    case IndicatorState::True:
        emit sigNetConfigTrue();
        return;
    case IndicatorState::SwitchingToFalse:
        qWarning() << "Netconfig no have handler and signal for SwitchingToFalse";
        return;
    case IndicatorState::Error:
        emit sigNetConfigError();
        return;
    case IndicatorState::ErrorAuth:
        emit sigNetConfigErrorAuth();
        return;
    case IndicatorState::ErrorNetwork:
        emit sigNetConfigErrorNetwork();
        return;
    }
    throw std::runtime_error("Can't handle netconfig state");
}
