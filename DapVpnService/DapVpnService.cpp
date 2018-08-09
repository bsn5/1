#include <QtDebug>
#include <QLocalServer>
#include <QSettings>
#include <QLocalSocket>
#include <QCoreApplication>
#include <QTimer>
#include <QDir>
#include <QFileInfo>
#include <QNetworkAccessManager>
#include <QNetworkConfigurationManager>
#ifdef Q_OS_ANDROID
#include <QtAndroid>
#include <QAndroidJniObject>
#endif
#include "DapVPNService.h"
#include "DapStreamer.h"
#include "DapChSockForw.h"
#include "DapCmdConnHandler.h"
#include "DapGuiCmdHandler.h"

/**
 * @brief DapVPNService::DapVPNService
 * @param parent
 */


DapVPNService::DapVPNService(QObject *parent) : QObject(parent)
{
    m_tunReadBytes=m_tunWriteBytes=m_tunReadPackets=m_tunWritePackets=0;
    isLogouting = false;
    tmStreamOpened.start();

    m_restoreTriesLeft=0;
    srvLocalPort = 23434;


    nam = new QNetworkAccessManager(this);
    streamer = new DapStreamer(this);
    streamer->addChProc('s', chSockForw = new  DapChSockForw(streamer) );

    connect(streamer,static_cast<void(DapStreamer::*)(const QString&)>(&DapStreamer::error), [=](QString a_msg){
        qDebug() << "[Streamer Er]"<< a_msg;
        onUsrMsg(a_msg);
        sendCmdAll(QString("status stream error %1").arg( QString::fromLatin1(a_msg.toUtf8().toBase64())  ) );
    });

    connect(DapSession::getInstance(), &DapSession::errorConnection,this,[=] {
        qWarning() << "[DapVPNService] DapSession connection error";
    });


    connect(DapSession::getInstance(),&DapSession::encryptInitialized,this,&DapVPNService::onEncInitialized );
    connect(DapSession::getInstance(), static_cast<void(DapSession::*)(const QString&)>(&DapSession::errorAuthorization),[=](QString a_msg){
        qWarning() << "[DapVPNService] Authorization error: "<<a_msg;
        onUsrMsg(a_msg);
        sendCmdAll(  QString("status authorize error %1").arg(QString::fromLatin1(a_msg.toLatin1().toBase64())));
    });

    connect(&DapCmdConnHandler::me(), &DapCmdConnHandler::sigConnect,
            [=](const QString& user, const QString& password) {
        // Maybe in feature come up with something better
        m_curUser = user;
        m_curPassword = password;
        emit sigRequestConnected();
    });

    connect(&DapCmdConnHandler::me(), &DapCmdConnHandler::sigDisconnect, [=] {
        emit sigRequestDisconnected();
    });

    connect(chSockForw,&DapChSockForw::usrMsg,this, &DapVPNService::onUsrMsg );

    connect(chSockForw,
            static_cast<void (DapChSockForw::*)
            (QString,QString)   > (&DapChSockForw::netConfigReceived),
            [=](QString a_ip, QString a_gw){
        m_tunAddr=a_ip;
        m_tunDest=a_gw;
        m_tunReadBytes = m_tunWriteBytes = m_tunReadPackets = m_tunWritePackets=0;
    });

    connect (chSockForw, &DapChSockForw::sigTunNativeCreate, [=]{
#ifdef Q_OS_ANDROID
        sendCmdAll(QString("android_tunnel_create %1 %2 %3")
                   .arg(m_tunAddr)
                   .arg(m_tunDest)
                   .arg(streamer->upstreamSocket())
                   );
#endif
    });

    connect(chSockForw , &DapChSockForw::tunDestroyed,[=]{
        tmrStat->stop();
    });

    connect(chSockForw , &DapChSockForw::tunCreated, this, [=]{
        tmrStat->start();
    });

    connect(chSockForw,&DapChSockForw::bytesRead, this, &DapVPNService::onReadBytes   );
    connect(chSockForw,&DapChSockForw::bytesWrite, this, &DapVPNService::onWriteBytes   );
    connect(chSockForw,&DapChSockForw::sigPacketRead, this, &DapVPNService::onSigPacketRead   );
    connect(chSockForw,&DapChSockForw::sigPacketWrite, this, &DapVPNService::onSigPacketWrite   );
    connect(chSockForw, &DapChSockForw::sendCmdAll, this, &DapVPNService::sendCmdAll);

    tmrStat= new QTimer(this);
    tmrStat->setInterval(500);
    connect(tmrStat, &QTimer::timeout,[=]{
        sendDapCmdAll(DapJsonCmd::generateCmd( DapJsonCommands::STATS, {
                              DapJsonParam(g_readKbytesParam, int(m_tunReadBytes / 1024)),
                              DapJsonParam(g_writeKbytesParam, int(m_tunWriteBytes / 1024)),
                          }));
//qint64 el=QDateTime::currentMSecsSinceEpoch()- dtStreamOpened.toMSecsSinceEpoch();
//        sendCmdAll(QString("stat %1 %2 %3 %4 %5:%6:%7 %8")
//                   .arg(m_tunReadBytes / 1024) // to Kb
//                   .arg(m_tunWriteBytes / 1024)
//                   .arg(m_tunReadPackets)
//                   .arg(m_tunWritePackets)
//                   .arg(el/3600000ll)
//                   .arg( (el%3600000ll)/60000ll)
//                   .arg( (el%60000ll )/1000ll).arg(dtStreamOpened.toMSecsSinceEpoch()) );
    });

    sm.setChildMode(QState::ParallelStates);

    // Authorization state indicator
    siList.append(siAuthorization = new DapSI(sm,"authorization"));
    siAuthorization->addStateSignal(DapSI::True,DapSession::getInstance(), SIGNAL(authorized(QString)) );
    siAuthorization->addStateSignal(DapSI::False,DapSession::getInstance(), SIGNAL(logouted())  );
    siAuthorization->addStateSignal(DapSI::SwitchingToTrue,DapSession::getInstance(), SIGNAL(authorizing() )  );

    siAuthorization->addStateSignal(DapSI::ErrorAuth,   DapSession::getInstance(), SIGNAL(errorAuthorization(QString)));
    siAuthorization->addStateSignal(DapSI::ErrorNetwork,DapSession::getInstance(), SIGNAL(errorConnection()));
    siAuthorization->addStateSignal(DapSI::ErrorAuth,   DapSession::getInstance(), SIGNAL(errorEncryption()));
    siAuthorization->addStateSignal(DapSI::ErrorNetwork,DapSession::getInstance(), SIGNAL(errorNetwork(QString)));
    siAuthorization->addStateSignal(DapSI::Error,       DapSession::getInstance(), SIGNAL(error(QString)));

    siAuthorization->addActionFor(DapSI::True,DapSession::getInstance(),"encryptInit");
    siAuthorization->addActionFor(DapSI::False,DapSession::getInstance(),"logout");

    connect(siAuthorization->state(DapSI::True), &QState::entered, [=]{
        qDebug() << " ==== siAUthorization == ::True ===";
        if(stateRequestConnected->active()) {
            siStream->doActionFor(DapSI::True);
            QByteArray baCmd = DapJsonCmd::generateCmd(DapJsonCommands::STATE, {
                                                           DapJsonParam("authorize", "true"),
                                                       });
            sendDapCmdAll(baCmd);

            sendCmdAll("status authorize true");
        } else if(stateRequestDisconnected->active()) {
            siAuthorization->doActionFor(DapSI::False);
        }
    });
    connect(siAuthorization->state(DapSI::False), &QState::entered, [=] {

    });

    // Stream indicator
    siList.append(siStream = new DapSI(sm,"stream"));
    siStream->addStateSignal(DapSI::True,streamer,SIGNAL(streamOpened()));
    siStream->addStateSignal(DapSI::False,streamer,SIGNAL(streamClosed()));

    // Error branch of siStream indicator
    siStream->addStateSignal(DapSI::ErrorAuth, streamer,SIGNAL(errorAuth(int)));
    siStream->addStateSignal(DapSI::ErrorNetwork,streamer,SIGNAL(errorNetwork(int)));


    //  siStream->addStateSignal(DapSI::Error,streamer,SIGNAL(error(QString))   );
    siStream->addActionFor(DapSI::True,streamer,"openDefault");
    siStream->addActionFor(DapSI::False,streamer,"close");

    siStream->addStateSignal(DapSI::SwitchingToTrue,streamer,SIGNAL(streamConnecting() ));
    siStream->addStateSignal(DapSI::SwitchingToTrue,streamer,SIGNAL(streamReconnecting()));
    siStream->addStateSignal(DapSI::SwitchingToFalse,streamer,SIGNAL(streamDisconnecting()));


    // --- enter in ::True state
    connect(siStream->state(DapSI::True), &QState::entered, [=]{
        tmStreamOpened.restart();
        dtStreamOpened = QDateTime::currentDateTime();
        if(stateRequestDisconnected->active()) {
            siStream->doActionFor(DapSI::False);
        } else if (stateRequestConnected->active()){
            siNetConfig->doActionFor(DapSI::True);
            sendCmdAll("status stream opened");
        }
    } );


    // --- Enter in ::False state
    connect(siStream->state(DapSI::False), &QState::entered, [=]{
        /**/
    });



    // -- Enter in ::ErrorAuth state
    connect(siStream->state(DapSI::ErrorAuth),&QState::entered,[=]{
        switch(siStream->previous()){
        case DapSI::False:
        case DapSI::SwitchingToTrue: {
            qCritical() <<"Trying to reauthorize";
            siAuthorization->doActionFor(DapSI::False);
        }break;
        case DapSI::True: {
            qWarning() <<"NetConfig rerequest";
            if(stateRequestConnected->active())
                siNetConfig->doActionFor(DapSI::True);
        }break;
        default:{}
        }
    });


    // NetConfig Indicator
    siList.append(siNetConfig = new DapSI(sm,"netconfig"));
    siNetConfig->addStateSignal(DapSI::True,chSockForw,SIGNAL(netConfigReceived(QString,QString)));
    siNetConfig->addStateSignal(DapSI::True,chSockForw,SIGNAL(netConfigReceivedSame()) );
    siNetConfig->addStateSignal(DapSI::SwitchingToTrue,chSockForw,SIGNAL(netConfigRequested()));
    siNetConfig->addStateSignal(DapSI::False,siStream->state(DapSI::False),SIGNAL(entered()) );
    //siNetConfig->addStateSignal(DapSI::False,siAuthorization->state(DapSI::False),SIGNAL(entered()));
    siNetConfig->addStateSignal(DapSI::False,chSockForw, SIGNAL(netConfigCleared()));

    siNetConfig->addActionFor(DapSI::True, chSockForw,"requestIP");
    siNetConfig->addActionFor(DapSI::False, chSockForw,"netConfigClear");

    siNetConfig->state(DapSI::SwitchingToFalse)->addTransition(siNetConfig->state(DapSI::False));

    connect(siNetConfig->state(DapSI::True), &QState::entered, [=]{
        qDebug() << "siNetConfig->state(DapSI::True)";
        if(stateRequestConnected->active()){
            if( (siTunnel->current()==DapSI::True )||(siTunnel->current()==DapSI::SwitchingToTrue) ){
                qDebug() << "Destroy tunnel to recreate it after";
                sendCmdAll("status net_config_received true");
                siTunnel->doActionFor(DapSI::False);
            }else{
                siTunnel->doActionFor(DapSI::True);
            }

        }else if(stateRequestDisconnected->active()) {
            siStream->doActionFor(DapSI::False);
        }
    });
    connect(siNetConfig->state(DapSI::False), &QState::entered, [=]{
        //        if(stateRequestConnected->active()){
        //            siNetConfig->doActionFor(DapSI::True);
        //            sendCmdAll("status net_config_received false");
        //        } else if(stateRequestDisconnected->active())
        //                siStream->doActionFor(DapSI::False);
        if(stateRequestDisconnected->active())
            siAuthorization->doActionFor(DapSI::False);
    });
    connect(siNetConfig->state(DapSI::Error),&QState::entered,[=]{
        switch(siNetConfig->previous()){
        case DapSI::False:
        case DapSI::SwitchingToTrue: {
            qCritical() <<"Can't request the network config";
            emit sigRequestDisconnected();
        }break;
        case DapSI::True: {
            qWarning() <<"NetConfig rerequest";
            if(stateRequestConnected->active())
                siNetConfig->doActionFor(DapSI::True);
        }break;
        default:{}
        }
    });

    siList.append( siTunnel = new DapSI(sm,"tunnel") );
    siTunnel->addStateSignal(DapSI::True,chSockForw,SIGNAL(tunCreated())) ;
    siTunnel->addStateSignal(DapSI::False,chSockForw,SIGNAL(tunDestroyed()) );
    siTunnel->addStateSignal(DapSI::Error,chSockForw,SIGNAL(tunError(QString)) );
    siTunnel->addActionFor(DapSI::True,chSockForw ,"tunCreate");
    siTunnel->addActionFor(DapSI::False,chSockForw ,"tunDestroy");

    connect(siTunnel->state(DapSI::True), &QState::entered, [=]{
        sendCmdAll("status tunnel_created true");
        QByteArray baCmd = DapJsonCmd::generateCmd(DapJsonCommands::STATE, {
                                                       DapJsonParam("tunnel", "true"),
                                                   });
        sendDapCmdAll(baCmd);
    });

    connect(siTunnel->state(DapSI::False), &QState::entered, [=]{
        if(stateRequestConnected->active()){
            siTunnel->doActionFor(DapSI::True);
            sendCmdAll("status tunnel_created false");
        }else if(stateRequestDisconnected->active())
            siStream->doActionFor(DapSI::False);
        //siNetConfig->doActionFor(DapSI::False);
        // siAuthorization->doActionFor(DapSI::False);
    });
    connect(siTunnel->state(DapSI::Error),&QState::entered,[=]{
        switch(siTunnel->previous()){
        case DapSI::False:
        case DapSI::SwitchingToTrue: {
            qCritical() <<"Can't bring up the tunnel";
            emit sigRequestDisconnected();
        }break;
        case DapSI::True: {
            qWarning() <<"Tunnel became broken, bringing that up if Connected state";
            if(stateRequestConnected->active()){
                siTunnel->doActionFor(DapSI::True);
                siTunnel->doActionFor(DapSI::True);
            }
        }break;
        default:{}
        }
    });

    // Common actions for all DapSI objects
    for (auto si: siList){
        si->addAllTransitions();
        si->state(DapSI::Error)->addTransition(si->state(DapSI::False)); // If we're in ::Error state - we go unconditionaly to the ::False state
        connect(si,static_cast<void (DapSI::*)(DapSI::IndicatorState)>( &DapSI::currentChanged),
                [=](DapSI::IndicatorState a_i){
            qDebug() <<"=== "<< si->name()<<" === got state "<< DapSI::toString(a_i);
            //   sendCmdAll(QString("state %1 %2").arg(si->name()).arg(DapSI::toString(a_i) ));
            QByteArray baCmd = DapJsonCmd::generateCmd(DapJsonCommands::STATE, {
                                                           DapJsonParam(si->name(), DapSI::toString(a_i)),
                                                       });
            sendDapCmdAll(baCmd);

            switch(a_i){
            case DapSI::True: onIndicatorStateTrue(); break;
            case DapSI::False: onIndicatorStateFalse(); break;
            default: {}
            }
        } );
    }

    // Global connected state indicator
    statesRequest = new QState(&sm);
    stateRequestConnected = new QState(statesRequest);
    stateRequestConnectedFirst = new QState(stateRequestConnected);
    // stateRequestConnectedAlways = new QState(stateRequestConnected);
    stateRequestDisconnected = new QState(statesRequest);
    statesRequest->setInitialState(stateRequestDisconnected);


    // [!} Exotic states
    // RequestDisconnected ==> RequestConnected if we say to go to Disconnected but haven't switched to RequestConnected state
    for(auto si: siList)
        stateRequestDisconnected->addTransition(si->state(DapSI::SwitchingToTrue) ,SIGNAL(entered()) , stateRequestConnected);
    // [!} Exotic states
    // RequestConnected ==> RequestDisconnected if we say to go to Connected but haven't switched to RequestConnected state
    for(auto si: siList)
        stateRequestConnected->addTransition(si->state(DapSI::SwitchingToFalse ) ,SIGNAL(entered()),stateRequestDisconnected);

    // Request for Disconnected state
    stateRequestDisconnected->addTransition(this,SIGNAL(sigRequestConnected()),stateRequestConnected);
    connect(stateRequestDisconnected,&QState::entered,[=]{
        qDebug() << "[SM] State Request Disconnected";
        QListIterator<DapSI*> i(siList);
        i.toBack();
        while(i.hasPrevious()){
            DapSI * const &si=i.previous(); // Java style, ko-ko-ko!
            if (si->current() == DapSI::SwitchingToFalse){ // everything is going on
                qDebug() << "Process of disconnecting is on " <<si->name()<< " state, everything looks good (only if its not holded here or/and repeating this line again and again and nothing except this";
                break;
            }
            // qInfo() << "Name:" << si->name();
            // if( si->name() == "authorization" )
            if (si->current() == DapSI::True){ // we found nobody in TrueToFalse state and get to get you on this
                qInfo() <<"Beginning the disconnect chain";
                si->doActionFor(DapSI::False);
                // sendCmdAll("request disconnecting");
                break;
            }
            // sendCmdAll("request disconnecting");
        }
    });

    // Request for Сonnected state
    //stateRequestConnected->addTransition(siAuthorization->state(DapSI::Error),SIGNAL(entered()),stateRequestDisconnected);
    stateRequestConnected->setInitialState(stateRequestConnectedFirst);
    stateRequestConnected->addTransition(this,SIGNAL(sigRequestDisconnected()),stateRequestDisconnected);

    /// RequestConnectedFirst ==>RequestConnectedAlways
    // stateRequestConnectedAlways temporarily OFF
    // stateRequestConnectedFirst->addTransition(siAuthorization->state(DapSI::True),SIGNAL(entered()),stateRequestConnectedAlways );

    connect(stateRequestConnected,&QState::entered,[=]{
        qDebug() << "[SM] State Request Connected";
        QListIterator<DapSI*> i(siList);
        i.toFront();
        while( i.hasNext()){
            DapSI * const &si=i.next(); // Java style, ko-ko-ko!
            if( si->current() == DapSI::SwitchingToTrue){ // everything is going on in switching on chain
                qDebug() << "Process of disconnecting is on " <<si->name()<< " state, everything looks good (only if its not holded here or/and repeating this line again and again and nothing except this";
                break;
            }
            if( si->current() == DapSI::False){ // we found nobody in FalseToTrue state and get to get you on this
                qInfo() <<"Beginning the Connect chain";
                si->doActionFor(DapSI::True);
                sendCmdAll("request connecting");
                break;
            }
        }
    });

    sm.start();
}

/**
 * @brief DapVPNService::onIndicatorStateTrue
 */
void DapVPNService::onIndicatorStateTrue()
{
    if( (siAuthorization->current()==DapSI::True) &&
            (siStream->current()==DapSI::True) &&
            (siNetConfig->current()==DapSI::True) &&
            (siTunnel->current()==DapSI::True)   )
        emit sigRequestConnected();
}

/**
 * @brief DapVPNService::onIndicatorStateFalse
 */
void DapVPNService::onIndicatorStateFalse()
{
    if( (siAuthorization->current()==DapSI::False) &&
            (siStream->current()==DapSI::False) &&
            (siNetConfig->current()==DapSI::False) &&
            (siTunnel->current()==DapSI::False)   )
        emit sigRequestDisconnected();
}

/**
 * @brief DapVPNService::init Initalize the service
 * @details Mostly here is present command processing because of often lambda usage
 * @return
 */
int DapVPNService::init()
{
    checkInstallation();
    srvLocal = new DapUiSocketServer();

    //#ifdef DAP_SERVICE_CONNECT_TCP
    if(srvLocal->listen(QHostAddress("127.0.0.1"),22143)) {
        //#else
        //    srvLocal->setSocketOptions(QLocalServer::WorldAccessOption);
        //    if(srvLocal->listen(DAP_BRAND)){
        //#endif
        qDebug() << "Listen for UI on local socket " << DAP_BRAND;
        connect(srvLocal,&DapUiSocketServer::newConnection, [=]{
            while(DapUiSocket * s = srvLocal->nextPendingConnection()){
                qDebug() << "New connect from UI";
                DapCmdParser * dapCmdParser = new DapCmdParser(s);
                connect(dapCmdParser, &DapCmdParser::cmdReady, this, &DapVPNService::procCmd);
                client.append(s);
                connect(s, &DapUiSocket::disconnected,[=]{
                    qDebug() << "UI client disconnected";
                    delete dapCmdParser;
                    client.removeAll(s);
                });
            }
        });
        qInfo() << "Initialization complete";
        return 0;
    }else{
        qCritical() << QString("Can't listen on %1 port").arg(srvLocalPort);
        return -1;
    }
}

void DapVPNService::procCmd(const QByteArray &a_cmd)
{
    //qDebug() << "command string: " << a_cmd;

    DapJsonCmdPtr cmdPtr = DapJsonCmd::load(a_cmd);
    if (cmdPtr == nullptr) {
        qWarning() << "Error parse cmd!";
        return;
    }

    DapGuiCmdHandler::handler(std::move(cmdPtr));

    //    QStringList cmdSub= QString(a_cmd).split(' ');
    //    qDebug() << "Process cmd "<<a_cmd;
    //    if(cmdSub.length()>0){
    //        if(cmdSub[0]=="version"){
    //            sendCmdAll(QString("version %s").arg(DAP_VERSION) );
    //        } else if(cmdSub[0] == "NativeServerInfo") {
    //            if (cmdSub.length() > 5) {
    //              /*  const BadServers badServers(cmdSub[1], cmdSub[2], cmdSub[3], cmdSub[4]);
    //                m_nativeServersInfo.push_back(badServers);*/
    //            } else {
    //                qWarning() <<"Wrong connect NativeServerInfo with only "<< cmdSub.length()<<" arguments (needs 6)";
    //            }
    //        } else if(cmdSub[0] == "get_list_servers") {
    //            qDebug() << "GET: get_list_servers";
    //            // FIXME!!! Доделать сохранение о тоом, что прога уже запускалась...
    //            QSettings firstRun;
    //           // DapSession::getInstance()->serverListRequester("" == firstRun.value("firstRunOfDapService").toString());
    //        } else if(cmdSub[0] ==  "connect") {
    //            if(cmdSub.length() == 5) { // address port user password
    //                m_curAddrLine=cmdSub[1];
    //                m_curUser=cmdSub[3];
    //                m_curPassword=cmdSub[4];
    //                qDebug() << "Command connect to" << m_curAddrLine
    //                         << cmdSub[2] << "with"
    //                         << m_curUser << "credentials";
    //                DapSession::getInstance()->setDapUri(cmdSub[1], cmdSub[2]);
    //                emit sigRequestConnected();
    //            }else
    //                qWarning() <<"Wrong connect command with "
    //                          << cmdSub.length() << " arguments (needs 4)";
    //                qWarning() <<"cmdBuf = "<< a_cmd;
    //        }else if(cmdSub[0] == "disconnect"){
    //            qDebug() << "Command disconnect";
    //            emit sigRequestDisconnected();
    //        }else if(cmdSub[0] == "get_states"){
    //            qDebug() << "Requested states";
    //            for (auto si: siList) {
    //                sendCmdAll(QString("state %1 %2").arg(si->name()).arg(DapSI::toString(si->current()) ));
    //            }

    //            if( stateRequestConnected->active())
    //                sendCmdAll("request connected");
    //            else if( stateRequestDisconnected->active())
    //                sendCmdAll("request disconnected");

    //        } else if(cmdSub[0] == "tunnel_create"){
    //            qDebug() << "Command tunnel_create with tun socket from native service";
    //            if(cmdSub.length()>1){
    //                 qDebug() << "Create worker with tun socket "<<cmdSub.at(1);
    //                 chSockForw->workerStart( cmdSub.at(1).toInt() );
    //            }

    //        }else if(cmdSub[0] == "exit"){
    //            qDebug() << "Command unroute all traffic from the network interface";
    //            ::exit(0);
    //        }/*else if (cmdSub.at(0).compare("forward_tcp")){
    //            if(cmdSub.length()>=5)
    //                chSockForw->addForwarding(cmdSub[3],cmdSub[4].toUShort(),cmdSub[2].toUShort());
    //        }*/else{
    //            qWarning() <<"Unrecognized command "<<cmdSub.at(0);
    //        }
    //    }else
    //        qWarning() << "Wrong command split, should be 1 member at least in the list";
}

void DapVPNService::checkInstallation()
{
    qDebug() <<"checkInstallation() for "<< DAP_BRAND;

#ifdef VPN_TUNTAP
    QFileInfo fi;
    fi.setFile(QDir("/dev"),"tun4");
    if(fi.isFile()){
        qDebug() << "TunTap driver is already loaded";
    }else{
        ::system(QString("/sbin/kextload /Applications/%1.app/Contents/MacOS/tun.kext")
                 .arg(DAP_BRAND).toLatin1().constData() );
        qInfo () << "Loaded TunTap driver";
    }
#endif
}

void DapVPNService::sendDapCmdAll(const QByteArray& cmd) {
    // qDebug() << "sendDapCmdAll() send command " << cmd;

    if(cmd[cmd.length() - 1] != '\n') {
        qWarning() << "Something wrong cmd must end with a character \\n"
                      " Command not sent";
        return;
    }

    for(auto s: client) {
        s->write(cmd + '\n'); // must be two \n symmbols for serialization
        s->flush();
    }
}

/**
 * @brief DapVPNService::sendCmdAll
 * @param a_cmd
 */
void DapVPNService::sendCmdAll(const QString& a_cmd) // Deprecated
{

    // qDebug() << "sendCmdAll() send command "<<a_cmd;
    return; // DEPCRECATED
    for(auto s: client) {
        //        qDebug() << "sendCmdAll() client "<<s->socketDescriptor();
        s->write(QString("%1%2").arg(a_cmd).arg("\n\n").toLatin1());
        s->flush();
    }
}
/**
 * @brief DapVPNService::onEncInitialized
 * @details Actions after encryption initialization - authorization (TODO: add authentification)
 */
void DapVPNService::onEncInitialized()
{
    //DapSession::getInstance()->authorize(m_curUser, m_curPassword, m_curAddrLine);
    // "divevpn.com"
    DapSession::getInstance()->authorize(m_curUser, m_curPassword, "divevpn.com");
}



/**
 * @brief DapVPNService::onUsrMsg
 * @param a_msg
 */
void DapVPNService::onUsrMsg(const QString &a_msg)
{
    sendCmdAll( QString("usr_msg %1").arg(QString::fromLatin1(a_msg.toUtf8().toBase64() )));
}

/**
 * @brief DapVPNService::onReadBytes
 * @param arg
 */
void DapVPNService::onReadBytes(int arg)
{
    m_tunReadBytes += quint64(arg);
}

/**
 * @brief DapVPNService::onWriteBytes
 * @param arg
 */
void DapVPNService::onWriteBytes(int arg)
{
    m_tunWriteBytes += quint64(arg);
}

/**
 * @brief DapVPNService::onSigPacketRead
 */
void DapVPNService::onSigPacketRead()
{
    m_tunReadPackets++;
}

/**
 * @brief DapVPNService::onSigPacketWrite
 */
void DapVPNService::onSigPacketWrite()
{
    m_tunWritePackets++;
}



/**
 * @brief DapVPNService::destroy
 * @details Deinit everything before destructor
 */
void DapVPNService::destroy()
{

}


/**
 * @brief DapVPNService::~DapVPNService
 * @details DapVPNService destructor, should free all the memory
 */
DapVPNService::~DapVPNService()
{
    qDebug() << "[DapVPNService] Destructor";
    delete streamer;
}
