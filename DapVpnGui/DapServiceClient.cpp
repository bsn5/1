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

    dapCmdParser = new DapCmdParser(sockCtl, this);

    connect(dapCmdParser, &DapCmdParser::cmdReady, this, &DapServiceClient::procCmdController);

    connect(sockCtl,static_cast<void(DapUiSocket::*)(DapUiSocketError)> (&DapUiSocket::error) ,
            this, &DapServiceClient::onCtlSocketError);
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
            sockCtl->connectToHost(QHostAddress::LocalHost, 22143);
#else
           // QTimer::singleShot(1000,[=]{
            //    qDebug() << "[connectToService]";
                sockCtl->connectToServer(DAP_BRAND);
           // });
#endif
}

/**
 * @brief DapServiceClient::sendCmd
 * @details Send command to the DapVPNService
 * @param a_cmd
 */
void DapServiceClient::sendCmd(const QString & a_cmd)
{
    //qDebug() << "[DapServiceClient] sock ctl send command "<< a_cmd;
    if(sockCtl->isWritable())
        sockCtl->write(QString("%1%2").arg(a_cmd).arg('\n').toLatin1()  );
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
    QByteArray readBytes = sockCtl->readAll();
    int nInd;
//    qDebug() << "[DapServiceClient] onCtlReadReady() readStr = "<<readStr;
lb_read_str:

    if((nInd=readBytes.indexOf("\n\n"))==-1 ){
        qDebug() << "[DapServiceClient] No CR symbol";
        readBuffer += readBytes;
    }else{
        nInd++; // move idx to second \n
        readBuffer += readBytes.left(nInd);
        procCmdController(readBuffer);
        readBytes = readBytes.mid(nInd+1);
        readBuffer.clear();
        if(readBytes.length()>0){
            goto lb_read_str;
        }
    }
}
