#include "DapGuiCmdController.h"
#include "ConnectHandler.h"

DapGuiCmdController::DapGuiCmdController(QList<DapSI*>* siList,
                                 QObject *parent) : QObject(parent) {
    if(siList == Q_NULLPTR) {
        qWarning() << "SI states list is NULL. DapCmdStatesHandler Can't be initialize";
    } else {
        m_commandHandlers[DapJsonCommands::GET_STATES] = new DapCmdStatesHandler(siList);
    }
    m_commandHandlers[DapJsonCommands::CONNECTION] = new DapCmdConnHandler;

}

DapCmdStatesHandler* DapGuiCmdController::getStatesObject() {
    auto iter = m_commandHandlers.find(DapJsonCommands::GET_STATES);
    DapCmdStatesHandler* res = Q_NULLPTR;
    if(iter != m_commandHandlers.end()) {
        res = dynamic_cast<DapCmdStatesHandler*>(*iter);
        if (res == Q_NULLPTR)
            qWarning() << "Can't cast to DapCmdStatesHandler";
    } else {
        qWarning() << "Can't find DapCmdStatesHandler in map handlers";
    }
    return res;
}


void DapGuiCmdController::cmdParser(DapJsonCmdPtr ptr) {
    auto iter = m_commandHandlers.find(ptr->getCommand());
    if (iter == m_commandHandlers.end()) {
        qWarning() << "Not found handler for command "
                   << ptr->commandToString(ptr->getCommand());
        return;
    }
    (*iter)->handler(ptr->getParams());
}

DapGuiCmdController::~DapGuiCmdController() {
    // clean handler objects
    for(auto s : m_commandHandlers) {
        delete s;
    }
}
