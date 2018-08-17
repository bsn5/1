#include "StatsHandler.h"
#include "DapJsonCmd.h"
#include <utility>

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
    emit DapCmdStatsHandler::me().sigReadWriteBytesStat(bRead, bWrite);
}

void DapCmdStatsHandler::handler(const QJsonObject* params) {
    if (params->value(g_readKbytesParam) != QJsonValue::Undefined &&
            params->value(g_writeKbytesParam) != QJsonValue::Undefined) {
        DapCmdStatsHandler::trafficStatsHandler(params->value(g_readKbytesParam),
                                                params->value(g_writeKbytesParam));
    }

}

