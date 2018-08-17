#ifndef DAPCMDSTATSHANDLER_H
#define DAPCMDSTATSHANDLER_H

#include <QObject>
#include "DapCmdHandlerAbstract.h"
class DapCmdStatsHandler : public QObject , public DapCmdHandlerAbstract
{
    Q_OBJECT
public:
    void handler(const QJsonObject* params);
    explicit DapCmdStatsHandler(QObject *parent = nullptr);
private:
    void trafficStatsHandler(const QJsonValue& readBytes,
                                    const QJsonValue& writeBytes);
    void readWriteBytesHandler();
signals:
    void sigReadWriteBytesStat(int read,
                               int write);
public slots:
};

#endif // DAPCMDSTATSHANDLER_H
