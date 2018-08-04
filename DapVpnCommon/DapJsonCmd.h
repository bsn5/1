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

    // Checks mandatory fields
    static bool isJsonValid(QString obj);
    static bool isJsonValid(QJsonObject obj);

    // Generate command without arguments
    static QJsonObject generateCmd(DapCommands command);

    static QJsonObject generateCmd(DapCommands command,
                                   std::initializer_list<QPair<QString, QJsonValue>>params);

    static QString commandToString(DapCommands cmd);
private:
    static QJsonObject getCommandJson(DapCommands command);

};

#endif // DAPJSONCMD_H
