#include <QCoreApplication>
#include "DapVPNService.h"
#include <QtDebug>
#include "staticdata.h"

#ifdef Q_OS_UNIX
#include "SigUnixHandler.h"
#endif

#define STANDART_EXECUTABLE

#include <QFile>
#include <QTime>
#include <QDir>
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

void myMessageHandler(QtMsgType type, const QMessageLogContext &ctx, const QString & msg)
{
    QString txt;
    QString ctxStr;

    QString mName= ctx.file;
    mName=mName.split('/').last();
    mName.remove(".cpp").remove(".h").remove(".");//cleaning up someshit


    QString fName= ctx.function;
    QStringList fNameSplitted = fName.split(' ');
    if(fNameSplitted.length()>1){
        if(fNameSplitted.first()=="auto")
            fName=QString("lambda_%1").arg(ctx.line);
        else{
            fName = fNameSplitted.at(1);
            fName=fName.split('(').first();
            fName+="()";
        }
    }else
        fName = "";

    if((fName.size()==0)&&(mName.size()==0))
        ctxStr="Qt";
    else
        ctxStr=QString("%1::%2").arg(mName).arg(fName);
    const QString LOG_TIME_FORMAT="dd.MM.yy hh:mm:ss.zzz";

    switch (type) {
    case QtDebugMsg:
        txt = QString("%2 [%3][Debug] %1").arg(msg).arg(QTime::currentTime().toString(LOG_TIME_FORMAT)).arg(ctxStr);
        break;
    case QtWarningMsg:
        txt = QString("%2 [%3][Warning] %1").arg(msg).arg(QTime::currentTime().toString(LOG_TIME_FORMAT)).arg(ctxStr);
    break;
    case QtInfoMsg:
        txt = QString("%2 [%3][Info] %1").arg(msg).arg(QTime::currentTime().toString(LOG_TIME_FORMAT)).arg(ctxStr);
    break;
    case QtCriticalMsg:
        txt = QString("%2 [%3][Critical] %1").arg(msg).arg(QTime::currentTime().toString(LOG_TIME_FORMAT)).arg(ctxStr);
    break;
    case QtFatalMsg:
        txt = QString("%2 [%3][Fatal] %1").arg(msg).arg(QTime::currentTime().toString(LOG_TIME_FORMAT)).arg(ctxStr);
    break;
    }
#ifdef Q_OS_MACOS
    QFile outFile(QString("/tmp/%1.log").arg(DAP_BRAND));
#endif
#ifdef Q_OS_WIN
    QFile outFile(QString((getSystemPath()+QDir::separator()+"%DiveVPNService.log")));
#endif
#ifdef Q_OS_LINUX
    ::system("test -d /opt/divevpn/log || mkdir -p /opt/divevpn/log ");
    QFile outFile(QString("/opt/divevpn/log/%1-%2.log").arg("UnswService").arg(QTime::currentTime().toString("dd-MM-yy") ));
#endif

    outFile.open(QIODevice::WriteOnly | QIODevice::Append);
    QTextStream ts(&outFile);
    ts << txt << endl;
#ifdef STANDART_EXECUTABLE
    ::fprintf(stdout,"%s\n",txt.toLatin1().constData());
    ::fflush(stdout);
#endif
}


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
}
#endif

#ifdef STANDART_EXECUTABLE
Q_DECL_EXPORT int main (int argc, char *argv[])
{
    qDebug() << "DapService Started!";
    qputenv("QT_LOGGING_RULES", "qt.network.ssl.warning=false");
    QCoreApplication a(argc, argv);
#ifdef Q_OS_ANDROID
//    qInstallMessageHandler(myMessageHandler);
#endif
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
