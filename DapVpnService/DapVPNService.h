#ifndef DAPVPNSERVICE_H
#define DAPVPNSERVICE_H

#include <QObject>
#include <QList>
#include <QTime>
#include <QDateTime>
#include <QVector>
#include <QStateMachine>
#include <QState>
#include <utility>

#include "DapSI.h"
#include "DapCmdParser.h"

#ifdef DAP_SERVICE_CONNECT_TCP
#include <QTcpServer>
#include <QTcpSocket>
#include "DapJsonCmd.h"
#include "DapGuiCmdController.h"


typedef class QTcpSocket DapUiSocket;
typedef QTcpSocket::SocketError DapUiSocketError;
typedef class QTcpServer DapUiSocketServer;
#else
#include <QLocalServer>
#include <QLocalSocket>
typedef class QLocalSocket DapUiSocket;
typedef QLocalSocket::LocalSocketError DapUiSocketError;
typedef class QLocalServer DapUiSocketServer;
#endif

#ifdef DAP_SERVICE_CONNECT_TCP
class QTcpSocket;
class QTcpServer;
#else
class QLocalSocket;
class QLocalServer;
#endif



class QNetworkAccessManager;
class DapStreamer;
class DapChSockForw;
class QTimer;

class DapVPNService : public QObject
{
    Q_OBJECT
private:
    DapStreamer * streamer;
    DapChSockForw * chSockForw;
#ifdef DAP_SERVICE_CONNECT_TCP
    QTcpServer * srvLocal;
    QList<QTcpSocket *> client;
#else
    QLocalServer * srvLocal;
    QList<QLocalSocket *> client;
#endif
    QTimer * tmrStat;
   // QVector<BadServers> m_nativeServersInfo;

    bool isLogined;
    bool isIfUp;
    bool isRouted;

    void logout();
    void stream_open();
    void route_all();
    void if_create();

    bool isLogouting;

    int m_restoreTriesLeft;

    QString m_curUser, m_curPassword, m_curAddrLine, m_curAddr;
    QString m_cookie;
    QString m_tunAddr;
    QString m_tunDest;

    quint64 m_tunReadBytes;
    quint64 m_tunWriteBytes;
    quint64 m_tunReadPackets;
    quint64 m_tunWritePackets;

    std::map <QString, QString> server_list;
    std::map <QString, QString> bad_server_list;
    bool server_is_bad (QString addr_line);
    QDateTime dtStreamOpened;

public:
    explicit DapVPNService(QObject *parent = Q_NULLPTR);
    virtual ~DapVPNService();
protected:
    QStateMachine sm;

    QState * statesRequest;
    QState * stateRequestDisconnected;
    QState * stateRequestConnectedFirst;
    QState * stateRequestConnectedAlways;
    QState * stateRequestConnected;

    DapSI *siAuthorization;
    DapSI *siStream;
    DapSI *siNetConfig;
    DapSI *siTunnel;

    QList<DapSI*> siList;

    QNetworkAccessManager * nam;
    DapGuiCmdController * m_guiHandler;
signals:
    void sigRequestDisconnected();
    void sigRequestConnected();
public slots:
    int init();
    void destroy();

    void sendDapCmdAll(const QByteArray& cmd);

    void checkInstallation();

    void onIndicatorStateTrue();
    void onIndicatorStateFalse();

private slots:
    void procCmdController(const QByteArray & a_cmd);
    void onEncInitialized();
    void onReadBytes(int arg);
    void onWriteBytes(int arg);
    void onSigPacketRead();
    void onSigPacketWrite();
};

#endif
