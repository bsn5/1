#ifndef DAPJSONPARAMAS_H
#define DAPJSONPARAMAS_H

#include <QString>
#include <QMap>
#include <QVector>
#include <QDebug>

enum class DapJsonCommands {
    STATE,
    CONNECTION,
    STATS,
    GET_STATES,
    AUTHORIZE_ERROR
};

class DapJsonParams
{
public:
    enum Params {
        VALUE, STATE_NAME, READ_BYTES, WRITE_BYTES,
        MESSAGE, ADDRESS, PORT, USER, PASSWORD, DISCONNECT
    };
    static const QString& toString(Params p);
    static Params fromString(QString &param);

    static bool isParamsAvailable(const DapJsonCommands& cmd,
                                  const Params& p);

    DapJsonParams() = delete;
    DapJsonParams(const DapJsonParams &) = delete;
private:
    /* For exampe: state can have parameter STATE_NAME
        Stats can have READ_BYTES, WRITE_BYTES etc. */
    static QMap<DapJsonCommands, QVector<Params>> availableParams;
};

#endif // DAPJSONPARAMAS_H
