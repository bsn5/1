#ifndef DAPJSONCMD_H
#define DAPJSONCMD_H

#include <QJsonObject>
#include <QJsonDocument>
#include <QDebug>
#include <QMap>
#include <memory>

// Key must be a string!
#define DapJsonParam(key,value) \
    QPair<QString, QJsonValue>(key, QJsonValue(value))

class DapJsonCmd;
using DapJsonCmdPtr = std::unique_ptr<DapJsonCmd>;
enum class DapCommands {
    STATE
};

/* In future can create base class DapJson and create
inheritor DapJsonCmd */
class DapJsonCmd
{
public:
    // Constructor
    static DapJsonCmdPtr load(const QString& obj);

    DapCommands getCommand() const;

    // return QJsonValue::Null if not found
    QJsonValue getParam(const QString& key) const;

    // Checks mandatory fields
    static bool isJsonValid(const QString& obj);
    static bool isJsonValid(QJsonObject obj);

    // Generate command without arguments
    static QJsonObject generateCmd(DapCommands command);

    static QJsonObject generateCmd(DapCommands command,
                                   std::initializer_list<QPair<QString, QJsonValue>>params);

    static QString commandToString(DapCommands cmd);
    static QJsonObject stringToJsonObject(const QString& obj);

    ~DapJsonCmd() { delete m_jsObj; }
private:
    DapJsonCmd() {}
    static QJsonObject getCommandJson(DapCommands command);
    QJsonObject * m_jsObj;
};

#endif // DAPJSONCMD_H
