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

    void generateCmd() {
        QJsonObject result = DapJsonCmd::generateCmd(DapCommands::STATE,
        {
            DapJsonParam("int_one", 1), DapJsonParam("auth_bool", true),
            DapJsonParam("two_double", 2.2), DapJsonParam("some_str", "some")
        });

        QVERIFY(result["command"].isString());
        QCOMPARE(result["command"], "state");

        QJsonObject resultParams = result["params"].toObject();

        QCOMPARE(resultParams["int_one"].toInt(), 1);

        QVERIFY(resultParams["auth_bool"].isBool());
        QCOMPARE(resultParams["auth_bool"].toBool(), true);

        QVERIFY(resultParams["two_double"].isDouble());
        QVERIFY(qFuzzyCompare(resultParams["two_double"].toDouble(), 2.2));

        QVERIFY(resultParams["some_str"].isString());
        QVERIFY(resultParams["some_str"].toString() == "some");
    }
};

#endif // DAPJSONCMD_TEST_HPP
