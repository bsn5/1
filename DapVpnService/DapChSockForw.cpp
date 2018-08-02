#include <QtDebug>

#include "DapStreamer.h"
#include "DapChSockForw.h"

#define STREAM_SF_PACKET_OP_CODE_CONNECTED 0x000000a9
#define STREAM_SF_PACKET_OP_CODE_CONNECT 0x000000aa
#define STREAM_SF_PACKET_OP_CODE_DISCONNECT 0x000000ab
#define STREAM_SF_PACKET_OP_CODE_SEND 0x000000ac
#define STREAM_SF_PACKET_OP_CODE_RECV 0x000000ad
#define STREAM_SF_PACKET_OP_CODE_PROBLEM 0x000000ae

#define STREAM_SF_PROBLEM_CODE_NO_FREE_ADDR 0x00000001
#define STREAM_SF_PROBLEM_CODE_TUNNEL_DOWN  0x00000002
#define STREAM_SF_PROBLEM_CODE_PACKET_LOST  0x00000003

#define STREAM_SF_PACKET_OP_CODE_RAW_L3 0x000000b0
#define STREAM_SF_PACKET_OP_CODE_RAW_L2 0x000000b1
#define STREAM_SF_PACKET_OP_CODE_RAW_L3_ADDR_REQUEST 0x000000b2
#define STREAM_SF_PACKET_OP_CODE_RAW_L3_ADDR_REPLY 0x000000b3
#define STREAM_SF_PACKET_OP_CODE_RAW_L3_DEST_REPLY 0x000000b4

#define STREAM_SF_PACKET_OP_CODE_RAW_SEND 0x000000bc
#define STREAM_SF_PACKET_OP_CODE_RAW_RECV 0x000000bd


#ifdef Q_OS_UNIX
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <netinet/ip.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <sys/select.h>
#endif

#include <QProcess>
#include <QFile>


#include <fcntl.h>
#include <errno.h>
#include <string.h>

/**
 * @brief DapChSockForw::delForwardingAll
 */
void DapChSockForw::delForwardingAll()
{
    for (auto s : m_socketsIn)
        delete s;
    m_socketsIn.clear();

    for (auto s : m_servs)
        delete s;
    m_servs.clear();
}

/**
 * @brief DapChSockForw::delForwarding
 * @param sockId
 */
void DapChSockForw::delForwarding(int sockId)
{
    QTcpServer * serv = m_servs[sockId];
    if (serv) {
        m_servs.remove(sockId);
        delete serv;
    } else qDebug() << "Not found socket by id: " << sockId;
}

/**
 * @brief DapChSockForw::onFwClientReadyRead
 */
void DapChSockForw::onFwClientReadyRead()
{
    qDebug() << "[onFwClientReadyRead]";
    QTcpSocket * sock = qobject_cast<QTcpSocket*>(QObject::sender());
    if (sock) {
        QByteArray ba = sock->readAll();
        qDebug() << "From client's socket have read " << ba.length() << "bytes.";
        DapSockForwPacket * pkt = (DapSockForwPacket *) calloc(1, ba.length() + sizeof(pkt->header));
        memcpy(pkt->data, ba.constData(), ba.length());
        pkt->header.op_data.data_size = ba.length();
        pkt->header.op_code = STREAM_SF_PACKET_OP_CODE_SEND;
        pkt->header.socket_id = sock->socketDescriptor();
        packetOut(pkt);
    } else{
        qCritical() << "Cant cast sender() object of type "
                 <<QObject::sender()->metaObject()->className()<< " to QTcpSocket";
    }
}

/**
 * @brief DapChSockForw::onFwClientDisconnected
 */
void DapChSockForw::onFwClientDisconnected()
{
    QTcpSocket *sock = qobject_cast<QTcpSocket*>(QObject::sender());
    if(sock) {
        qDebug() << "Disconnected client " << sock->localAddress();
        m_socketsIn.remove(sock->socketDescriptor());

        DapSockForwPacket* pkt = (DapSockForwPacket*) calloc(1,sizeof(pkt->header) + 1);
        pkt->header.op_code = STREAM_SF_PACKET_OP_CODE_DISCONNECT;
        pkt->header.socket_id = sock->socketDescriptor();
        pkt->header.raw.data_size = 2; ///TODO prev value is 0
        packetOut(pkt);
    } else{
        qCritical() << "Cant cast sender() object of type "
                 <<QObject::sender()->metaObject()->className()<< " to QTcpSocket";
    }
}

/**
 * @brief DapChSockForw::onFwServerConnected
 */
void DapChSockForw::onFwServerConnected()
{
    qDebug() << "[onFwServerConnected()]";
    QTcpServer *serv = qobject_cast<QTcpServer*>(QObject::sender());
    QTcpSocket *sock;
    QByteArray remoteAddrBA;
    DapSockForwPacket *pkt;

    if(serv) {
        if(!serv->hasPendingConnections())
            qDebug() << "No pendidng connections on the server";
        else do {

            sock = serv->nextPendingConnection();
            remoteAddrBA = (m_servsRemoteAddr[serv->socketDescriptor()]).toLatin1();
            m_socketsIn.insert(sock->socketDescriptor(),sock);
            pkt = (DapSockForwPacket*) calloc(1, sizeof(pkt->header) +  remoteAddrBA.length());

            pkt->header.op_code = STREAM_SF_PACKET_OP_CODE_CONNECT;
            pkt->header.socket_id = sock->socketDescriptor();
            pkt->header.op_connect.addr_size = remoteAddrBA.length();

            memcpy(pkt->data, remoteAddrBA.constData(), pkt->header.op_connect.addr_size);

            pkt->header.op_connect.port = m_servsRemotePort[serv->socketDescriptor()];

            qDebug() << "New connection enstablished to "
                     << m_servsRemoteAddr[serv->socketDescriptor()]
                     << ":" << pkt->header.op_connect.port
                     << " addr size" << pkt->header.op_connect.addr_size;

            connect(sock, &QTcpSocket::readyRead, this, &DapChSockForw::onFwClientReadyRead);
            connect(sock, &QTcpSocket::disconnected, this, &DapChSockForw::onFwClientDisconnected);

            packetOut(pkt);

        } while( serv->hasPendingConnections() );
    } else
        qDebug() << "Can't cast object to QTcpServer";
}

/**
 * @brief DapChSockForw::addForwarding
 * @param remoteAddr
 * @param remotePort
 * @param localPort
 * @return
 */
quint16 DapChSockForw::addForwarding(const QString remoteAddr, quint16 remotePort, quint16 localPort)
{
    qDebug() << "addForwarding " << QString("127.0.0.1:%1 => %2:%3")
                                                                    .arg(localPort)
                                                                    .arg(remoteAddr)
                                                                    .arg(remotePort);
    QTcpServer *server = new QTcpServer;
    if(server->listen(QHostAddress("127.0.0.1"), localPort)) {
        qDebug() << "Sucessfully set up " << localPort << " to listening";
        m_servs.insert(server->socketDescriptor(), server);
        m_servsRemoteAddr.insert(server->socketDescriptor(), remoteAddr);
        m_servsRemotePort.insert(server->socketDescriptor(), remotePort);
        connect(server, &QTcpServer::newConnection, this, &DapChSockForw::onFwServerConnected);
        connect(server, &QTcpServer::acceptError,[=]{
           qDebug() << "QTcpServer::acceptError() "<< server->errorString() ;
        });
        return server->serverPort();
    } else {
        qWarning() << "Can't open local port for listening";
        emit error(QString("Can't open local port %1").arg(localPort));
        delete server;
        return 0;
    }
}

/**
 * @brief DapChSockForw::DapChSockForw
 */
DapChSockForw::DapChSockForw(DapStreamer * a_streamer)
    :DapChBase(nullptr, 's'), m_streamer(a_streamer)
{
    tun = new DapTunNative();
    connect(tun, &DapTunNative::created, this, &DapChSockForw::tunCreated);
    connect(tun, &DapTunNative::destroyed, this, &DapChSockForw::tunDestroyed);
    connect(tun, &DapTunNative::error , this, &DapChSockForw::tunError);
    connect(tun, &DapTunNative::packetOut, this, &DapChSockForw::packetOut);
    connect(tun, &DapTunNative::sendCmd, this, &DapChSockForw::sendCmdAll);
    connect(tun, &DapTunNative::bytesRead, this, &DapChSockForw::bytesRead);
    connect(tun, &DapTunNative::bytesWrite, this, &DapChSockForw::bytesWrite);
    connect(tun, &DapTunNative::nativeCreateRequest, this, &DapChSockForw::sigTunNativeCreate);
    connect(tun, &DapTunNative::nativeDestroyRequest, this, &DapChSockForw::sigNativeDestroy);
}

/**
 * @brief DapChSockForw::packetOut
 * @param pkt
 */
void DapChSockForw::packetOut(DapSockForwPacket *pkt)
{
    DapChannelPacketHdr* hdr= (DapChannelPacketHdr *) ::calloc(1, sizeof(DapChannelPacketHdr));
    hdr->id='s';
    hdr->type='d';
    hdr->size=sizeof(pkt->header);
    switch(pkt->header.op_code){
        case STREAM_SF_PACKET_OP_CODE_SEND:
        case STREAM_SF_PACKET_OP_CODE_RECV:
        case STREAM_SF_PACKET_OP_CODE_RAW_SEND:
        case STREAM_SF_PACKET_OP_CODE_RAW_RECV:
        case STREAM_SF_PACKET_OP_CODE_RAW_L3_ADDR_REPLY:
        case STREAM_SF_PACKET_OP_CODE_RAW_L3:
        case STREAM_SF_PACKET_OP_CODE_RAW_L2:
            hdr->size+=pkt->header.op_data.data_size;
    }
    //qDebug() << "PacketOut() hdr.size = "<<hdr->size<<" op_code= "<<pkt->header.op_code;
    emit pktChOut(hdr,pkt);
}

/**
 * @brief DapChSockForw::requestIP
 */
void DapChSockForw::requestIP()
{
    emit netConfigRequested();
    DapSockForwPacket * pktOut = reinterpret_cast<DapSockForwPacket*>( ::calloc(1,sizeof(pktOut->header))  ) ;
    pktOut->header.op_code=STREAM_SF_PACKET_OP_CODE_RAW_L3_ADDR_REQUEST;
    packetOut(pktOut);
}

/**
 * @brief DapChSockForw::netConfigClear
 */
void DapChSockForw::netConfigClear()
{
    m_addr.clear();
    m_gw.clear();
    emit netConfigCleared();
}

/**
 * @brief DapChSockForw::tunCreate
 * @param a_addr
 * @param a_gw
 */
void DapChSockForw::tunCreate(const QString &a_addr, const QString &a_gw)
{
    m_addr = a_addr;
    m_gw = a_gw;
    tun->create(a_addr,a_gw,DapSession::getInstance()->upstreamIp(), streamer()->upstreamSocket() );
}

/**
 * @brief DapChSockForw::tunCreate
 */
void DapChSockForw::tunCreate()
{
    tun->create(m_addr,m_gw,DapSession::getInstance()->upstreamIp(), streamer()->upstreamSocket() );
    tun->workerStart();
}

void DapChSockForw::tunDestroy()
{
    tun->destroy();
}

/**
 * @brief DapChSockForw::afterTunCreate
 * @param a_tunSocket
 */
void DapChSockForw::workerStart(int a_tunSocket)
{
    tun->setTunSocket(a_tunSocket);
    tun->workerStart(); // start loop
}

/**
 * @brief DapChSockForw::onPktIn
 * @param pkt
 */
void DapChSockForw::onPktIn(DapChannelPacket* pkt)
{
    // qDebug() << "onPktIn: id ="<<pkt->hdr()->id << " type = "<< pkt->hdr()->type<< " ch_data_size = "<<pkt->hdr()->size;
    DapSockForwPacket * pktSF=(DapSockForwPacket *) pkt->data();
    //qDebug() << " onPktIn: SampSFPacket op_code ="<< pktSF->header.op_code;
    switch(pktSF->header.op_code){
        case STREAM_SF_PACKET_OP_CODE_SEND:
        case STREAM_SF_PACKET_OP_CODE_CONNECT:{
            qDebug() << "Backforward connection are not processed";
        } break;
        case STREAM_SF_PACKET_OP_CODE_DISCONNECT:{

            break;
            QTcpSocket *sc=m_socketsIn[pktSF->header.socket_id];
            if (sc) {
                m_socketsIn.remove(pktSF->header.socket_id);
                sc->close();
            } else {
                qDebug() <<" Unknown socket_id "<<pktSF->header.socket_id;
            }
        } break;
        case STREAM_SF_PACKET_OP_CODE_RECV:{
            QTcpSocket *sc=m_socketsIn[pktSF->header.socket_id];
            if (sc) {
                sc->write((const char*)pktSF->data, pktSF->header.op_data.data_size);
            } else {
                qDebug() <<" Unknown socket_id "<<pktSF->header.socket_id;
            }
        } break;
        case STREAM_SF_PACKET_OP_CODE_RAW_L3_ADDR_REPLY:{
            m_addr = QHostAddress(ntohl (*((quint32*) pktSF->data))).toString() ;
            m_gw = QHostAddress(ntohl ( ((quint32*) pktSF->data)[1])).toString() ;
            emit netConfigReceived(m_addr,m_gw);
        }break;
        case STREAM_SF_PACKET_OP_CODE_RAW_RECV:{
            pkt->unleashData(); // Uleash *data section from pkt object
            tun->tunWriteData(pktSF);
        }break;
        /*case STREAM_SF_PACKET_OP_CODE_CONNECTED:
            qDebug() << "[DapChSockForw] Get connected packet."; // Repsonse to the connect Packet
            tcp_sock = m_socketsIn[sockForwPacket->header.socket_id];
            if(tcp_sock)
                connect(tcp_sock, &QTcpSocket::readyRead, this, &DapChSockForw::onClientReadyRead);
            else
                qWarning() << ("[DapChSockForw] onPktIn() Not Find Socket by socket_id!");
            break;*/
    }


    delete pkt;
}







