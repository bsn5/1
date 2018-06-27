#include <QtDebug>
#include <unistd.h>
#include <strings.h>

#include <errno.h>
#include <netinet/ip.h>

#include "DapTunWorkerUnix.h"

const struct timespec DapTunWorkerUnix::BREAK_LATENCY = { 0, 1 * 1000 * 1000 };



/**
 * @brief DapTunWorkerUnix::DapTunWorkerUnix
 */
DapTunWorkerUnix::DapTunWorkerUnix(DapTunAbstract *a_tun)
                :DapTunWorkerAbstract(a_tun)
{
    m_breakerSet[0] = m_breakerSet[1] = -1;
    if (::pipe(m_breakerSet) < 0){
        qCritical() <<"Can't create unix named pipe!";
        throw QString("Can't create unix named pipe!");
    }
}


/**
 * @brief DapTunWorkerUnix::loop
 */
void DapTunWorkerUnix::loop()
{
  /*  qDebug() << "void DapTunWorkerUnix::loop()";
    quint8 *tmpBuf;
    size_t tmpBufSize ;

    fd_set fds_read, fds_read_active;
    fd_set fds_write, fds_write_active;

    FD_ZERO (&fds_read);
    FD_ZERO (&fds_write);
    FD_SET (tunSocket(), &fds_read);
    FD_SET (tunSocket(), &fds_write);
    int select_breaker = breaker(0);
    FD_SET (select_breaker,&fds_read);

#
    const size_t DAP_IP_MTU = 0xffff;
    tmpBuf = (quint8 *) ::calloc(1,DAP_IP_MTU ); // Maximum size of IP packet
    tmpBufSize = 0;
    qDebug() <<"Start with MTU = " <<  DAP_IP_MTU<< " tunSocket() = " <<tunSocket();

    emit loopStarted();
    size_t pktOutPos=0;
    DapSockForwPacket * pktOut = nullptr;
    do {
        int ret;
        fds_read_active = fds_read;
        fds_write_active = fds_write;

        /*if (pktOut == nullptr) {  // if no non-sent data
            pktOut = writeDequeue();
        }

        if (pktOut) {
            ret = ::select(FD_SETSIZE,&fds_read_active,&fds_write_active,NULL,NULL);
        } else {*/
           // ret = ::select(FD_SETSIZE,&fds_read_active,NULL,NULL,NULL);
        //}







    uint8_t *tmp_buf;
    ssize_t tmp_buf_size ;
    static int tun_MTU = 100000; /// TODO Replace with detection of MTU size

    tmp_buf = (uint8_t *) calloc(1,tun_MTU);
    tmp_buf_size = 0;

    fd_set fds_read, fds_read_active;

    FD_ZERO (&fds_read);
    FD_SET (tunSocket(), &fds_read);
    int select_breaker = breaker(0);
    FD_SET ( select_breaker,&fds_read);

    emit loopStarted();
    size_t pktOutPos=0;
    DapSockForwPacket * pktOut = nullptr;
    /// Main cycle
    do{
        fds_read_active=fds_read;
        int ret = select(FD_SETSIZE,&fds_read_active,NULL,NULL,NULL) ;








        qDebug() << "select returned ret = "<<ret;
        if (ret > 0) {

            if (FD_ISSET (tunSocket(), &fds_read_active)){ // Event from Tun socket
                qDebug() << "Tun socket is ready for read() ";
                int readRet = ::read(tunSocket(), tmp_buf + tmp_buf_size,tun_MTU-tmp_buf_size);

                qDebug() << "Read "<< readRet<<" bytes";
                if (readRet < 0) {
                    qCritical() << "Read returned "<< readRet;
                    emit loopError(QString("read() returned %1 (%2)").arg(readRet).arg(::strerror(errno)));
                    break;
                }
                tmp_buf_size += readRet;


                if (tmp_buf_size > (int)sizeof(struct ip)) {
                    procDataFromTun(tmp_buf,tmp_buf_size); // Pack data with SF headers and emit it out to DapStreamer
                    tmp_buf_size = 0;
                }

           /* }else if( (pktOut) && (FD_ISSET (tunSocket(), &fds_write_active))){ // Event to Tun socket
                 qDebug() << "Tun socket is ready for write() ";
                 int writeRet = ::write(tunSocket(), pktOut->data+pktOutPos,pktOut->header.op_data.data_size - pktOutPos );

                 qDebug() << "Wrote"<< writeRet<<" bytes";
                 if (writeRet < 0) {
                     qCritical() << "Write returned "<< writeRet;
                     emit loopError(QString("write() returned %1 (%2)").arg(writeRet).arg(::strerror(errno)));
                     break;
                 }
                 pktOutPos += writeRet;
                 if(pktOutPos>= pktOut->header.op_data.data_size ){ // Packet is sent into the tunnel
                     delete pktOut;
                     pktOut = nullptr;
                     pktOutPos = 0;
                 }
            */ } else if(FD_ISSET (select_breaker, &fds_read_active)) { // Event to stop the socket
                char ctlCode=0;
                int readRet = ::read(select_breaker, &ctlCode,1 );
                if( ctlCode == 0 ){ // Other variants - just waked up to write smth out
                    qInfo() << "Got the signal to stop the loop";
                    break;
                }
            } else {
                qCritical() << "Select has no tun handler in the returned set";
                emit loopError(QString("Select has no tun handler in the returned set"));
                break;
            }
        } else {
            qCritical() << "select() returned "<< ret;
            emit loopError(QString("select() returned %1 (%2)").arg(ret).arg(::strerror(errno)));
            break;
        }
    } while(1);
    ::free(tmp_buf);

    emit loopStopped();
}
