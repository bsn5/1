#include "MainWindow.h"
#include "DapUiScreen.h"
#include <QApplication>
#include <QDir>
#include <QFile>
#include <QTime>
#include <QtDebug>


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

#ifdef DAP_PLATFORM_MOBILE
    QFile outFile(QString("%1-%2.log").arg(DAP_BRAND).arg(QTime::currentTime().toString("dd-MM-yy") ));
#endif

#ifdef Q_OS_LINUX
#ifndef DAP_PLATFORM_MOBILE
    ::system("test -d ~/.divevpn/log || mkdir -p ~/.divevpn/log ");
    QFile outFile(QString("~/.divevpn/log/%1-%2.log").arg(DAP_BRAND).arg(QTime::currentTime().toString("dd-MM-yy") ));
#endif
#endif

    outFile.open(QIODevice::WriteOnly | QIODevice::Append);
    QTextStream ts(&outFile);
    ts << txt << endl;
#ifdef STANDART_EXECUTABLE
    ::fprintf(stdout,"%s\n",txt.toLatin1().constData());
    ::fflush(stdout);
#endif
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    qDebug() << "[main.cpp] QApplication initialized";
    a.setOrganizationName("demlabs");
    a.setOrganizationDomain("demlabs.net");
    a.setApplicationName(DAP_BRAND);
    a.setApplicationVersion(DAP_VERSION);

    // Setup style
    QFile appStyleF;

    DapUiScreen::ScreenSize screenSize=DapUiScreen::getScreenSize();

    if(screenSize == DapUiScreen::Big)
        appStyleF.setFileName(QString(":/main_%1_big.css").arg(DAP_PLATFORM));
    else if( screenSize == DapUiScreen::Small)
        appStyleF.setFileName(QString(":/main_%1_small.css").arg(DAP_PLATFORM));
    else
        appStyleF.setFileName(QString(":/main_%1.css").arg(DAP_PLATFORM) );

    appStyleF.open(QFile::ReadOnly); // Load style
    QString appStyleS = appStyleF.readAll();

    a.setStyleSheet(appStyleS); //set up as default


    MainWindow w;
    w.show();

    return a.exec();
}
