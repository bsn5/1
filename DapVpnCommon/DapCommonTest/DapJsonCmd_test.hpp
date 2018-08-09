#ifndef DAPJSONCMD_TEST_HPP
#define DAPJSONCMD_TEST_HPP

#include <QTest>
#include "../DapJsonCmd.h"

class DapJsonCmdTest : public QObject {
    Q_OBJECT
private slots:
    void commandToString() {
        QCOMPARE(DapJsonCmd::commandToString(DapJsonCommands::CONNECTION),
                 "connection");
    }

    void stringToCommand() {
        QCOMPARE(DapJsonCmd::stringToCommand("stats"), DapJsonCommands::STATS);
    }

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
        QByteArray bResult = DapJsonCmd::generateCmd(DapJsonCommands::STATE,
        {
            DapJsonParam("int_one", 1), DapJsonParam("auth_bool", true),
            DapJsonParam("two_double", 2.2), DapJsonParam("some_str", "some")
        });

        auto result = QJsonDocument::fromJson(bResult);
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

    void generateCmdWithoutParams() {
        QByteArray bResult = DapJsonCmd::generateCmd(DapJsonCommands::STATE);
        auto result = QJsonDocument::fromJson(bResult);
        QVERIFY(result["command"].isString());
        QCOMPARE(result["command"], "state");
    }

    void load() {
        QString sJson = R"({"command": "state", "params": {"authorized":true,"two_int":2}})";
        DapJsonCmdPtr cmd = DapJsonCmd::load(sJson);

        QCOMPARE(cmd->getCommand(), DapJsonCommands::STATE);
        QJsonValue auth = cmd->getParam("authorized");
        QVERIFY(auth.isBool());
        QCOMPARE(auth.toBool(), true);
        QJsonValue two_int = cmd->getParam("two_int");
        QCOMPARE(two_int.toInt(), 2);
    }

    void getParams() {
        QString sJson = R"({"command": "state", "params": {"authorized":true,"two_int":2}})";
        DapJsonCmdPtr cmd = DapJsonCmd::load(sJson);
        const QJsonObject * params = cmd->getParams();

        QVERIFY(params->value("authorized").isBool());
        QCOMPARE(params->value("authorized").toBool(), true);

        QCOMPARE(params->value("two_int").toInt(), 2);
        qDebug() << "Params " << params->value("authorized").toBool();
    }
};

#endif // DAPJSONCMD_TEST_HPP
