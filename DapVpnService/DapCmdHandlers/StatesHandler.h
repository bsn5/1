#ifndef DAPCMDSTATESHANDLER_H
#define DAPCMDSTATESHANDLER_H

#include <QObject>
#include "DapCmdHandlerAbstract.h"
#include "DapJsonCmd.h"
#include <QDebug>
#include "DapSI.h"

class DapCmdStatesHandler : public QObject, public DapCmdHandlerAbstract
{
    Q_OBJECT
private:
    QList<DapSI*> *m_indicatorsStateList = Q_NULLPTR;
public:
    explicit DapCmdStatesHandler(QList<DapSI*> *siList, QObject *parent = nullptr);
    void handler(const QJsonObject* params) override;
signals:
    void sendDapCmd(const QByteArray& ba);
public slots:
};

#endif // DAPCMDSTATESHANDLER_H
