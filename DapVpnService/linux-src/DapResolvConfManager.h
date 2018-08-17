#ifndef DAP_RESOLV_CONF_H
#define DAP_RESOLV_CONF_H

#include <QStringList>
#include <QFile>
#include <QObject>
#include <QDebug>

class ResolvConfManager : public QObject {
    Q_OBJECT
public:
    static const char* resolvConfHeader;
    static const QString resolvConfPath;

    ResolvConfManager(QStringList dns, QObject* obj = nullptr);

    /* Saves current in m_userResolvConfBuf
     * and fill /etc/resolv.conf our parameters */
    void rewriteResolvConf();

    /* Writes in /etc/resolv.conf from  m_userResolvConfBuf */
    void restoreResolvConf();

private:
    QStringList m_dns;

    /* /etc/resolv.conf file before upping DiveVpn Interface
       using for restoring after down DiveVpn interface */
    QByteArray m_userResolvConfBuf;

    QFile m_resolvConf;
private slots:
    void destroy();
};

#endif
