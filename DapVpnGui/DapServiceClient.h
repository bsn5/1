#ifndef DAPSERVICECLIENT_H
#define DAPSERVICECLIENT_H

#include <QObject>
#include <QTimer>

#define DAP_UI_SOCKET_TCP

#ifdef DAP_UI_SOCKET_TCP
#include <QTcpSocket>
#include <QHostAddress>
typedef QTcpSocket DapUiSocket;
typedef QTcpSocket::SocketError DapUiSocketError;
#else
#include <QLocalSocket>
typedef QLocalSocket DapUiSocket;
typedef QLocalSocket::LocalSocketError DapUiSocketError;
#endif



#ifdef Q_OS_ANDROID
#include <QAndroidJniEnvironment>
#include <QAndroidJniObject>
#include <QtAndroid>
#endif

#if defined(Q_OS_ANDROID)
#include "DapServiceNativeAndroid.h"
typedef class DapServiceNativeAndroid DapServiceNative;
#elif defined(Q_OS_LINUX)
#include "DapServiceNativeLinux.h"
typedef class DapServiceNativeLinux DapServiceNative;
#elif defined(Q_OS_MACOS)
#include "DapServiceNativeMacOS.h"
typedef class DapServiceNativeMacOS DapServiceNative;
#elif defined(Q_OS_WINDOWS)
#include "DapServiceNativeWindows.h"
typedef class DapServiceNativeWindows DapServiceNative;
#elif defined(Q_OS_IOS)
#include "DapServiceNativeIOS.h"
typedef class DapServiceNativeIOS DapServiceNative;
#endif

#include "DapCmdParser.h"

class DapServiceClient : public QObject, public DapServiceNative
{
    Q_OBJECT
public:
    const QString& serviceName(){ return m_serviceName; }
signals:
    void ctlConnected();
    void ctlDisconnected();
    void ctlError(QString);

    void error(QString);
public slots:
    void sendCmd(const QString & a_cmd);
    void init() override;

protected:

    DapServiceClient(const QString& a_serviceName);
protected slots:
    virtual void procCmdController(const QByteArray &a_cmd) = 0;
private:
    const int RECONNECT_TIMEOUT_MS = 500;
    QTimer connectTimer;
    DapUiSocket * sockCtl;
    QString m_serviceName;
    QByteArray readBuffer;
    DapCmdParser * dapCmdParser;
    bool isAuthRoot;

private slots:
    void onCtlSocketConnected();
    void connectToService();
    void onCtlSocketError(DapUiSocketError socketError);
};
#endif // DAPSERVICECLIENT_H
