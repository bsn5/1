#include "StatsHandler.h"
#include "DapJsonCmd.h"
#include <utility>

const QString DapCmdStatsHandler::readBytesParam =
        DapJsonParams::toString(DapJsonParams::READ_BYTES);

const QString DapCmdStatsHandler::writeBytesParam =
        DapJsonParams::toString(DapJsonParams::WRITE_BYTES);

DapCmdStatsHandler::DapCmdStatsHandler(QObject *parent)
    : QObject(parent)
{

}

void DapCmdStatsHandler::trafficStatsHandler(
        const QJsonValue& readBytes, const QJsonValue& writeBytes) {
    int bRead = readBytes.toInt(-1),
            bWrite = writeBytes.toInt(-1);
    if (bRead == -1 || bWrite == -1) {
        qWarning() << "Something wrong can't parse traffic stats";
        return;
    }
    emit sigReadWriteBytesStat(bRead, bWrite);
}

void DapCmdStatsHandler::handler(const QJsonObject* params) {
    if (params->value(readBytesParam) != QJsonValue::Undefined &&
            params->value(writeBytesParam) != QJsonValue::Undefined) {
        DapCmdStatsHandler::trafficStatsHandler(params->value(readBytesParam),
                                                params->value(writeBytesParam));
    }
}

