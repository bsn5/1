#ifndef DAPGUICMDHANDLER_H
#define DAPGUICMDHANDLER_H

#include <QObject>
#include <QDebug>
#include <QMap>

#include "DapJsonCmd.h"
#include "DapSI.h"

class DapGuiCmdHandler : public QObject
{
    Q_OBJECT
private:
    QList<DapSI*> *m_indicatorsStateList = Q_NULLPTR;
    DapJsonCmdHandlersMap<DapGuiCmdHandler> m_commandHandlers;
    void statesHandler(const QJsonObject* params);
public:
    explicit DapGuiCmdHandler(QObject *parent = nullptr);
    //static DapGuiCmdHandler& me() {static DapGuiCmdHandler _me; return _me;}
    void setIndicatorsStateList(QList<DapSI*>* siList);
    void handler(DapJsonCmdPtr ptr);
signals:
    void sendDapCmd(const QByteArray& ba);
public slots:
};

#endif // DAPGUICMDHANDLER_H
