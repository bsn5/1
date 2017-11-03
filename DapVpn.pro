TEMPLATE = subdirs
SUBDIRS = DapVpnGui DapVpnService

android{
    SUBDIRS -= DapVpnCli
#    DEFINES += -DDAP_PLATFORM_MOBILE
}
ios{
#    DEFINES += -DDAP_PLATFORM_MOBILE
    SUBDIRS -= DapVpnCli
}

DapVpnService.subdir = DapVpnService
DapVpnGui.subdir = DapVpnGui
DapVpnGui.depends = DapVpnService

TARGET = DiveVPN

RESOURCES += \
    DapVpnGui/ui/resources/DiveVPN/main.qrc \
    DapVpnGui/ui/resources/DiveVPN/main.qrc \
    DapVpnGui/ui/resources/common/common.qrc \
    DapVpnGui/ui/resources/DiveVPN/main.qrc \
    DapVpnGui/ui/resources/common/common.qrc \
    DapVpnGui/ui/resources/DiveVPN/main.qrc \
    DapVpnGui/ui/resources/common/common.qrc \
    DapVpnGui/ui/resources/DiveVPN/main.qrc \
    DapVpnGui/ui/resources/common/common.qrc \
    DapVpnGui/ui/resources/DiveVPN/main.qrc \
    DapVpnGui/ui/resources/common/common.qrc \
    DapVpnGui/ui/resources/DiveVPN/main.qrc \
    DapVpnGui/ui/resources/common/common.qrc \
    DapVpnGui/ui/resources/DiveVPN/main.qrc \
    DapVpnGui/ui/resources/common/common.qrc \
    DapVpnGui/ui/resources/DiveVPN/main.qrc \
    DapVpnGui/ui/resources/common/common.qrc \
    DapVpnGui/ui/resources/DiveVPN/main.qrc \
    DapVpnGui/ui/resources/common/common.qrc \
    DapVpnGui/ui/resources/DiveVPN/main.qrc \
    DapVpnGui/ui/resources/DiveVPN/main.qrc \
    DapVpnGui/ui/resources/DiveVPN/main.qrc \
    DapVpnGui/ui/resources/DiveVPN/main.qrc \
    DapVpnGui/ui/resources/DiveVPN/main.qrc \
    DapVpnGui/ui/resources/DiveVPN/main.qrc \
    DapVpnGui/ui/resources/common/common.qrc \
    DapVpnGui/ui/resources/DiveVPN/main.qrc \
    DapVpnGui/ui/resources/common/common.qrc \
    DapVpnGui/ui/resources/DiveVPN/main.qrc \
    DapVpnGui/ui/resources/common/common.qrc
