#include <QtDebug>

#include <QAndroidJniObject>

#include <QtAndroid>

#include "DapServiceNativeAndroid.h"

DapServiceNativeAndroid::DapServiceNativeAndroid()
{

}

void DapServiceNativeAndroid::restartService()
{

    qWarning() << "[DapServiceNativeAndroid] restartService()";
    // Calling Service
    QAndroidJniObject::callStaticMethod<void>("com/demlabs/dap/DapVpnService",
                                              "restartService",
                                              "(Landroid/content/Context;)V",
                                              QtAndroid::androidActivity().object());

}

void DapServiceNativeAndroid::checkInstallation()
{
    //qWarning() << "[DapServiceNativeAndroid] checkInstallation() not implemented";
    //restartService();
    bool isConnectable = QAndroidJniObject::callStaticMethod<jboolean>("com/demlabs/dap/DapVpnService",
                                                                    "isServiceConnectable",
                                                                    "()Z");

    if (isConnectable)
        qInfo() << "Service is connectable.";
    else
        qCritical() << "Service is not connectable!";

    bool isServiceRunning = QAndroidJniObject::callStaticMethod<jboolean>("com/demlabs/dap/DapVpnService",
                                                                          "checkServiceRunning",
                                                                          "(Landroid/content/Context;)Z",
                                                                          QtAndroid::androidActivity().object());
    qWarning() << "[SapServiceNativeAndroid] checkIsServiceRunning() result: " << isServiceRunning;

    if (!isServiceRunning) {
        qInfo() << "Starting service...";
        restartService();
//        static int once = 0;
//        if (!once)
//            checkInstallation();
//        once++;
    }
}
