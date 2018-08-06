#include "DapStatesHandler.h"


DapStatesHandler::DapStatesHandler(QObject *parent)
    : QObject(parent)
{

}

void DapStatesHandler::handler(const QJsonObject * params) {
    qDebug() << "Call stateHandler" << *params;
    for(auto &k : params->keys()) {
        if (k == "authorize") {
            DapStatesHandler::authorizeHandler(params->value(k).toString());
        } else if(k == "tunnel") {
            DapStatesHandler::tunnelHandler(params->value(k).toString());
        }
    }
}

void DapStatesHandler::authorizeHandler(const QString& state) {
    if (state == "true") {
        emit DapStatesHandler::me().sigStateAuthorized();
    } else if (state == "false") {
        emit DapStatesHandler::me().sigStateUnauthorized();
    }
    // TODO
}

void DapStatesHandler::tunnelHandler(const QString& state) {
    if (state == "true") {
        emit DapStatesHandler::me().sigStateTunnelCreated();
    } else if (state == "false") {
        emit DapStatesHandler::me().sigStateUnauthorized();
    }
    // TODO
}

void DapStatesHandler::streamHandler(const QString& state) {
    // TODO
    Q_UNUSED(state);
}

void DapStatesHandler::netconfigHandler(const QString& state) {
    // TODO
    Q_UNUSED(state);
}
