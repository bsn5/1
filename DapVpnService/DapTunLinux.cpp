#include <QtDebug>
#include <QProcess>
#include <QFile>
#include <QSettings>

#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/socket.h>

#include <linux/if.h>
#include <linux/if_tun.h>

#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stropts.h>
#include "DapTunLinux.h"
#include "DapTunWorkerUnix.h"
#include "staticdata.h"

int DapTunLinux::nmcliVersionMajor=1;
int DapTunLinux::nmcliVersionMinor=6;
int DapTunLinux::nmcliVersionBuild=0;


/**
 * @brief DapTunLinux::DapTunLinux
 */
DapTunLinux::DapTunLinux()
{


    if(nmcliVersion.size()==0){ // If not detected before - detect nmcli version
        QProcess cmdProcess;
        // Thats command takes the last one piece of 'nmcli -v' output
//        cmdProcess.start("/bin/sh  'for i in `nmcli -v`; do a=$i; done; echo $a'");
        cmdProcess.start("nmcli -v");
        cmdProcess.waitForFinished(-1);
        QByteArray cmdOutput = cmdProcess.readAllStandardOutput();
        nmcliVersion= QString::fromUtf8(cmdOutput).split(' ').constLast();
        nmcliVersion=nmcliVersion.replace("\n","");
        QStringList nmcliVersionNumbers= nmcliVersion.split('.');
        if(nmcliVersionNumbers.size()==3){
            nmcliVersionMajor=nmcliVersionNumbers.at(0).toInt();
            nmcliVersionMinor=nmcliVersionNumbers.at(1).toInt();
            nmcliVersionBuild=nmcliVersionNumbers.at(2).toInt();
        }
        qDebug()<< QString("nmcli version detected: %1.%2.%3")
                   .arg(nmcliVersionMajor)
                   .arg(nmcliVersionMinor)
                   .arg(nmcliVersionBuild);
    }

}

/**
 * @brief DapTunLinux::requestTunDeviceCreate
 */
void DapTunLinux::tunDeviceCreate()
{
    qDebug() << "[DapTunLinux::tunDeviceCreate]";
    int fd=-1;
    struct ifreq ifr;
    char clonedev[] = "/dev/net/tun";
    char dev[IFNAMSIZ] = {0};
    int flags = IFF_TUN |IFF_NO_PI;

    /* Arguments taken by the function:
    *
    * char *dev: the name of an interface (or '\0'). MUST have enough
    *   space to hold the interface name if '\0' is passed
    * int flags: interface flags (eg, IFF_TUN etc.)
    */

    /* open the clone device */
    if ((fd = ::open(clonedev, O_RDWR)) < 0 ) {
       qCritical() << "Can't open /dev/net/tun device!";
     return;
    }
    
    ::memset(&ifr,0,sizeof(ifr));

    ifr.ifr_flags = flags;   /* IFF_TUN or IFF_TAP, plus maybe IFF_NO_PI */

    if (dev[0]) {
     /* if a device name was specified, put it in the structure; otherwise,
      * the kernel will try to allocate the "next" device of the
      * specified type */
     ::strncpy(ifr.ifr_name, dev, IFNAMSIZ);
    }else
    /* try to create the device */


    if (::ioctl(fd, TUNSETIFF, (void *) &ifr) < 0) {
       qCritical() << "Can't create tun network interface!";

     ::close(fd);
     return;
    }

    /* if the operation was successful, write back the name of the
    * interface to the variable "dev", so the caller can know
    * it. Note that the caller MUST reserve space in *dev (see calling
    * code below) */

    ::strncpy(dev, ifr.ifr_name,IFNAMSIZ);
    m_tunDeviceName = QString::fromLatin1(dev);
    qInfo() << "Created "<<m_tunDeviceName<<" network interface";


    /* this is the special file descriptor that the caller will use to talk
    * with the virtual interface */

    m_tunSocket = fd;
    m_listen_socket = fd;

    QFile resolvConf;
    resolvConf.setFileName("/etc/resolv.conf");
    resolvConf.open(QFile::ReadOnly);
    QString resolvConfStr = resolvConf.readAll();
    resolvConf.close();

    if (resolvConfStr.split("\n")[0] == "#Sap service resolver") {
        qWarning() << "[SapStreamChSF] #Sap service resolver error";
    } else {
          QSettings resolvSettigs;
          resolvSettigs.setValue("resolver", resolvConfStr);
          resolvSettigs.sync();
    }

    resolvConf.open(QFile::WriteOnly);
    QTextStream ts(&resolvConf);
    ts << "#Sap service resolver\n" << "nameserver " + m_gw << endl;
    resolvConf.close();

    /*
    struct in_addr client_addr_last;
    struct in_addr int_network_mask;
    struct in_addr int_network_addr;
    struct in_addr int_network;
    int tun_ctl_fd;
    struct ifreq ifr;

    int tun_fd;
    int tun_tx;
    int tun_rx;


    qDebug() << "[DapTunLinux::tunDeviceCreate]";
    inet_aton("10.0.6.1", & int_network );
    inet_aton("255.255.255.0", & int_network_mask );
    int_network_addr.s_addr= (int_network.s_addr | 0x01000000); // grow up some shit here!
    client_addr_last.s_addr = int_network_addr.s_addr;

    if( (tun_ctl_fd = open("/dev/net/tun", O_RDWR)) < 0 ) {
    qDebug() << "Opening /dev/net/tun error: " << strerror(errno);
    } else{
    int err;
    memset(&ifr, 0, sizeof(ifr));
    ifr.ifr_flags = IFF_TUN | IFF_NO_PI;
   if( (err = ioctl(tun_ctl_fd, TUNSETIFF, (void *)& ifr)) < 0 ) {
        qDebug() << "ioctl(TUNSETIFF) error:  " << strerror(errno);
        close(tun_ctl_fd);
        tun_ctl_fd=-1;
    } else{
        char buf[256];
        qDebug() << "Bringed up virtual network interface " << ifr.ifr_name << inet_ntoa(int_network_addr) << "255.255.255.0";
        tun_fd= tun_ctl_fd; // Looks yes, its so
        snprintf(buf,sizeof(buf),"ip link set %s up",ifr.ifr_name);
        system(buf);
        snprintf(buf,sizeof(buf),"ip addr add %s/%s dev %s ",inet_ntoa(int_network_addr),"255.255.255.0", ifr.ifr_name );
        system(buf);
    }
  }


    m_tunSocket = tun_ctl_fd;
    */
}

/**
 * @brief DapTunLinux::onWorkerStarted
 */
void DapTunLinux::onWorkerStarted()
{


#if defined(Q_OS_ANDROID)
    if(a_socket.length()==0 ){

        QtAndroid::androidService().callMethod<void>( "stopSapVpnServiceNative"
                                               ,"()V" );
        QtAndroid::androidService().callMethod<void>( "startSapVpnServiceNative"
                                           ,"(Ljava/lang/String;Ljava/lang/String;I)V"
                                           ,QAndroidJniObject::fromString(m_tunAddr).object<jstring>()
                                           ,QAndroidJniObject::fromString(m_tunDest).object<jstring>()
                                           ,socketUpstream());
    }else{
        m_tuneSocket=a_socket.toInt();
        m_isTunCreated =true;
        listenTunnel(0);
        emit tunnelCreated();
    }
#elif defined(Q_OS_LINUX)

    qDebug() << "[SapStreamChSF] tunnelCreate()";
    QProcess process;


    //int s_ret=-1;


    if(m_listen_socket <=0){
        qCritical()<< "[SapStreamChSF] Can't bring up network interface ";
        return;
    }

    /* Save Network Configuration to file, for restore if need */
    //SapStreamChSF::me()->saveNetworkInformationToFile();

    process.start("bash", QStringList() << "-c" <<  "netstat -rn|grep 'UG '| head -n 1| awk '{print $2;}'");
    process.waitForFinished(-1);
    m_defaultGwOld=process.readAllStandardOutput();
    m_defaultGwOld.chop(1);
    if(m_defaultGwOld.isEmpty()){
        qWarning() << "[SapStreamChSF] There is no default gateway, may be we've broken that last time? Trying to check that...";
        process.start("bash",QStringList() << "-c" << QString("netstat -rn|grep %1|awk '{print $2;}'").arg(upstreamAddress())  );
        process.waitForFinished(-1);

        m_defaultGwOld=process.readAllStandardOutput();
        m_defaultGwOld.chop(1);
        if(m_defaultGwOld.isEmpty()){
            qWarning() << "[SapStreamChSF] Not found old gateway, looks like its better to restart the network";
            return;
        }

        QString run = QString("route add -host %2 gw %1")
            .arg(m_defaultGwOld).arg(upstreamAddress()).toLatin1().constData();
        ::system(run.toLatin1().constData() );
    }
    ::system("nmcli c delete DiveVPN");

    QString add = addr();
    QString gateway = gw();
    QString gwOld = m_defaultGwOld;
    QString stream = upstreamAddress();

    QString cmdConnAdd = QString(
                "nmcli connection add type tun con-name DiveVPN autoconnect false ifname %1 "
                "mode tun ip4 %2 gw4 %3")
                .arg(tunDeviceName()).arg(addr()).arg(gw())
                /*.arg(nmcliDnsPart)*/;

    qDebug() << "[SapChSf] Cmd to created interface: " <<  cmdConnAdd.toLatin1().constData();



    ::system(cmdConnAdd.toLatin1().constData());

    ::system((QString("nmcli connection modify DiveVPN"
        " +ipv4.ignore-auto-routes true")
        ).toLatin1().constData());

    ::system((QString("nmcli connection modify DiveVPN"
        " +ipv4.ignore-auto-dns true")
        ).toLatin1().constData());

    ::system((QString("nmcli connection modify DiveVPN"
        " +ipv4.dns-search divevpn")
        ).toLatin1().constData());

    ::system((QString("nmcli connection modify DiveVPN"
        " +ipv4.method manual")
        ).toLatin1().constData());

    ::system((QString("nmcli connection modify DiveVPN"
        " +ipv4.dns \"8.8.8.8 %1\"")
        .arg(gw())).toLatin1().constData());
    ::system((QString("nmcli connection modify DiveVPN"
        " +ipv4.route-metric 10")
        ).toLatin1().constData());

//    ::system((QString("nmcli connection modify " + SAP_BRAND +
//        " +ipv4.routes \"%1/32 %2 10\"").arg(SapSession::me()->address()).arg(m_defaultGwOld))
//        .toLatin1().constData());

 /*   QString run = QString("route add -host %2 gw %1 metric 10")
        .arg(m_defaultGwOld).arg(upstreamAddress()).toLatin1().constData();
    qDebug() << "[SapStreamChSF] Execute "<<run;
    ::system(run.toLatin1().constData());*/  //TODO: IVAN . NOT WORKING WITH LAN

    ::system("nmcli connection up DiveVPN");


#elif defined (Q_OS_MACOS)
    qDebug() << "[SapStreamChSF] tunnelCreate()";
    QProcess process;
    m_tuneSocket=a_socket.toInt();

    pthread_barrier_init(&m_listenTunnelThreadBarrier, NULL, 2);
    listenTunnel(0);
    qDebug() << "[SapStreamChSF] Listen tunnel to socket "<<a_socket;
    pthread_barrier_wait(&m_listenTunnelThreadBarrier);
    pthread_barrier_destroy(&m_listenTunnelThreadBarrier);
    process.start("bash",QStringList() << "-c" << "netstat -rn|grep default| head -n 1| awk '{print $2;}'");
    process.waitForFinished(-1);
    m_defaultGwOld=process.readAllStandardOutput();
    m_defaultGwOld.chop(1);
    if(m_defaultGwOld.isEmpty()){
        qWarning() << "[SapStreamChSF] There is no default gateway, may be we've broken that last time? Trying to check that...";
        process.start("bash",QStringList() << "-c" << QString("netstat -rn|grep %1|awk '{print $2;}'").arg(SapSession::me()->address())  );
        process.waitForFinished(-1);

        m_defaultGwOld=process.readAllStandardOutput();
        m_defaultGwOld.chop(1);
        if(m_defaultGwOld.isEmpty()){
            qWarning() << "[SapStreamChSF] Not found old gateway, looks like its better to restart the network";
            emit tunnelCreateError("NoGateway");
            return;
        }

        QString run = QString("route add -host %2 %1; route delete default %1")
            .arg(m_defaultGwOld).arg(SapSession::me()->address()).toLatin1().constData();
        qDebug() << "cmd run [" << run << ']';
         ::system(run.toLatin1().constData() );
        run = QString("route add default %1").arg(me()->m_tunDest);
        ::system(run.toLatin1().constData() );
    }

    QString run;
    run = QString("route add -host %2 %1")
        .arg(m_defaultGwOld).arg(SapSession::me()->address()).toLatin1().constData();
    qDebug() << "cmd run [" << run << ']';
     ::system(run.toLatin1().constData() );

    run = QString("route delete default %1")
        .arg(m_defaultGwOld);
    qDebug() << "cmd run [" << run << ']';
     ::system(run.toLatin1().constData() );

    run = QString("ifconfig %1 %2 %3 up")
                         .arg(SapStreamChSF::me()->m_tunDevice).arg(SapStreamChSF::me()->m_tunAddr)
                        .arg(me()->m_tunDest);
   qDebug() << "cmd run [" << run << ']';
     ::system(run.toLatin1().constData() );

   run = QString("route add default %1 -hopcount -1 -lock").arg(me()->m_tunDest);
   qDebug() << "cmd run [" << run << ']';
     ::system(run.toLatin1().constData() );

   ::system("cp /etc/resolv.conf /etc/resolv_backup.conf");
     //::system("echo nameserver 8.8.8.8  | sudo tee /etc/resolv.conf");
     QString cmd("echo nameserver ");
     cmd.append(m_tunDest);
     cmd.append("  | tee /etc/resolv.conf");
     ::system(cmd.toLatin1().constData());

#elif defined (Q_OS_WIN)
    qDebug() << "[SapStreamChSF] tunnelCreate()";
    QProcess process;
    m_tuneSocket=a_socket.toInt();
#ifndef Q_OS_WIN
    pthread_barrier_init(&m_listenTunnelThreadBarrier, NULL, 2);
#endif
    listenTunnel(0);
    qDebug() << "[SapStreamChSF] Listen tunnel to socket "<<a_socket;
#ifndef Q_OS_WIN
    pthread_barrier_wait(&m_listenTunnelThreadBarrier);
    pthread_barrier_destroy(&m_listenTunnelThreadBarrier);
#endif

    {   // DNS Block
         m_defaultDNSRecord = TunTap::getDefaultDNSList();
          qDebug() << "default dns records:" << m_defaultDNSRecord;
          m_defaultGwOld = TunTap::getInstance().getDefaultGateWay();
          QString char_to_wchar = TunTap::getAdapterName(TunTap::getInstance().getDefaultAdapterIndex());
          m_ethDevice = QString::fromStdString(WinRegReader::getHumanName(char_to_wchar.toStdWString()));
          char_to_wchar = TunTap::getAdapterName(TunTap::getInstance().getTunTapAdapterIndex());
          m_TunDeviceH = QString::fromStdString(WinRegReader::getHumanName(char_to_wchar.toStdWString()));
    }

    if(m_defaultGwOld.isEmpty()){
        //if (_counts < 2000) {++_counts; Sleep(10); goto protez;} else {}
        qWarning() << "[SapStreamChSF] Not found old gateway, looks like its better to restart the network";
        return;
    }
    {
        //
        std::string run = "netsh interface ipv4 delete dnsserver \"";
        run = run +m_ethDevice.toStdString();
        run = run + "\" all";
        qDebug() << "run DELETE cmd = " << run.c_str();
        ::system(run.c_str());

        run = "netsh interface ipv4 delete dnsserver \"";
        run = run +m_TunDeviceH.toStdString();
        run = run + "\" all";
        qDebug() << "run DELETE cmd = " << run.c_str();
        ::system(run.c_str());

        //run = "netsh interface ipv4 add dnsserver " +m_ethDevice+ " adress=8.8.8.8 index=1";
        run = "netsh interface ipv4 add dnsserver \"";
        run = run + m_ethDevice.toStdString();
        ///run = run + "\" address=8.8.8.8 index=1";
        run = run + "\" address="+m_tunDest.toStdString()+"index=1"; //for our dns servers
        qDebug() << "run ADD cmd = " << run.c_str();
        ::system(run.c_str());

        run = "netsh interface ipv4 add dnsserver \"";
        run = run + m_TunDeviceH.toStdString();
        ///run = run + "\" address=8.8.8.8 index=1";
        run = run + "\" address="+m_tunDest.toStdString()+"index=1"; //for our dns servers
        qDebug() << "run ADD cmd = " << run.c_str();
        ::system(run.c_str());
    }
    SapNetworkMonitorWindows::Instance().stop();
    TunTap::getInstance().makeRoute(TunTap::ETH, SapSession::me()->address(), m_defaultGwOld);
    TunTap::getInstance().defaultRouteDelete();

    QString run_cmd_head = "ROUTE ADD ";
    QString run_cmd_all;

    QString run_cmd = run_cmd_head + "0.0.0.0" + " MASK " + "0.0.0.0" + " " + m_defaultGwOld + " "  + "METRIC 25 IF " + QString::number(TunTap::getInstance().getDefaultAdapterIndex());
    qDebug() << "routeAdd external cmd = "  << run_cmd;
    //::system(run_cmd.toLatin1().constData());
    run_cmd_all = run_cmd + QString(" &");

    run_cmd = run_cmd_head + "0.0.0.0" + " MASK " + "128.0.0.0" + " " + m_tunDest + " "  + "METRIC 35 IF " + QString::number(TunTap::getInstance().getTunTapAdapterIndex());
    qDebug() << "routeAdd external cmd = "  << run_cmd;
    //::system(run_cmd.toLatin1().constData());
    run_cmd_all = run_cmd_all + run_cmd + QString(" &");

    run_cmd = run_cmd_head + "128.0.0.0" + " MASK " + "128.0.0.0" + " " + m_tunDest + " "  + "METRIC 35 IF " + QString::number(TunTap::getInstance().getTunTapAdapterIndex());
    qDebug() << "routeAdd external cmd = "  << run_cmd;
    //::system(run_cmd.toLatin1().constData());
    run_cmd_all = run_cmd_all + run_cmd + QString(" &");

    run_cmd = "ROUTE DELETE " + SapSession::me()->address();
    qDebug() << "routeAdd external cmd = "  << run_cmd;
    //::system(run_cmd.toLatin1().constData());
    run_cmd_all = run_cmd_all + run_cmd + QString(" &");

    run_cmd = run_cmd_head + SapSession::me()->address() + " MASK " + "255.255.255.255" + " " + m_defaultGwOld + " "  + "METRIC 10 IF " + QString::number(TunTap::getInstance().getDefaultAdapterIndex());
    qDebug() << "routeAdd external cmd = "  << run_cmd;
    //::system(run_cmd.toLatin1().constData());
    run_cmd_all = run_cmd_all + run_cmd;

    ::system(run_cmd_all.toLatin1().constData());
    SapNetworkMonitorWindows::Instance().start();

    ///TunTap::routeAdd(SapSession::me()->address().toStdString());
#endif


}

/**
 * @brief DapTunLinux::onWorkerStopped
 */
void DapTunLinux::tunDeviceDestroy()
{
    ::system(QString("ifconfig %1 down")
           .arg(tunDeviceName() ).toLatin1().constData() );
    ::system("nmcli connection down DiveVPN");
    ::system("nmcli connection delete DiveVPN");
    DapTunUnixAbstract::tunDeviceDestroy();
}




