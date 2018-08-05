#include "DapJsonCmd.h"

DapCommands DapJsonCmd::getCommand() const {
    return DapCommands((*m_jsObj)["command"].toInt());
}

QJsonValue DapJsonCmd::getParam(const QString& key) const {
    return (*m_jsObj)["params"].toObject()[key];
}

DapJsonCmdPtr DapJsonCmd::load(const QByteArray& ba) {
    QJsonDocument doc = QJsonDocument::fromJson(ba);
    if (doc.isEmpty())
        return nullptr;
    DapJsonCmd *cmd = new DapJsonCmd;
    cmd->m_jsObj = new QJsonObject(doc.object());
    return DapJsonCmdPtr(cmd);
}

DapJsonCmdPtr DapJsonCmd::load(const QString& obj) {
    return DapJsonCmd::load(obj.toLatin1());
}

bool DapJsonCmd::isJsonValid(const QString& obj) {
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
    DapJsonCmd::commandToString(DapCommands(obj[mandatoryField].toInt()));
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

QByteArray DapJsonCmd::generateCmd(DapCommands command) {
    QJsonDocument Doc(DapJsonCmd::getCommandJson(command));
    return Doc.toJson();
}

QByteArray DapJsonCmd::generateCmd(DapCommands command,
                                    std::initializer_list<QPair<QString, QJsonValue>>params) {
    QJsonObject cmdObj = DapJsonCmd::getCommandJson(command);
    QJsonObject paramsObj;
    for(auto& s : params) {
        paramsObj[s.first] = s.second;
    }
    cmdObj.insert("params", paramsObj);

    QJsonDocument Doc(cmdObj);

    return Doc.toJson();
}
