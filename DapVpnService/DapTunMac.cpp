#include <QtDebug>
#include <QProcess>
#include <QFile>
#include <QSettings>

#include <stdio.h>
#include <unistd.h>

#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/sys_domain.h>
#include <sys/ioctl.h> // ioctl
#include <sys/kern_control.h> // struct socketaddr_ctl
#include <net/if_utun.h> // UTUN_CONTROL_NAME

#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include "DapTunMac.h"
#include "DapTunWorkerMac.h"

/**
 * @brief DapTunMac::DapTunMac
 */
DapTunMac::DapTunMac()
{
    qDebug() << "[DapTunMacAbstract::DapTunMacAbstract]";
    tunWorker = new DapTunWorkerMac(this);
    initWorker();
}

/**
 * @brief DapTunMac::tunDeviceCreate
 */
void DapTunMac::tunDeviceCreate()
{
    qDebug() << "[DapTunMac::tunDeviceCreate]";
    setTunDeviceName("tun4");

    m_tunSocket = ::open (("/dev/"+m_tunDeviceName).toLatin1().constData(),O_RDWR);

    if(m_tunSocket <= 0){
        qCritical() << "[DapTunMac] tunDeviceCreate() can't open "<< m_tunDeviceName;
        return;
    }

    if (::fcntl(m_tunSocket, F_SETFL, O_NONBLOCK) < 0) {    ;
        qCritical() << "[DapTunMac] tunDeviceCreate() can't switch socket to non-block mode";
        return;
    }

    if (::fcntl(m_tunSocket, F_SETFD, FD_CLOEXEC) < 0){    ;
        qCritical() << "[DapTunMac] tunDeviceCreate() can't switch socket to not be passed across execs";
        return;
    }
}

/**
 * @brief DapTunMac::onWorkerStarted
 */
void DapTunMac::onWorkerStarted()
{
    QProcess process;
    process.start("bash",QStringList() << "-c" << "netstat -rn|grep default| head -n 1| awk '{print $2;}'");
    process.waitForFinished(-1);
    m_defaultGwOld=process.readAllStandardOutput();

    m_defaultGwOld.chop(1);

    if(m_defaultGwOld.isEmpty()){
        qWarning() << "[DapTunMac] There is no default gateway, may be we've broken that last time? Trying to check that...";
        process.start("bash",QStringList() << "-c" << QString("netstat -rn|grep %1|awk '{print $2;}'").arg(upstreamAddress())  );
        process.waitForFinished(-1);

        m_defaultGwOld=process.readAllStandardOutput();
        m_defaultGwOld.chop(1);
        if(m_defaultGwOld.isEmpty()){
            qWarning() << "[DapTunMac] Not found old gateway, looks like its better to restart the network";
            return;
        }

        QString run = QString("route add -host %2 %1; route delete default %1")
            .arg(m_defaultGwOld).arg(upstreamAddress()).toLatin1().constData();
        qDebug() << "cmd run [" << run << ']';
         ::system(run.toLatin1().constData() );
        run = QString("route add default %1").arg(gw());
        ::system(run.toLatin1().constData() );
    }

    QString run;
    run = QString("route add -host %2 %1")
        .arg(m_defaultGwOld).arg(upstreamAddress()).toLatin1().constData();
    qDebug() << "cmd run [" << run << ']';
     ::system(run.toLatin1().constData() );

    run = QString("route delete default %1")
        .arg(m_defaultGwOld);
    qDebug() << "cmd run [" << run << ']';
     ::system(run.toLatin1().constData() );

    run = QString("ifconfig %1 %2 %3 up")
                         .arg(tunDeviceName()).arg(addr())
                        .arg(gw());
   qDebug() << "cmd run [" << run << "]" << m_addr << "],[" << m_gw << "],[" << m_upstreamAddress << "]";

   ::system(run.toLatin1().constData() );

   run = QString("route add default %1 -hopcount -1 -lock").arg(gw());
   qDebug() << "cmd run [" << run << ']';
     ::system(run.toLatin1().constData() );

   ::system("cp /etc/resolv.conf /etc/resolv_backup.conf");
     //::system("echo nameserver 8.8.8.8  | sudo tee /etc/resolv.conf");
     QString cmd("echo nameserver ");
     cmd.append(gw());
     cmd.append("  | tee /etc/resolv.conf");
     ::system(cmd.toLatin1().constData());

    qInfo() << "Created "<<m_tunDeviceName<<" network interface";
/* this is the special file descriptor that the caller will use to talk
* with the virtual interface */
    emit created();
}

/**
 * @brief DapTunMac::tunDeviceDestroy
 */
void DapTunMac::tunDeviceDestroy()
{
    qDebug() << "[DapTunMac::tunDeviceDestroy] Start";

    ::system(QString("route delete -host %2 %1; route delete default %3; route add default %1")
            .arg(m_defaultGwOld).arg(m_addr).arg(m_gw).toLatin1().constData()  );
    ::system(QString("ifconfig %1 down")
             .arg(tunDeviceName()).toLatin1().constData() );

    ::close(m_tunSocket);
    qDebug() << "[DapTunMac::tunDeviceDestroy] Closed tun socket";

    qInfo() << "Close tun device (and usualy destroy that after)";
    m_tunSocket = -1;
    emit destroyed();
}

DapTunMac::~DapTunMac() {
}

