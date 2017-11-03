#include <QString>
#include <stdlib.h>
#ifdef Q_OS_WIN
#include <ctime>
#endif
#include "msgforuser.h"

MsgForuser::MsgForuser()
{
    msg = QString("");
    m_time = time(NULL);
}

void MsgForuser::setMSG(QString newMSG)
{
    int newTime = time(NULL);

    if (newTime <= m_time + 2) {
        if (msg == "Subscribe has been expired") return;
        if (msg == "Incorrect password for the login.") return;
        if (msg == "Incorrect user login.") return;
        if (msg == "In login or password is space character.") return;
    }

    m_time = newTime;

    if (newMSG == "QLocalSocket::connectToServer: Invalid name: Trying to connect again...") {
        msg = "VPN Servis is off.";
        return;
    }
    msg    = newMSG;
}

QString MsgForuser::getMSG()
{
    return msg;
}
