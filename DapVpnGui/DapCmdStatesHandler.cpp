#include "DapCmdStatesHandler.h"


DapCmdStatesHandler::DapCmdStatesHandler(QObject *parent)
    : QObject(parent)
{

}

void DapCmdStatesHandler::handler(const QJsonObject * params) {
    qDebug() << "Call stateHandler" << *params;
    for(auto &k : params->keys()) {
        if (k == "authorize") {
            DapCmdStatesHandler::authorizeHandler(params->value(k).toString());
        } else if(k == "tunnel") {
            DapCmdStatesHandler::tunnelHandler(params->value(k).toString());
        }
    }
}

void DapCmdStatesHandler::authorizeHandler(const QString& state) {
    if (state == "true") {
        emit DapCmdStatesHandler::me().sigStateAuthorized();
    } else if (state == "false") {
        emit DapCmdStatesHandler::me().sigStateUnauthorized();
    }
    // TODO
}

void DapCmdStatesHandler::tunnelHandler(const QString& state) {
    if (state == "true") {
        emit DapCmdStatesHandler::me().sigStateTunnelCreated();
    } else if (state == "false") {
        emit DapCmdStatesHandler::me().sigStateUnauthorized();
    }
    // TODO
}

void DapCmdStatesHandler::streamHandler(const QString& state) {
    // TODO
    Q_UNUSED(state);
}

void DapCmdStatesHandler::netconfigHandler(const QString& state) {
    // TODO
    Q_UNUSED(state);
}
