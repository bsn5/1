#include "DapJsonCmd.h"

DapJsonCmd::DapJsonCmd() {

}


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
