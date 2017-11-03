#include <QStandardPaths>

#include <QStandardItemModel>
#include <QXmlStreamReader>
#include <QDir>
#include <QFile>
#include <QtDebug>
#include <QSettings>
#include <QCoreApplication>

#include "datalocal.h"

DataLocal *DataLocal::_me=NULL;

DataLocal::DataLocal()
{
    qDebug() << "[DL] Constructor";
    m_title = "DapVPN";

    m_ratingEnabled=true;
    m_loginWidth=400;
    m_loginHeight=300;

    m_mainWidth=1000;
    m_mainHeight=400;

    m_votingBorderWidth=1;
    QCoreApplication::setOrganizationName("veiledtech");
    QCoreApplication::setOrganizationDomain("veiledtech.com");
    QCoreApplication::setApplicationName("DAP_BRAND");
    m_settings = new QSettings();

    parseXML(":/data.xml");
}

void DataLocal::parseXML(const QString& a_fname)
{
    m_title = "DapVPN";


    qDebug() << "[DL] Constructor";
    QFile file(a_fname);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text)){
        qWarning() << "[DL] Can't open data.xml from built in resource";
    }

    QXmlStreamReader *sr = new QXmlStreamReader(&file);
    if(sr->readNextStartElement()){
        if(sr->name() == "data"){
            while ( sr->readNextStartElement() ){
                if( sr->name() == "servers"){
                    while ( sr->readNextStartElement() ){
                        if( sr->name() == "server"){
                            DapServerInfo item;
                            while ( sr->readNextStartElement() ){
                                if(sr->name() == "name"){
                                    item.name=sr->readElementText();
                                } else if (sr->name() == "address") {
                                    item.address=sr->readElementText();
                                } else if( sr->name() == "port") {
                                    item.port=sr->readElementText();
                                } else if(sr->name() == "location") {
                                    item.location = sr->readElementText();
                                } else if (sr->name() == "ip") {
                                    item.ip = sr->readElementText();
                                } else {
                                    qDebug() << "[DL] Inside tag 'server': Unknown tag "<<sr->name();
                                    sr->skipCurrentElement();
                                }
                            }
                            qDebug() << "[DL] Server "<<item.name<<" added";
                            m_servers.append(item);
                        }else{
                            qDebug() << "[DL] Inside tag 'servers': unknown tag "<<sr->name();
                            sr->skipCurrentElement();
                        }
                    }
                }if( sr->name() == "interface"){
                    while ( sr->readNextStartElement() ){
                        if (sr->name() == "title"){
                            m_title=sr->readElementText();
                        }
                    }
                }else{
                    qDebug() << "[DL] Inside tag 'data' unknown tag "<<sr->name();
                    //sr->skipCurrentElement();
                }
            }
        }
    }
#ifdef  QT_DEBUG
    addServer( "USSR", "Testing", "62.210.73.95", "testing.divevpn.com:8003");
    addServer( "USSR", "Dev1",    "62.210.73.95", "dev1.demlabs.net:8001");
    addServer( "USSR", "Dev2",    "62.210.73.95", "dev2.demlabs.net:8002");
#endif


    delete sr;
}

void DataLocal::addServer(const QString& a_location, const QString& a_name, const QString & a_ip, const QString& a_addrLine )
{
    for(auto s: servers() )
        if( s.name == a_name){
            s.location = a_location;
            s.address = a_addrLine.split(":").at(0);
            s.port = a_addrLine.split(":").at(1);
            s.ip = a_ip;
            return;
        }

    DapServerInfo ss;
    ss.name = a_name ;
    ss.location = a_location;
    ss.address = a_addrLine.split(":").at(0);
    ss.port = a_addrLine.split(":").at(1);
    ss.ip = a_ip;
    m_servers.append(ss);
}

/**
 * @brief DataLocal::locationToIcon
 * @param a_location
 * @return
 */
const QString& DataLocal::locationToIcon(const QString & a_location)
{
    static const QString l_de(":/country/Flag_de.png");
    static const QString l_fr(":/country/Flag_fr.png");
    static const QString l_usa(":/country/Flag_us.png");
    static const QString l_nl(":/country/Flag_nl.png");
    static const QString l_ru(":/country/Flag_ru.png");
    static const QString l_ukr(":/country/Flag_ua.png");
    static const QString l_uk(":/country/Flag_uk.png");
    static const QString l_ussr(":/country/ussr.png");
    if(a_location.contains("France")){
        return l_fr;
    } if(a_location.contains("Germany")){
        return l_de;
    }else if(a_location.contains("USA")){
        return l_usa;
    }else if(a_location.contains("Netherlands")){
        return l_nl;
    }else if(a_location.contains("Russia")){
        return l_ru;
    }else if(a_location.contains("Ukraine")){
        return l_ukr;
    }else if(a_location.contains("England")){
        return l_uk;
    }else
        return l_ussr;
}
