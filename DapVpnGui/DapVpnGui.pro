#-------------------------------------------------
#
# Project created by QtCreator 2017-06-24T09:30:57
#
#-------------------------------------------------

QT       += gui network widgets xml widgets core

CONFIG += c++11

TEMPLATE = app


SOURCES += main.cpp\
    datalocal.cpp \
    ScreenLogin.cpp \
    ScreenDashboard.cpp \
    schedule.cpp \
    pair.cpp \
    schedules.cpp \
    usrmsg.cpp \
    ScreenVpnAbstract.cpp \
    DapUiScreen.cpp \
    DapUiDialog.cpp \
    ServiceCtl.cpp \
    DapServiceClient.cpp \
    DapServiceNativeAbstract.cpp \
    MainWindow.cpp \
    DapUiVpnStateIndicator.cpp \
    DapUiMainWindow.cpp \
    DapServerInfo.cpp

HEADERS  += \
    datalocal.h \
    schedule.h \
    DapUiScreen.h \
    ScreenLogin.h \
    ScreenDashboard.h \
    pair.h \
    schedules.h \
    usrmsg.h \
    ScreenVpnAbstract.h \
    DapUiDialog.h \
    ServiceCtl.h \
    DapServiceClient.h \
    DapServiceNativeAbstract.h \
    MainWindow.h \
    DapUiVpnStateIndicator.h \
    DapUiMainWindow.h \
    DapServerInfo.h

FORMS    +=  \
             ui/dashboard_desktop_big.ui \
             ui/dashboard_desktop_small.ui \
             ui/dashboard_desktop.ui \
             ui/dashboard_mobile_hor_big.ui \
             ui/dashboard_mobile_hor_small.ui \
             ui/dashboard_mobile_hor.ui \
             ui/dashboard_mobile_ver_big.ui \
             ui/dashboard_mobile_ver_small.ui \
             ui/dashboard_mobile_ver.ui \
             ui/login_desktop_big.ui \
             ui/login_desktop_small.ui \
             ui/login_desktop.ui \
             ui/login_mobile_hor_big.ui \
             ui/login_mobile_hor_small.ui \
             ui/login_mobile_hor.ui \
             ui/login_mobile_ver_big.ui \
             ui/login_mobile_ver_small.ui \
             ui/login_mobile_ver.ui \
    ui/dlg_settings.ui \
    ui/dlg_about.ui



!defined(BRAND, var)
{
    BRAND = DiveVPN
    #BRAND = KelvinVPN
}

DEFINES += DAP_BRAND=\\\"$$BRAND\\\"
DEFINES += DAP_VERSION=\\\"111\\\"


RESOURCES += resources/common/common.qrc

defined(BRAND, var)
{
#    message("Defined brand $$BRAND")
    RESOURCES += resources/$$BRAND/main.qrc
}

defined(BRAND_TARGET,var){
    TARGET = $$BRAND_TARGET
#    message("Defined brand target $$BRAND_TARGET")
}
win32{
    QMAKE_CXXFLAGS +=  -mno-ms-bitfields
}

android {
DISTFILES += \
    $$PWD/android/AndroidManifest.xml \
    $$PWD/android/gradle/wrapper/gradle-wrapper.jar \
    $$PWD/android/gradlew \
    $$PWD/android/res/values/libs.xml \
    $$PWD/android/res/drawable-hdpi/divevpn.png \
    $$PWD/android/res/drawable-ldpi/divevpn.png \
    $$PWD/android/res/drawable-mdpi/divevpn.png \
    $$PWD/android/build.gradle \
    $$PWD/android/gradle/wrapper/gradle-wrapper.properties \
    $$PWD/android/src/com/demlabs/dap/DapVpnMainActivity.java \
    $$PWD/android/src/com/demlabs/dap/DapVpnService.java \
    $$PWD/android/src/com/demlabs/dap/DapVpnServiceNative.java
    $$PWD/android/gradlew.bat

    ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android
    QT += androidextras
    #ANDROID_ETRA_LIBS+= $$PWD/../../libDapClient/android/arm/lib/libssl.so

 #   CONFIG += link_prl
#    ANDROID_EXTRA_LIBS = $$PWD/../../libDapClient/android/arm/lib/libssl.so $$PWD/../libDapClient/android/arm/lib/libcrypto.so
}


android{
    DEFINES += DAP_PLATFORM_MOBILE DAP_SERVICE_CONNECT_TCP
    DEFINES += DAP_PLATFORM=\\\"mobile\\\"
    QT +=  androidextras
    SOURCES += DapServiceNativeAndroid.cpp
    HEADERS += DapServiceNativeAndroid.h
}

ios{
    DEFINES += DAP_PLATFORM_MOBILE
    DEFINES += DAP_PLATFORM=\\\"mobile\\\"
    SOURCES += DapServiceNativeIOS.cpp
    HEADERS += DapServiceNativeIOS.h
}

linux-* {
        DEFINES += DAP_PLATFORM=\\\"desktop\\\"
        DEFINES += DAP_PLATFORM_DESKTOP
        SOURCES += DapServiceNativeLinux.cpp
        HEADERS += DapServiceNativeLinux.h
}
win32 {
        DEFINES += DAP_PLATFORM=\\\"desktop\\\"
        DEFINES += DAP_PLATFORM_DESKTOP
        SOURCES += DapServiceNativeWindows.cpp
        HEADERS += DapServiceNativeWindows.h
}
macos {
        DEFINES += DAP_PLATFORM=\\\"desktop\\\"
        DEFINES += DAP_PLATFORM_DESKTOP DAP_SERVICE_CONNECT_TCP
        SOURCES += DapServiceNativeMacOS.cpp
        HEADERS += DapServiceNativeMacOS.h

}


unix: !mac : !android {
    gui_target.files = DiveVPN
    gui_target.path = /opt/divevpn/bin/
    gui_data_static.path = /opt/divevpn/share
    gui_data_static.files = dists/share/*


    INSTALLS += gui_target gui_data_static
}
