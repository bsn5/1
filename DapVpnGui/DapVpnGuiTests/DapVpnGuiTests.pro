QT += testlib

CONFIG += qt console warn_on depend_includepath testcase
CONFIG -= app_bundle

TEMPLATE = app

# test headers and source
SOURCES +=  main.cpp

HEADERS += \
    TestHeaders.h \
    datalocal_test.hpp \

# tested classes
SOURCES +=  ../datalocal.cpp

HEADERS += ../datalocal.h

RESOURCES += \
    testdata.qrc
