#include <QtDebug>
#include <QTimer>

#include "DapServiceClient.h"

/**
 * @brief DapServiceClient::SapServiceClient
 * @param parent
 */
DapServiceClient::DapServiceClient(const QString& a_serviceName)
    : QObject(nullptr)
{
    isAuthRoot = false;
    m_serviceName = a_serviceName;

    sockCtl = new DapUiSocket();
    connect(sockCtl,&DapUiSocket::readyRead,this, &DapServiceClient::onCtlReadReady);
    connect(sockCtl,static_cast<void(DapUiSocket::*)(DapUiSocketError)> (&DapUiSocket::error) ,
            this, &DapServiceClient::onCtlSocketError );
    connect(sockCtl,&DapUiSocket::connected, this, &DapServiceClient::ctlConnected);

    connect(sockCtl,&DapUiSocket::disconnected, this, &DapServiceClient::ctlDisconnected);
}

/**
 * @brief DapServiceClient::init
 */
void DapServiceClient::init()
{
    qDebug() << "[DapServiceClient::init]";
    DapServiceNative::init();
    connectToService();
}

/**
 * @brief DapServiceClient::connectToService
 */
void DapServiceClient::connectToService()
{


#ifdef DAP_UI_SOCKET_TCP
            QTimer::singleShot(500,[=]{sockCtl->connectToHost(QHostAddress("127.0.0.1"),22142);});
#else
            QTimer::singleShot(1000,[=]{
                qDebug() << "[connectToService]";
                sockCtl->connectToServer(DAP_BRAND);
            });
#endif
}

/**
 * @brief DapServiceClient::sendCmd
 * @details Send command to the DapVPNService
 * @param a_cmd
 */
void DapServiceClient::sendCmd(const QString & a_cmd)
{
    qDebug() << "[DapServiceClient] sock ctl send command "<<a_cmd;
    if(sockCtl->isWritable())
        sockCtl->write( QString("%1%2").arg(a_cmd).arg('\n').toLatin1()  );
}




/**
 * @brief DapServiceClient::onCtlSocketError
 * @param socketError
 */
void DapServiceClient::onCtlSocketError(DapUiSocketError socketError)
{
    Q_UNUSED(socketError);
    qDebug() << "[DapServiceClient] onCtlSocketError() sockCtl->errorString() == "<<sockCtl->errorString();
    emit ctlError(sockCtl->errorString());
    if (sockCtl->isOpen()){
        return;
    }else
        connectToService();
}

/**
 * @brief DapServiceClient::onCtlReadReady
 */
void DapServiceClient::onCtlReadReady()
{
    QString readStr = QString::fromLatin1( sockCtl->readAll());
    int nInd;
//    qDebug() << "[DapServiceClient] onCtlReadReady() readStr = "<<readStr;
lb_read_str:

    if((nInd=readStr.indexOf('\n'))==-1 ){
        qDebug() << "[DapServiceClient] No CR symbol";
        readStrBuffer += readStr;
    }else{
        readStrBuffer += readStr.left(nInd);
//        qDebug() << "Processing cmd: "<<readStrBuffer;
        procCmd(readStrBuffer);
        readStr= readStr.mid(nInd+1);
//        qDebug() << "[DapServiceClient] proc next buf " << readStr;
        readStrBuffer.clear();
        if(readStr.length()>0){
            goto lb_read_str;
        }
    }
}
