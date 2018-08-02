#ifndef DAPCHSOCKFORW_H
#define DAPCHSOCKFORW_H

#include "DapChBase.h"
#include "DapSession.h"

#include "DapSockForwPacket.h"
#include "DapTunNative.h"

#include <QDebug>
#include <QTcpSocket>
#include <QTcpServer>


struct DapSockForwPacket;
class DapStreamer;
class DapChSockForw : public DapChBase
{
    Q_OBJECT
public:
private:
    // DATAS
    DapStreamer * m_streamer;

    QMap<int, QTcpSocket* > m_socketsIn;
    QMap<int, QTcpServer* > m_servs;
    QMap<int, QString > m_servsRemoteAddr;
    QMap<int, quint16 > m_servsRemotePort;

    QMap<int, QTcpSocket* > m_socksIn;
    QMap<int, QTcpSocket* > m_socksOut;

    DapTunNative * tun;
    // METHODS
    QString m_addr, m_gw;
private slots:
    void onFwServerConnected();//ok
    void onFwClientReadyRead();//ok
    void onFwClientDisconnected();//ok

signals:
    void bytesRead(int);
    void bytesWrite(int);

    void fwdDisconnected(int sockServ, int sockClient);
    void usrMsg(QString);
    void sigPacketRead();
    void sigPacketWrite();

    void sigTunNativeCreate();
    void sigNativeDestroy();


public:
    DapChSockForw(DapStreamer * a_streamer);

    bool isTunCreated(){return tun->isCreated();}

    void tunCreate (const QString& a_addr, const QString& a_gw);
    void workerStart(int a_tunSocket);

    quint16 addForwarding(const QString remoteAddr, quint16 remotePort, quint16 localPort);//ok
    void delForwarding(int sockId); //ok
    void delForwardingAll(); //ok

    DapStreamer * streamer(){ return m_streamer; }
signals:

    void netConfigReceived(QString,QString); //ok
    void netConfigRequested();
    void netConfigReceivedSame();
    void netConfigCleared();
    void tunCreated(); //ok
    void tunDestroyed(); //ok
    void tunError(const QString&); //ok

    void tunWriteData();

    void sendCmdAll(const QString&);
public slots:
    void onPktIn(DapChannelPacket *pkt) override; //ok
    void packetOut(DapSockForwPacket *pkt); //ok

    void requestIP(); //ok
    void netConfigClear();

    void tunCreate(); // create with all predefined before values
    void tunDestroy();
};

#endif // DAPCHSOCKFORW_H
