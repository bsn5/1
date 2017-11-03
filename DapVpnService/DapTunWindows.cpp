#include <QtDebug>
#include <QProcess>

#include <windows.h>
#include <iphlpapi.h>

#include "DapTunWorkerWindows.h".h"
#include "DapTunWindows.h"

DapTunWindows::DapTunWindows()

{
    qInfo() << "Native windows tun driver ctl initialized";
    tunWorker = new DapTunWorkerWindows(this);
    initWorker();
}

/**
 * @brief DapTunWindows::tunDeviceCreate
 */
void DapTunWindows::tunDeviceCreate()
{
    m_tunSocket=TunTap::getInstance().makeTunTapDevice(m_tunDeviceName);

}

void DapTunWindows::tunDeviceDestroy()
{
    QString _gw(getInstance()->m_tunDest);
    _gw.chop(1);
    _gw.append('0');
    qDebug() << "DELETE CMD IS    " << QString("ROUTE DELETE 0.0.0.0 & ROUTE DELETE 128.0.0.0 & ROUTE DELETE %6 & ROUTE DELETE %1 & ROUTE DELETE %4 & ROUTE DELETE %5 & ROUTE ADD 0.0.0.0 MASK 0.0.0.0 %2 IF %3").arg(DapSession::getInstance()->get_ip()).arg(m_defaultGwOld).arg(TunTap::getInstance().getDefaultAdapterIndex()).arg(_gw).arg(getInstance()->m_tunDest).arg(m_tunAddr).toLatin1().constData();
    ::system(QString("ROUTE DELETE 0.0.0.0 & ROUTE DELETE 128.0.0.0 & ROUTE DELETE %6 & ROUTE DELETE %1 & ROUTE DELETE %4 & ROUTE DELETE %5 & ROUTE ADD 0.0.0.0 MASK 0.0.0.0 %2 IF %3").arg(DapSession::getInstance()->get_ip()).arg(m_defaultGwOld).arg(TunTap::getInstance().getDefaultAdapterIndex()).arg(_gw).arg(getInstance()->m_tunDest).arg(m_tunAddr).toLatin1().constData());
    //qDebug() << "_______________ unroute run cmd" <<
    //  QString("ROUTE DELETE 0.0.0.0 & ROUTE DELETE 128.0.0.0 & ROUTE DELETE %6 & ROUTE DELETE %1 & ROUTE DELETE %4 & ROUTE DELETE %5 & ROUTE ADD 0.0.0.0 MASK 0.0.0.0 %2 IF %3").arg(SaSession::me()->address()).arg(m_defaultGwOld).arg(TunTap::getInstance().getDefaultAdapterIndex()).arg(_gw).arg(me()->m_tunDest).arg(m_tunAddr).toLatin1().constData();
//    {
//        //DNS block
//        std::string run = "netsh interface ipv4 delete dnsserver \"";
//        run = run +m_ethDevice.toStdString();
//        run = run + "\" all";
//        qDebug() << "run DELETE cmd = " << run.c_str();
//        ::system(run.c_str());
//        int index = 1;
//        foreach (auto &str, m_defaultDNSRecord) {
//            //run = "netsh interface ipv4 add dnsserver " +m_ethDevice+ " adress="+str+" index="+QString::number(index++);
//            run = "netsh interface ipv4 add dnsserver \"";
//            run = run + m_ethDevice.toStdString();
//            run = run + "\" address="; run = run+str.toStdString();
//            run = run + " index=";
//            run = run + std::to_string(index++);
//            qDebug() << "run ADD cmd = " << run.c_str();
//            ::system(run.c_str());

//        }
//    }
    TunTap::getInstance().doCloseTun();
}

void DapTunWindows::onWorkerStarted(){
    DapTunAbstract::onWorkerStarted();
    QProcess process;

    {   // DNS Block
        m_defaultDNSRecord = TunTap::getDefaultDNSList();
        qDebug() << "[DapChSockForw] default dns records:" << m_defaultDNSRecord;
        m_defaultGwOld = TunTap::getInstance().getDefaultGateWay();
        QString char_to_wchar = TunTap::getAdapterName(TunTap::getInstance().getDefaultAdapterIndex());
        std::wstring wstr = char_to_wchar.toStdWString();
        m_ethDevice = QString::fromStdString(WinRegReader::getHumanName(wstr));
        QString tunName = TunTap::getAdapterName(TunTap::getInstance().getTunTapAdapterIndex());
        wstr = tunName.toStdWString();
        m_TunDeviceH = QString::fromStdString(WinRegReader::getHumanName(wstr));

        QString dns = "8.8.8.8";
        WinRegWriter::getInstance()->setDNS(tunName, dns);
    }

    if (m_defaultGwOld.isEmpty()) {
        qWarning() << "[DapChSockForw] Not found old gateway, looks like its better to restart the network";
        return;
    }

    //qDebug() << "need call == " << QString("ifconfig %1 %2 %3 up")
    //            .arg(DapChSockForw::getInstance()->m_tunDevice).arg(DapChSockForw::getInstance()->m_tunAddr)
    //           .arg(getInstance()->m_tunDest);

    s_ret = TunTap::getInstance().setAdress(addr(),gw(), QString("255.255.240.0"));

//    {
//        std::string run = "netsh interface ipv4 delete dnsserver \"";
//        run = run +m_TunDeviceH.toStdString();
//        run = run + "\" all";
//        qDebug() << "run DELETE cmd = " << run.c_str();
//        ::system(run.c_str());

//        run = "netsh interface ipv4 add dnsserver \"";
//        run = run + m_TunDeviceH.toStdString();
//        run = run + "\" address=8.8.8.8";
//        qDebug() << "run ADD cmd = " << run.c_str();
//        ::system(run.c_str());
//    }

    TunTap::getInstance().makeRoute(TunTap::ETH, DapSession::getInstance()->get_ip(), m_defaultGwOld);
    TunTap::getInstance().defaultRouteDelete();

    QString run_cmd_head = "ROUTE ADD ";
    QString run_cmd_all;

    QString run_cmd = run_cmd_head + "0.0.0.0" + " MASK " + "0.0.0.0" + " " + m_defaultGwOld + " "  + "METRIC 75 IF " + QString::number(TunTap::getInstance().getDefaultAdapterIndex());
    qDebug() << "routeAdd external cmd = "  << run_cmd;
    run_cmd_all = run_cmd + QString(" &");

    run_cmd = run_cmd_head + "0.0.0.0" + " MASK " + "128.0.0.0" + " " + m_tunDest + " "  + "METRIC 10 IF " + QString::number(TunTap::getInstance().getTunTapAdapterIndex());
    qDebug() << "routeAdd external cmd = "  << run_cmd;
    run_cmd_all = run_cmd_all + run_cmd + QString(" &");

    run_cmd = run_cmd_head + "128.0.0.0" + " MASK " + "128.0.0.0" + " " + m_tunDest + " "  + "METRIC 10 IF " + QString::number(TunTap::getInstance().getTunTapAdapterIndex());
    qDebug() << "routeAdd external cmd = "  << run_cmd;
    run_cmd_all = run_cmd_all + run_cmd + QString(" &");

    run_cmd = "ROUTE DELETE " + DapSession::getInstance()->get_ip();
    qDebug() << "routeAdd external cmd = "  << run_cmd;
    run_cmd_all = run_cmd_all + run_cmd + QString(" &");

    run_cmd = run_cmd_head + DapSession::getInstance()->get_ip() + " MASK " + "255.255.255.255" + " " + m_defaultGwOld + " "
            + "METRIC 10 IF " + QString::number(TunTap::getInstance().getDefaultAdapterIndex());
    qDebug() << "routeAdd external cmd = "  << run_cmd;
    run_cmd_all = run_cmd_all + run_cmd;

    ::system(run_cmd_all.toLatin1().constData());
}





