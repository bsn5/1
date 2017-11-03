#include <QCoreApplication>
#include <DapConnectBase.h>
#include "DapCrypt.h"
#include "DapSession.h"
#include "testconnect.h"

#include "DapKeyAes.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/aes.h>
#include "DapKeyRsa.h"

#include "DapVPNService.h"
//#define CONFCALL_DEVEL

int main (int argc, char **argv)
{
#ifndef CONFCALL_DEVEL
    qDebug() << "DapService Started!";
    qputenv("QT_LOGGING_RULES", "qt.network.ssl.warning=false");
    QCoreApplication a(argc, argv);

    DapVPNService dapService;
    dapService.init();
#else
    TestConnect tc;
#endif
    return a.exec();
}
