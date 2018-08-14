#include "MainWindow.h"
#include "DapUiScreen.h"
#include "DapLogger.h"
#include <QApplication>
#include <QDir>
#include <QFile>
#include <QTime>
#include <QtDebug>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
#ifdef Q_OS_UNIX
    DapLogger d;
#endif

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
