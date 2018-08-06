#include "DapJsonCmd.h"

DapJsonCommands DapJsonCmd::getCommand() const {
    return DapJsonCommands((*m_jsObj)["command"].toInt());
}

QJsonValue DapJsonCmd::getParam(const QString& key) const {
    return (*m_jsObj)["params"].toObject()[key];
}

const QJsonObject* DapJsonCmd::getParams() {
    return m_jsParamsObj;
}

DapJsonCmdPtr DapJsonCmd::load(const QByteArray& ba) {
    QJsonDocument doc = QJsonDocument::fromJson(ba);
    if (doc.isEmpty())
        return nullptr;
    DapJsonCmd *cmd = new DapJsonCmd;
    cmd->m_jsObj = new QJsonObject(doc.object());

    auto paramsObj = (*cmd->m_jsObj)["params"];
    if(paramsObj != QJsonValue::Null) {
        cmd->m_jsParamsObj = new QJsonObject(paramsObj.toObject());
    }
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
    DapJsonCmd::commandToString(DapJsonCommands(obj[mandatoryField].toInt()));
    return true;
}

QString DapJsonCmd::commandToString(DapJsonCommands command) {
    static QMap<DapJsonCommands, QString> cmdStrings = {
        {DapJsonCommands::STATE, "state"}
    };

    QString sCmd = cmdStrings.value(command);
    if (sCmd == "") {
        throw std::runtime_error("Unknown DapCommand. Add his to cmdStrings!");
    }
    return sCmd;
}

QJsonObject DapJsonCmd::getCommandJson(DapJsonCommands command) {
    return QJsonObject
    {
        {"command", DapJsonCmd::commandToString(command)}
    };
}

QByteArray DapJsonCmd::generateCmd(DapJsonCommands command) {
    QJsonDocument Doc(DapJsonCmd::getCommandJson(command));
    return Doc.toJson();
}

QByteArray DapJsonCmd::generateCmd(DapJsonCommands command,
                                    std::initializer_list<QPair<QString, QJsonValue>>params) {
    QJsonObject cmdObj = DapJsonCmd::getCommandJson(command);
    QJsonObject paramsObj;
    for(auto& s : params) {
        paramsObj[s.first] = s.second;
    }
    cmdObj.insert("params", paramsObj);

    QJsonDocument doc(cmdObj);

    return doc.toJson();
}

DapJsonCmd::~DapJsonCmd() {
    delete m_jsObj;
    if(m_jsParamsObj != Q_NULLPTR)
        delete m_jsParamsObj;
}
