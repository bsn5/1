#include "DapGuiCmdHandler.h"
#include "DapCmdConnHandler.h"
#include "DapSession.h"
#include <functional>
#include <iostream>

DapGuiCmdHandler::DapGuiCmdHandler(QObject *parent)
    : QObject(parent) {
   // m_commandHandlers[DapJsonCommands::CONNECTION] = DapCmdConnHandler::handler;

    m_commandHandlers[DapJsonCommands::GET_STATES] = &DapGuiCmdHandler::statesHandler;
  //   m_commandHandlers[DapJsonCommands::GET_STATES](this, Q_NULLPTR)
  //  (this->*m_commandHandlers[DapJsonCommands::GET_STATES])(Q_NULLPTR);// example Call function

}

void DapGuiCmdHandler::setIndicatorsStateList(QList<DapSI*> * ist) {
    m_indicatorsStateList = ist;
}

void DapGuiCmdHandler::statesHandler(const QJsonObject* params) {
    Q_UNUSED(params);
    if(m_indicatorsStateList == Q_NULLPTR) {
        qCritical() << "Indicator list states is NULL";
        return;
    }

    for (const auto si: *m_indicatorsStateList) {
            QByteArray baCmd = DapJsonCmd::generateCmd(DapJsonCommands::STATE, {
                                                           DapJsonParam(g_stateName, si->name()),
                                                           DapJsonParam("value", DapIndicatorState::toString(
                                                           si->current())),
                                                       });
        emit sendDapCmd(baCmd);
    }
}

void DapGuiCmdHandler::handler(DapJsonCmdPtr ptr) {
    auto iter = m_commandHandlers.find(ptr->getCommand());
    if (iter == m_commandHandlers.end()) {
        qWarning() << "Not found handler for command "
                   << ptr->commandToString(ptr->getCommand());
        return;
    }
    (this->*(*iter))(ptr->getParams());
  //  iter
   // this->*m_commandHandlers.find(ptr->getCommand());
   // (*m_commandHandlers[ptr->getCommand()]
    // cal handler function
  //  iter.key()
  //  (*iter)(ptr->getParams());
}
