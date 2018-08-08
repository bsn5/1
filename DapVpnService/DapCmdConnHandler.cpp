#include "DapCmdConnHandler.h"
#include <QMap>
#include <QDebug>
#include "DapSession.h"

const QString DapCmdConnHandler::disconnectParam = "disconnect";

DapCmdConnHandler::DapCmdConnHandler(QObject *parent)
    : QObject(parent) {

}

void DapCmdConnHandler::handler(const QJsonObject* params) {
    if(params->value(disconnectParam) != QJsonValue::Undefined) { //this is disconnect request
        emit DapCmdConnHandler::me().sigDisconnect();
        return;
    }

    QMap<QString, QJsonValue> mandatoryConnParams = {
        {"address", QJsonValue::Undefined },
        {"password", QJsonValue::Undefined },
        {"port", QJsonValue::Undefined },
        {"user", QJsonValue::Undefined },
    };

    for(auto &k: mandatoryConnParams.keys()) {
        QJsonValue val = params->value(k);
        if (val == QJsonValue::Undefined) {
            qWarning() << "Not found mandatory key"
                       << k << " for establish a connection";
            return;
        }
        mandatoryConnParams[k] = val;
    }
    DapSession::getInstance()->setDapUri(mandatoryConnParams["address"].toString(),
                                         uint16_t(mandatoryConnParams["port"].toInt()));

    emit DapCmdConnHandler::me().sigConnect(mandatoryConnParams["user"].toString(),
            mandatoryConnParams["password"].toString());
}
