#include "DapGuiCmdHandler.h"
#include "DapCmdConnHandler.h"
#include "DapSession.h"

DapJsonCmdHandlersMap DapGuiCmdHandler::m_commandHandlers = {
    {DapJsonCommands::CONNECTION, DapCmdConnHandler::handler},
    {DapJsonCommands::GET_STATES, DapGuiCmdHandler::getStatesHandler}
};


DapGuiCmdHandler::DapGuiCmdHandler(QObject *parent)
    : QObject(parent) {

}

void DapGuiCmdHandler::setIndicatorsStateList(QList<DapSI*> * ist) {
    m_indicatorsStateList = ist;
}

void DapGuiCmdHandler::getStatesHandler(const QJsonObject* params) {
    Q_UNUSED(params);
    if(me().m_indicatorsStateList == Q_NULLPTR) {
        qCritical() << "Indicator list states is NULL";
        return;
    }

    for (const auto si: *me().m_indicatorsStateList) {
            QByteArray baCmd = DapJsonCmd::generateCmd(DapJsonCommands::STATE, {
                                                           DapJsonParam(g_stateName, si->name()),
                                                           DapJsonParam("value", DapIndicatorState::toString(
                                                           si->current())),
                                                       });
        emit me().sendDapCmd(baCmd);
    }
}

void DapGuiCmdHandler::handler(DapJsonCmdPtr ptr) {
    auto iter = m_commandHandlers.find(ptr->getCommand());
    if (iter == m_commandHandlers.end()) {
        qWarning() << "Not found handler for command "
                   << ptr->commandToString(ptr->getCommand());
        return;
    }
    // cal handler function
    (*iter)(ptr->getParams());
}
