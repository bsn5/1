#include "AuthErrorHandler.h"

DapCmdAuthErrorHandler::DapCmdAuthErrorHandler(QObject *parent)
    : QObject(parent)
{

}

void DapCmdAuthErrorHandler::handler(const QJsonObject* params) {
    qDebug() << "DapCmdAuthErrorHandler";
    if(!params->contains("message")) {
        qWarning() << "Not found mandatory parameter!";
        return;
    }
    emit errorMessage(params->value("message").toString());
}
