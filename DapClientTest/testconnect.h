#ifndef TESTCONNECT_H
#define TESTCONNECT_H


#include <QObject>
#include "DapSession.h"
#include "DapStreamer.h"
#include "DapChSockForw.h"

class TestConnect : public QObject
{
    Q_OBJECT
    char hostname[HOST_NAME_MAX];
public:
    explicit TestConnect(QObject *parent = 0);
    DapStreamer *streamer;
signals:
    void onInit();
public slots:
    void onEncInitialized()
    {
        DapSession::getInstance()->authorize("usr33", "12345", "domain.com");
        addSockForw();

    }

    void addSockForw()
    {
        streamer->open("socket_forward","sf=1");

        if (!strcmp(hostname, "debianvbox"))
           DapChSockForw::getInstance()->addForwarding(QString("192.168.0.109"),22,11115);
        else
           DapChSockForw::getInstance()->addForwarding(QString("dap-dev.demlabs.net"),22,11115);
    }

    void addVPN() {

    }
};

#endif // TESTCONNECT_H
