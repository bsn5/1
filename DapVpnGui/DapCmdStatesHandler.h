#ifndef DAPSTATESHANDLER_H
#define DAPSTATESHANDLER_H

#include <QObject>
#include <QJsonObject>
#include <QDebug>

class DapStatesHandler : public QObject
{
    Q_OBJECT
public:
    static DapStatesHandler& me(){static DapStatesHandler _me; return _me; }
    static void handler(const QJsonObject * params);
private:
    static void authorizeHandler(const QString& state);
    static void tunnelHandler(const QString& state);
    static void streamHandler(const QString& state);
    static void netconfigHandler(const QString& state);
    explicit DapStatesHandler(QObject *parent = nullptr);
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
