#define STREAM_SF_PACKET_OP_CODE_RAW_SEND 0xbc

#include <QtDebug>
#include "DapTunWorkerAbstract.h"
#include "DapChSockForw.h"

DapTunWorkerAbstract::DapTunWorkerAbstract( DapTunAbstract * a_tun )
    :m_tunSocket(-1),m_tun(a_tun)
{
    m_writeQueue     = a_tun->writeQueue();
    m_writeQueueCond = a_tun->writeQueueCond();
    m_writeQueueLock = a_tun->writeQueueLock();

}

/**
 * @brief DapTunWorkerAbstract::procDataFromTun
 * @param a_buf
 * @param a_bufSize
 */
void DapTunWorkerAbstract::procDataFromTun(void * a_buf,size_t a_bufSize)
{
    // struct ip *iph = (struct ip* ) tmpBuf;
    // qDebug() << "[DapChSockForw] saddr = " << ::inet_ntoa(iph->ip_src)<< " dadrr = " << inet_ntoa( iph->ip_dst) << " size = "<<tmpBufSize  ;
    DapSockForwPacket * pktOut =
        (DapSockForwPacket *)::calloc(1,sizeof(pktOut->header)+a_bufSize );
    pktOut->header.op_code = STREAM_SF_PACKET_OP_CODE_RAW_SEND;
    pktOut->header.socket_id = m_tunSocket;
    pktOut->header.op_data.data_size = a_bufSize;
    memcpy(pktOut->data, a_buf, pktOut->header.op_data.data_size);

    emit packetOut(pktOut);
    emit readBytes(a_bufSize);
    emit readPackets();
}
