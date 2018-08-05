#ifndef DAPJSONCMD_H
#define DAPJSONCMD_H

#include <QJsonObject>
#include <QJsonDocument>
#include <QDebug>
#include <QMap>

// Key must be a string!
#define DapJsonParam(key,value) \
    QPair<QString, QJsonValue>(key, QJsonValue(value))

enum class DapCommands {
    STATE
};

class DapJsonCmd
{
public:
    DapJsonCmd() {}

    bool load(const QString& obj);
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
private:

    static QJsonObject getCommandJson(DapCommands command);
    QJsonObject * m_jsObj = Q_NULLPTR;
};

#endif // DAPJSONCMD_H
