QT += core network xml
QT -= gui
CONFIG += c++11 stl c11
QMAKE_CFLAGS_DEBUG = \
    -std=gnu99

QMAKE_CFLAGS_RELEASE = \
    -std=gnu99

include(../config.pri)

CONFIG += console
DEFINES += DAP_BRAND=\\\"$${BRAND}\\\"
DEFINES += DAP_VERSION=\\\"111\\\"

DEFINES += DAP_SERVICE_CONNECT_TCP

!android{
    TEMPLATE = app
    SOURCES += main.cpp
}


SOURCES +=  DapChSockForw.cpp \
    SigUnixHandler.cpp \
    DapTunAbstract.cpp\
    DapSI.cpp \
    DapTunWorkerAbstract.cpp \
    DapTunUnixAbstract.cpp \
    DapTunNative.cpp \
    DapVpnService.cpp \
    DapTunWorkerAndroid.cpp \
    DapGuiCmdController.cpp \
    DapStateMachine.cpp

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

HEADERS +=     DapChSockForw.h \
    DapVPNService.h \ 
    SigUnixHandler.h \
    DapTunAbstract.h \
    DapSI.h \
    DapTunWorkerAbstract.h \
    DapTunNative.h \
    DapSockForwPacket.h \
    DapTunUnixAbstract.h \
    DapTunWorkerAndroid.h \
    DapGuiCmdController.h \
    DapStateMachine.h

include (./DapCmdHandlers/dap-cmd-handlers.pri)
INCLUDEPATH += $$_PRO_FILE_PWD_/DapCmdHandlers/

unix: !win32 {
    HEADERS += DapTunWorkerUnix.h
    SOURCES += DapTunWorkerUnix.cpp
}

linux-* {
    include (./linux-src/linux-src.pri)
    INCLUDEPATH += $$_PRO_FILE_PWD_/linux-src/
}

win32 {
    HEADERS += DapTunWindows.h DapTunWorkerWindows.h
    SOURCES += DapTunWindows.cpp DapTunWorkerWindows.cpp
}

android{
    HEADERS += DapTunAndroid.h
    SOURCES += DapTunAndroid.cpp
}

darwin {
    HEADERS  += DapTunDarwin.h
    SOURCES += DapTunDarwin.cpp
    #INCLUDEPATH += /usr/local/opt/openssl/include
    #LIBS += -L/usr/local/opt/openssl/lib
    #LIBS += -lcrypto
}

macos {
    DEFINES += DAP_SERVICE_CONNECT_TCP
}

win32{
    QMAKE_CXXFLAGS +=  -mno-ms-bitfields
    ### for MSVC Compiler
    #INCLUDEPATH += C:\OpenSSL-Win64\include
    #LIBS += C:\OpenSSL-Win64\lib\libcrypto.lib
    #LIBS += C:\OpenSSL-Win64\lib\openssl.lib
    #LIBS += C:\OpenSSL-Win64\lib\libssl.lib

    HEADERS+= $$PWD/../../libDapClient/windows/ip.h
    HEADERS+= $$PWD/../../libDapClient/windows/registry/registry.h
    HEADERS+= $$PWD/../../libDapClient/windows/tuntap/tuntap.h

    SOURCES+= $$PWD/../../libDapClient/windows/registry/registry.cpp
    SOURCES+= $$PWD/../../libDapClient/windows/tuntap/tuntap.cpp

    ### for MinGW Compiler
    #INCLUDEPATH += C:\OpenSSL-Win32\include
    #LIBS += C:\OpenSSL-Win32\lib\libcrypto.lib
    #LIBS += C:\OpenSSL-Win32\lib\openssl.lib
    #LIBS += C:\OpenSSL-Win32\lib\libssl.lib
    ###

    LIBS += -lWS2_32
    LIBS += -lAdvapi32
    LIBS += -lIphlpapi
    CONFIG += static
    #DEFINES += VPN_TUNTAP

}

android{
    TEMPLATE = lib
    CONFIG += shared
    CONFIG -= console
    DEFINES += DAP_SERVICE_BUILD_LIB DAP_SERVICE_CONNECT_TCP
    QT += androidextras
    DEFINES += DAP_PLATFORM_MOBILE
}

ios{
    DEFINES += DAP_PLATFORM_MOBILE
}



unix: !mac : !android {
#    target.files = DapVpnService qt.conf
    target.path = /opt/$$lower($$BRAND)/bin/
    data_static.path = /opt/$$lower($$BRAND)/share
    data_static.files = $$BRAND/dists/share/*
    INSTALLS += target data_static
}


include (../libdap-qt/libdap-qt.pri)
INCLUDEPATH += $$_PRO_FILE_PWD_/../libdap-qt/

include (../libdap-qt-stream/libdap-qt-stream.pri)
INCLUDEPATH += $$_PRO_FILE_PWD_/../libdap-qt-stream/


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
