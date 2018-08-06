#ifndef DAPSTATESHANDLER_H
#define DAPSTATESHANDLER_H

#include <QObject>
#include <QJsonObject>
#include <QDebug>

class DapStatesHandler : public QObject
{
    Q_OBJECT
    explicit DapStatesHandler(QObject *parent = nullptr);
public:
    static DapStatesHandler& me(){static DapStatesHandler _me; return _me; }
    static void handler(const QJsonObject * params);
signals:
    void sigStateAuthorized();
    void sigStateAuthorizeError();
    void sigStateUnauthorized();

    void sigStateStreamOpened();
    void sigStateStreamClosed();

    void sigStateNetConfigTrue();
    void sigStateNetConfigFalse();

    void sigStateTunnelCreated();
    void sigStateTunnelDestroyed();
public slots:
};

#endif // DAPSTATESHANDLER_H
