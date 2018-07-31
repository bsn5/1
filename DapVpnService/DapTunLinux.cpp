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

    emit created();
}


QString DapTunLinux::currentConnectionInterface() {
    QProcess process;

    // https://askubuntu.com/questions/787547/nmcli-how-to-get-the-last-used-connection
    process.start("bash", QStringList() << "-c" <<  "nmcli -t -f NAME,TIMESTAMP con show | sort -t: -nk2 | tail -n1 | cut -d: -f1");
    process.waitForFinished(-1);

    QString result = process.readAllStandardOutput();
    if(result.isEmpty()) {
        qWarning() << "Can't get current connection interface name!";

    }
    result.chop(1); // delete \n symbol
    return result;
}

/**
 * @brief DapTunLinux::onWorkerStarted
 */
void DapTunLinux::onWorkerStarted()
{
    qDebug() << "[SapStreamChSF] tunnelCreate()";
    QProcess process;

    if(m_tunSocket <=0){
        qCritical()<< "[SapStreamChSF] Can't bring up network interface ";
        return;
    }

    m_lastUsedConnectionName = currentConnectionInterface();

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

    if(!isLocalAddress(upstreamAddress()))
    {
        QString upstream = upstreamAddress();
        // This route dont need if address is local
        QString run = QString("route add -host %2 gw %1 metric 10")
            .arg(m_defaultGwOld).arg(upstreamAddress()).toLatin1().constData();
        qDebug() << "[SapStreamChSF] Execute "<<run;
        ::system(run.toLatin1().constData());
    }

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

    ::system("nmcli c delete DiveVPN");

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

    ::system("nmcli connection up DiveVPN");
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
    ::system(QString("nmcli connection up %1")
             .arg(m_lastUsedConnectionName).toLatin1().constData());
    DapTunUnixAbstract::tunDeviceDestroy();
}




