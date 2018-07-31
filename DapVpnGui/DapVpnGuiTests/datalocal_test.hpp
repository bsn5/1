#ifndef DATALOCAL_TEST_HPP
#define DATALOCAL_TEST_HPP

#include <QTest>
#include "../datalocal.h"

class DatalocalTest : public QObject {
    Q_OBJECT
private:
    // see Resources->testdata.qrc->/->data.xml
    size_t COUNT_SERVERS_IN_TESTDATA_XML = 1;
private slots:
//    void testParsingXmlData() {
//        auto serverLists = DataLocal::me()->servers();

//        /* in debug mode servers are adding by hard code.
//        That's why we searching by loop, our server from data xml */

//        for(auto &server: serverLists) {
//            if (server.location == "France" &&
//                server.name == "testServer" &&
//                server.address == "fr1.divevpn.com" &&
//                server.port == "80" &&
//                server.ip == "163.172.48.80") {
//                // Ok our server from xml successfully parsed
//                return;
//            }
//        }

//        // Not found server from data xml, test fail!
//        QVERIFY(false);
//    }

//    void testAddServer() {
//       auto serverLists = DataLocal::me()->servers();
//       DataLocal::me()->addServer("USSR", "Testing", "62.210.73.95", "testing.divevpn.com:8003");
//       for(auto &ts: serverLists) {
//       if (ts.location == "USSR" &&
//               ts.name == "Testing" &&
//               ts.address == "testing.divevpn.com" &&
//               ts.port == "8003" &&
//               ts.ip == "62.210.73.95")
//       return;
//       }
//       QVERIFY2(false, "Server not added");
//    }
};

#endif // DATALOCAL_TEST_HPP
