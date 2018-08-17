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
    explicit DapGuiCmdHandler(QObject *parent = nullptr);
    static DapJsonCmdHandlersMap m_commandHandlers;
    static void getStatesHandler(const QJsonObject* params);
public:
    static DapGuiCmdHandler& me() {static DapGuiCmdHandler _me; return _me;}
    void setIndicatorsStateList(QList<DapSI*>* siList);
    static void handler(DapJsonCmdPtr ptr);
signals:
    void sendDapCmd(const QByteArray& ba);
public slots:
};

#endif // DAPGUICMDHANDLER_H
