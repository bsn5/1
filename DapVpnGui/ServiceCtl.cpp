#include <QtDebug>
#include <QSettings>
#include <QTimer>
#ifdef Q_OS_ANDROID
#include <QtAndroid>
#include <QtAndroidExtras/QAndroidJniObject>
#endif
#include "datalocal.h"
#include "ServiceCtl.h"
#include "StatesHandler.h"
#include "StatsHandler.h"

//DapJsonCmdHandlersMap ServiceCtl::m_commandHandlers = {
//    {DapJsonCommands::STATE, DapCmdStatesHandler::handler},
//    {DapJsonCommands::STATS, DapCmdStatsHandler::handler}
//};

ServiceCtl::ServiceCtl()
    : DapServiceClient("DAP_SERVICE_NAME")
{
    tmRestart = new QTimer(this);

    connect(this,&ServiceCtl::ctlConnected, [=]{
        qInfo() << "[ServiceCtl] Connected to ctl socket,request for status";
        sendCmd(DapJsonCmd::generateCmd(DapJsonCommands::GET_STATES));
    });
    connect(this,&ServiceCtl::ctlDisconnected, [=]{
        qInfo() << "[ServiceCtl] Disconnected from backend";
        tmRestart->start(2000);
    });

    connect(tmRestart,&QTimer::timeout, [=]{ restartService(); });
}

void ServiceCtl::procCmdController(const QByteArray &a_cmd)
{
    auto djc = DapJsonCmd::load(a_cmd);
    if (djc == nullptr) {
        qWarning() << "Error load DapJsonCmd object!";
        return;
    }

//    auto iter = m_commandHandlers.find(djc->getCommand());
//    if (iter == m_commandHandlers.end()) {
//        qWarning() << "Not found handler for command "
//                   << djc->commandToString(djc->getCommand());
//        return;
//    }
    // cal handler function
    //(*iter)(djc->getParams());

    // TODO part for android see code below (#ifdef Q_OS_ANDROID)

    /*
    // ошибки и сообщения пользователю, нужно встроить... сделаю.
        } else if (infos[0] == "usr_msg") {
            if (infos.length() >=2) {
                logUsr(QString::fromUtf8(QByteArray::fromBase64(infos[1].toLatin1())));
                user_msg.setMsq(QString::fromUtf8(QByteArray::fromBase64(infos[1].toLatin1())));
                updateUsrMsg();
            }else
                qWarning() << "[?] Empty usr_msg";
        } else if (infos[0] == "logout_is_finished") {
            setStatusText("");
            emit sigStateUnauthorized();
        } else if (infos[0] == "number_of_args"){
            setStatusText("In login or password is space character.");
            emit sigStateUnauthorized();
        } else if (infos[0] == "subscribe_expired" ) {
            setStatusText("Subscribe has been expired.");
            emit sigStateUnauthorized();
        } else if (infos[0] == "login_incorrect_pswd" ) {
            setStatusText("Incorrect password for the login.");
            emit sigStateUnauthorized();
        } else if (infos[0] == "not_found_login_in_db" ) {
            setStatusText("Incorrect user login.");
            emit sigStateUnauthorized();
        } else if (infos[0] == "reconnecting_subscribe_expired" ) {
            setStatusText("Subscribe has been expired");
            emit sigStateUnauthorized();
        } else if (infos[0] == "reconnecting_login_incorrect_pswd" ) {
            setStatusText("Incorrect password for the login.");
            emit sigStateUnauthorized();
        } else if (infos[0] == "reconnecting_not_found_login_in_db" ) {
            setStatusText("Incorrect user login.");
            emit sigStateUnauthorized();
        } else if (infos[0] == "connection_refused") {
            setStatusText("Server is not available.");
            emit sigStateUnauthorized();
        } else if (infos[0] == "auth_error") {
            setStatusText(QString("Authorization error: %1")
                .arg(infos.length()>1 ? QString::fromUtf8(QByteArray::fromBase64(infos[1].toLatin1())):"unkown error"));
            emit sigStateUnauthorized();
    }
    */

//    if (infos[0] != "stat")
//        qDebug() << infos;

//    if(infos.length()>0){
//        if(infos[0] == "version"){
//            if( DAP_VERSION != infos[1] ) {
//                emit error(tr("Versions mismatch, expect to have %1 but in reply is %2").arg(DAP_VERSION).arg(infos[1]));
//                emit error(tr("Versions mismatch"));
//            }else
//                qInfo() << "[ServiceCtl] Version are checked, they're same";
//         }else if( infos[0] == "usr_msg") {
//            if(infos.length()>=2){
//                qDebug() <<"[ServiceCtl] UsrMsg: "<<QString::fromUtf8(  QByteArray::fromBase64(  infos[1].toLatin1()  ) ) ;
//            }else
//                qWarning() <<"[ServiceCtl] Empty usr_msg";
//        } else if ( infos[0] == "status" ) {
//            qDebug() << "[ServiceCtl] status cmd received "<<a_cmd;
//            tmRestart->stop(); // If get_status command passed well - we stop backcounting to the service restart
//            if( infos[1] == "authorize") {
//                if(infos[2] == "true"){
//                    QSettings settings;
//                    settings.setValue("username",m_username);
//                    settings.setValue("password",m_password);
//                    settings.sync();
//                    emit sigStateAuthorized();
//                }else if( infos[2] == "false"){
//                    emit sigStateUnauthorized();
//                }else if ( infos[2] == "error"){
//                    emit sigStateAuthorizeError();
//                }
//            }else if (infos[1] == "stream") {
//                if( infos[2] == "opened" ) {
//                    qDebug() << "[ServiceCtl] status stream opened";
//                    emit sigStateStreamOpened();
//                }else if (infos[2] == "closed") {
//                    qDebug() << "[ServiceCtl] status stream closed";
//                    emit sigStateStreamClosed();
//                }
//            } else if( infos[1] == "net_config_received"){
//                if( infos[2] == "true"){
//                    qDebug() << "[ServiceCtl] status net_config_received true";
//                    emit sigStateNetConfigTrue();
//                } else {
//                    qDebug() << "[ServiceCtl] status net_config_received false";
//                    emit sigStateNetConfigFalse();
//                }
//            }else if(infos[1] == "tunnel_created") {
//                if(infos[2] == "true") {
//                    qDebug() << "[ServiceCtl] status tunnel_created true";
//                    emit sigStateTunnelCreated();
//                } else {
//                    qDebug() << "[ServiceCtl] status tunnel_created false";
//                    emit sigStateTunnelDestroyed();
//                }
//            }
//        } else if( infos[0] == "disconnected"){
//            emit sigStateUnauthorized();
//        } else if( infos[0]  == "stat"){
//            if(infos.length()>=7){
//                emit sigStatistics(infos[1],infos[2]);
//            }
//        }
//#ifdef Q_OS_ANDROID
//        else if (infos[0] == "android_tunnel_create"){
//            if( infos.length()>3){
//                QtAndroid::androidActivity().callMethod<void>( "stopSapVpnServiceNative"
//                                                       ,"()V" );
//                QtAndroid::androidActivity().callMethod<void>( "startSapVpnServiceNative"
//                                                   ,"(Ljava/lang/String;Ljava/lang/String;I)V"
//                                                   ,QAndroidJniObject::fromString(infos[1]).object<jstring>()
//                                                   ,QAndroidJniObject::fromString(infos[2]).object<jstring>()
//                                                   ,infos[3].toInt());
//                qWarning() <<"[ServiceCtl] Android tunnel created with " << infos[1]<< infos[2] << infos[3];
//            }else
//                qWarning() <<"[ServiceCtl] Command \"android_tunnel_create\" with only "<<infos.length()<<" parameters";
//        }
//#endif
//    }else{
//        qWarning() << "[ServiceCtl] Empty reply from backend service";
//    }
}
