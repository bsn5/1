QT += testlib network
QT -= gui

CONFIG += qt console warn_on depend_includepath testcase
CONFIG -= app_bundle

TEMPLATE = app

INCLUDEPATH += ../../DapVpnCommon/
INCLUDEPATH += ../

SOURCES += \ 
    main.cpp \
    ../../DapVpnCommon/DapCmdHandlerAbstract.cpp \
    ../../DapVpnCommon/DapCmdParser.cpp \
    ../../DapVpnCommon/DapIndicatorState.cpp \
    ../../DapVpnCommon/DapJsonCmd.cpp \
    ../../DapVpnCommon/DapJsonParams.cpp \
    ../../DapVpnCommon/DapServerInfo.cpp \
    ../DapStateMachine.cpp \

HEADERS += \
    ../../DapVpnCommon/DapClientDefinitions.h \
    ../../DapVpnCommon/DapCmdHandlerAbstract.h \
    ../../DapVpnCommon/DapCmdParser.h \
    ../../DapVpnCommon/DapIndicatorState.h \
    ../../DapVpnCommon/DapJsonCmd.h \
    ../../DapVpnCommon/DapJsonParams.h \
    ../../DapVpnCommon/DapServerInfo.h \
    ../DapStateMachine.h \
    DapStateMachine_test.hpp
