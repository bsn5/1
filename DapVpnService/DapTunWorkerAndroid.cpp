#include "DapTunWorkerAndroid.h"

DapTunWorkerAndroid::DapTunWorkerAndroid(DapTunAbstract *a_tun)
    :DapTunWorkerAbstract(a_tun)
{
    // Здесь всё для всех ОС в куче, чтоб не потерялось. При портировании под конкретную ОС - разбрать
   /* const size_t DAP_IP_MTU = 0xffff;
    int socket = listenSocket;
    int argVal = 0;
#ifndef Q_OS_ANDROID
    if (arg != nullptr) {
        argVal = *static_cast<int*>(arg);
        ::free(arg);
    }
#endif
    qDebug() << "listenTunnelThread with arg = " << argVal;
    Q_UNUSED(arg);
#ifdef Q_OS_MACOS
    long a_num = 4;
    me()->m_tunDevice=QString("tun%1").arg(a_num);

    me()->m_tuneSocket = ::open (("/dev/"+me()->m_tunDevice).toLatin1().constData(),O_RDWR);
    me()->emitUsrMsg("OS: Opened /dev/"+me()->m_tunDevice);
    if(me()->m_tuneSocket<=0){
        qCritical() << "[SapStreamChSF] listenTunnelThread() can't open "<< me()->m_tunDevice;
        return NULL;
    }
    if (::fcntl(me()->m_tuneSocket, F_SETFL, O_NONBLOCK) < 0){    ;
        qCritical() << "[SapStreamChSF] listenTunnelThread() can't switch socket to non-block mode";
        return NULL;
    }
    if (::fcntl(me()->m_tuneSocket, F_SETFD, FD_CLOEXEC) < 0){    ;
        qCritical() << "[SapStreamChSF] listenTunnelThread() can't switch socket to not be passed across execs";
        return NULL;
    }
#endif
#ifdef Q_OS_WIN //maybe include in for all?
    me()->m_tuneSocket=me()->tunnelAllocate();
    //int s_ret;
#endif
    if(socket<0){
        qCritical() <<"[SapStreamChSF] Tunnel is not allocated!";
        return NULL;
    }
#ifdef Q_OS_WIN
    s_ret = TunTap::getInstance().setAdress(SapStreamChSF::me()->m_tunAddr, me()->m_tunDest, QString("255.255.255.0"));
#endif
    quint8 *tmpBuf;
    size_t tmpBufSize ;

    tmpBuf = (quint8 *) calloc(1,100000);
    tmpBufSize = 0;
    qDebug() << "[SapStreamChSF] listenTunnelThread() start with MTU = "<<100000;

#ifndef Q_OS_ANDROID
#ifndef Q_OS_WIN
    //pthread_barrier_wait(&SapStreamChSF::me()->m_listenTunnelThreadBarrier);
#endif
#endif

#ifndef Q_OS_WIN
    fd_set fds_read, fds_read_active;
    fd_set fds_write, fds_write_active;
    size_t pktOutPos=0;
    DapSockForwPacket * pktOut = nullptr;
    FD_ZERO (&fds_read);
    FD_ZERO (&fds_write);
    FD_SET (socket, &fds_read);
    FD_SET (socket, &fds_write);

   // int select_breaker = 1;
   // FD_SET (select_breaker,&fds_read);
    do{
        fds_read_active=fds_read;
        fds_write_active = fds_write;

        if (pktOut == nullptr) {  // if no non-sent data
            pktOut = StaticData::me()->getPacket();
        }
        int ret = 0;
        if (pktOut)
            ret = ::select(FD_SETSIZE,&fds_read_active,&fds_write_active,NULL,NULL);
        else
            ret = ::select(socket + 1,&fds_read_active,NULL,NULL,NULL) ;
        //
        if(ret > 0) {
            if (FD_ISSET (socket, &fds_read_active)){
               // qDebug() << "Tun socket is ready for read() ";
                int readRet = ::read(socket, tmpBuf + tmpBufSize,DAP_IP_MTU-tmpBufSize);

               // qDebug() << "Read "<< readRet<<" bytes";
                if (readRet < 0) {
                   // qCritical() << "Read returned "<< readRet;
                   // qDebug() << QString("read() returned %1 (%2)").arg(readRet).arg(::strerror(errno));
                    break;
                }
                tmpBufSize += readRet;

                DapSockForwPacket * pktOut =
                    (DapSockForwPacket *)::calloc(1,sizeof(pktOut->header)+tmpBufSize );
                pktOut->header.op_code = STREAM_SF_PACKET_OP_CODE_RAW_SEND;
                pktOut->header.socket_id = socket;
                pktOut->header.op_data.data_size = tmpBufSize;
                memcpy(pktOut->data, tmpBuf, pktOut->header.op_data.data_size);
                tmpBufSize = 0;
                StaticData::me()->sendPacketOut(pktOut);


            }else if( (pktOut) && (FD_ISSET (socket, &fds_write_active))){ // Event to Tun socket
                qDebug() << "Tun socket is ready for write() ";
                int writeRet = ::write(socket, pktOut->data+pktOutPos,pktOut->header.op_data.data_size - pktOutPos );

                qDebug() << "Wrote"<< writeRet<<" bytes";
                if (writeRet < 0) {
                    qCritical() << "Write returned "<< writeRet;
                    break;
                }
                pktOutPos += writeRet;
                if(pktOutPos>= pktOut->header.op_data.data_size ){ // Packet is sent into the tunnel
                    delete pktOut;
                    pktOut = nullptr;
                    pktOutPos = 0;
                }
            }else if(FD_ISSET (1, &fds_read_active)){
                qDebug() << "[SapStreamChSF] select_breaker signaled us to close listen thread";
                break;
            }else{
                qCritical() << "[SapStreamChSF] listenTunnelThread() select has no tun handler in the returned set";
                break;

            }
        }else {
            qCritical() << "[SapStreamChSF] listenTunnelThread() select() returned "<< ret;
            break;
        }

    }while(1);
#else
    do {
            if (!TunTap::getInstance()) {
                qDebug() << "+_ DO CLOSE TUN CONNECT";
                TunTap::getInstance().closeTun();
                me()->emitUsrMsg(QString("OS:  %1 virtual network device removed").arg(me()->m_tunDevice));
                me()->destroyTunnelAfter();
                break;
            }
            int readRet=TunTap::getInstance().read_tun(SapStreamChSF::me()->m_tuneSocket,tmpBuf+tmpBufSize,SapStreamChSF::me()->m_MTU-tmpBufSize);
            //qDebug() << "[SapStreamChSF] listenTunnelThread() read "<< readRet<<" bytes";
            tmpBufSize += readRet;
            if(tmpBufSize >(int) sizeof(struct ip) ){
                //struct ip *iph = (struct ip* ) tmpBuf;
                //qDebug() << "[SapStreamChSF] saddr = " << ::inet_ntoa(iph->ip_src)<< " dadrr = " << inet_ntoa( iph->ip_dst) << " size = "<<tmpBufSize  ;
                SapSfPacket * pktOut =(SapSfPacket *) ::calloc(1,sizeof(pktOut->header)+tmpBufSize );
                pktOut->header.op_code=STREAM_SF_PACKET_OP_CODE_RAW_SEND;
                pktOut->header.sock_id=SapStreamChSF::me()->m_tuneSocket;
                pktOut->header.op_data.data_size=(quint32)tmpBufSize;
                memcpy(pktOut->data,tmpBuf,pktOut->header.op_data.data_size);
                SapStreamChSF::me()->packetOut(pktOut);
                SapStreamChSF::me()->emitBytesWrite((int)tmpBufSize);
                SapStreamChSF::me()->emitSigPacketWrite();
                tmpBufSize=0;
            }
    } while (1);
#endif
    qDebug() << "[SapStreamChSF] Listen thread finished";
    ::free(tmpBuf);
    return NULL;*/
}
