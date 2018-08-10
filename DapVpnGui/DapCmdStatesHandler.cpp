#include "DapCmdStatesHandler.h"
#include "DapJsonCmd.h"


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
        case IndicatorState::True:
            emit DapCmdStatesHandler::me().sigStateAuthorized();
        break;
        case IndicatorState::False:
            emit emit DapCmdStatesHandler::me().sigStateUnauthorized();
    }
    // TODO
}

void DapCmdStatesHandler::tunnelHandler(IndicatorState state) {
    switch (state) {
        case IndicatorState::True:
            emit DapCmdStatesHandler::me().sigStateTunnelCreated();
        break;
        case IndicatorState::False:
            emit DapCmdStatesHandler::me().sigStateUnauthorized();
    }
}

void DapCmdStatesHandler::streamHandler(IndicatorState state) {
    // TODO
    Q_UNUSED(state);
}

void DapCmdStatesHandler::netconfigHandler(IndicatorState state) {
    // TODO
    Q_UNUSED(state);
}
