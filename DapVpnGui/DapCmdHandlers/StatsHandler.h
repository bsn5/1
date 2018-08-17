#ifndef DAPCMDSTATSHANDLER_H
#define DAPCMDSTATSHANDLER_H

#include <QObject>

class DapCmdStatsHandler : public QObject
{
    Q_OBJECT
public:
    static DapCmdStatsHandler& me() { static DapCmdStatsHandler _me; return _me; }
    static void handler(const QJsonObject* params);
private:
    static void trafficStatsHandler(const QJsonValue& readBytes,
                                    const QJsonValue& writeBytes);
    explicit DapCmdStatsHandler(QObject *parent = nullptr);
    void readWriteBytesHandler();
signals:
    void sigReadWriteBytesStat(int read,
                               int write);
public slots:
};

#endif // DAPCMDSTATSHANDLER_H
