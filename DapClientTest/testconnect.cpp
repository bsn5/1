#include "testconnect.h"
#include "DapStreamer.h"
#include "DapSession.h"
#include "DapChSockForw.h"
#include <unistd.h>
#include <limits.h>
#include "DapChannelPacket.h"
#include "DapChannelChat.h"
#include <BaseObject.h>
#include <string.h>

TestConnect::TestConnect(QObject *parent) : QObject(parent)
{

    qputenv("QT_LOGGING_RULES", "qt.network.ssl.warning=false");
    gethostname(hostname, HOST_NAME_MAX);

    QString addrLine;

    if (!strcmp(hostname, "debianvbox"))//
         addrLine.append("192.168.0.109:80");//
    else
        addrLine.append("dap-dev.demlabs.net:8033");

    DapSession::getInstance()->SetDapUri(addrLine);

    DapSession::getInstance()->encryptInit();

    streamer = new DapStreamer(Q_NULLPTR);
    DapChannelChat* ch = new DapChannelChat(streamer);
    streamer->addChProc('s', DapChSockForw::getInstance());
    streamer->addChProc('t', ch);

    connect(streamer, &DapStreamer::streamOpened, [=] {
        //ch->cmd_addContact("user2","domain.com");
    });

    connect(DapSession::getInstance(),SIGNAL(encryptInitialized()),this,SLOT(onEncInitialized() ));


}
