#ifndef DAPJSONCMD_H
#define DAPJSONCMD_H

#include <QJsonObject>
#include <QJsonDocument>
#include <QDebug>

class DapJsonCmd
{
public:
    DapJsonCmd();

    // checks mandatory fields
    static bool isJsonValid(QString obj);
    static bool isJsonValid(QJsonObject obj);
};

#endif // DAPJSONCMD_H
