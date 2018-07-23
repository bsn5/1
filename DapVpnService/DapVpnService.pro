QT += core network xml
QT -= gui
CONFIG += c++11 stl

TARGET = DapVpnService
CONFIG += console
DEFINES += DAP_BRAND=\\\"DiveVPN\\\"
DEFINES += DAP_VERSION=\\\"111\\\"

DEFINES += DAP_SERVICE_CONNECT_TCP

!android{
    TEMPLATE = app
    SOURCES += main.cpp
}

SOURCES +=  DapChSockForw.cpp \
    SigUnixHandler.cpp \
    CommandBuffer.cpp \
    DapTunAbstract.cpp\
    DapSI.cpp \
    DapTunWorkerAbstract.cpp \
    DapTunUnixAbstract.cpp \
    DapTunNative.cpp \
    DapVpnService.cpp \
    DapSB.cpp \
    DapTunWorkerAndroid.cpp



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
    CommandBuffer.h \
    DapTunAbstract.h \
    DapSI.h \
    DapTunWorkerAbstract.h \
    DapTunNative.h \
    DapSockForwPacket.h \
    DapTunUnixAbstract.h \
    DapSB.h \
    DapTunWorkerAndroid.h

unix: !win32 {
    HEADERS += DapTunWorkerUnix.h
    SOURCES += DapTunWorkerUnix.cpp
}

linux-* {
    HEADERS += DapTunLinux.h
    SOURCES += DapTunLinux.cpp
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
    INCLUDEPATH += /usr/local/opt/openssl/include
    LIBS += -L/usr/local/opt/openssl/lib
    LIBS += -lcrypto
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
    INCLUDEPATH += C:\OpenSSL-Win32\include
    LIBS += C:\OpenSSL-Win32\lib\libcrypto.lib
    LIBS += C:\OpenSSL-Win32\lib\openssl.lib
    LIBS += C:\OpenSSL-Win32\lib\libssl.lib
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
    target.path = /opt/divevpn/bin/
    data_static.path = /opt/divevpn/share
    data_static.files = dists/share/*
    INSTALLS += target data_static
}


include (../libdap-qt/libdap-qt.pri)
INCLUDEPATH += $$_PRO_FILE_PWD_/../libdap-qt/

include (../libdap-qt-stream/libdapstreamclient.pri)
INCLUDEPATH += $$_PRO_FILE_PWD_/../libdap-qt-stream/
