#include "StatesHandler.h"
#include "DapJsonCmd.h"
#include <QDebug>

DapCmdStatesHandler::DapCmdStatesHandler(QObject *parent)
    : QObject(parent)
{

}

void DapCmdStatesHandler::handler(const QJsonObject * params) {
    static QMap<QString, void(*)(IndicatorState)> stateCallbacks = {
    {"authorization", DapCmdStatesHandler::authorizeHandler},
    {"tunnel", DapCmdStatesHandler::tunnelHandler},
    {"stream", DapCmdStatesHandler::streamHandler},
    {"netconfig", DapCmdStatesHandler::netconfigHandler}
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

    stateCallbacks[stateName](state);
}

void DapCmdStatesHandler::authorizeHandler(IndicatorState state) {
    switch (state) {
    case IndicatorState::False:
        emit DapCmdStatesHandler::me().sigUnauthorized();
        return;
    case IndicatorState::SwitchingToTrue:
        emit DapCmdStatesHandler::me().sigAuthorizing();
        return;
    case IndicatorState::True:
        emit DapCmdStatesHandler::me().sigAuthorized();
        return;
    case IndicatorState::SwitchingToFalse:
        emit DapCmdStatesHandler::me().sigUnauthorizing();
        return;
    case IndicatorState::Error:
        emit DapCmdStatesHandler::me().sigAuthorizeError();
        return;
    case IndicatorState::ErrorAuth:
        emit DapCmdStatesHandler::me().sigAuthorizeErrorAuth();
        return;
    case IndicatorState::ErrorNetwork:
        emit DapCmdStatesHandler::me().sigAuthorizeErrorNetwork();
        return;
    }
    throw std::runtime_error("Can't handle Authorize state");
}

void DapCmdStatesHandler::tunnelHandler(IndicatorState state) {
    switch (state) {
    case IndicatorState::False:
        emit DapCmdStatesHandler::me().sigTunnelDestroyed();
        return;
    case IndicatorState::SwitchingToTrue:
        emit DapCmdStatesHandler::me().sigTunnelCreating();
        return;
    case IndicatorState::True:
        emit DapCmdStatesHandler::me().sigTunnelCreated();
        return;
    case IndicatorState::SwitchingToFalse:
        emit DapCmdStatesHandler::me().sigTunnelDestroying();
        return;
    case IndicatorState::Error:
        emit DapCmdStatesHandler::me().sigTunnelError();
        return;
    case IndicatorState::ErrorAuth:
        emit DapCmdStatesHandler::me().sigTunnelErrorAuth();
        return;
    case IndicatorState::ErrorNetwork:
        emit DapCmdStatesHandler::me().sigTunnelErrorNetwork();
        return;
    }
    throw std::runtime_error("Can't handle Tunnel state");
}

void DapCmdStatesHandler::streamHandler(IndicatorState state) {
    switch (state) {
    case IndicatorState::False:
        emit DapCmdStatesHandler::me().sigStreamClosed();
        return;
    case IndicatorState::SwitchingToTrue:
        emit DapCmdStatesHandler::me().sigStreamOpening();
        return;
    case IndicatorState::True:
        emit DapCmdStatesHandler::me().sigStreamOpened();
        return;
    case IndicatorState::SwitchingToFalse:
        emit DapCmdStatesHandler::me().sigStreamClosing();
        return;
    case IndicatorState::Error:
        emit DapCmdStatesHandler::me().sigStreamError();
        return;
    case IndicatorState::ErrorAuth:
        emit DapCmdStatesHandler::me().sigStreamErrorAuth();
        return;
    case IndicatorState::ErrorNetwork:
        emit DapCmdStatesHandler::me().sigStreamErrorNetwork();
        return;
    }
    throw std::runtime_error("Can't handle stream state");
}

void DapCmdStatesHandler::netconfigHandler(IndicatorState state) {
    switch (state) {
    case IndicatorState::False:
        emit DapCmdStatesHandler::me().sigNetConfigFalse();
        return;
    case IndicatorState::SwitchingToTrue:
        emit DapCmdStatesHandler::me().sigNetConfigRequesting();
        return;
    case IndicatorState::True:
        emit DapCmdStatesHandler::me().sigNetConfigTrue();
        return;
    case IndicatorState::SwitchingToFalse:
        qWarning() << "Netconfig no have handler and signal for SwitchingToFalse";
        return;
    case IndicatorState::Error:
        emit DapCmdStatesHandler::me().sigNetConfigError();
        return;
    case IndicatorState::ErrorAuth:
        emit DapCmdStatesHandler::me().sigNetConfigErrorAuth();
        return;
    case IndicatorState::ErrorNetwork:
        emit DapCmdStatesHandler::me().sigNetConfigErrorNetwork();
        return;
    }
    throw std::runtime_error("Can't handle netconfig state");
}
