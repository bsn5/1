#include "DapGuiCmdHandler.h"
#include "DapCmdConnHandler.h"
#include "DapSession.h"

DapJsonCmdHandlersMap DapGuiCmdHandler::m_commandHandlers = {
    {DapJsonCommands::CONNECTION, DapCmdConnHandler::handler},
};


DapGuiCmdHandler::DapGuiCmdHandler(QObject *parent)
    : QObject(parent)
{

}

void DapGuiCmdHandler::getStatesHandler(const QJsonObject* params) {

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
