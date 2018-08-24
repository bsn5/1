#include "StatesHandler.h"

void DapCmdStatesHandler::handler(const QJsonObject* params) {
    Q_UNUSED(params);
    if(m_indicatorsStateList == Q_NULLPTR) {
        qCritical() << "Indicator list states is NULL";
        return;
    }

    for (const auto si: *m_indicatorsStateList) {
            QByteArray baCmd = DapJsonCmd::generateCmd(DapJsonCommands::STATE, {
                                                           DapJsonParam(DapJsonParams::STATE_NAME, si->name()),
                                                           DapJsonParam(DapJsonParams::VALUE, DapIndicatorState::toString(
                                                           si->current())),
                                                       });
        emit sendDapCmd(baCmd);
    }
}

DapCmdStatesHandler::DapCmdStatesHandler(QList<DapSI*> *siList, QObject *parent)
    : QObject(parent) {
    m_indicatorsStateList = siList;
}
