#ifndef DAPGUICMDHANDLER_H
#define DAPGUICMDHANDLER_H

#include <QObject>
#include <QDebug>
#include <QMap>

#include "DapJsonCmd.h"
#include "DapSI.h"
#include "DapCmdHandlerAbstract.h"
#include "StatesHandler.h"

class DapGuiCmdController : public QObject
{
    Q_OBJECT
private:
    DapJsonCmdHandlersMap m_commandHandlers;

    void statesHandler(const QJsonObject* params);
    void connectionHandler(const QJsonObject* params);
public:
    explicit DapGuiCmdController(QList<DapSI*>* siList, QObject *parent = nullptr);
    ~DapGuiCmdController();
    void cmdParser(DapJsonCmdPtr ptr);

    // returns Q_NULLPTR if not success
    DapCmdStatesHandler* getStatesObject();
public slots:
};

#endif // DAPGUICMDHANDLER_H
