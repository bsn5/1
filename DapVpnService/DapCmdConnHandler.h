#ifndef DAPCMDCONNHANDLER_H
#define DAPCMDCONNHANDLER_H

#include <QObject>
#include <QJsonObject>

class DapCmdConnHandler : public QObject
{
    Q_OBJECT
private:
    static const QString disconnectParam;
    explicit DapCmdConnHandler(QObject *parent = nullptr);

public:
    static DapCmdConnHandler& me() {
        static DapCmdConnHandler _me; return _me;
    }
    static void handler(const QJsonObject* params);
signals:
    void sigConnect(const QString& user, const QString& password);
    void sigDisconnect();
public slots:
};

#endif // DAPCMDCONNHANDLER_H
