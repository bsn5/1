#-------------------------------------------------
#
# Project created by QtCreator 2017-06-24T09:30:57
#
#-------------------------------------------------

QT       += gui network widgets xml widgets core

CONFIG += c++14

TEMPLATE = app

QMAKE_CFLAGS_DEBUG = \
    -std=gnu99

QMAKE_CFLAGS_RELEASE = \
    -std=gnu99

include(../config.pri)
TARGET = $${BRAND}

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


include (./DapCmdHandlers/dap-cmd-handlers.pri)
INCLUDEPATH += $$_PRO_FILE_PWD_/DapCmdHandlers/

include (../libdap-qt/libdap-qt.pri)
INCLUDEPATH += $$_PRO_FILE_PWD_/../libdap-qt/

DEFINES += DAP_BRAND=\\\"$$BRAND\\\"
DEFINES += DAP_VERSION=\\\"111\\\"


RESOURCES += resources/common/common.qrc
RESOURCES += resources/$$BRAND/main.qrc

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
    $$PWD/android/res/drawable-ldpi/divevpn.png \ # ATTENTION! Need change to dynamically linking for different brands
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
    gui_target.files = $$BRAND
    gui_target.path = /opt/$$lower($$BRAND)/bin/
    gui_data_static.path = /opt/$$lower($$BRAND)/share
    gui_data_static.files = resources/$$BRAND/dists/share/*
    INSTALLS += gui_target gui_data_static
}

### Link DapVpnCommon ###
win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../DapVpnCommon/release/ -lDapVpnCommon
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../DapVpnCommon/debug/ -lDapVpnCommon
else:unix: LIBS += -L$$OUT_PWD/../DapVpnCommon/ -lDapVpnCommon

INCLUDEPATH += $$PWD/../DapVpnCommon
DEPENDPATH += $$PWD/../DapVpnCommon

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../DapVpnCommon/release/libDapVpnCommon.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../DapVpnCommon/debug/libDapVpnCommon.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../DapVpnCommon/release/DapVpnCommon.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../DapVpnCommon/debug/DapVpnCommon.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../DapVpnCommon/libDapVpnCommon.a
### Link DapVpnCommon ###

contains(ANDROID_TARGET_ARCH,armeabi-v7a) {
    ANDROID_EXTRA_LIBS = \
        $$PWD/../../build-DapVpn-Android_for_armeabi_v7a_GCC_4_9_Qt_5_9_1_android_armv7-Debug/DapVpnService/libDapVpnService.so
}
