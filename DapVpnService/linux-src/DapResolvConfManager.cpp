#include "DapResolvConfManager.h"
#include "SigUnixHandler.h"
#include <QCoreApplication>

const char* ResolvConfManager::resolvConfHeader = "#Dap service resolver\n";
const QString ResolvConfManager::resolvConfPath = "/etc/resolv.conf";

ResolvConfManager::ResolvConfManager(QStringList dns, QObject* obj)
    : QObject (obj)
{
    m_dns = dns;
    m_resolvConf.setFileName(resolvConfPath);
    connect(SigUnixHandler::getInstance(), &SigUnixHandler::sigKill,
            this, &ResolvConfManager::destroy, Qt::DirectConnection);
}

void ResolvConfManager::destroy() {
    if(!m_userResolvConfBuf.isEmpty() &&
            m_resolvConf.open(QFile::WriteOnly)) {
        qInfo() << "Restore reslov conf file";
        m_resolvConf.write(m_userResolvConfBuf);
        m_resolvConf.close();
    }
}

void ResolvConfManager::rewriteResolvConf() {

    if(!m_userResolvConfBuf.isEmpty()) {
        qCritical() << "Something wrong. Buffer is not empty!";
        return;
    }
    if(!m_resolvConf.open(QFile::ReadWrite)) {
        qCritical() << "Something wrong. Can't open resolv File!";
        return;
    }

    m_userResolvConfBuf = m_resolvConf.readAll();
    m_resolvConf.resize(0); // clear file
    m_resolvConf.write(resolvConfHeader, qint64(strlen(resolvConfHeader)));

    for(const auto& dns: m_dns) {
        m_resolvConf.write(
                        QString("nameserver %1\n").arg(dns).toLatin1()
                    );
    }

    m_resolvConf.close();
}

void ResolvConfManager::restoreResolvConf() {

    if(m_userResolvConfBuf.isEmpty()) {
        qCritical() << "Something wrong. Buffer is empty!";
        return;
    }
    if(!m_resolvConf.open(QFile::WriteOnly)) {
        qCritical() << "Something wrong. Can't open resolv File!";
        return;
    }

    m_resolvConf.write(m_userResolvConfBuf);
    m_resolvConf.close();
    m_userResolvConfBuf.clear();
}
