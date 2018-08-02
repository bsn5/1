#include <io.h>
#include <time.h>
#include <string>
#include <Winsock2.h>
#include "windows/ip.h"
#include "windows/tuntap/tuntap.h"

#ifndef IP_MTU
#define IP_MTU 0xffff // Until this is integrated from linux/in.h to netinet/in.h
#endif

#include "DapTunWorkerWindows.h"

/**
 * @brief DapTunWorkerWindows::DapTunWorkerWindows
 */
DapTunWorkerWindows::DapTunWorkerWindows(DapTunAbstract *a_tun)
    :DapTunWorkerAbstract(a_tun)

{

}

/**
 * @brief DapTunWorkerWindows::loop
 */
void DapTunWorkerWindows::loop()
{
    // TODO implement sending data from writeQueue
    // int ret=TunTap::getInstance().write_tun(m_tuneSocket,pktSF->data,pktSF->header.op_data.data_size);


    quint8 *tmpBuf;
    size_t tmpBufSize ;
    do {
        int readRet = TunTap::getInstance().read_tun(DapChSockForw::getInstance()->m_tuneSocket,tmpBuf+tmpBufSize,DapChSockForw::getInstance()->m_MTU-tmpBufSize);
        //qDebug() << "[DapChSockForw] listenTunnelThread() read "<< readRet<<" bytes";
        if (readRet < 0)
        {
            qWarning() << "[DapSaChSockForw] Error read from tun";
            continue;
        }
        tmpBufSize += readRet;
        if(tmpBufSize >(int) sizeof(struct ip) ){
            //struct ip *iph = (struct ip* ) tmpBuf;
            //qDebug() << "[DapChSockForw] saddr = " << ::inet_ntoa(iph->ip_src)<< " dadrr = " << inet_ntoa( iph->ip_dst) << " size = "<<tmpBufSize  ;
            DapSockForwPacket * pktOut =(DapSockForwPacket *) ::calloc(1,sizeof(pktOut->header)+tmpBufSize );
            pktOut->header.op_code=STREAM_SF_PACKET_OP_CODE_RAW_SEND;
            pktOut->header.socket_id=DapChSockForw::getInstance()->m_tuneSocket;
            pktOut->header.op_data.data_size=(quint32)tmpBufSize;
            memcpy(pktOut->data,tmpBuf,pktOut->header.op_data.data_size);
            emit packetOut(pktOut);
            emit bytesRead((int)tmpBufSize);
            emit readPackets();
            tmpBufSize=0;
        }
    } while (1);

    ::free(tmpBuf);
}
