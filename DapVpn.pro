TEMPLATE = subdirs

SUBDIRS = DapVpnGui DapVpnService \
    DapVpnCommon

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
    DapVpnGui/ui/resources/common/common.qrc \
QMAKE_CFLAGS_DEBUG = \
    -std=gnu99

QMAKE_CFLAGS_RELEASE = \
    -std=gnu99
