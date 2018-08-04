#ifndef DAPJSONCMD_TEST_HPP
#define DAPJSONCMD_TEST_HPP

#include <QTest>
#include "../DapJsonCmd.h"

class DapJsonCmdTest : public QObject {
    Q_OBJECT
private:
private slots:
    void validJson() {
        QJsonObject mainObject
        {
            {"command", "state"}
        };

        QJsonObject params
        {
            {"authorized", true}
        };
        mainObject.insert("params", params);
        QVERIFY(DapJsonCmd::isJsonValid(mainObject));
    }

    void validQStringJson() {
        QString json = R"({"command": "state", "params": {"authorized": true}})";
        QVERIFY(DapJsonCmd::isJsonValid(json));
    }

    void notValidJson() {
        QString badJson = R"({"command": "state" "params": {"authorized": true}})";
        QVERIFY(DapJsonCmd::isJsonValid(badJson) == false);
    }
};

#endif // DAPJSONCMD_TEST_HPP
