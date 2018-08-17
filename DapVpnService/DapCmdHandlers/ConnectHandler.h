#ifndef DAPCMDCONNHANDLER_H
#define DAPCMDCONNHANDLER_H

#include <QObject>
#include <QJsonObject>
#include "DapCmdHandlerAbstract.h"

class DapCmdConnHandler : public QObject, public DapCmdHandlerAbstract
{
    Q_OBJECT
private:
    static const QString disconnectParam;
public:
    explicit DapCmdConnHandler(QObject *parent = nullptr);
    virtual ~DapCmdConnHandler() override;
    static DapCmdConnHandler& me() {
        static DapCmdConnHandler _me; return _me;
    }
    void handler(const QJsonObject* params) override;
signals:
    void sigConnect(const QString& user, const QString& password);
    void sigDisconnect();
public slots:
};

#endif // DAPCMDCONNHANDLER_H
