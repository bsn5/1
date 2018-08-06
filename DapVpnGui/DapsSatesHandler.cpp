#include "DapStatesHandler.h"


DapStatesHandler::DapStatesHandler(QObject *parent)
    : QObject(parent)
{

}

void DapStatesHandler::handler(const QJsonObject * params) {
    qDebug() << "Call stateHandler" << *params;
    for(auto &k : params->keys()) {
        if (k == "authorize") {
            if (params->value(k).toString() == "true") {
                emit DapStatesHandler::me().sigStateAuthorized();
            } else {
                emit DapStatesHandler::me().sigStateUnauthorized();
            }
        } else if(k == "tunnel") {
            if (params->value(k).toString() == "true") {
                emit DapStatesHandler::me().sigStateTunnelCreated();
            } else {
                emit DapStatesHandler::me().sigStateUnauthorized();
            }
        }
    }
}
