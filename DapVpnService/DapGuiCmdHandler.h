#ifndef DAPGUICMDHANDLER_H
#define DAPGUICMDHANDLER_H

#include <QObject>
#include <QDebug>
#include <QMap>

#include "DapJsonCmd.h"

class DapGuiCmdHandler : public QObject
{
    Q_OBJECT
private:
    explicit DapGuiCmdHandler(QObject *parent = nullptr);
    static DapJsonCmdHandlersMap m_commandHandlers;
    static void getStatesHandler(const QJsonObject* params);
public:
    static DapGuiCmdHandler& me(){static DapGuiCmdHandler _me; return _me; }
    static void handler(DapJsonCmdPtr ptr);
signals:
public slots:
};

#endif // DAPGUICMDHANDLER_H
