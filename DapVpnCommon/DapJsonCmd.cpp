#include "DapJsonCmd.h"

bool DapJsonCmd::isJsonValid(QString obj) {
    QJsonParseError err;

    QJsonDocument doc = QJsonDocument::fromJson(obj.toUtf8(), &err);
    if (err.error != QJsonParseError::ParseError::NoError) {
        qWarning() << "Json is not valid. Error parse!";
        return false;
    }

    return isJsonValid(doc.object());
}

bool DapJsonCmd::isJsonValid(QJsonObject obj) {
    static QString mandatoryField = "command";
    if(obj[mandatoryField] == QJsonValue::Null) {
        return false;
    }
    return true;
}

QString DapJsonCmd::commandToString(DapCommands command) {
    static QMap<DapCommands, QString> cmdStrings = {
        {DapCommands::STATE, "state"}
    };

    QString sCmd = cmdStrings.value(command);
    if (sCmd == "") {
        throw std::runtime_error("Unknown DapCommand. Add his to cmdStrings!");
    }
    return sCmd;
}

QJsonObject DapJsonCmd::getCommandJson(DapCommands command) {
    return QJsonObject
    {
        {"command", DapJsonCmd::commandToString(command)}
    };
}

QJsonObject DapJsonCmd::generateCmd(DapCommands command) {
    return DapJsonCmd::getCommandJson(command);
}

QJsonObject DapJsonCmd::generateCmd(DapCommands command,
                                    std::initializer_list<QPair<QString, QJsonValue>>params) {
    QJsonObject cmdObj = DapJsonCmd::getCommandJson(command);
    QJsonObject paramsObj;
    for(auto& s : params) {
        paramsObj[s.first] = s.second;
    }
    cmdObj.insert("params", paramsObj);
    return cmdObj;
}
