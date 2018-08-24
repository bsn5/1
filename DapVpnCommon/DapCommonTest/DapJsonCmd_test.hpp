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
            DapJsonParam(DapJsonParams::STATE_NAME, "stateName"), DapJsonParam(DapJsonParams::VALUE, true)
        });

        auto result = QJsonDocument::fromJson(bResult);
        QVERIFY(result["command"].isString());
        QCOMPARE(result["command"], DapJsonCmd::commandToString(DapJsonCommands::STATE));

        QJsonObject resultParams = result["params"].toObject();

        QVERIFY(resultParams["value"].isBool());
        QCOMPARE(resultParams["value"].toBool(), true);
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
