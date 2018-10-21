#include <QCoreApplication>
#include "DapVPNService.h"
#include "DapLogger.h"
#include <QtDebug>

#ifdef Q_OS_UNIX
#include "SigUnixHandler.h"
#endif

#define STANDART_EXECUTABLE

#include <QFile>
#include <QTime>
#include <QDir>
#include <QFile>
#ifdef Q_OS_WIN
    #include <Windows.h>
    #include <QStringList>
    #include <QProcess>
    #include <QRegExp>
    SERVICE_STATUS serviceStatus;
    SERVICE_STATUS_HANDLE serviceStatusHandle;
    LPCWSTR serviceName = L"DiveVPNService";

    static auto getSystemPath() -> const QString &
    {
        auto env = QProcess::systemEnvironment();
        if ( auto i = env.indexOf(QRegExp("^WINDIR=.*", Qt::CaseSensitive)) > -1) {
            return env[i].split('=').last();
        }
        return *env.end();
    }
#else
#endif

#ifdef Q_OS_ANDROID
const char*const applicationName=DAP_BRAND;

#include <android/log.h>
void myMessageHandler(
  QtMsgType type,
  const QMessageLogContext& context,
  const QString& msg
) {
  QString report=msg;
  if (context.file && !QString(context.file).isEmpty()) {
    report+=" in file ";
    report+=QString(context.file);
    report+=" line ";
    report+=QString::number(context.line);
  }
  if (context.function && !QString(context.function).isEmpty()) {
    report+=+" function ";
    report+=QString(context.function);
  }
  const char*const local=report.toLocal8Bit().constData();
  switch (type) {
  case QtDebugMsg:
    __android_log_write(ANDROID_LOG_DEBUG,applicationName,local);
    break;
  case QtInfoMsg:
    __android_log_write(ANDROID_LOG_INFO,applicationName,local);
    break;
  case QtWarningMsg:
    __android_log_write(ANDROID_LOG_WARN,applicationName,local);
    break;
  case QtCriticalMsg:
    __android_log_write(ANDROID_LOG_ERROR,applicationName,local);
    break;
  case QtFatalMsg:
  default:
    __android_log_write(ANDROID_LOG_FATAL,applicationName,local);
    abort();
  }
  QFile outFile(QString("/sdcard/%1.log").arg(DAP_BRAND"Service"));
  outFile.open(QIODevice::WriteOnly | QIODevice::Append);
  QTextStream ts(&outFile);
  ts << txt << endl;
  outFile.close();
}
#endif

#ifdef STANDART_EXECUTABLE
int main (int argc, char *argv[])
{
    qputenv("QT_LOGGING_RULES", "qt.network.ssl.warning=false");

#ifdef Q_OS_UNIX
    //DapLogger d;
#endif

    QCoreApplication a(argc, argv);
//    QFile outFile(QString("/sdcard/%1.log").arg(DAP_BRAND"Service"));
//    outFile.open(QIODevice::WriteOnly | QIODevice::Append);
//    QTextStream ts(&outFile);
//    ts << "Main is running....." << endl;
//    outFile.close();
#ifdef Q_OS_ANDROID
    qInstallMessageHandler(myMessageHandler);
#endif
    qInfo() << "[Service] Starting service";
//    a.setApplicationName(DAP_BRAND);
#ifdef Q_OS_UNIX
#ifndef Q_OS_ANDROID
    SigUnixHandler::getInstance(); // Create SigUnixHandler object
#endif
#endif
    DapVPNService service;
    service.init();
    return a.exec();
}
#else
    #ifdef Q_OS_WIN
    void ControlHandler(DWORD request)
    {
        switch(request)
        {
        case SERVICE_CONTROL_STOP:
            qDebug () << "DiveVPNService Stopped.";
            serviceStatus.dwWin32ExitCode = 0;
            serviceStatus.dwCurrentState = SERVICE_STOPPED;
            SetServiceStatus (serviceStatusHandle, &serviceStatus);
            return;

        case SERVICE_CONTROL_SHUTDOWN:
            qDebug () << "DiveVPNService Shutdown.";

            serviceStatus.dwWin32ExitCode = 0;
            serviceStatus.dwCurrentState = SERVICE_STOPPED;
            SetServiceStatus (serviceStatusHandle, &serviceStatus);
            return;

        default:
            break;
        }

        SetServiceStatus (serviceStatusHandle, &serviceStatus);
        return;
    }

    void ServiceMain(int argc, char *argv[])
    {
        int error;

        serviceStatus.dwServiceType      = SERVICE_WIN32_OWN_PROCESS;
        serviceStatus.dwCurrentState     = SERVICE_START_PENDING;
        serviceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN;
        serviceStatus.dwWin32ExitCode    = 0;
        serviceStatus.dwCheckPoint       = 0;
        serviceStatus.dwWaitHint         = 0;
        serviceStatus.dwServiceSpecificExitCode = 0;

        serviceStatusHandle = RegisterServiceCtrlHandler(serviceName, (LPHANDLER_FUNCTION)ControlHandler);
        if (serviceStatusHandle == (SERVICE_STATUS_HANDLE)0) {
            return;
        }

        qDebug() << "SapVpnService Started!";
        qputenv("QT_LOGGING_RULES", "qt.network.ssl.warning=false");
        QCoreApplication a(argc, argv);
        DapVPNService service;

        error = service.init();
        if (error) {
            serviceStatus.dwCurrentState    = SERVICE_STOPPED;
            serviceStatus.dwWin32ExitCode   = -1;
            SetServiceStatus(serviceStatusHandle, &serviceStatus);
            return;
        }

        serviceStatus.dwCurrentState = SERVICE_RUNNING;
        SetServiceStatus (serviceStatusHandle, &serviceStatus);

        while (serviceStatus.dwCurrentState == SERVICE_RUNNING)
        {
            int result = a.exec();
            if (result) {
                serviceStatus.dwCurrentState    = SERVICE_STOPPED;
                serviceStatus.dwWin32ExitCode   = -1;
                SetServiceStatus(serviceStatusHandle, &serviceStatus);
                return;
            }
        }
        return;
    }

    int main(int argc, char *argv[])
    {
        argc = argc; argv=argv;
        SERVICE_TABLE_ENTRY winService[1];
        winService[0].lpServiceName = (LPWSTR)serviceName;
        winService[0].lpServiceProc = (LPSERVICE_MAIN_FUNCTION)ServiceMain;
        StartServiceCtrlDispatcher(winService);
        return 0;
    }
    #endif
#endif
