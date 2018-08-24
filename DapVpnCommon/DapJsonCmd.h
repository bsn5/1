#ifndef DAPJSONCMD_H
#define DAPJSONCMD_H

#include <QJsonObject>
#include <QJsonDocument>
#include <QDebug>
#include <QMap>
#include <memory>
#include "DapCmdHandlerAbstract.h"
#include "DapJsonParams.h"

// Key must be a string!
#define DapJsonParam(key,value) \
    QPair<DapJsonParams::Params, QJsonValue>(key, QJsonValue(value))

class DapJsonCmd;
using DapJsonCmdPtr = std::unique_ptr<DapJsonCmd>;
using DapJsonCmdHandlersMap = QMap<DapJsonCommands, DapCmdHandlerAbstract*>;

/* In future can create base class DapJson and create
inheritor DapJsonCmd */
class DapJsonCmd
{
public:
    // Constructor
    static DapJsonCmdPtr load(const QString& obj);
    static DapJsonCmdPtr load(const QByteArray& ba);

    DapJsonCommands getCommand() const;

    // return QJsonValue::Null if not found
    QJsonValue getParam(const QString& key) const;

    // return json object with all params
    const QJsonObject* getParams();

    // Checks mandatory fields
    static bool isJsonValid(const QString& obj);
    static bool isJsonValid(QJsonObject obj);

    // Generate command without arguments
    static QByteArray generateCmd(DapJsonCommands command);

    static QByteArray generateCmd(DapJsonCommands command,
                                   std::initializer_list<QPair<DapJsonParams::Params, QJsonValue>>params);

    static QString commandToString(DapJsonCommands cmd);
    static DapJsonCommands stringToCommand(const QString& cmd);

    static QJsonObject stringToJsonObject(const QString& obj);

    ~DapJsonCmd();
private:
    static QMap<DapJsonCommands, QString> cmdStrings;

    DapJsonCmd() {}
    static QJsonObject getCommandJson(DapJsonCommands command);
    QJsonObject * m_jsObj;
    QJsonObject * m_jsParamsObj = Q_NULLPTR; // is a part of m_jsObj
};

#endif // DAPJSONCMD_H
